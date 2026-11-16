# -*- coding: utf-8 -*-
"""
Complete V-AutoKeyPresser v2.5.0 Multi-Tab Studio Page (`AutoKeyPresserPage`)
integrated natively into V-MacroPlus:
- Multi-tab bar (`+ New Tab`, Rename Tab, Duplicate Tab, Close Tab, Save/Load Workspace).
- Global `Stop All Tabs` button + 2-way synchronized `Stop All Tabs Hotkey` (`hotkey_autopresser_stop_all`, synced with Settings).
- Per-Tab `Tab Start/Stop Toggle Hotkey` (`tab.stop_hotkey`, default `F6`) that starts the tab when stopped and stops it when running.
- Distinct Cyan/Teal `Choose Process...` picker with native Win32 `.exe` icons, PID, window title, and tab ownership indicator.
- 36 Built-in Key Tiles arranged strictly on 3 Rows x 12 Columns:
  * Row 1 (12 keys): `F1` .. `F12`
  * Row 2 (12 keys): `1` .. `9`, `0`, `-`, `=`
  * Row 3 (12 keys): `Num1` .. `Num9`, `Num0`, `Num/`, `Num*`
  + Custom Added Keys section below via a single `+ Add Custom Key` button that prompts for a key press and adds it immediately.
- Instant 1-click key arm/disarm toggle (with zero double-toggle conflict): arming keys on the grid marks them ready;
  pressing only starts when the user clicks `Start Tab` or presses the Tab's Start/Stop Hotkey.
- Unified 20 Hardware, Kernel, Foreground & Background Key-Pressing Methods + `Test All 20 Methods on Target` sweep with live log report.
- Multi-Point Mouse Click Automation with `F8` live coordinate capture, 8 Mouse Click Modes, and `Test All 8 Mouse Modes` sweep.
- Per-Tab Activity Log that persists across language switches and re-renders in the active UI language.
"""

from __future__ import annotations
from typing import Any, Dict, List, Optional, Tuple
from PySide6.QtCore import QEvent, QPointF, QRectF, QTimer, Qt, Signal
from PySide6.QtGui import QColor, QFont, QIntValidator, QMouseEvent, QPainter, QPen, QWheelEvent
from PySide6.QtWidgets import (
    QFileDialog,
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QScrollArea,
    QVBoxLayout,
    QWidget,
)

from app.domain.autopresser_engine import (
    AUTOPRESSER_KEY_METHODS,
    AUTOPRESSER_MOUSE_MODES,
    AutoKeyPresserEngine,
    AutoPresserKeyItem,
    AutoPresserMousePoint,
    AutoPresserTab,
    IS_WINDOWS,
    BACKGROUND_HWND_KEY_METHODS,
    _find_best_input_child_hwnd,
    _get_window_class_name,
    dispatch_autopresser_key,
    dispatch_autopresser_mouse,
    dispatch_autopresser_mouse_detailed,
    format_diagnostic_reason,
    focus_and_bring_hwnd_to_front,
    list_detailed_windows_for_picker,
    resolve_target_hwnd,
)
from app.domain.macro_engine import MacroEngine
from app.i18n import bidi_text, fmt_num, get_lang_font_family, is_rtl, t
from app.state import AppState
from app.theme import T
from app.ui.widgets import (
    Button,
    GlassCard,
    IconButton,
    KeyCaptureButton,
    NumberInput,
    SearchCombo,
    SegmentedControl,
    StepCard,
    TintedScrollBox,
    ToggleSwitch,
    framed_horizontal_scroll_style,
    make_font,
    modern_scroll_style,
)


def get_akp_key_method_options() -> List[Tuple[str, str]]:
    return [(k, t(tr_key)) for k, tr_key in AUTOPRESSER_KEY_METHODS]


def get_akp_mouse_mode_options() -> List[Tuple[str, str]]:
    return [(k, t(tr_key)) for k, tr_key in AUTOPRESSER_MOUSE_MODES]


