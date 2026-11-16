# -*- coding: utf-8 -*-
"""
Unified Recordings Page («التسجيلات») for V-MacroPlus v3.2.0:
1. Native Windows Mouse + Keyboard Recording (`WH_MOUSE_LL` + `WH_KEYBOARD_LL`):
   Captures Keyboard, all 5 Mouse Buttons (`left`, `right`, `middle`, `x1` Back, `x2` Forward),
   4-Way Scroll Wheel (`down`, `up`, `left`, `right` with notch count), and Cursor Path natively!
   Automatically ignores and strips any mouse click on the Stop Recording buttons (`btn_rec` or floating pill)
   and any key presses belonging to the Stop Recording Hotkey (`hotkey_record_toggle`).
2. Instant Stop Playback via Hotkey (`hotkey_playback_toggle` e.g. `F10` or `hotkey_panic_stop` e.g. `Esc`):
   Includes a direct `زرار إيقاف التشغيل الفوري:` (`KeyCaptureButton`) right inside the Repeat Panel so the user
   can stop a 10-minute (or any duration/count) recording repeat at any second using their chosen Hotkey!
3. Unified Key Press (`+ ضغطة زرار`) with integrated Hold Duration (`مدة الضغط: ms`) — redundant `+ ضغطة مطوّلة`
   button removed since `+ ضغطة زرار` already calculates and edits hold duration!
"""

from __future__ import annotations
import json
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple
from PySide6.QtCore import QPoint, Qt, QTimer, Signal
from PySide6.QtGui import QFont, QMouseEvent
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
from app.domain.macro_engine import MacroEngine
from app.domain.models import MacroEvent
from app.domain.win32_input import get_cursor_pos, scancode_for_label
from app.i18n import fmt_num, get_lang_font_family, ltr_value, t, tl
from app.state import AppState
from app.theme import T
from app.ui.pages.combos_page import get_mouse_button_options, get_scroll_dir_options
from app.ui.widgets import (
    Button,
    DragGripHandle,
    GlassCard,
    IconButton,
    KeyCaptureButton,
    NumberInput,
    ResponsiveFlowBar,
    SearchCombo,
    SegmentedControl,
    StepCard,
    ToggleSwitch,
    make_font,
    modern_scroll_style,
)

REC_CATEGORY_COLORS = {
    "key_tap": "#8B5CF6",
    "key_hold": "#8B5CF6",
    "key_down": "#7C3AED",
    "key_up": "#C084FC",
    "wait": "#F59E0B",
    "mouse_click": "#10B981",
    "mouse_down": "#059669",
    "mouse_up": "#34D399",
    "mouse_move": "#06B6D4",
    "mouse_scroll": "#EC4899",
}


class _EditableEventRow(QWidget):
    """
    Alternating Two-Tone Editable Timeline Row (`StepCard`) with:
    - Live Mouse Drag-and-Drop Reordering (`DragGripHandle` + direct row dragging)
    - Hold Duration (`hold_ms`) on `key_tap` and `mouse_click`
    - All 5 Mouse Buttons (`left`, `right`, `middle`, `x1` Back, `x2` Forward) + `Pick (F8)` + Live `(X, Y)`
    - 4-Way Mouse Scroll (`down`, `up`, `left`, `right` + Scroll Amount)
    """

    deleteClicked = Signal(int)
    dragMoveRequested = Signal(object, int)
    dragFinished = Signal()

    def __init__(
        self,
        index: int,
        ev: Dict[str, Any],
        state: AppState,
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self.index = index
        self.ev = ev
        self.state = state
        self._picking_live_xy = False
        self._row_dragging = False

        kind = ev.get("kind", "key_tap")
        self.cat_hex = REC_CATEGORY_COLORS.get(kind, "#8B5CF6")

        self.card = StepCard(index=self.index, accent_hex=self.cat_hex, parent=self)
        outer = QVBoxLayout(self)
        outer.setContentsMargins(0, 0, 0, 0)
        outer.addWidget(self.card)

        lay = QHBoxLayout(self.card)
        lay.setContentsMargins(10, 6, 10, 6)
        lay.setSpacing(6)

        self.grip = DragGripHandle(self.card)
        self.grip.dragStarted.connect(self._on_drag_started)
        self.grip.dragMoved.connect(lambda gy: self.dragMoveRequested.emit(self, gy))
        self.grip.dragFinished.connect(self._on_drag_finished)
        lay.addWidget(self.grip)

        self.idx_lbl = QLabel(f"#{fmt_num(index + 1)}")
        self.idx_lbl.setFont(make_font(12, QFont.Weight.ExtraBold))
        self.idx_lbl.setStyleSheet(f"color: {self.cat_hex}; background: transparent;")
        lay.addWidget(self.idx_lbl)

        kind_labels = {
            "key_tap": t("step_row_key_tap"),
            "key_hold": t("step_row_key_tap"),
            "key_down": t("step_row_key_down"),
            "key_up": t("step_row_key_up"),
            "wait": t("step_row_wait"),
            "mouse_click": t("step_row_mouse_click"),
            "mouse_down": t("step_row_mouse_down"),
            "mouse_up": t("step_row_mouse_up"),
            "mouse_move": t("step_row_mouse_move"),
            "mouse_scroll": t("lbl_scroll_dir"),
        }
        k_lbl = QLabel(kind_labels.get(kind, kind))
        k_lbl.setFont(make_font(12, QFont.Weight.Bold))
        k_lbl.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        lay.addWidget(k_lbl)

        if kind in ("key_tap", "key_hold", "key_down", "key_up"):
            self.key_btn = KeyCaptureButton(
                current_key=str(ev.get("key", "Space")),
                scan_code=int(ev.get("scan_code", 0)),
                is_extended=bool(ev.get("is_extended", False)),
                allow_chord=False,
                state=self.state,
            )
            self.key_btn.setFixedSize(112, 32)
            self.key_btn.scanKeyCaptured.connect(self._on_key_edited)
            lay.addWidget(self.key_btn)

            if kind in ("key_tap", "key_hold"):
                lbl_hold = QLabel(t("lbl_hold_duration"))
                lbl_hold.setFont(make_font(11, QFont.Weight.Bold))
                lbl_hold.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
                default_hold = int(ev.get("hold_ms", 0))
                ev["hold_ms"] = default_hold
                self.num_hold = NumberInput(
                    value=float(default_hold),
                    min_val=0.0,
                    max_val=60000.0,
                    step=100.0,
                    decimals=0,
                    suffix="ms",
                )
                self.num_hold.setFixedSize(108, 32)
                self.num_hold.valueChanged.connect(lambda v: self._set_field("hold_ms", int(v)))
                lay.addWidget(lbl_hold)
                lay.addWidget(self.num_hold)

        elif kind == "mouse_scroll":
            s_dir = str(ev.get("scroll_dir", ""))
            if not s_dir:
                dy = int(ev.get("dy", 0))
                dx = int(ev.get("dx", 0))
                if dy != 0:
                    s_dir = "up" if dy > 0 else "down"
                elif dx != 0:
                    s_dir = "right" if dx > 0 else "left"
                else:
                    s_dir = "down"
                ev["scroll_dir"] = s_dir

            self.scroll_dir_combo = SearchCombo(
                get_scroll_dir_options(),
                selected_key=s_dir,
            )
            self.scroll_dir_combo.setFixedSize(118, 32)
            self.scroll_dir_combo.currentChanged.connect(lambda d: self._set_field("scroll_dir", d))
            lay.addWidget(self.scroll_dir_combo)

            lbl_amt = QLabel(t("lbl_scroll_amount"))
            lbl_amt.setFont(make_font(12, QFont.Weight.Bold))
            lbl_amt.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            amt_val = int(ev.get("scroll_amount", 0)) or max(1, abs(int(ev.get("dy", 0))) or abs(int(ev.get("dx", 0))) or 3)
            ev["scroll_amount"] = amt_val
            self.num_scroll_amt = NumberInput(
                value=float(amt_val),
                min_val=1.0,
                max_val=100.0,
                step=1.0,
                decimals=0,
                suffix="",
            )
            self.num_scroll_amt.setFixedSize(98, 32)
            self.num_scroll_amt.valueChanged.connect(lambda v: self._set_field("scroll_amount", max(1, int(v))))
            lay.addWidget(lbl_amt)
            lay.addWidget(self.num_scroll_amt)

        elif "mouse" in kind:
            if kind in ("mouse_click", "mouse_down", "mouse_up"):
                self.btn_combo = SearchCombo(
                    get_mouse_button_options(),
                    selected_key=str(ev.get("button", "left")),
                )
                self.btn_combo.setFixedSize(122, 32)
                self.btn_combo.currentChanged.connect(lambda b: self._set_field("button", b))
                lay.addWidget(self.btn_combo)

            x_lbl = QLabel("X:")
            x_lbl.setFont(make_font(11, QFont.Weight.Bold))
            x_lbl.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            self.num_x = NumberInput(float(ev.get("x", 500)), -32768.0, 32768.0, 10, 0, "")
            self.num_x.setFixedSize(88, 32)
            self.num_x.valueChanged.connect(lambda v: self._set_field("x", int(v)))

            y_lbl = QLabel("Y:")
            y_lbl.setFont(make_font(11, QFont.Weight.Bold))
            y_lbl.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            self.num_y = NumberInput(float(ev.get("y", 500)), -32768.0, 32768.0, 10, 0, "")
            self.num_y.setFixedSize(88, 32)
            self.num_y.valueChanged.connect(lambda v: self._set_field("y", int(v)))

            self.btn_pick_xy = Button(
                t("btn_pick_live_xy"),
                variant="glass",
                icon_name="crosshair",
                tint_hex="#10B981",
            )
            self.btn_pick_xy.setFixedHeight(30)
            self.btn_pick_xy.clicked.connect(self._toggle_live_xy_picker)

            self.lbl_live_xy = QLabel("")
            self.lbl_live_xy.setFont(make_font(11, QFont.Weight.Bold))
            self.lbl_live_xy.setStyleSheet(f"color: {T.token_hex('ok')}; background: transparent;")

            lay.addWidget(x_lbl)
            lay.addWidget(self.num_x)
            lay.addWidget(y_lbl)
            lay.addWidget(self.num_y)
            lay.addWidget(self.btn_pick_xy)
            lay.addWidget(self.lbl_live_xy)

            if kind == "mouse_click":
                lbl_hold = QLabel(t("lbl_hold_duration"))
                lbl_hold.setFont(make_font(11, QFont.Weight.Bold))
                lbl_hold.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
                self.num_hold = NumberInput(
                    value=float(ev.get("hold_ms", 0)),
                    min_val=0.0,
                    max_val=60000.0,
                    step=100.0,
                    decimals=0,
                    suffix="ms",
                )
                self.num_hold.setFixedSize(102, 32)
                self.num_hold.valueChanged.connect(lambda v: self._set_field("hold_ms", int(v)))
                lay.addWidget(lbl_hold)
                lay.addWidget(self.num_hold)

            self._xy_timer = QTimer(self)
            self._xy_timer.setInterval(50)
            self._xy_timer.timeout.connect(self._tick_live_xy)
            self.num_x.focused.connect(self._start_live_xy_preview)
            self.num_y.focused.connect(self._start_live_xy_preview)
            self.state.cursor_pick_hotkey_pressed.connect(self._on_f8_picked)

        lay.addStretch(1)

        d_lbl = QLabel(t("lbl_pre_delay"))
        d_lbl.setFont(make_font(11, QFont.Weight.Medium))
        d_lbl.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        lay.addWidget(d_lbl)

        self.num_delay = NumberInput(
            value=float(ev.get("delay_ms", 40)),
            min_val=1.0,
            max_val=600000.0,
            step=25.0,
            decimals=0,
            suffix="ms",
        )
        self.num_delay.setFixedSize(112, 32)
        self.num_delay.valueChanged.connect(lambda v: self._set_field("delay_ms", int(v)))
        lay.addWidget(self.num_delay)

        btn_del = IconButton("trash", size=28, variant="danger", tooltip=t("btn_delete_rec"))
        btn_del.clicked.connect(lambda: self.deleteClicked.emit(self.index))
        lay.addWidget(btn_del)

    def set_step_index(self, new_idx: int) -> None:
        self.index = new_idx
        self.idx_lbl.setText(f"#{fmt_num(new_idx + 1)}")
        self.card.setIndex(new_idx)

    def _on_drag_started(self) -> None:
        self.card.setDragging(True)

    def _on_drag_finished(self) -> None:
        self.card.setDragging(False)
        self.dragFinished.emit()

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            self._row_dragging = True
            self.setCursor(Qt.CursorShape.ClosedHandCursor)
            self.card.setDragging(True)
            event.accept()
            return
        super().mousePressEvent(event)

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        if self._row_dragging:
            self.dragMoveRequested.emit(self, int(event.globalPosition().y()))
            event.accept()
            return
        super().mouseMoveEvent(event)

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if self._row_dragging:
            self._row_dragging = False
            self.setCursor(Qt.CursorShape.ArrowCursor)
            self.card.setDragging(False)
            self.dragFinished.emit()
            event.accept()
            return
        super().mouseReleaseEvent(event)

    def _start_live_xy_preview(self) -> None:
        self._picking_live_xy = True
        if hasattr(self, "_xy_timer") and not self._xy_timer.isActive():
            self._xy_timer.start()

    def _toggle_live_xy_picker(self) -> None:
        self._picking_live_xy = not self._picking_live_xy
        if self._picking_live_xy:
            self._xy_timer.start()
            self.btn_pick_xy.setText(t("btn_press_f8"))
        else:
            self._xy_timer.stop()
            self.btn_pick_xy.setText(t("btn_pick_live_xy"))
            self.lbl_live_xy.setText("")

    def _tick_live_xy(self) -> None:
        cx, cy = get_cursor_pos()
        self.lbl_live_xy.setText(ltr_value(f"({cx}, {cy})"))

    def _on_f8_picked(self, cx: int, cy: int) -> None:
        if not self._picking_live_xy:
            return
        self.num_x.setValue(float(cx), emit_signal=True)
        self.num_y.setValue(float(cy), emit_signal=True)
        self._picking_live_xy = False
        self._xy_timer.stop()
        self.btn_pick_xy.setText(t("btn_pick_live_xy"))
        self.lbl_live_xy.setText(ltr_value(f"✓ ({cx},{cy})"))

    def _on_key_edited(self, sc: int, ext: bool, lbl: str) -> None:
        self.ev["key"] = lbl
        self.ev["scan_code"] = sc
        self.ev["is_extended"] = ext

    def _set_field(self, field_name: str, val: Any) -> None:
        self.ev[field_name] = val


class RecorderPage(QWidget):
    """
    Merged Recorder + Saved Recordings Library with Native Win32 Mouse & Keyboard Capture,
    Unified Key Press + Hold Duration, 5 Mouse Buttons, 4-Way Scroll Wheel, Instant Stop Playback Hotkey,
    and 100% 1-to-1 Smart Key Conversion!
    """

    def __init__(self, state: AppState, engine: MacroEngine, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.state = state
        self.engine = engine
        self._last_deleted_macro: Optional[Dict[str, Any]] = None
        self._event_rows: List[_EditableEventRow] = []
        self._sidebar_collapsed: bool = bool(getattr(self.state, 'recorder_sidebar_collapsed', False))
        self._build_ui()
        self._connect_signals()
        self.state.register_stop_button_rect_provider(self._get_stop_btn_screen_rect)
        self._refresh_timeline()
        self._refresh_saved_library()

    def _get_stop_btn_screen_rect(self) -> Optional[Tuple[int, int, int, int]]:
        if not hasattr(self, "btn_rec") or not self.btn_rec.isVisible():
            return None
        tl_pt = self.btn_rec.mapToGlobal(QPoint(0, 0))
        return (
            tl_pt.x(),
            tl_pt.y(),
            tl_pt.x() + self.btn_rec.width(),
            tl_pt.y() + self.btn_rec.height(),
        )

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(16, 8, 16, 12)
        root.setSpacing(10)

        top_card = GlassCard(accent_tint=True, parent=self)
        t_lay = QHBoxLayout(top_card)
        t_lay.setContentsMargins(18, 10, 18, 10)
        t_lay.setSpacing(10)

        t_col = QVBoxLayout()
        t_col.setSpacing(2)
        title = QLabel(t("rec_title"))
        title.setFont(make_font(16, QFont.Weight.ExtraBold))
        title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        sub = QLabel(t("rec_subtitle"))
        sub.setFont(make_font(12, QFont.Weight.Medium))
        sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        t_col.addWidget(title)
        t_col.addWidget(sub)
        t_lay.addLayout(t_col, 1)

        hk_rec = self.state.config.hotkey_record_toggle or "F9"
        self.btn_rec = Button(f"{t('btn_start_rec')} ({hk_rec})", variant="danger", icon_name="record")
        self.btn_rec.clicked.connect(lambda: self.engine.toggle_recording(via_mouse=True))

        hk_play = self.state.config.hotkey_playback_toggle or "F10"
        self.btn_play = Button(f"{t('btn_play_macro')} ({hk_play})", variant="primary", icon_name="play")
        self.btn_play.clicked.connect(self._on_play_clicked)

        self.btn_to_combo = Button(t("btn_convert_to_combo"), variant="glass", icon_name="bolt", tint_hex="#8B5CF6")
        self.btn_to_combo.clicked.connect(self._convert_current_to_combo)

        self.btn_clear = Button(t("btn_clear_events"), variant="ghost", icon_name="trash")
        self.btn_clear.clicked.connect(self._clear_events)

        self.btn_toggle_sidebar = Button(
            t("btn_show_macros_sidebar") if self._sidebar_collapsed else t("btn_hide_macros_sidebar"),
            variant="glass",
            icon_name="dashboard",
            tint_hex=T.token_hex("accent"),
        )
        self.btn_toggle_sidebar.clicked.connect(self._toggle_rec_sidebar)

        t_lay.addWidget(self.btn_rec)
        t_lay.addWidget(self.btn_play)
        t_lay.addWidget(self.btn_to_combo)
        t_lay.addWidget(self.btn_clear)
        t_lay.addWidget(self.btn_toggle_sidebar)
        root.addWidget(top_card)

        body = QHBoxLayout()
        body.setSpacing(12)

        # Left Column
        lib_card = GlassCard(parent=self)
        self.lib_card = lib_card
        lib_card.setFixedWidth(315)
        lib_card.setVisible(not self._sidebar_collapsed)
        l_lay = QVBoxLayout(lib_card)
        l_lay.setContentsMargins(12, 12, 12, 12)
        l_lay.setSpacing(8)

        lib_hdr = QLabel(t("saved_recordings_header"))
        lib_hdr.setFont(make_font(14, QFont.Weight.ExtraBold))
        lib_hdr.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        l_lay.addWidget(lib_hdr)

        save_row = QHBoxLayout()
        save_row.setSpacing(6)
        self.name_edit = QLineEdit(self.state.current_macro_name)
        self.name_edit.setPlaceholderText(t("ph_macro_name"))
        self.name_edit.setFixedHeight(34)
        self.name_edit.setStyleSheet(
            f"""
            QLineEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('text')};
                border: 1px solid {T.token_hex('border_hi')};
                border-radius: 9px;
                padding: 4px 10px;
                font-family: '{get_lang_font_family()}';
                font-size: 13px;
                font-weight: 600;
            }}
            """
        )
        btn_save = Button(t("btn_save_macro"), variant="primary", icon_name="save", fixed_compact=True)
        btn_save.setFixedHeight(34)
        btn_save.clicked.connect(lambda: self.engine.save_current_macro_to_library(self.name_edit.text()))
        save_row.addWidget(self.name_edit, 1)
        save_row.addWidget(btn_save, 1)
        l_lay.addLayout(save_row)

        lib_scroll = QScrollArea()
        lib_scroll.setWidgetResizable(True)
        lib_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        lib_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        lib_scroll.setStyleSheet(modern_scroll_style())
        self.lib_container = QWidget()
        self.lib_container.setStyleSheet("background: transparent;")
        self.lib_vbox = QVBoxLayout(self.lib_container)
        self.lib_vbox.setContentsMargins(0, 0, 4, 0)
        self.lib_vbox.setSpacing(8)
        lib_scroll.setWidget(self.lib_container)
        l_lay.addWidget(lib_scroll, 1)

        imp_row = QHBoxLayout()
        imp_row.setSpacing(6)
        btn_exp_all = Button(t("btn_export_all"), variant="glass", icon_name="save", fixed_compact=True)
        btn_exp_all.setFixedHeight(32)
        btn_exp_all.clicked.connect(self._export_full_backup)

        btn_imp_all = Button(t("btn_import_all"), variant="ghost", icon_name="folder", fixed_compact=True)
        btn_imp_all.setFixedHeight(32)
        btn_imp_all.clicked.connect(self._import_json_or_backup)

        self.btn_undo_lib = IconButton("chevron_up", size=32, variant="ghost", tooltip=t("btn_undo_delete"))
        self.btn_undo_lib.clicked.connect(self._undo_delete_macro)
        self.btn_undo_lib.hide()

        imp_row.addWidget(btn_exp_all, 1)
        imp_row.addWidget(btn_imp_all, 1)
        imp_row.addWidget(self.btn_undo_lib)
        l_lay.addLayout(imp_row)

        body.addWidget(lib_card)

        # Right Column
        tl_card = GlassCard()
        r_lay = QVBoxLayout(tl_card)
        r_lay.setContentsMargins(16, 10, 16, 10)
        r_lay.setSpacing(7)

        # Row 1: Capture Filters (Keyboard + Clicks + Scroll + Cursor Path) + Repeat Speed
        flt_bar = QHBoxLayout()
        flt_bar.setSpacing(8)

        lbl_kb = QLabel(t("lbl_rec_keyboard"))
        lbl_kb.setFont(make_font(12, QFont.Weight.Bold))
        lbl_kb.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        tog_kb = ToggleSwitch(self.state.config.record_keyboard)
        tog_kb.toggled.connect(lambda v: self.state.update_setting("record_keyboard", v))

        lbl_mc = QLabel(t("lbl_rec_clicks"))
        lbl_mc.setFont(make_font(12, QFont.Weight.Bold))
        lbl_mc.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        tog_mc = ToggleSwitch(self.state.config.record_mouse_clicks)
        tog_mc.toggled.connect(lambda v: self.state.update_setting("record_mouse_clicks", v))

        lbl_ms = QLabel(t("lbl_rec_scroll"))
        lbl_ms.setFont(make_font(12, QFont.Weight.Bold))
        lbl_ms.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        tog_ms = ToggleSwitch(self.state.config.record_mouse_scroll)
        tog_ms.toggled.connect(lambda v: self.state.update_setting("record_mouse_scroll", v))

        lbl_mm = QLabel(t("lbl_rec_moves"))
        lbl_mm.setFont(make_font(12, QFont.Weight.Bold))
        lbl_mm.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        tog_mm = ToggleSwitch(self.state.config.record_mouse_moves)
        tog_mm.toggled.connect(lambda v: self.state.update_setting("record_mouse_moves", v))

        lbl_spd = QLabel(t("lbl_repeat_speed"))
        lbl_spd.setFont(make_font(12, QFont.Weight.ExtraBold))
        lbl_spd.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        self.num_speed = NumberInput(
            value=float(self.state.config.default_playback_speed),
            min_val=0.25,
            max_val=10.0,
            step=0.25,
            decimals=2,
            suffix="x",
        )
        self.num_speed.setFixedSize(118, 32)
        self.num_speed.valueChanged.connect(lambda v: self.state.update_setting("default_playback_speed", float(v)))

        flt_bar.addWidget(lbl_kb)
        flt_bar.addWidget(tog_kb)
        flt_bar.addWidget(lbl_mc)
        flt_bar.addWidget(tog_mc)
        flt_bar.addWidget(lbl_ms)
        flt_bar.addWidget(tog_ms)
        flt_bar.addWidget(lbl_mm)
        flt_bar.addWidget(tog_mm)
        flt_bar.addStretch(1)
        flt_bar.addWidget(lbl_spd)
        flt_bar.addWidget(self.num_speed)
        r_lay.addLayout(flt_bar)

        # Row 2: Collision-Free Repeat Settings Panel + Stop Playback Hotkey
        rep_panel = GlassCard()
        rp_lay = QVBoxLayout(rep_panel)
        rp_lay.setContentsMargins(14, 8, 14, 8)
        rp_lay.setSpacing(6)

        mode_row = QHBoxLayout()
        mode_row.setSpacing(10)

        self.rep_mode_seg = SegmentedControl(
            [
                ("once", t("rec_repeat_mode_once")),
                ("count", t("rec_repeat_mode_count")),
                ("duration", t("rec_repeat_mode_duration")),
            ],
            selected_key=self.state.config.repeat_mode,
        )
        self.rep_mode_seg.setFixedHeight(34)
        self.rep_mode_seg.changed.connect(self._on_repeat_mode_changed)
        mode_row.addWidget(self.rep_mode_seg, 1)

        self.num_repeat_count = NumberInput(
            value=float(self.state.config.default_repeat_count),
            min_val=0.0,
            max_val=99999.0,
            step=1.0,
            decimals=0,
            suffix=t("unit_times"),
        )
        self.num_repeat_count.setFixedSize(130, 32)
        self.num_repeat_count.valueChanged.connect(
            lambda v: self.state.update_setting("default_repeat_count", int(v))
        )
        mode_row.addWidget(self.num_repeat_count)

        self.num_duration_val = NumberInput(
            value=float(self.state.config.repeat_duration_value),
            min_val=1.0,
            max_val=9999.0,
            step=1.0,
            decimals=0,
            suffix="",
        )
        self.num_duration_val.setFixedSize(104, 32)
        self.num_duration_val.valueChanged.connect(
            lambda v: self.state.update_setting("repeat_duration_value", int(v))
        )

        self.combo_duration_unit = SearchCombo(
            [
                ("seconds", t("unit_seconds")),
                ("minutes", t("unit_minutes")),
                ("hours", t("unit_hours")),
            ],
            selected_key=self.state.config.repeat_duration_unit,
        )
        self.combo_duration_unit.setFixedSize(124, 32)
        self.combo_duration_unit.currentChanged.connect(
            lambda u: self.state.update_setting("repeat_duration_unit", u)
        )

        mode_row.addWidget(self.num_duration_val)
        mode_row.addWidget(self.combo_duration_unit)
        rp_lay.addLayout(mode_row)

        opts_grid = QGridLayout()
        opts_grid.setContentsMargins(0, 4, 0, 2)
        opts_grid.setHorizontalSpacing(18)
        opts_grid.setVerticalSpacing(8)

        # Cell (0, 0): Delay between repeats + ms input
        c00 = QHBoxLayout()
        c00.setSpacing(8)
        self.tog_delay_between = ToggleSwitch(self.state.config.repeat_delay_enabled)
        self.tog_delay_between.toggled.connect(self._on_delay_between_toggled)
        lbl_set_delay = QLabel(t("lbl_set_delay_between"))
        lbl_set_delay.setFont(make_font(12, QFont.Weight.Bold))
        lbl_set_delay.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        self.num_delay_between = NumberInput(
            value=float(self.state.config.repeat_delay_ms),
            min_val=10.0,
            max_val=3600000.0,
            step=250.0,
            decimals=0,
            suffix="ms",
        )
        self.num_delay_between.setFixedSize(124, 30)
        self.num_delay_between.valueChanged.connect(
            lambda v: self.state.update_setting("repeat_delay_ms", int(v))
        )
        c00.addWidget(self.tog_delay_between)
        c00.addWidget(lbl_set_delay)
        c00.addWidget(self.num_delay_between)
        c00.addStretch(1)

        # Cell (0, 1): Random delay
        c01 = QHBoxLayout()
        c01.setSpacing(8)
        self.tog_delay_random = ToggleSwitch(self.state.config.repeat_delay_random)
        self.tog_delay_random.toggled.connect(
            lambda v: self.state.update_setting("repeat_delay_random", v)
        )
        lbl_rand = QLabel(t("lbl_delay_random"))
        lbl_rand.setFont(make_font(12, QFont.Weight.DemiBold))
        lbl_rand.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        c01.addWidget(self.tog_delay_random)
        c01.addWidget(lbl_rand)
        c01.addStretch(1)

        # Cell (1, 0): Do not activate window on stop
        c10 = QHBoxLayout()
        c10.setSpacing(8)
        self.tog_no_activate = ToggleSwitch(self.state.config.do_not_activate_on_stop)
        self.tog_no_activate.toggled.connect(
            lambda v: self.state.update_setting("do_not_activate_on_stop", v)
        )
        lbl_no_act = QLabel(t("lbl_do_not_activate"))
        lbl_no_act.setFont(make_font(12, QFont.Weight.Bold))
        lbl_no_act.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        c10.addWidget(self.tog_no_activate)
        c10.addWidget(lbl_no_act)
        c10.addStretch(1)

        # Cell (1, 1): Shutdown computer when finished
        c11 = QHBoxLayout()
        c11.setSpacing(8)
        self.tog_shutdown = ToggleSwitch(self.state.config.shutdown_on_finish)
        self.tog_shutdown.toggled.connect(
            lambda v: self.state.update_setting("shutdown_on_finish", v)
        )
        lbl_shut = QLabel(t("lbl_shutdown_when_finished"))
        lbl_shut.setFont(make_font(12, QFont.Weight.Bold))
        lbl_shut.setStyleSheet(f"color: {T.token_hex('danger')}; background: transparent;")
        c11.addWidget(self.tog_shutdown)
        c11.addWidget(lbl_shut)
        c11.addStretch(1)

        opts_grid.addLayout(c00, 0, 0)
        opts_grid.addLayout(c01, 0, 1)
        opts_grid.addLayout(c10, 1, 0)
        opts_grid.addLayout(c11, 1, 1)
        opts_grid.setColumnStretch(0, 1)
        opts_grid.setColumnStretch(1, 1)
        rp_lay.addLayout(opts_grid)

        r_lay.addWidget(rep_panel)
        self._sync_repeat_mode_inputs()

        # Live Recording Banner
        self.lbl_live_rec_banner = QLabel("")
        self.lbl_live_rec_banner.setFont(make_font(15, QFont.Weight.ExtraBold))
        self.lbl_live_rec_banner.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.lbl_live_rec_banner.setStyleSheet(
            f"color: {T.token_hex('danger')}; background: {T.token_hex('bg1')}; border-radius: 12px; padding: 24px;"
        )
        self.lbl_live_rec_banner.hide()
        r_lay.addWidget(self.lbl_live_rec_banner)

        # Timeline ScrollArea
        self.scroll = QScrollArea()
        self.scroll.setWidgetResizable(True)
        self.scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.scroll.setStyleSheet(modern_scroll_style())
        self.tl_container = QWidget()
        self.tl_container.setStyleSheet("background: transparent;")
        self.tl_vbox = QVBoxLayout(self.tl_container)
        self.tl_vbox.setContentsMargins(0, 0, 6, 0)
        self.tl_vbox.setSpacing(6)
        self.scroll.setWidget(self.tl_container)
        r_lay.addWidget(self.scroll, 1)

        # Bottom Responsive Adder Bar (4 distinct step types; redundant `+ ضغطة مطوّلة` removed!)
        b_add_k = Button(t("step_add_key_tap"), variant="glass", icon_name="keyboard", tint_hex="#8B5CF6")
        b_add_k.clicked.connect(lambda: self._insert_step("key_tap"))

        b_add_w = Button(t("step_add_wait"), variant="glass", icon_name="clock", tint_hex="#F59E0B")
        b_add_w.clicked.connect(lambda: self._insert_step("wait"))

        b_add_c = Button(t("step_add_mouse_click"), variant="glass", icon_name="mouse", tint_hex="#10B981")
        b_add_c.clicked.connect(lambda: self._insert_step("mouse_click"))

        b_add_s = Button(t("step_add_mouse_scroll"), variant="glass", icon_name="mouse", tint_hex="#EC4899")
        b_add_s.clicked.connect(lambda: self._insert_step("mouse_scroll"))

        self.rec_adder_bar = ResponsiveFlowBar(
            [b_add_k, b_add_w, b_add_c, b_add_s],
            row_height=32,
            h_spacing=6,
            v_spacing=6,
        )
        r_lay.addWidget(self.rec_adder_bar)

        body.addWidget(tl_card, 1)
        root.addLayout(body, 1)

    def _toggle_rec_sidebar(self) -> None:
        self._sidebar_collapsed = not self._sidebar_collapsed
        setattr(self.state, "recorder_sidebar_collapsed", self._sidebar_collapsed)
        self.lib_card.setVisible(not self._sidebar_collapsed)
        self.btn_toggle_sidebar.setText(
            t("btn_show_macros_sidebar") if self._sidebar_collapsed else t("btn_hide_macros_sidebar")
        )

    def _connect_signals(self) -> None:
        self.state.events_updated.connect(self._refresh_timeline)
        self.state.recording_count_changed.connect(self._on_live_recording_count)
        self.state.macros_library_updated.connect(self._refresh_saved_library)
        self.state.recording_state_changed.connect(self._sync_transport_buttons)
        self.state.playback_state_changed.connect(self._sync_transport_buttons)
        self.state.config_changed.connect(self._sync_transport_buttons)

    def _on_stop_play_hotkey_captured(self, key_str: str) -> None:
        clean = key_str.strip() or "Esc"
        self.state.update_setting("hotkey_panic_stop", clean)
        self._sync_transport_buttons()

    def _on_repeat_mode_changed(self, mode: str) -> None:
        self.state.update_setting("repeat_mode", mode)
        self._sync_repeat_mode_inputs()

    def _on_delay_between_toggled(self, val: bool) -> None:
        self.state.update_setting("repeat_delay_enabled", val)
        self.num_delay_between.setEnabled(val)
        self.tog_delay_random.setEnabled(val)

    def _sync_repeat_mode_inputs(self) -> None:
        mode = self.state.config.repeat_mode
        self.num_repeat_count.setVisible(mode == "count")
        self.num_duration_val.setVisible(mode == "duration")
        self.combo_duration_unit.setVisible(mode == "duration")
        delay_en = bool(self.state.config.repeat_delay_enabled)
        self.num_delay_between.setEnabled(delay_en)
        self.tog_delay_random.setEnabled(delay_en)

    def _on_live_recording_count(self, count: int) -> None:
        hk = self.state.config.hotkey_record_toggle or "F9"
        self.lbl_live_rec_banner.setText(
            tl(
                f"● جاري التسجيل الحي للماوس والكيبورد... ({fmt_num(count)} خطوة — اضغط {hk} أو الزر العائم أسفل الشاشة للإيقاف)",
                f"● Live Mouse & Keyboard Recording... ({count} events — Press {hk} or click the floating Stop button)",
            )
        )

    def _sync_transport_buttons(self, *_) -> None:
        hk_rec = self.state.config.hotkey_record_toggle or "F9"
        hk_play = self.state.config.hotkey_playback_toggle or "F10"
        hk_stop = self.state.config.hotkey_panic_stop or "Esc"
        stop_combo_lbl = hk_play if hk_play.upper() == hk_stop.upper() else f"{hk_play} / {hk_stop}"

        if self.state.is_recording:
            self.btn_rec.setText(f"{t('btn_stop_rec')} ({hk_rec})")
            self.btn_rec.setIconName("stop")
            self.scroll.hide()
            self.lbl_live_rec_banner.show()
            self._on_live_recording_count(len(self.state.recorded_events))
        else:
            self.btn_rec.setText(f"{t('btn_start_rec')} ({hk_rec})")
            self.btn_rec.setIconName("record")
            self.lbl_live_rec_banner.hide()
            self.scroll.show()

        if self.state.is_playing:
            self.btn_play.setText(f"{t('btn_stop_play')} ({stop_combo_lbl})")
            self.btn_play.setVariant("danger")
            self.btn_play.setIconName("stop")
        else:
            self.btn_play.setText(f"{t('btn_play_macro')} ({hk_play})")
            self.btn_play.setVariant("primary")
            self.btn_play.setIconName("play")

        self._refresh_saved_library()

    def _on_play_clicked(self) -> None:
        if self.state.is_playing:
            self.engine.stop_all()
        else:
            self.engine.start_playback()

    def _convert_current_to_combo(self) -> None:
        if not self.state.recorded_events:
            self.state.toast_requested.emit(tl("سجّل أو أضف خطوات أولاً", "Add steps first"), "danger")
            return
        self.engine.convert_recording_to_smart_combo(
            self.name_edit.text().strip() or "Macro",
            self.state.recorded_events,
        )

    def _clear_events(self) -> None:
        self.state.recorded_events.clear()
        self._refresh_timeline()

    def _insert_step(self, kind: str) -> None:
        sp_sc, sp_ext = scancode_for_label("Space")
        cx, cy = get_cursor_pos() if "mouse" in kind else (500, 500)
        if kind == "key_tap":
            ev = MacroEvent(kind="key_tap", delay_ms=50, hold_ms=0, key="Space", scan_code=sp_sc, is_extended=sp_ext)
        elif kind == "wait":
            ev = MacroEvent(kind="wait", delay_ms=1000, key="Wait")
        elif kind == "mouse_scroll":
            ev = MacroEvent(kind="mouse_scroll", delay_ms=50, x=cx, y=cy, scroll_dir="down", scroll_amount=3, dy=-3)
        else:
            ev = MacroEvent(kind="mouse_click", delay_ms=50, hold_ms=0, x=cx, y=cy, button="left")
        self.state.recorded_events.append(ev.to_dict())
        self._refresh_timeline()

    def _delete_event_at(self, idx: int) -> None:
        if 0 <= idx < len(self.state.recorded_events):
            self.state.recorded_events.pop(idx)
            self._refresh_timeline()

    def _refresh_timeline(self) -> None:
        if self.state.is_recording:
            return
        self._event_rows.clear()
        while self.tl_vbox.count():
            it = self.tl_vbox.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.deleteLater()

        events = self.state.recorded_events
        if not events:
            empty = QLabel(t("timeline_empty"))
            empty.setFont(make_font(13, QFont.Weight.DemiBold))
            empty.setAlignment(Qt.AlignmentFlag.AlignCenter)
            empty.setWordWrap(True)
            empty.setStyleSheet(f"color: {T.token_hex('text3')}; padding: 36px;")
            self.tl_vbox.addWidget(empty)
            return

        for idx, ev in enumerate(events[:250]):
            row = _EditableEventRow(idx, ev, self.state, self.tl_container)
            row.deleteClicked.connect(self._delete_event_at)
            row.dragMoveRequested.connect(self._on_event_drag_moved)
            self.tl_vbox.addWidget(row)
            self._event_rows.append(row)
        self.tl_vbox.addStretch(1)

    def _on_event_drag_moved(self, dragged_row: _EditableEventRow, global_y: int) -> None:
        events = self.state.recorded_events
        if dragged_row not in self._event_rows:
            return
        cur_idx = self._event_rows.index(dragged_row)
        local_y = self.tl_container.mapFromGlobal(QPoint(0, global_y)).y()

        target_idx = cur_idx
        for i, rw in enumerate(self._event_rows):
            ry = rw.y()
            rh = max(24, rw.height())
            if i < cur_idx and local_y < (ry + rh * 0.55):
                target_idx = i
                break
            elif i > cur_idx and local_y > (ry + rh * 0.45):
                target_idx = i

        if target_idx != cur_idx and 0 <= target_idx < len(events):
            item = events.pop(cur_idx)
            events.insert(target_idx, item)
            self._event_rows.pop(cur_idx)
            self._event_rows.insert(target_idx, dragged_row)
            self.tl_vbox.removeWidget(dragged_row)
            self.tl_vbox.insertWidget(target_idx, dragged_row)
            for idx_i, rw in enumerate(self._event_rows):
                rw.set_step_index(idx_i)

    def _refresh_saved_library(self) -> None:
        while self.lib_vbox.count():
            it = self.lib_vbox.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.deleteLater()

        playing = self.state.is_playing
        for idx, m in enumerate(self.state.saved_macros):
            card = StepCard(index=idx, accent_hex="#8B5CF6")
            cl = QVBoxLayout(card)
            cl.setContentsMargins(12, 8, 10, 8)
            cl.setSpacing(6)

            nm = QLabel(f"{m.get('name', 'Macro')} ({fmt_num(len(m.get('events', [])))} {t('unit_steps')})")
            nm.setWordWrap(True)
            nm.setFont(make_font(13, QFont.Weight.ExtraBold))
            nm.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
            cl.addWidget(nm)

            mid_str = str(m.get("id", ""))
            hk_row = QHBoxLayout()
            hk_row.setSpacing(5)

            hk_btn = KeyCaptureButton(
                current_key=str(m.get("hotkey", "")),
                allow_chord=True,
                state=self.state,
            )
            hk_btn.setFixedHeight(30)
            hk_btn.keyCaptured.connect(lambda k_str, mid=mid_str: self.engine.update_saved_macro_hotkey(mid, k_str))

            b_cmb = IconButton("bolt", size=30, variant="glass", tooltip=t("btn_convert_to_combo"))
            b_cmb.clicked.connect(
                lambda md=m: self.engine.convert_recording_to_smart_combo(
                    md.get("name", "Macro"), md.get("events", [])
                )
            )

            b_exp = IconButton("save", size=30, variant="ghost", tooltip=t("btn_export_all"))
            b_exp.clicked.connect(lambda md=m: self._export_json(md))

            b_del = IconButton("trash", size=30, variant="danger", tooltip=t("btn_delete_rec"))
            b_del.clicked.connect(lambda md=m: self._delete_saved_with_undo(md))

            hk_row.addWidget(hk_btn, 1)
            hk_row.addWidget(b_cmb)
            hk_row.addWidget(b_exp)
            hk_row.addWidget(b_del)
            cl.addLayout(hk_row)

            btns = QHBoxLayout()
            btns.setSpacing(6)
            b_play = Button(
                t("btn_stop_play") if playing else t("btn_play_macro"),
                variant="danger" if playing else "primary",
                icon_name="stop" if playing else "play",
                fixed_compact=True,
            )
            b_play.setFixedHeight(32)
            b_play.clicked.connect(
                lambda md=m: (self.engine.stop_all() if self.state.is_playing else self.engine.start_playback(events=md.get("events", [])))
            )

            b_edit = Button(t("btn_load_to_editor"), variant="glass", fixed_compact=True)
            b_edit.setFixedHeight(32)
            b_edit.clicked.connect(lambda md=m: self._load_saved_into_timeline(md))

            btns.addWidget(b_play, 1)
            btns.addWidget(b_edit, 1)
            cl.addLayout(btns)

            self.lib_vbox.addWidget(card)

        self.lib_vbox.addStretch(1)

    def _load_saved_into_timeline(self, m: Dict[str, Any]) -> None:
        self.name_edit.setText(m.get("name", "Macro"))
        self.state.recorded_events = [dict(e) for e in m.get("events", [])]
        self._refresh_timeline()

    def _delete_saved_with_undo(self, m: Dict[str, Any]) -> None:
        self._last_deleted_macro = dict(m)
        self.btn_undo_lib.show()
        self.engine.delete_saved_macro(str(m.get("id", "")))

    def _undo_delete_macro(self) -> None:
        if self._last_deleted_macro is not None:
            self.state.recorded_events = self._last_deleted_macro.get("events", [])
            self.engine.save_current_macro_to_library(
                self._last_deleted_macro.get("name", "Macro"),
                self._last_deleted_macro.get("hotkey", ""),
            )
            self._last_deleted_macro = None
            self.btn_undo_lib.hide()

    def _export_json(self, m: Dict[str, Any]) -> None:
        path, _ = QFileDialog.getSaveFileName(self, t("btn_export_all"), f"{m.get('name', 'macro')}.json", "JSON (*.json)")
        if path:
            Path(path).write_text(json.dumps(m, ensure_ascii=False, indent=2), encoding="utf-8")

    def _export_full_backup(self) -> None:
        path, _ = QFileDialog.getSaveFileName(
            self,
            t("btn_export_all"),
            "V-MacroPlus_Full_Backup.json",
            "JSON Backup (*.json)",
        )
        if path:
            self.engine.export_all_backup(path)

    def _import_json_or_backup(self) -> None:
        path, _ = QFileDialog.getOpenFileName(self, t("btn_import_all"), "", "JSON (*.json)")
        if path:
            self.engine.import_all_backup(path)