class _KeyPillToggleButton(QWidget):
    """
    Crisp 1-click Key Toggle Pill inside `_KeyTileWidget`:
    Clicking once toggles `checked` immediately without any parent event propagation or double-toggle conflict.
    """

    toggled = Signal(bool)

    def __init__(self, key_label: str, checked: bool = False, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self._key_label = key_label
        self._checked = checked
        self._hovered = False
        self.setFixedHeight(30)
        self.setMinimumWidth(36)
        self.setCursor(Qt.CursorShape.PointingHandCursor)

    def setKeyLabel(self, key_label: str) -> None:
        if self._key_label != key_label:
            self._key_label = key_label
            self.update()

    def isChecked(self) -> bool:
        return self._checked

    def setChecked(self, checked: bool, emit_signal: bool = False) -> None:
        if self._checked == checked:
            return
        self._checked = checked
        self.update()
        if emit_signal:
            self.toggled.emit(self._checked)

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            event.accept()
            self.setChecked(not self._checked, emit_signal=True)
            return
        super().mousePressEvent(event)

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            event.accept()
            return
        super().mouseReleaseEvent(event)

    def enterEvent(self, event) -> None:
        self._hovered = True
        self.update()

    def leaveEvent(self, event) -> None:
        self._hovered = False
        self.update()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        r = QRectF(self.rect()).adjusted(1.0, 1.0, -1.0, -1.0)

        if self._checked:
            bg = T.c("accent")
            border_c = T.c("accent_hi")
            fg = T.c("on_accent")
            txt = f"✓ {self._key_label}"
        else:
            bg = T.c("surface_hi" if self._hovered else "bg2", 225)
            border_c = T.c("accent" if self._hovered else "border_hi")
            fg = T.c("text")
            txt = self._key_label

        p.setBrush(bg)
        p.setPen(QPen(border_c, 1.2))
        p.drawRoundedRect(r, 7.0, 7.0)

        p.setFont(make_font(10, QFont.Weight.ExtraBold))
        p.setPen(fg)
        p.drawText(r, int(Qt.AlignmentFlag.AlignCenter), txt)
        p.end()


class _KeyMacroModeButton(QWidget):
    """
    1-click toggle button (`⚡`) inside `_KeyTileWidget` that switches the key between:
    - `False`: Normal Key Press (`⌨` — sends the key normally)
    - `True`: Smart Key Macro Mode (`⚡` — triggers the Smart Key macro bound to this key in `الأزرار الذكية`)
    """

    toggled = Signal(bool)

    def __init__(self, checked: bool = False, has_linked_macro: bool = False, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self._checked = checked
        self._has_linked = has_linked_macro
        self._hovered = False
        self.setFixedSize(26, 28)
        self.setCursor(Qt.CursorShape.PointingHandCursor)
        self._sync_tooltip()

    def setHasLinkedMacro(self, has_linked: bool) -> None:
        if self._has_linked != has_linked:
            self._has_linked = has_linked
            self.update()

    def setChecked(self, checked: bool, emit_signal: bool = False) -> None:
        if self._checked == checked:
            return
        self._checked = checked
        self._sync_tooltip()
        self.update()
        if emit_signal:
            self.toggled.emit(self._checked)

    def _sync_tooltip(self) -> None:
        self.setToolTip(t("akp_tip_key_mode_macro") if self._checked else t("akp_tip_key_mode_normal"))

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            event.accept()
            self.setChecked(not self._checked, emit_signal=True)
            return
        super().mousePressEvent(event)

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            event.accept()
            return
        super().mouseReleaseEvent(event)

    def enterEvent(self, event) -> None:
        self._hovered = True
        self.update()

    def leaveEvent(self, event) -> None:
        self._hovered = False
        self.update()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        r = QRectF(self.rect()).adjusted(1.0, 1.0, -1.0, -1.0)
        if self._checked:
            bg = QColor("#F59E0B")
            border_c = QColor("#FBBF24")
            fg = QColor("#FFFFFF")
        else:
            bg = T.c("surface_hi" if self._hovered else "bg2", 210)
            border_c = QColor("#F59E0B") if self._has_linked else T.c("border_hi" if self._hovered else "border")
            fg = QColor("#F59E0B") if (self._has_linked or self._hovered) else T.c("text2")
        p.setBrush(bg)
        p.setPen(QPen(border_c, 1.1))
        p.drawRoundedRect(r, 5.0, 5.0)
        p.setFont(make_font(9, QFont.Weight.ExtraBold))
        p.setPen(fg)
        p.drawText(r, int(Qt.AlignmentFlag.AlignCenter), "⚡")
        p.end()


class _CompactMsEdit(QLineEdit):
    """Compact millisecond interval editor for 12-column key tiles (supports typing + mouse wheel +/-50 ms)."""

    msChanged = Signal(int)

    def __init__(self, value_ms: int = 1000, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self._val_ms = max(10, min(3600000, int(value_ms)))
        self.setFixedHeight(25)
        self.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.setValidator(QIntValidator(10, 3600000, self))
        self.setText(f"{self._val_ms}")
        self.setToolTip("Interval (ms) — Type value or scroll mouse wheel")
        self.setStyleSheet(
            f"""
            QLineEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('text')};
                border: 1px solid {T.token_hex('border')};
                border-radius: 6px;
                padding: 0px 2px;
                font-family: '{get_lang_font_family()}';
                font-size: 11px;
                font-weight: bold;
            }}
            QLineEdit:focus {{
                border: 1px solid {T.token_hex('accent')};
            }}
            """
        )
        self.editingFinished.connect(self._commit_text)

    def setValueMs(self, val_ms: int) -> None:
        v = max(10, min(3600000, int(val_ms)))
        if v != self._val_ms or self.text() != str(v):
            self._val_ms = v
            self.setText(str(v))

    def _commit_text(self) -> None:
        raw = self.text().strip()
        try:
            v = max(10, min(3600000, int(raw)))
        except ValueError:
            v = self._val_ms
        self._val_ms = v
        self.setText(str(v))
        self.msChanged.emit(v)

    def wheelEvent(self, event: QWheelEvent) -> None:
        delta = event.angleDelta().y()
        if delta != 0:
            step = 50 if abs(delta) >= 120 else 10
            new_v = max(10, min(3600000, self._val_ms + (step if delta > 0 else -step)))
            if new_v != self._val_ms:
                self._val_ms = new_v
                self.setText(str(new_v))
                self.msChanged.emit(new_v)
            event.accept()
            return
        super().wheelEvent(event)


class _KeyTileWidget(QWidget):
    """
    Compact 12-per-row Key Tile inside the 36-Key (3 rows x 12 cols) + Custom Key Grid:
    - Single-click `_KeyPillToggleButton` arms/disarms (`self.item.enabled`) cleanly with zero double-toggle bug.
    - Compact `_CompactMsEdit` + `ms` label lets the user adjust the repeat interval in milliseconds.
    - `▶` button fires a single immediate test press using the tab's active method & target window.
    - Custom keys also show a `×` remove button.
    """

    changed = Signal()
    testClicked = Signal(str)
    removeClicked = Signal(str)

    def __init__(
        self,
        item: AutoPresserKeyItem,
        has_linked_macro: bool = False,
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self.item = item
        self._hovered = False
        self.setFixedHeight(78)
        self.setMinimumWidth(70)

        lay = QVBoxLayout(self)
        lay.setContentsMargins(6, 5, 6, 5)
        lay.setSpacing(5)

        top = QHBoxLayout()
        top.setContentsMargins(0, 0, 0, 0)
        top.setSpacing(4)

        self.pill_btn = _KeyPillToggleButton(self.item.key, checked=self.item.enabled, parent=self)
        self.pill_btn.toggled.connect(self._on_toggled)
        top.addWidget(self.pill_btn, 1)

        self.btn_macro_mode = _KeyMacroModeButton(
            checked=bool(getattr(self.item, "use_smart_combo", False)),
            has_linked_macro=has_linked_macro,
            parent=self,
        )
        self.btn_macro_mode.toggled.connect(self._on_macro_mode_toggled)
        top.addWidget(self.btn_macro_mode)

        if self.item.is_custom:
            self.btn_del = IconButton("close", size=22, variant="danger", tooltip=t("btn_delete_rec"), parent=self)
            self.btn_del.clicked.connect(lambda: self.removeClicked.emit(self.item.key))
            top.addWidget(self.btn_del)

        lay.addLayout(top)

        bot = QHBoxLayout()
        bot.setContentsMargins(0, 0, 0, 0)
        bot.setSpacing(3)
        self.ms_edit = _CompactMsEdit(self.item.interval_ms, parent=self)
        self.ms_edit.msChanged.connect(self._on_interval_changed)
        lbl_ms = QLabel("ms", self)
        lbl_ms.setFont(make_font(9, QFont.Weight.Bold))
        lbl_ms.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        self.btn_test = IconButton("play", size=23, variant="glass", tooltip=t("akp_test_single_key"), parent=self)
        self.btn_test.clicked.connect(lambda: self.testClicked.emit(self.item.key))
        bot.addWidget(self.ms_edit, 1)
        bot.addWidget(lbl_ms)
        bot.addWidget(self.btn_test)
        lay.addLayout(bot)

    def bind_item(self, item: AutoPresserKeyItem, has_linked_macro: bool = False) -> None:
        self.item = item
        self.pill_btn.setKeyLabel(item.key)
        self.pill_btn.setChecked(item.enabled, emit_signal=False)
        self.btn_macro_mode.setHasLinkedMacro(has_linked_macro)
        self.btn_macro_mode.setChecked(bool(getattr(item, "use_smart_combo", False)), emit_signal=False)
        self.ms_edit.setValueMs(item.interval_ms)
        self.update()

    def _on_toggled(self, val: bool) -> None:
        self.item.enabled = val
        self.update()
        self.changed.emit()

    def _on_macro_mode_toggled(self, val: bool) -> None:
        self.item.use_smart_combo = val
        self.update()
        self.changed.emit()

    def _on_interval_changed(self, val: int) -> None:
        self.item.interval_ms = max(10, int(val))
        self.changed.emit()

    def enterEvent(self, event) -> None:
        self._hovered = True
        self.update()

    def leaveEvent(self, event) -> None:
        self._hovered = False
        self.update()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(0.8, 0.8, -0.8, -0.8)
        if self.item.enabled:
            bg = T.c("accent", 46 if T.is_dark() else 34)
            border_c = T.c("accent")
            pen_w = 1.7
        else:
            bg = T.c("surface_hi" if self._hovered else "bg1", 215)
            border_c = T.c("border_hi" if self._hovered else "border")
            pen_w = 1.0
        p.setBrush(bg)
        p.setPen(QPen(border_c, pen_w))
        p.drawRoundedRect(r, 9.0, 9.0)
        p.end()


class _AddCustomKeyButton(Button):
    """
    Single button `+ إضافة زرار مخصص` (`+ Add Custom Key`):
    When clicked, it enters live capture mode (`⌨ اضغط أي زرار الآن لإضافته... (Esc للإلغاء)`),
    captures the next physical key or mouse button pressed by the user, and emits `customKeyCaptured(str)`.
    """

    customKeyCaptured = Signal(str)

    def __init__(self, state: AppState, parent: Optional[QWidget] = None) -> None:
        super().__init__(t("akp_btn_add_custom_key"), variant="primary", icon_name="plus", parent=parent)
        self.state = state
        self._capturing = False
        self.setFixedHeight(32)
        self.clicked.connect(self._toggle_capture)
        self.state.hook_key_event.connect(self._on_hook_key)

    def _toggle_capture(self) -> None:
        if self._capturing:
            self._stop_capture()
        else:
            self._capturing = True
            self.state.is_capturing_key = True
            self.setText(t("akp_btn_capturing_custom_key"))
            self.setVariant("danger")

    def _stop_capture(self) -> None:
        self._capturing = False
        self.state.is_capturing_key = False
        self.setText(t("akp_btn_add_custom_key"))
        self.setVariant("primary")

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if self._capturing:
            if event.button() in (Qt.MouseButton.BackButton, Qt.MouseButton.XButton1):
                event.accept()
                self._stop_capture()
                self.customKeyCaptured.emit("MouseBack")
                return
            if event.button() in (Qt.MouseButton.ForwardButton, Qt.MouseButton.XButton2):
                event.accept()
                self._stop_capture()
                self.customKeyCaptured.emit("MouseForward")
                return
        super().mousePressEvent(event)

    def _on_hook_key(self, scan: int, ext: bool, is_down: bool, label: str) -> None:
        if not self._capturing or not is_down:
            return
        if label.lower() in ("esc", "escape"):
            self._stop_capture()
            return
        clean = label.strip()
        self._stop_capture()
        if clean:
            self.customKeyCaptured.emit(clean)


class _MousePointRowWidget(QWidget):
    """Single Multi-Point Mouse Click Row with live F8 coordinate capture, 5 mouse buttons (incl. Back/Forward), and live Test button."""

    changed = Signal()
    removed = Signal(int)
    pickRequested = Signal(int)
    testRequested = Signal(int)

    def __init__(
        self,
        index: int,
        mp: AutoPresserMousePoint,
        is_picking: bool,
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self.index = index
        self.mp = mp
        self.card = StepCard(index=index, accent_hex="#06B6D4", parent=self)

        outer = QVBoxLayout(self)
        outer.setContentsMargins(0, 0, 0, 0)
        outer.addWidget(self.card)

        lay = QHBoxLayout(self.card)
        lay.setContentsMargins(12, 6, 12, 6)
        lay.setSpacing(8)

        self.tog = ToggleSwitch(self.mp.enabled)
        self.tog.toggled.connect(self._on_enabled)
        lay.addWidget(self.tog)

        lbl_idx = QLabel(f"#{fmt_num(index + 1)}")
        lbl_idx.setFont(make_font(12, QFont.Weight.ExtraBold))
        lbl_idx.setStyleSheet("color: #06B6D4; background: transparent;")
        lay.addWidget(lbl_idx)

        self.btn_combo = SearchCombo(
            [
                ("left", t("mouse_btn_left")),
                ("right", t("mouse_btn_right")),
                ("middle", t("mouse_btn_middle")),
                ("x1", t("mouse_btn_x1")),
                ("x2", t("mouse_btn_x2")),
            ],
            selected_key=self.mp.button,
            parent=self.card,
        )
        self.btn_combo.setFixedWidth(170)
        self.btn_combo.currentChanged.connect(self._on_btn_changed)
        lay.addWidget(self.btn_combo)

        lbl_x = QLabel("X:")
        lbl_x.setFont(make_font(11, QFont.Weight.Bold))
        lbl_x.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        self.num_x = NumberInput(value=float(self.mp.x), min_val=-32768.0, max_val=32768.0, step=10.0, decimals=0)
        self.num_x.setFixedSize(92, 30)
        self.num_x.valueChanged.connect(self._on_x)

        lbl_y = QLabel("Y:")
        lbl_y.setFont(make_font(11, QFont.Weight.Bold))
        lbl_y.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        self.num_y = NumberInput(value=float(self.mp.y), min_val=-32768.0, max_val=32768.0, step=10.0, decimals=0)
        self.num_y.setFixedSize(92, 30)
        self.num_y.valueChanged.connect(self._on_y)

        self.btn_pick = Button(
            t("press_f8_live") if is_picking else t("btn_pick_live_xy"),
            variant="primary" if is_picking else "glass",
            icon_name="crosshair",
            tint_hex="#10B981",
        )
        self.btn_pick.setFixedHeight(30)
        self.btn_pick.clicked.connect(lambda: self.pickRequested.emit(self.index))

        lbl_int = QLabel(t("lbl_pre_delay"))
        lbl_int.setFont(make_font(11, QFont.Weight.Bold))
        lbl_int.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        self.num_int = NumberInput(
            value=float(self.mp.interval_ms),
            min_val=10.0,
            max_val=3600000.0,
            step=100.0,
            decimals=0,
            suffix="ms",
        )
        self.num_int.setFixedSize(118, 30)
        self.num_int.valueChanged.connect(self._on_int)

        self.btn_test_mp = Button(
            t("akp_btn_test_mouse_pt"),
            variant="glass",
            icon_name="play",
            tint_hex="#10B981",
            parent=self.card,
        )
        self.btn_test_mp.setFixedHeight(30)
        self.btn_test_mp.setToolTip(t("akp_tip_test_mouse_pt"))
        self.btn_test_mp.clicked.connect(lambda: self.testRequested.emit(self.index))

        btn_del = IconButton("trash", size=30, variant="danger", tooltip=t("btn_delete_rec"), parent=self.card)
        btn_del.clicked.connect(lambda: self.removed.emit(self.index))

        lay.addWidget(lbl_x)
        lay.addWidget(self.num_x)
        lay.addWidget(lbl_y)
        lay.addWidget(self.num_y)
        lay.addWidget(self.btn_pick)
        lay.addWidget(self.btn_test_mp)
        lay.addStretch(1)
        lay.addWidget(lbl_int)
        lay.addWidget(self.num_int)
        lay.addWidget(btn_del)

    def _on_enabled(self, v: bool) -> None:
        self.mp.enabled = v
        self.changed.emit()

    def _on_btn_changed(self, k: str) -> None:
        self.mp.button = k
        self.changed.emit()

    def _on_x(self, v: float) -> None:
        self.mp.x = int(v)
        self.changed.emit()

    def _on_y(self, v: float) -> None:
        self.mp.y = int(v)
        self.changed.emit()

    def _on_int(self, v: float) -> None:
        self.mp.interval_ms = max(10, int(v))
        self.changed.emit()


class AutoKeyPresserPage(QWidget):
    """
    Full V-AutoKeyPresser v2.5.0 Multi-Tab Studio Page integrated inside V-MacroPlus.
    """

    def __init__(self, state: AppState, engine: MacroEngine, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.state = state
        self.macro_engine = engine
        if not hasattr(self.state, "autopresser_engine") or self.state.autopresser_engine is None:
            self.state.autopresser_engine = AutoKeyPresserEngine(self.state)
        self.akp: AutoKeyPresserEngine = self.state.autopresser_engine
        self.akp.set_macro_engine(self.macro_engine)

        self._sub_view: str = "keys"  # "keys", "mouse", "log"
        self._picking_mouse_idx: int = -1
        self._window_cache: List[Dict[str, Any]] = []
        self._last_log_sig: Tuple[str, int, str] = ("", -1, "")
        self._sweep_running: bool = False
        self._builtin_tiles: List[_KeyTileWidget] = []

        self._save_timer = QTimer(self)
        self._save_timer.setSingleShot(True)
        self._save_timer.setInterval(220)
        self._save_timer.timeout.connect(self.akp.save_workspace)

        self._build_ui()
        self._refresh_tabs_bar()
        self._load_current_tab_into_ui()

        self.state.cursor_pick_hotkey_pressed.connect(self._on_f8_captured)
        self.state.settings_changed.connect(self._on_external_settings_changed)
        T.theme_changed.connect(self._on_theme_updated)

        self._status_timer = QTimer(self)
        self._status_timer.setInterval(350)
        self._status_timer.timeout.connect(self._tick_ui_status)
        self._status_timer.start()

    def _current_tab(self) -> AutoPresserTab:
        if not self.akp.tabs:
            self.akp.add_new_tab(save_immediately=False)
        self._schedule_save_workspace()
        idx = max(0, min(len(self.akp.tabs) - 1, self.akp.active_tab_index))
        self.akp.active_tab_index = idx
        return self.akp.tabs[idx]

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(14, 6, 14, 8)
        root.setSpacing(6)

        # 1. Header & Global Stop-All Hotkey + Workspace Actions Card
        hdr_card = GlassCard(accent_tint=True, parent=self)
        h_lay = QHBoxLayout(hdr_card)
        h_lay.setContentsMargins(14, 7, 14, 7)
        h_lay.setSpacing(8)

        t_col = QVBoxLayout()
        t_col.setSpacing(1)
        title = QLabel(t("akp_title"))
        title.setFont(make_font(14, QFont.Weight.ExtraBold))
        title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        sub = QLabel(t("akp_subtitle"))
        sub.setFont(make_font(11, QFont.Weight.Medium))
        sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        t_col.addWidget(title)
        t_col.addWidget(sub)
        h_lay.addLayout(t_col, 1)

        lbl_stop_all_hk = QLabel(t("akp_lbl_stop_all_hotkey"))
        lbl_stop_all_hk.setFont(make_font(11, QFont.Weight.Bold))
        lbl_stop_all_hk.setStyleSheet(f"color: {T.token_hex('danger')}; background: transparent;")

        self.hk_stop_all_tabs = KeyCaptureButton(
            getattr(self.state.config, "hotkey_autopresser_stop_all", "F7") or "F7",
            allow_chord=True,
            state=self.state,
        )
        self.hk_stop_all_tabs.setFixedSize(128, 30)
        self.hk_stop_all_tabs.keyCaptured.connect(self._on_stop_all_hotkey_changed)

        self.btn_stop_all = Button(t("akp_btn_stop_all_tabs"), variant="danger", icon_name="stop")
        self.btn_stop_all.setFixedHeight(30)
        self.btn_stop_all.clicked.connect(self._on_stop_all_tabs)

        btn_save_ws = Button(t("akp_btn_save_workspace"), variant="glass", icon_name="save")
        btn_save_ws.setFixedHeight(30)
        btn_save_ws.clicked.connect(self._on_save_workspace)

        btn_load_ws = Button(t("akp_btn_load_workspace"), variant="ghost", icon_name="folder")
        btn_load_ws.setFixedHeight(30)
        btn_load_ws.clicked.connect(self._on_load_workspace)

        h_lay.addWidget(lbl_stop_all_hk)
        h_lay.addWidget(self.hk_stop_all_tabs)
        h_lay.addWidget(self.btn_stop_all)
        h_lay.addWidget(btn_save_ws)
        h_lay.addWidget(btn_load_ws)
        root.addWidget(hdr_card)

        # 2. Multi-Tab Strip Card (`Tab 1`, `Tab 2`, `+ New Tab`, Rename, Duplicate, Close)
        tab_card = GlassCard(parent=self)
        tb_lay = QHBoxLayout(tab_card)
        tb_lay.setContentsMargins(12, 5, 12, 5)
        tb_lay.setSpacing(6)

        self.tabs_scroll_box = TintedScrollBox(tint_hex="")
        self.tabs_scroll_box.setFixedHeight(44)
        ts_box_lay = QHBoxLayout(self.tabs_scroll_box)
        ts_box_lay.setContentsMargins(6, 3, 6, 3)
        ts_box_lay.setSpacing(0)

        self.tabs_scroll = QScrollArea()
        self.tabs_scroll.setWidgetResizable(True)
        self.tabs_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.tabs_scroll.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.tabs_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.tabs_scroll.setStyleSheet(modern_scroll_style())

        self.tabs_btn_container = QWidget()
        self.tabs_btn_container.setStyleSheet("background: transparent;")
        self.tabs_btn_layout = QHBoxLayout(self.tabs_btn_container)
        self.tabs_btn_layout.setContentsMargins(4, 2, 4, 2)
        self.tabs_btn_layout.setSpacing(6)
        self.tabs_scroll.setWidget(self.tabs_btn_container)
        ts_box_lay.addWidget(self.tabs_scroll)
        tb_lay.addWidget(self.tabs_scroll_box, 1)

        self.tab_name_edit = QLineEdit()
        self.tab_name_edit.setPlaceholderText(t("akp_ph_tab_name"))
        self.tab_name_edit.setFixedWidth(130)
        self.tab_name_edit.setFixedHeight(28)
        self.tab_name_edit.setStyleSheet(
            f"""
            QLineEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('text')};
                border: 1px solid {T.token_hex('border_hi')};
                border-radius: 8px;
                padding: 2px 8px;
                font-family: '{get_lang_font_family()}';
                font-size: 12px;
                font-weight: bold;
            }}
            """
        )
        self.tab_name_edit.textEdited.connect(self._on_tab_name_edited)

        btn_new_tab = Button(t("akp_btn_new_tab"), variant="primary", icon_name="plus")
        btn_new_tab.setFixedHeight(28)
        btn_new_tab.clicked.connect(self._on_add_tab)

        btn_dup_tab = Button(t("akp_btn_dup_tab"), variant="glass", icon_name="copy")
        btn_dup_tab.setFixedHeight(28)
        btn_dup_tab.clicked.connect(self._on_dup_tab)

        btn_close_tab = IconButton("close", size=28, variant="danger", tooltip=t("akp_btn_close_tab"))
        btn_close_tab.clicked.connect(self._on_close_current_tab)

        tb_lay.addWidget(self.tab_name_edit)
        tb_lay.addWidget(btn_new_tab)
        tb_lay.addWidget(btn_dup_tab)
        tb_lay.addWidget(btn_close_tab)
        root.addWidget(tab_card)

        # 3. Per-Tab Target Process + Per-Tab Start/Stop Hotkey + 20 Methods + Sweep All Methods Card
        ctrl_card = GlassCard(parent=self)
        c_lay = QVBoxLayout(ctrl_card)
        c_lay.setContentsMargins(12, 7, 12, 7)
        c_lay.setSpacing(6)

        # Row A: Distinct Cyan/Teal Choose Process Button + Process SearchCombo + Run in BG + Per-Tab Hotkey + Start/Stop Tab
        r_proc = QHBoxLayout()
        r_proc.setSpacing(6)

        self.btn_choose_proc = Button(
            t("akp_btn_choose_process"),
            variant="glass",
            icon_name="",
            tint_hex="#06B6D4",
        )
        self.btn_choose_proc.setFixedHeight(32)
        self.btn_choose_proc.clicked.connect(self._refresh_process_dropdown_and_open)

        self.proc_combo = SearchCombo([], selected_key="")
        self.proc_combo.setMinimumWidth(230)
        self.proc_combo.currentChanged.connect(self._on_process_selected)

        btn_ref_proc = IconButton("search", size=30, variant="glass", tooltip=t("toast_apps_refreshed"))
        btn_ref_proc.clicked.connect(lambda: self._refresh_process_dropdown(force_os_scan=True))

        self.btn_switch_win = Button(
            t("akp_btn_switch_win"),
            variant="glass",
            icon_name="",
            tint_hex="#F59E0B",
        )
        self.btn_switch_win.setFixedHeight(30)
        self.btn_switch_win.clicked.connect(self._on_switch_to_target_window)

        lbl_bg = QLabel(t("akp_lbl_run_in_bg"))
        lbl_bg.setFont(make_font(11, QFont.Weight.Bold))
        lbl_bg.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        self.tog_bg = ToggleSwitch(True)
        self.tog_bg.toggled.connect(self._on_bg_toggled)

        lbl_tab_hk = QLabel(t("akp_lbl_tab_hotkey"))
        lbl_tab_hk.setFont(make_font(11, QFont.Weight.Bold))
        lbl_tab_hk.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.hk_tab_toggle = KeyCaptureButton("F6", allow_chord=True, state=self.state)
        self.hk_tab_toggle.setFixedSize(115, 30)
        self.hk_tab_toggle.keyCaptured.connect(self._on_tab_hotkey_changed)

        self.btn_start_tab = Button(t("akp_btn_start_tab"), variant="primary", icon_name="play")
        self.btn_start_tab.setFixedHeight(34)
        self.btn_start_tab.clicked.connect(self._on_toggle_current_tab)

        r_proc.addWidget(self.btn_choose_proc)
        r_proc.addWidget(self.proc_combo, 1)
        r_proc.addWidget(btn_ref_proc)
        r_proc.addWidget(self.btn_switch_win)
        r_proc.addSpacing(4)
        r_proc.addWidget(lbl_bg)
        r_proc.addWidget(self.tog_bg)
        r_proc.addSpacing(6)
        r_proc.addWidget(lbl_tab_hk)
        r_proc.addWidget(self.hk_tab_toggle)
        r_proc.addSpacing(4)
        r_proc.addWidget(self.btn_start_tab)
        c_lay.addLayout(r_proc)

        # Row B: Unified 20 Key Methods + Hold Duration (ms) + Test Current Method + Test All 20 Methods + Save/Load Tab Profile
        r_meth = QHBoxLayout()
        r_meth.setSpacing(6)

        lbl_m = QLabel(t("akp_lbl_method"))
        lbl_m.setFont(make_font(11, QFont.Weight.Bold))
        lbl_m.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.method_combo = SearchCombo(get_akp_key_method_options(), selected_key="attach_state")
        self.method_combo.setMinimumWidth(250)
        self.method_combo.currentChanged.connect(self._on_method_changed)

        lbl_hold = QLabel(t("lbl_hold_duration"))
        lbl_hold.setFont(make_font(11, QFont.Weight.Bold))
        lbl_hold.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")

        self.num_hold = NumberInput(value=50.0, min_val=10.0, max_val=2000.0, step=10.0, decimals=0, suffix="ms")
        self.num_hold.setFixedSize(106, 30)
        self.num_hold.valueChanged.connect(self._on_hold_changed)

        btn_test_m = Button(t("akp_btn_test_method"), variant="glass", icon_name="bolt", tint_hex="#10B981")
        btn_test_m.setFixedHeight(30)
        btn_test_m.clicked.connect(self._on_test_active_method)

        self.btn_sweep_all = Button(t("akp_btn_test_all_methods"), variant="glass", icon_name="shield", tint_hex="#06B6D4")
        self.btn_sweep_all.setFixedHeight(30)
        self.btn_sweep_all.clicked.connect(self._on_sweep_all_key_methods)

        btn_save_prof = Button(t("akp_btn_save_profile"), variant="ghost", icon_name="save")
        btn_save_prof.setFixedHeight(30)
        btn_save_prof.clicked.connect(self._on_save_tab_profile)

        btn_load_prof = Button(t("akp_btn_load_profile"), variant="ghost", icon_name="folder")
        btn_load_prof.setFixedHeight(30)
        btn_load_prof.clicked.connect(self._on_load_tab_profile)

        r_meth.addWidget(lbl_m)
        r_meth.addWidget(self.method_combo, 1)
        r_meth.addWidget(lbl_hold)
        r_meth.addWidget(self.num_hold)
        r_meth.addWidget(btn_test_m)
        r_meth.addWidget(self.btn_sweep_all)
        r_meth.addWidget(btn_save_prof)
        r_meth.addWidget(btn_load_prof)
        c_lay.addLayout(r_meth)

        root.addWidget(ctrl_card)

        # 4. Sub-View Bar (`Keys Grid (36 + Custom)` | `Multi-Point Mouse Clicks (F8)` | `Activity Log`) + Single `+ Add Custom Key` Button
        sub_bar = QHBoxLayout()
        sub_bar.setSpacing(8)

        self.sub_seg = SegmentedControl(
            [
                ("keys", t("akp_sub_keys")),
                ("mouse", t("akp_sub_mouse")),
                ("log", t("akp_sub_log")),
            ],
            selected_key="keys",
        )
        # Do NOT shrink minimumWidth below its calculated full text width so labels NEVER clip with "..."!
        self.sub_seg.changed.connect(self._on_sub_view_changed)
        sub_bar.addWidget(self.sub_seg)

        sub_bar.addStretch(1)

        self.btn_add_custom_key = _AddCustomKeyButton(self.state, self)
        self.btn_add_custom_key.customKeyCaptured.connect(self._on_custom_key_captured)

        btn_arm_all = Button(t("akp_btn_arm_all"), variant="ghost", parent=self)
        btn_arm_all.setFixedHeight(32)
        btn_arm_all.clicked.connect(lambda: self._set_all_keys_enabled(True))

        btn_disarm_all = Button(t("akp_btn_disarm_all"), variant="ghost", parent=self)
        btn_disarm_all.setFixedHeight(32)
        btn_disarm_all.clicked.connect(lambda: self._set_all_keys_enabled(False))

        sub_bar.addWidget(self.btn_add_custom_key)
        sub_bar.addWidget(btn_arm_all)
        sub_bar.addWidget(btn_disarm_all)
        root.addLayout(sub_bar)

        # 5. Main Content Area (Keys Grid / Mouse Clicks / Activity Log)
        self.body_card = GlassCard(parent=self)
        b_lay = QVBoxLayout(self.body_card)
        b_lay.setContentsMargins(10, 8, 10, 8)
        b_lay.setSpacing(6)

        # 5a. Keys Grid Scroll Area (36 Built-in Keys strictly on 3 Rows x 12 Columns + Custom Keys Row)
        self.keys_scroll = QScrollArea()
        self.keys_scroll.setWidgetResizable(True)
        self.keys_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.keys_scroll.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.keys_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.keys_scroll.setStyleSheet(modern_scroll_style())

        self.keys_container = QWidget()
        self.keys_container.setStyleSheet("background: transparent;")
        self.keys_outer_vbox = QVBoxLayout(self.keys_container)
        self.keys_outer_vbox.setContentsMargins(2, 2, 4, 2)
        self.keys_outer_vbox.setSpacing(6)

        self.lbl_builtin_hdr = QLabel(t("lbl_builtin_36_section"))
        self.lbl_builtin_hdr.setFont(make_font(11, QFont.Weight.Bold))
        self.lbl_builtin_hdr.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        self.keys_outer_vbox.addWidget(self.lbl_builtin_hdr)

        self.builtin_grid_w = QWidget()
        self.builtin_grid_w.setStyleSheet("background: transparent;")
        self.keys_grid = QGridLayout(self.builtin_grid_w)
        self.keys_grid.setContentsMargins(0, 0, 0, 0)
        self.keys_grid.setHorizontalSpacing(7)
        self.keys_grid.setVerticalSpacing(7)
        for col_idx in range(12):
            self.keys_grid.setColumnStretch(col_idx, 1)
        self.keys_outer_vbox.addWidget(self.builtin_grid_w)

        self.lbl_custom_hdr = QLabel(t("lbl_custom_keys_section"))
        self.lbl_custom_hdr.setFont(make_font(11, QFont.Weight.Bold))
        self.lbl_custom_hdr.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        self.keys_outer_vbox.addWidget(self.lbl_custom_hdr)

        self.custom_grid_w = QWidget()
        self.custom_grid_w.setStyleSheet("background: transparent;")
        self.custom_keys_grid = QGridLayout(self.custom_grid_w)
        self.custom_keys_grid.setContentsMargins(0, 0, 0, 0)
        self.custom_keys_grid.setHorizontalSpacing(6)
        self.custom_keys_grid.setVerticalSpacing(6)
        for col_idx in range(8):
            self.custom_keys_grid.setColumnStretch(col_idx, 1)
        self.keys_outer_vbox.addWidget(self.custom_grid_w)
        self.keys_outer_vbox.addStretch(1)

        self.keys_scroll.setWidget(self.keys_container)
        b_lay.addWidget(self.keys_scroll, 1)

        # 5b. Multi-Point Mouse Clicks Container
        self.mouse_panel = QWidget()
        self.mouse_panel.setStyleSheet("background: transparent;")
        mp_lay = QVBoxLayout(self.mouse_panel)
        mp_lay.setContentsMargins(0, 0, 0, 0)
        mp_lay.setSpacing(8)

        mp_top = QHBoxLayout()
        mp_top.setSpacing(8)
        self.tog_mouse_master = ToggleSwitch(False)
        self.tog_mouse_master.toggled.connect(self._on_mouse_master_toggled)
        lbl_mm = QLabel(t("akp_lbl_enable_mouse"))
        lbl_mm.setFont(make_font(13, QFont.Weight.ExtraBold))
        lbl_mm.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

        lbl_mmode = QLabel(t("akp_lbl_mouse_mode"))
        lbl_mmode.setFont(make_font(12, QFont.Weight.Bold))
        lbl_mmode.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        self.mouse_mode_combo = SearchCombo(get_akp_mouse_mode_options(), selected_key="auto")
        self.mouse_mode_combo.setMinimumWidth(220)
        self.mouse_mode_combo.currentChanged.connect(self._on_mouse_mode_changed)

        self.btn_sweep_mouse = Button(t("akp_btn_test_all_mouse_modes"), variant="glass", icon_name="shield", tint_hex="#06B6D4")
        self.btn_sweep_mouse.setFixedHeight(32)
        self.btn_sweep_mouse.clicked.connect(self._on_sweep_all_mouse_modes)

        btn_add_mp = Button(t("akp_btn_add_mouse_pt"), variant="primary", icon_name="plus")
        btn_add_mp.setFixedHeight(32)
        btn_add_mp.clicked.connect(self._on_add_mouse_point)

        mp_top.addWidget(self.tog_mouse_master)
        mp_top.addWidget(lbl_mm)
        mp_top.addSpacing(8)
        mp_top.addWidget(lbl_mmode)
        mp_top.addWidget(self.mouse_mode_combo, 1)
        mp_top.addWidget(self.btn_sweep_mouse)
        mp_top.addWidget(btn_add_mp)
        mp_lay.addLayout(mp_top)

        self.mouse_scroll = QScrollArea()
        self.mouse_scroll.setWidgetResizable(True)
        self.mouse_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.mouse_scroll.setStyleSheet(modern_scroll_style())
        self.mouse_list_w = QWidget()
        self.mouse_list_w.setStyleSheet("background: transparent;")
        self.mouse_vbox = QVBoxLayout(self.mouse_list_w)
        self.mouse_vbox.setContentsMargins(0, 0, 4, 0)
        self.mouse_vbox.setSpacing(6)
        self.mouse_scroll.setWidget(self.mouse_list_w)
        mp_lay.addWidget(self.mouse_scroll, 1)

        self.mouse_panel.hide()
        b_lay.addWidget(self.mouse_panel, 1)

        # 5c. Per-Tab Activity Log Container
        self.log_panel = QWidget()
        self.log_panel.setStyleSheet("background: transparent;")
        lg_lay = QVBoxLayout(self.log_panel)
        lg_lay.setContentsMargins(0, 0, 0, 0)
        lg_lay.setSpacing(6)

        lg_top = QHBoxLayout()
        lbl_lg = QLabel(t("akp_lbl_activity_log"))
        lbl_lg.setFont(make_font(13, QFont.Weight.ExtraBold))
        lbl_lg.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        btn_clr_lg = Button(t("btn_clear_test_log"), variant="ghost", icon_name="trash")
        btn_clr_lg.setFixedHeight(30)
        btn_clr_lg.clicked.connect(self._on_clear_tab_log)
        lg_top.addWidget(lbl_lg, 1)
        lg_top.addWidget(btn_clr_lg)
        lg_lay.addLayout(lg_top)

        self.log_scroll = QScrollArea()
        self.log_scroll.setWidgetResizable(True)
        self.log_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.log_scroll.setStyleSheet(modern_scroll_style())
        self.log_list_w = QWidget()
        self.log_list_w.setStyleSheet("background: transparent;")
        self.log_vbox = QVBoxLayout(self.log_list_w)
        self.log_vbox.setContentsMargins(0, 0, 4, 0)
        self.log_vbox.setSpacing(6)
        self.log_scroll.setWidget(self.log_list_w)
        lg_lay.addWidget(self.log_scroll, 1)

        self.log_panel.hide()
        b_lay.addWidget(self.log_panel, 1)

        root.addWidget(self.body_card, 1)

    def _schedule_save_workspace(self) -> None:
        self._save_timer.start()

    def _on_theme_updated(self) -> None:
        self.tabs_scroll_box.update()
        self.tabs_scroll.setStyleSheet(modern_scroll_style())
        self.keys_scroll.setStyleSheet(modern_scroll_style())
        self.update()

    def _on_external_settings_changed(self, key: str, val: object) -> None:
        if key == "hotkey_autopresser_stop_all":
            self.hk_stop_all_tabs.setKeyString(str(val or "F7"))

    def _on_stop_all_hotkey_changed(self, new_key: str) -> None:
        clean = (new_key or "F7").strip()
        self.state.update_setting("hotkey_autopresser_stop_all", clean)

    def _on_tab_hotkey_changed(self, new_key: str) -> None:
        tab = self._current_tab()
        tab.stop_hotkey = (new_key or "").strip()
        self.akp.save_workspace()
        self._refresh_tabs_bar()

    def _refresh_tabs_bar(self) -> None:
        self.tabs_btn_container.setUpdatesEnabled(False)
        try:
            while self.tabs_btn_layout.count():
                it = self.tabs_btn_layout.takeAt(0)
                w = it.widget()
                if w is not None:
                    w.hide()
                    w.deleteLater()

            for idx, tab in enumerate(self.akp.tabs):
                running = self.akp.is_tab_running(tab.tab_id)
                active = idx == self.akp.active_tab_index
                prefix = "🟢 " if running else ""
                hk_badge = f" [{tab.stop_hotkey}]" if tab.stop_hotkey else ""
                btn = Button(
                    f"{prefix}{tab.title}{hk_badge}",
                    variant="primary" if active else "glass",
                    icon_name="play" if running else "",
                    parent=self.tabs_btn_container,
                )
                btn.setFixedHeight(28)
                btn.setFixedWidth(btn.naturalCompactWidth() + 8)
                btn.clicked.connect(lambda i=idx: self._select_tab(i))
                self.tabs_btn_layout.addWidget(btn)
            self.tabs_btn_layout.addStretch(1)
        finally:
            self.tabs_btn_container.setUpdatesEnabled(True)

    def _select_tab(self, index: int) -> None:
        self.akp.active_tab_index = index
        self._schedule_save_workspace()
        self._refresh_tabs_bar()
        self._load_current_tab_into_ui()

    def _load_current_tab_into_ui(self) -> None:
        self.setUpdatesEnabled(False)
        try:
            tab = self._current_tab()
            self.tab_name_edit.setText(tab.title)
            self.tog_bg.setChecked(tab.run_in_background)
            self.hk_tab_toggle.setKeyString(tab.stop_hotkey or "F6")
            self.method_combo.setSelectedKey(tab.key_method, emit_signal=False)
            self.num_hold.setValue(float(tab.hold_ms), emit_signal=False)
            self.tog_mouse_master.setChecked(tab.mouse_enabled)
            self.mouse_mode_combo.setSelectedKey(tab.mouse_mode, emit_signal=False)
            self._refresh_process_dropdown(selected_proc=tab.target_process, force_os_scan=False)
            self._rebuild_keys_grid()
            self._rebuild_mouse_points_list()
            if self._sub_view == "log":
                self._rebuild_activity_log()
            self._sync_start_button_state()
        finally:
            self.setUpdatesEnabled(True)

    def _refresh_process_dropdown(self, selected_proc: Optional[str] = None, force_os_scan: bool = False) -> None:
        tab = self._current_tab()
        sel = selected_proc if selected_proc is not None else tab.target_process
        if force_os_scan or not self._window_cache:
            self._window_cache = list_detailed_windows_for_picker()

        other_bound_map: Dict[str, List[str]] = {}
        for t_obj in self.akp.tabs:
            if t_obj.target_process and t_obj.tab_id != tab.tab_id:
                other_bound_map.setdefault(t_obj.target_process.lower(), []).append(t_obj.title)

        items: List[Tuple] = [("", t("all_apps"), "", "")]
        seen = set()
        sel_low = (sel or "").lower().strip()
        for w in self._window_cache:
            pname = w["proc_name"]
            key_low = pname.lower()
            if key_low in seen:
                continue
            seen.add(key_low)
            other_owners = other_bound_map.get(key_low, [])
            is_this_tab = bool(sel_low and key_low == sel_low)
            if other_owners and is_this_tab:
                badge = f"  [🟢 {t('akp_badge_used_in_this_tab')} + 🔶 {t('akp_badge_used_in_other_tab')}: {', '.join(other_owners)}]"
                row_tint = "#F59E0B"
            elif other_owners:
                badge = f"  [🔶 {t('akp_badge_used_in_other_tab')}: {', '.join(other_owners)}]"
                row_tint = "#F59E0B"
            elif is_this_tab:
                badge = f"  [✓ {t('akp_badge_used_in_this_tab')}]"
                row_tint = "#10B981"
            else:
                badge = ""
                row_tint = ""
            disp = f"{pname} — {w['title'][:38]} (PID: {w['pid']}){badge}"
            items.append((pname, disp, w["exe_path"], row_tint))

        if sel and sel_low not in seen:
            other_owners = other_bound_map.get(sel_low, [])
            row_tint = "#F59E0B" if other_owners else "#10B981"
            items.append((sel, sel, "", row_tint))

        self.proc_combo.setItems(items, selected_key=sel)

    def _refresh_process_dropdown_and_open(self) -> None:
        self._refresh_process_dropdown(force_os_scan=True)
        ev = QMouseEvent(
            QEvent.Type.MouseButtonRelease,
            QPointF(10, 10),
            Qt.MouseButton.LeftButton,
            Qt.MouseButton.LeftButton,
            Qt.KeyboardModifier.NoModifier,
        )
        self.proc_combo.mouseReleaseEvent(ev)

    def _on_process_selected(self, proc_name: str) -> None:
        tab = self._current_tab()
        tab.target_process = proc_name
        tab.target_hwnd = 0
        tab.target_title = ""
        for w in self._window_cache:
            if w["proc_name"].lower() == proc_name.lower():
                tab.target_hwnd = w["hwnd"]
                tab.target_title = w["title"]
                break
        self.akp.save_workspace()
        self._refresh_process_dropdown(selected_proc=proc_name)

    def _rebuild_keys_grid(self) -> None:
        tab = self._current_tab()
        self.keys_container.setUpdatesEnabled(False)
        try:
            # 1. Built-in 36 Keys strictly on 3 Rows x 12 Columns (pooled & bound in-place for <0.3ms tab switching!)
            builtin_items = [k for k in tab.keys if not k.is_custom][:36]
            cols_builtin = 12
            if len(self._builtin_tiles) != len(builtin_items):
                while self.keys_grid.count():
                    it = self.keys_grid.takeAt(0)
                    w = it.widget()
                    if w is not None:
                        w.hide()
                        w.deleteLater()
                self._builtin_tiles.clear()
                for idx, k_item in enumerate(builtin_items):
                    has_macro = self.akp.find_matching_smart_combo(k_item.key) is not None
                    tile = _KeyTileWidget(k_item, has_linked_macro=has_macro, parent=self.builtin_grid_w)
                    tile.changed.connect(self._schedule_save_workspace)
                    tile.testClicked.connect(self._on_test_single_key)
                    r = idx // cols_builtin
                    c = idx % cols_builtin
                    self.keys_grid.addWidget(tile, r, c)
                    self._builtin_tiles.append(tile)
            else:
                for tile, k_item in zip(self._builtin_tiles, builtin_items):
                    has_macro = self.akp.find_matching_smart_combo(k_item.key) is not None
                    tile.bind_item(k_item, has_linked_macro=has_macro)

            builtin_rows = max(1, (len(builtin_items) + cols_builtin - 1) // cols_builtin)
            self.builtin_grid_w.setMinimumHeight(builtin_rows * 78 + max(0, builtin_rows - 1) * 6)

            # 2. Custom Added Keys below the 3 built-in rows
            while self.custom_keys_grid.count():
                it = self.custom_keys_grid.takeAt(0)
                w = it.widget()
                if w is not None:
                    w.hide()
                    w.deleteLater()

            custom_items = [k for k in tab.keys if k.is_custom]
            self.lbl_custom_hdr.setVisible(bool(custom_items))
            self.custom_grid_w.setVisible(bool(custom_items))
            cols_custom = 8
            for idx, k_item in enumerate(custom_items):
                has_macro = self.akp.find_matching_smart_combo(k_item.key) is not None
                tile = _KeyTileWidget(k_item, has_linked_macro=has_macro, parent=self.custom_grid_w)
                tile.changed.connect(self._schedule_save_workspace)
                tile.testClicked.connect(self._on_test_single_key)
                tile.removeClicked.connect(self._on_remove_custom_key)
                r = idx // cols_custom
                c = idx % cols_custom
                self.custom_keys_grid.addWidget(tile, r, c)
            if custom_items:
                custom_rows = max(1, (len(custom_items) + cols_custom - 1) // cols_custom)
                self.custom_grid_w.setMinimumHeight(custom_rows * 78 + max(0, custom_rows - 1) * 6)
            else:
                self.custom_grid_w.setMinimumHeight(0)
        finally:
            self.keys_container.setUpdatesEnabled(True)

    def _rebuild_mouse_points_list(self) -> None:
        tab = self._current_tab()
        self.mouse_list_w.setUpdatesEnabled(False)
        try:
            while self.mouse_vbox.count():
                it = self.mouse_vbox.takeAt(0)
                w = it.widget()
                if w is not None:
                    w.hide()
                    w.deleteLater()

            for idx, mp in enumerate(tab.mouse_points):
                row = _MousePointRowWidget(
                    index=idx,
                    mp=mp,
                    is_picking=(self._picking_mouse_idx == idx),
                    parent=self.mouse_list_w,
                )
                row.changed.connect(self._schedule_save_workspace)
                row.removed.connect(self._on_remove_mouse_point)
                row.pickRequested.connect(self._on_pick_mouse_point)
                row.testRequested.connect(self._on_test_single_mouse_point)
                self.mouse_vbox.addWidget(row)
            self.mouse_vbox.addStretch(1)
        finally:
            self.mouse_list_w.setUpdatesEnabled(True)

    def _on_test_single_mouse_point(self, idx: int) -> None:
        tab = self._current_tab()
        if not (0 <= idx < len(tab.mouse_points)):
            return
        hwnd_check = self._ensure_target_window_and_focus()
        if not hwnd_check:
            return
        hwnd = resolve_target_hwnd(tab)
        mp = tab.mouse_points[idx]
        import time
        t0 = time.perf_counter()
        ok, meta_m = dispatch_autopresser_mouse_detailed(
            mp,
            mouse_mode=tab.mouse_mode,
            target_hwnd=hwnd,
            run_in_background=tab.run_in_background,
            strict_no_fallback=True,
        )
        dt_ms = max(1, int((time.perf_counter() - t0) * 1000))
        self.akp.append_tab_log(
            tab.tab_id,
            {
                "kind": "sent_mouse",
                "btn": mp.button,
                "x": mp.x,
                "y": mp.y,
                "client_x": meta_m.get("client_x", mp.x),
                "client_y": meta_m.get("client_y", mp.y),
                "child_cls": meta_m.get("child_cls", "TopWindow"),
                "dest_hwnd": meta_m.get("dest_hwnd", f"0x{hwnd:X}" if hwnd else "Foreground"),
                "reason_code": meta_m.get("reason_code", ""),
                "mode": tab.mouse_mode,
                "target_app": tab.target_process or "Active Window",
                "is_bg": tab.run_in_background,
                "dt_ms": dt_ms,
                "ok": ok,
            },
            "#06B6D4" if ok else "#EF4444",
        )
        self.state.toast_requested.emit(
            f"{'✓' if ok else '✗'} {t('log_mouse_click_at')} ({mp.button.upper()}) -> ({mp.x}, {mp.y}) [{tab.mouse_mode}]",
            "ok" if ok else "danger",
        )
        if self._sub_view == "log":
            self._rebuild_activity_log()

    def _render_log_html(self, entry: Dict) -> str:
        kind = entry.get("kind", "")
        if kind == "started":
            mode_lbl = t("log_lbl_mode_bg") if entry.get("is_bg", True) else t("log_lbl_mode_fg")
            return (
                f"<b style='color:#10B981;'>▶ {t('akp_log_started')}</b> — "
                f"{t('akp_sub_keys')}: <b>{fmt_num(entry.get('keys_count', 0))}</b> | "
                f"Mouse: <b>{fmt_num(entry.get('mouse_count', 0))}</b> | "
                f"{t('akp_lbl_method')} <b dir='ltr' style='color:#8B5CF6;'>{entry.get('method', '')}</b> "
                f"({mode_lbl})"
            )
        if kind == "stopped":
            return f"<b style='color:#F59E0B;'>⏹ {t('akp_log_stopped')}</b>"
        if kind == "no_keys_armed":
            return f"<b style='color:#F59E0B;'>⚠ {t('akp_log_no_keys')}</b>"
        if kind == "sweep_header":
            mode_lbl = t("log_lbl_mode_bg") if entry.get("is_bg", True) else t("log_lbl_mode_fg")
            return (
                f"<b style='color:#06B6D4;'>⚡ {t('akp_log_sweep_header')}</b> "
                f"<span dir='ltr'><b>{entry.get('target', '')}</b> [{entry.get('hwnd', '')}]</span> ({mode_lbl}) — "
                f"<b>{t('log_lbl_action')}</b> <span dir='ltr' style='color:#A78BFA;'><b>[{entry.get('test_key', 'F1')}]</b></span>"
            )
        if kind == "sweep_item":
            ok = bool(entry.get("ok", False))
            idx = int(entry.get("idx", 1))
            total = int(entry.get("total", 20))
            tr_k = str(entry.get("method_tr_key", ""))
            m_id = str(entry.get("method_id", ""))
            m_name = t(tr_k) if tr_k else m_id
            badge_col = "#10B981" if ok else "#EF4444"
            badge_txt = t("akp_log_sweep_item_ok") if ok else t("akp_log_sweep_item_fail")
            dt_ms = fmt_num(entry.get("dt_ms", 1))
            target_app = str(entry.get("target_app", "Active Window"))
            hwnd_str = str(entry.get("dest_hwnd", entry.get("hwnd", "Foreground")))
            child_cls = str(entry.get("child_cls", "TopWindow"))
            mode_lbl = t("log_lbl_mode_bg") if entry.get("is_bg", True) else t("log_lbl_mode_fg")
            reason_str = format_diagnostic_reason(entry)

            if entry.get("sweep_type") == "mouse":
                sx, sy = entry.get("screen_x", 0), entry.get("screen_y", 0)
                cx, cy = entry.get("client_x", sx), entry.get("client_y", sy)
                btn_u = str(entry.get("btn", "left")).upper()
                act_str = f"🖱 {btn_u} @ Screen({sx}, {sy}) &rarr; Client({cx}, {cy})"
            else:
                act_str = f"⌨ [{entry.get('key', 'F1')}]"

            line1 = (
                f"<span dir='ltr' style='color:#06B6D4; font-weight:800;'>[#{idx:02d}/{total}]</span> "
                f"<b style='color:{badge_col};'>{badge_txt}</b> — "
                f"<b>{m_name}</b> "
                f"<span dir='ltr' style='color:#3B82F6; font-weight:bold;'>({dt_ms} ms)</span>"
            )
            line2 = (
                f"<span style='color:#94A3B8; font-size:11px;'>"
                f"🎯 <b>{t('log_lbl_target')}</b> <span dir='ltr'>{target_app} [{hwnd_str} &rarr; {child_cls}]</span> ({mode_lbl}) "
                f"&nbsp;|&nbsp; ⚡ <b>{t('log_lbl_action')}</b> <span dir='ltr' style='color:#A78BFA;'>{act_str}</span> "
                f"&nbsp;|&nbsp; 📌 <b>{t('log_lbl_details')}</b> <span style='color:{badge_col};'>{reason_str}</span>"
                f"</span>"
            )
            return f"<div>{line1}<br>{line2}</div>"
        if kind == "sweep_summary":
            mode_lbl = t("log_lbl_mode_bg") if entry.get("is_bg", True) else t("log_lbl_mode_fg")
            msg = (
                t("akp_log_sweep_summary")
                .replace("{ok}", fmt_num(entry.get("ok_count", 0)))
                .replace("{fail}", fmt_num(entry.get("fail_count", 0)))
            )
            return f"<b style='color:#10B981;'>🏁 {msg}</b> — <span dir='ltr'><b>{entry.get('target', '')}</b></span> ({mode_lbl})"
        if kind == "triggered_smart_macro":
            return (
                f"<b style='color:#F59E0B;'>{t('akp_log_triggered_smart_macro')}</b> "
                f"<b dir='ltr'>[ {entry.get('key', '')} ] &rarr; {entry.get('combo_name', '')}</b> "
                f"<span dir='ltr'>({fmt_num(entry.get('interval_ms', 1000))} ms)</span>"
            )
        if kind == "window_required_warn":
            return f"<b style='color:#EF4444;'>{t('akp_err_must_select_window')}</b>"
        if kind == "sent_key":
            ok = bool(entry.get("ok", True))
            badge_col = "#10B981" if ok else "#EF4444"
            m_id = str(entry.get("method", ""))
            dt_ms = fmt_num(entry.get("dt_ms", 1))
            return (
                f"⌨ {t('akp_log_sent_key')} <b dir='ltr' style='color:#8B5CF6;'>[ {entry.get('key', '')} ]</b> "
                f"<span dir='ltr'>({fmt_num(entry.get('interval_ms', 1000))} ms | {dt_ms} ms)</span> — "
                f"{t('log_actual_engine')} <b dir='ltr' style='color:{badge_col};'>{m_id}</b>"
            )
        if kind == "sent_mouse":
            ok = bool(entry.get("ok", True))
            badge_col = "#10B981" if ok else "#EF4444"
            sx, sy = entry.get("x", 0), entry.get("y", 0)
            cx, cy = entry.get("client_x", sx), entry.get("client_y", sy)
            child_cls = entry.get("child_cls", "TopWindow")
            dt_ms = fmt_num(entry.get("dt_ms", 1))
            reason_str = format_diagnostic_reason(entry) if entry.get("reason_code") else ""
            extra_r = f" — <span style='color:{badge_col};'>{reason_str}</span>" if reason_str else ""
            return (
                f"🖱 {t('log_mouse_click_at')} <b dir='ltr' style='color:#06B6D4;'>({str(entry.get('btn', 'left')).upper()})</b> "
                f"{t('log_at_coords')} <b dir='ltr'>Screen({sx}, {sy}) &rarr; Client({cx}, {cy}) [{child_cls}]</b> — "
                f"<b dir='ltr' style='color:{badge_col};'>{entry.get('mode', '')}</b> <span dir='ltr'>({dt_ms} ms)</span>{extra_r}"
            )
        return str(entry.get("text", ""))

    def _compute_log_sig(self, tab_id: str) -> Tuple[str, int, str]:
        logs = self.akp.get_tab_logs(tab_id)
        top_ts = f"{logs[0].get('ts', '')}_{logs[0].get('kind', '')}_{logs[0].get('method_id', '')}" if logs else ""
        return (tab_id, len(logs), top_ts)

    def _rebuild_activity_log(self) -> None:
        tab = self._current_tab()
        self._last_log_sig = self._compute_log_sig(tab.tab_id)
        while self.log_vbox.count():
            it = self.log_vbox.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.deleteLater()

        logs = self.akp.get_tab_logs(tab.tab_id)
        for idx, item in enumerate(logs[:80]):
            accent_hex = str(item.get("accent", "#8B5CF6"))
            card = StepCard(index=idx, accent_hex=accent_hex, parent=self.log_list_w)
            lay = QHBoxLayout(card)
            lay.setContentsMargins(12, 5, 12, 5)
            lay.setSpacing(10)

            lbl_ts = QLabel(str(item.get("ts", "")))
            lbl_ts.setFont(make_font(11, QFont.Weight.Bold))
            lbl_ts.setStyleSheet(f"color: {accent_hex}; background: transparent;")

            lbl_msg = QLabel(self._render_log_html(item))
            lbl_msg.setTextFormat(Qt.TextFormat.RichText)
            lbl_msg.setWordWrap(True)
            lbl_msg.setFont(make_font(12, QFont.Weight.DemiBold))
            lbl_msg.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

            lay.addWidget(lbl_ts)
            lay.addWidget(lbl_msg, 1)
            self.log_vbox.addWidget(card)
        self.log_vbox.addStretch(1)

    def _on_sub_view_changed(self, view_key: str) -> None:
        self._sub_view = view_key
        self.keys_scroll.setVisible(view_key == "keys")
        self.mouse_panel.setVisible(view_key == "mouse")
        self.log_panel.setVisible(view_key == "log")
        if view_key == "log":
            self._rebuild_activity_log()

    def _on_tab_name_edited(self, txt: str) -> None:
        tab = self._current_tab()
        tab.title = txt.strip() or f"Tab {self.akp.active_tab_index + 1}"
        self.akp.save_workspace()
        self._refresh_tabs_bar()

    def _on_add_tab(self) -> None:
        self.akp.add_new_tab(save_immediately=False)
        self._schedule_save_workspace()
        self._refresh_tabs_bar()
        self._load_current_tab_into_ui()
        QTimer.singleShot(25, lambda: self.tabs_scroll.horizontalScrollBar().setValue(self.tabs_scroll.horizontalScrollBar().maximum()))

    def _on_dup_tab(self) -> None:
        self.akp.add_new_tab(duplicate_from=self.akp.active_tab_index, save_immediately=False)
        self._schedule_save_workspace()
        self._refresh_tabs_bar()
        self._load_current_tab_into_ui()
        QTimer.singleShot(25, lambda: self.tabs_scroll.horizontalScrollBar().setValue(self.tabs_scroll.horizontalScrollBar().maximum()))

    def _on_close_current_tab(self) -> None:
        if len(self.akp.tabs) <= 1:
            return
        self.akp.close_tab(self.akp.active_tab_index, save_immediately=False)
        self._schedule_save_workspace()
        self._refresh_tabs_bar()
        self._load_current_tab_into_ui()

    def _on_bg_toggled(self, val: bool) -> None:
        tab = self._current_tab()
        tab.run_in_background = val
        self.akp.save_workspace()

    def _on_method_changed(self, method_key: str) -> None:
        tab = self._current_tab()
        tab.key_method = method_key
        self.akp.save_workspace()

    def _on_hold_changed(self, val: float) -> None:
        tab = self._current_tab()
        tab.hold_ms = max(10, int(val))
        self.akp.save_workspace()

    def _ensure_target_window_and_focus(self, focus_window: Optional[bool] = None) -> int:
        """
        Verify that the user has selected a specific target window (NOT 'All Applications')
        and resolve its live HWND.
        - If `focus_window` is None, it defaults to `not tab.run_in_background`, so when
          `Run in background` (`تشغيل في الخلفية`) is ON, testing/sweeping NEVER steals focus!
        - When clicking `Switch` (`إنتقال`), `focus_window=True` is passed explicitly.
        """
        tab = self._current_tab()
        if not (tab.target_process or "").strip():
            self.akp.append_tab_log(tab.tab_id, {"kind": "window_required_warn"}, "#EF4444")
            self.state.toast_requested.emit(t("akp_err_must_select_window"), "danger")
            if self._sub_view == "log":
                self._rebuild_activity_log()
            return 0

        hwnd = resolve_target_hwnd(tab)
        if IS_WINDOWS and hwnd <= 0:
            self.state.toast_requested.emit(t("akp_err_window_not_found"), "danger")
            return 0

        should_focus = (not tab.run_in_background) if focus_window is None else bool(focus_window)
        if hwnd > 0 and should_focus:
            focus_and_bring_hwnd_to_front(hwnd)
        return hwnd if hwnd > 0 else 1

    def _on_switch_to_target_window(self) -> None:
        """Immediately restore and bring the selected target window to the foreground when clicking `Switch`."""
        self._ensure_target_window_and_focus(focus_window=True)

    def _on_test_active_method(self) -> None:
        tab = self._current_tab()
        armed = [k.key for k in tab.keys if k.enabled]
        test_key = armed[0] if armed else "F1"
        self._on_test_single_key(test_key)

    def _on_sweep_all_key_methods(self) -> None:
        if self._sweep_running:
            return
        tab = self._current_tab()
        if not self._ensure_target_window_and_focus():
            return
        armed = [k.key for k in tab.keys if k.enabled]
        test_key = armed[0] if armed else "F1"
        self._sweep_running = True
        self.btn_sweep_all.setBusy(True)
        self.sub_seg.setSelectedKey("log", emit_signal=True)
        self._on_sub_view_changed("log")

        import threading

        def _bg_sweep() -> None:
            try:
                ok_c, fail_c, _ = self.akp.sweep_all_key_methods(tab, test_key=test_key)
                summary_msg = (
                    t("akp_log_sweep_summary")
                    .replace("{ok}", fmt_num(ok_c))
                    .replace("{fail}", fmt_num(fail_c))
                )
                self.state.toast_requested.emit(summary_msg, "ok" if ok_c > 0 else "info")
            finally:
                self._sweep_running = False
                self.btn_sweep_all.setBusy(False)

        threading.Thread(target=_bg_sweep, daemon=True).start()

    def _on_sweep_all_mouse_modes(self) -> None:
        if self._sweep_running:
            return
        tab = self._current_tab()
        if not self._ensure_target_window_and_focus():
            return
        self._sweep_running = True
        self.btn_sweep_mouse.setBusy(True)
        self.sub_seg.setSelectedKey("log", emit_signal=True)
        self._on_sub_view_changed("log")

        import threading

        def _bg_ms_sweep() -> None:
            try:
                ok_c, fail_c, _ = self.akp.sweep_all_mouse_modes(tab)
                summary_msg = (
                    t("akp_log_sweep_summary")
                    .replace("{ok}", fmt_num(ok_c))
                    .replace("{fail}", fmt_num(fail_c))
                )
                self.state.toast_requested.emit(summary_msg, "ok" if ok_c > 0 else "info")
            finally:
                self._sweep_running = False
                self.btn_sweep_mouse.setBusy(False)

        threading.Thread(target=_bg_ms_sweep, daemon=True).start()

    def _on_test_single_key(self, key_label: str) -> None:
        tab = self._current_tab()
        hwnd_check = self._ensure_target_window_and_focus()
        if not hwnd_check:
            return
        hwnd = resolve_target_hwnd(tab)
        k_item = next((k for k in tab.keys if k.key.lower() == key_label.lower()), None)
        if k_item is not None and getattr(k_item, "use_smart_combo", False):
            matched_combo = self.akp.find_matching_smart_combo(key_label)
            if matched_combo is not None and self.macro_engine is not None:
                self.macro_engine.execute_smart_combo(matched_combo)
                self.akp.append_tab_log(
                    tab.tab_id,
                    {
                        "kind": "triggered_smart_macro",
                        "key": key_label,
                        "combo_name": str(matched_combo.get("name") or key_label),
                        "interval_ms": k_item.interval_ms,
                    },
                    "#F59E0B",
                )
                self.state.toast_requested.emit(
                    f"{t('akp_log_triggered_smart_macro')} [{key_label}]",
                    "ok",
                )
                if self._sub_view == "log":
                    self._rebuild_activity_log()
                return

        ok, actual_m = dispatch_autopresser_key(
            key_label,
            method=tab.key_method,
            hold_ms=tab.hold_ms,
            target_hwnd=hwnd,
            run_in_background=tab.run_in_background,
            strict_no_fallback=True,
        )
        self.akp.append_tab_log(
            tab.tab_id,
            {"kind": "sent_key", "key": key_label, "interval_ms": tab.hold_ms, "method": actual_m, "ok": ok},
            "#10B981" if ok else "#EF4444",
        )
        self.state.toast_requested.emit(f"✓ {key_label} -> {actual_m}", "ok" if ok else "danger")
        if self._sub_view == "log":
            self._rebuild_activity_log()

    def _on_custom_key_captured(self, key_str: str) -> None:
        clean = key_str.strip()
        if not clean:
            return
        tab = self._current_tab()
        existing = next((k for k in tab.keys if k.key.lower() == clean.lower()), None)
        has_smart = self.akp.find_matching_smart_combo(clean) is not None
        if existing is not None:
            existing.enabled = True
            if has_smart:
                existing.use_smart_combo = True
        else:
            tab.keys.append(
                AutoPresserKeyItem(
                    key=clean,
                    enabled=True,
                    interval_ms=1000,
                    is_custom=True,
                    use_smart_combo=has_smart,
                )
            )
        self.akp.save_workspace()
        self._rebuild_keys_grid()
        QTimer.singleShot(40, lambda: self.keys_scroll.verticalScrollBar().setValue(self.keys_scroll.verticalScrollBar().maximum()))
        self.state.toast_requested.emit(f"{t('akp_toast_custom_key_added')} [ {clean} ]", "ok")

    def _on_remove_custom_key(self, key_str: str) -> None:
        tab = self._current_tab()
        tab.keys = [k for k in tab.keys if not (k.is_custom and k.key == key_str)]
        self.akp.save_workspace()
        self._rebuild_keys_grid()

    def _set_all_keys_enabled(self, enabled: bool) -> None:
        tab = self._current_tab()
        for k in tab.keys:
            k.enabled = enabled
        self.akp.save_workspace()
        self._rebuild_keys_grid()

    def _on_mouse_master_toggled(self, val: bool) -> None:
        tab = self._current_tab()
        tab.mouse_enabled = val
        self.akp.save_workspace()

    def _on_mouse_mode_changed(self, mode_key: str) -> None:
        tab = self._current_tab()
        tab.mouse_mode = mode_key
        self.akp.save_workspace()

    def _on_add_mouse_point(self) -> None:
        tab = self._current_tab()
        from app.domain.win32_input import get_cursor_pos
        cx, cy = get_cursor_pos()
        tab.mouse_points.append(
            AutoPresserMousePoint(
                name=f"Click {len(tab.mouse_points) + 1}",
                x=cx,
                y=cy,
                button="left",
                interval_ms=1000,
                enabled=True,
            )
        )
        tab.mouse_enabled = True
        self.tog_mouse_master.setChecked(True)
        self.akp.save_workspace()
        self._rebuild_mouse_points_list()

    def _on_remove_mouse_point(self, idx: int) -> None:
        tab = self._current_tab()
        if 0 <= idx < len(tab.mouse_points):
            tab.mouse_points.pop(idx)
            self.akp.save_workspace()
            self._rebuild_mouse_points_list()

    def _on_pick_mouse_point(self, idx: int) -> None:
        self._picking_mouse_idx = -1 if self._picking_mouse_idx == idx else idx
        self._rebuild_mouse_points_list()

    def _on_f8_captured(self, x: int, y: int) -> None:
        tab = self._current_tab()
        if self._sub_view == "mouse":
            if 0 <= self._picking_mouse_idx < len(tab.mouse_points):
                tab.mouse_points[self._picking_mouse_idx].x = x
                tab.mouse_points[self._picking_mouse_idx].y = y
                self._picking_mouse_idx = -1
            else:
                tab.mouse_points.append(
                    AutoPresserMousePoint(
                        name=f"Click {len(tab.mouse_points) + 1}",
                        x=x,
                        y=y,
                        button="left",
                        interval_ms=1000,
                        enabled=True,
                    )
                )
                tab.mouse_enabled = True
                self.tog_mouse_master.setChecked(True)
            self.akp.save_workspace()
            self._rebuild_mouse_points_list()

    def _on_toggle_current_tab(self) -> None:
        tab = self._current_tab()
        running = self.akp.toggle_tab(tab)
        if not running and not any(k.enabled for k in tab.keys) and not (tab.mouse_enabled and any(m.enabled for m in tab.mouse_points)):
            self.state.toast_requested.emit(t("akp_log_no_keys"), "info")
        self._sync_start_button_state()
        self._refresh_tabs_bar()

    def _on_stop_all_tabs(self) -> None:
        self.akp.stop_all_tabs()
        self._sync_start_button_state()
        self._refresh_tabs_bar()

    def _sync_start_button_state(self) -> None:
        tab = self._current_tab()
        running = self.akp.is_tab_running(tab.tab_id)
        self.btn_start_tab.setText(t("akp_btn_stop_tab") if running else t("akp_btn_start_tab"))
        self.btn_start_tab.setVariant("danger" if running else "primary")
        self.btn_start_tab.setIconName("stop" if running else "play")

    def _tick_ui_status(self) -> None:
        self._sync_start_button_state()
        if self._sub_view == "log":
            tab = self._current_tab()
            if self._compute_log_sig(tab.tab_id) != self._last_log_sig:
                self._rebuild_activity_log()

    def _on_clear_tab_log(self) -> None:
        tab = self._current_tab()
        self.akp.clear_tab_logs(tab.tab_id)
        self._rebuild_activity_log()

    def _on_save_workspace(self) -> None:
        path, _ = QFileDialog.getSaveFileName(
            self,
            t("akp_btn_save_workspace"),
            "V-AutoKeyPresser_Workspace.json",
            "JSON Workspace (*.json)",
        )
        if path and self.akp.save_workspace(path):
            self.state.toast_requested.emit(t("toast_backup_exported"), "ok")

    def _on_load_workspace(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self,
            t("akp_btn_load_workspace"),
            "",
            "JSON Workspace (*.json)",
        )
        if path and self.akp.load_workspace(path):
            self.akp.save_workspace()
            self._refresh_tabs_bar()
            self._load_current_tab_into_ui()
            self.state.toast_requested.emit(t("toast_backup_imported"), "ok")

    def _on_save_tab_profile(self) -> None:
        tab = self._current_tab()
        path, _ = QFileDialog.getSaveFileName(
            self,
            t("akp_btn_save_profile"),
            f"{tab.title.replace(' ', '_')}_Profile.json",
            "JSON Tab Profile (*.json)",
        )
        if path and self.akp.save_tab_profile(tab, path):
            self.state.toast_requested.emit(t("toast_backup_exported"), "ok")

    def _on_load_tab_profile(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self,
            t("akp_btn_load_profile"),
            "",
            "JSON Tab Profile (*.json)",
        )
        if path and self.akp.load_tab_profile(self.akp.active_tab_index, path):
            self._refresh_tabs_bar()
            self._load_current_tab_into_ui()
            self.state.toast_requested.emit(t("toast_backup_imported"), "ok")
