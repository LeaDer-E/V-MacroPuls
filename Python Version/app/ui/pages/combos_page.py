# -*- coding: utf-8 -*-
"""
Upgraded Smart Keys & Custom Macros Page («الأزرار الذكية») for V-MacroPlus v3.2.0:
1. Full 1-to-1 Compatibility with Converted Recordings:
   Supports every single step type (`key_tap`, `key_down`, `key_up`, `wait`, `mouse_click`,
   `mouse_down`, `mouse_up`, `mouse_scroll`, `mouse_move`, `open_folder`, `powershell`, `cmd`) and shows
   each step's exact `delay_ms` (`التأخير`) and `hold_ms` (`مدة الضغط`)!
2. Unified Key Press (`+ ضغطة زرار`) with integrated Hold Duration (`مدة الضغط: ms`) — redundant `+ ضغطة مطوّلة`
   button removed since `+ ضغطة زرار` already calculates and edits hold duration!
3. Full Mouse Buttons (`Left`, `Right`, `Middle`, `Back X1`, `Forward X2`) + 4-Way Mouse Scroll
   (`Down ↓`, `Up ↑`, `Left ←`, `Right →` with Scroll Amount)!
4. Collision-Free Responsive Step Adder Toolbar so buttons NEVER overlap when resizing the window!
5. Live Interactive Mouse Drag-and-Drop Reordering (`DragGripHandle` + direct row dragging).
"""

from __future__ import annotations
import time
from typing import Any, Dict, List, Optional, Tuple
import copy
from PySide6.QtCore import QPoint, Qt, QTimer, Signal
from PySide6.QtGui import QFont, QKeySequence, QMouseEvent, QShortcut
from PySide6.QtWidgets import (
    QFileDialog,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QPlainTextEdit,
    QScrollArea,
    QSizePolicy,
    QVBoxLayout,
    QWidget,
)
from app.domain.macro_engine import MacroEngine
from app.domain.python_presets import (
    delete_user_python_script,
    get_python_script_code_by_key,
    get_python_script_dropdown_items,
    get_python_script_name_by_key,
    upsert_user_python_script,
)
from app.domain.models import ComboStep, SmartCombo
from app.domain.win32_input import (
    get_cursor_pos,
    list_running_window_apps_with_paths,
    scancode_for_label,
)
from app.i18n import fmt_num, get_lang_font_family, is_rtl, ltr_value, t, tl
from app.state import AppState
from app.theme import T
from app.ui.icons import logo_pixmap
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
    StepEnableToggleButton,
    StepNoteIconButton,
    TintedScrollBox,
    ToggleSwitch,
    framed_horizontal_scroll_style,
    make_font,
    get_row_logical_y,
    modern_scroll_style,
    smooth_reorder_vbox_widget,
)

STEP_CATEGORY_COLORS = {
    "key_tap": "#8B5CF6",       # Vivid Violet
    "key_hold": "#8B5CF6",      # Unified with Key Press
    "key_down": "#7C3AED",      # Key Down
    "key_up": "#C084FC",        # Key Up
    "wait": "#F59E0B",          # Warm Amber
    "mouse_click": "#10B981",   # Emerald Green
    "mouse_down": "#059669",    # Mouse Down
    "mouse_up": "#34D399",      # Mouse Up
    "mouse_move": "#14B8A6",    # Teal
    "mouse_scroll": "#EC4899",  # Pink/Magenta (Scroll Wheel)
    "open_folder": "#3B82F6",   # Royal Blue
    "powershell": "#06B6D4",    # Cyber Cyan
    "cmd": "#F43F5E",           # Coral Rose
    "type_text": "#06B6D4",     # Cyan Text Stream
    "python_script": "#F59E0B", # Amber Python Script
}


def get_mouse_button_options() -> List[Tuple[str, str]]:
    return [
        ("left", t("mouse_btn_left")),
        ("right", t("mouse_btn_right")),
        ("middle", t("mouse_btn_middle")),
        ("x1", t("mouse_btn_x1")),
        ("x2", t("mouse_btn_x2")),
    ]


def get_scroll_dir_options() -> List[Tuple[str, str]]:
    return [
        ("down", t("scroll_down")),
        ("up", t("scroll_up")),
        ("left", t("scroll_left")),
        ("right", t("scroll_right")),
    ]


class _StepRowWidget(QWidget):
    """
    Alternating two-tone step row (`StepCard`) inside the Macro Editor with:
    - Live Drag-and-Drop Reordering (`DragGripHandle` + direct row dragging)
    - Hold Duration (`hold_ms`) on Key & Mouse Click steps
    - All 5 Mouse Buttons (`left`, `right`, `middle`, `x1` Back, `x2` Forward) + 4-Way Mouse Scroll
    - Full support for converted Recorder steps (`key_down`, `key_up`, `mouse_down`, `mouse_up`, `mouse_move`)
      and their exact `delay_ms` timing!
    """

    changed = Signal()
    removed = Signal(int)
    moveUp = Signal(int)
    moveDown = Signal(int)
    testSingleStep = Signal(int)
    dragMoveRequested = Signal(object, int)
    dragFinished = Signal()

    def __init__(
        self,
        index: int,
        step_dict: Dict[str, Any],
        state: AppState,
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self.index = index
        self.step = step_dict
        self.state = state
        self._picking_live_xy = False
        self._row_dragging = False

        st = self.step.get("step_type", "key_tap")
        self.cat_hex = STEP_CATEGORY_COLORS.get(st, "#8B5CF6")

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

        self.step_num = QLabel(f"#{fmt_num(self.index + 1)}")
        self.step_num.setFont(make_font(12, QFont.Weight.ExtraBold))
        self.step_num.setStyleSheet(f"color: {self.cat_hex}; background: transparent;")
        lay.addWidget(self.step_num)

        step_en = bool(self.step.get("enabled", True))
        self.step["enabled"] = step_en
        self.card.setStepEnabled(step_en)

        self.btn_enable_step = StepEnableToggleButton(step_en, parent=self.card)
        self.btn_enable_step.toggled.connect(self._on_step_enabled_toggled)
        lay.addWidget(self.btn_enable_step)

        self.lbl_paused = QLabel(t("lbl_step_paused_badge"))
        self.lbl_paused.setFont(make_font(11, QFont.Weight.ExtraBold))
        self.lbl_paused.setStyleSheet("color: #F59E0B; background: transparent;")
        self.lbl_paused.setVisible(not step_en)
        lay.addWidget(self.lbl_paused)

        if st == "wait":
            lbl = QLabel(t("step_row_wait"))
            lbl.setFont(make_font(12, QFont.Weight.Bold))
            lbl.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
            lay.addWidget(lbl)

            self.num_ms = NumberInput(
                value=float(self.step.get("delay_ms", 1000)),
                min_val=1.0,
                max_val=600000.0,
                step=100.0,
                decimals=0,
                suffix="ms",
            )
            self.num_ms.setFixedSize(134, 32)
            self.num_ms.valueChanged.connect(self._on_wait_ms_changed)
            lay.addWidget(self.num_ms)

        elif st in ("mouse_click", "mouse_down", "mouse_up", "mouse_move"):
            lbl_map = {
                "mouse_click": t("step_row_mouse_click"),
                "mouse_down": t("step_row_mouse_down"),
                "mouse_up": t("step_row_mouse_up"),
                "mouse_move": t("step_row_mouse_move"),
            }
            lbl = QLabel(lbl_map.get(st, t("step_row_mouse_click")))
            lbl.setFont(make_font(12, QFont.Weight.Bold))
            lbl.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
            lay.addWidget(lbl)

            if st != "mouse_move":
                self.btn_combo = SearchCombo(
                    get_mouse_button_options(),
                    selected_key=str(self.step.get("value", "left")),
                )
                self.btn_combo.setFixedSize(124, 32)
                self.btn_combo.currentChanged.connect(self._on_mouse_btn_changed)
                lay.addWidget(self.btn_combo)

            lbl_x = QLabel("X:")
            lbl_x.setFont(make_font(11, QFont.Weight.Bold))
            lbl_x.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            self.num_x = NumberInput(
                value=float(self.step.get("x", 500)),
                min_val=-32768.0,
                max_val=32768.0,
                step=10.0,
                decimals=0,
                suffix="",
            )
            self.num_x.setFixedSize(88, 32)
            self.num_x.valueChanged.connect(self._on_x_changed)

            lbl_y = QLabel("Y:")
            lbl_y.setFont(make_font(11, QFont.Weight.Bold))
            lbl_y.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            self.num_y = NumberInput(
                value=float(self.step.get("y", 500)),
                min_val=-32768.0,
                max_val=32768.0,
                step=10.0,
                decimals=0,
                suffix="",
            )
            self.num_y.setFixedSize(88, 32)
            self.num_y.valueChanged.connect(self._on_y_changed)

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

            lay.addWidget(lbl_x)
            lay.addWidget(self.num_x)
            lay.addWidget(lbl_y)
            lay.addWidget(self.num_y)
            lay.addWidget(self.btn_pick_xy)
            lay.addWidget(self.lbl_live_xy)

            if st == "mouse_click":
                lbl_hold = QLabel(t("lbl_hold_duration"))
                lbl_hold.setFont(make_font(11, QFont.Weight.Bold))
                lbl_hold.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
                self.num_hold = NumberInput(
                    value=float(self.step.get("hold_ms", 0)),
                    min_val=0.0,
                    max_val=60000.0,
                    step=100.0,
                    decimals=0,
                    suffix="ms",
                )
                self.num_hold.setFixedSize(102, 32)
                self.num_hold.valueChanged.connect(self._on_hold_ms_changed)
                lay.addWidget(lbl_hold)
                lay.addWidget(self.num_hold)

            self._xy_timer = QTimer(self)
            self._xy_timer.setInterval(50)
            self._xy_timer.timeout.connect(self._tick_live_xy)
            self.num_x.focused.connect(self._start_live_xy_preview)
            self.num_y.focused.connect(self._start_live_xy_preview)
            self.state.cursor_pick_hotkey_pressed.connect(self._on_f8_picked)

        elif st == "mouse_scroll":
            lbl = QLabel(t("lbl_scroll_dir"))
            lbl.setFont(make_font(12, QFont.Weight.ExtraBold))
            lbl.setStyleSheet(f"color: {self.cat_hex}; background: transparent;")
            lay.addWidget(lbl)

            self.scroll_dir_combo = SearchCombo(
                get_scroll_dir_options(),
                selected_key=str(self.step.get("value", "down")),
            )
            self.scroll_dir_combo.setFixedSize(118, 32)
            self.scroll_dir_combo.currentChanged.connect(self._on_mouse_btn_changed)
            lay.addWidget(self.scroll_dir_combo)

            lbl_amt = QLabel(t("lbl_scroll_amount"))
            lbl_amt.setFont(make_font(12, QFont.Weight.Bold))
            lbl_amt.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            self.num_scroll_amt = NumberInput(
                value=float(self.step.get("scroll_amount", 3)),
                min_val=1.0,
                max_val=100.0,
                step=1.0,
                decimals=0,
                suffix="",
            )
            self.num_scroll_amt.setFixedSize(98, 32)
            self.num_scroll_amt.valueChanged.connect(self._on_scroll_amt_changed)
            lay.addWidget(lbl_amt)
            lay.addWidget(self.num_scroll_amt)

        elif st == "open_folder":
            lbl = QLabel(t("lbl_open_folder_step"))
            lbl.setFont(make_font(12, QFont.Weight.ExtraBold))
            lbl.setStyleSheet(f"color: {self.cat_hex}; background: transparent;")
            lay.addWidget(lbl)

            self.folder_edit = QLineEdit(self.step.get("value", "C:\\"))
            self.folder_edit.setPlaceholderText("C:\\Users\\...")
            self.folder_edit.setFixedHeight(32)
            self.folder_edit.setStyleSheet(
                f"""
                QLineEdit {{
                    background: {T.token_hex('bg0')};
                    color: {T.token_hex('text')};
                    border: 1px solid {T.token_hex('border_hi')};
                    border-radius: 8px;
                    padding: 2px 10px;
                    font-family: 'Consolas', '{get_lang_font_family()}';
                    font-size: 13px;
                }}
                """
            )
            self.folder_edit.textChanged.connect(self._on_cmd_changed)
            lay.addWidget(self.folder_edit, 1)

            btn_browse_dir = Button(
                t("btn_browse_folder"),
                variant="glass",
                icon_name="folder",
                tint_hex=self.cat_hex,
            )
            btn_browse_dir.setFixedHeight(32)
            btn_browse_dir.clicked.connect(self._browse_folder)
            lay.addWidget(btn_browse_dir)

            btn_browse_app = Button(
                t("btn_browse_app_file"),
                variant="glass",
                icon_name="terminal",
                tint_hex=self.cat_hex,
            )
            btn_browse_app.setFixedHeight(32)
            btn_browse_app.clicked.connect(self._browse_program_or_file)
            lay.addWidget(btn_browse_app)

        elif st == "type_text":
            lbl = QLabel(t("step_row_type_text"))
            lbl.setFont(make_font(12, QFont.Weight.ExtraBold))
            lbl.setStyleSheet(f"color: {self.cat_hex}; background: transparent;")
            lay.addWidget(lbl)

            self.cmd_edit = QLineEdit(self.step.get("value", ""))
            self.cmd_edit.setPlaceholderText(t("ph_type_text"))
            self.cmd_edit.setFixedHeight(32)
            self.cmd_edit.setStyleSheet(
                f"QLineEdit {{ background: {T.token_hex('bg0')}; color: {T.token_hex('text')}; border: 1px solid {T.token_hex('border_hi')}; border-radius: 8px; padding: 2px 10px; font-family: '{get_lang_font_family()}'; font-size: 13px; }}"
            )
            self.cmd_edit.textChanged.connect(self._on_cmd_changed)
            lay.addWidget(self.cmd_edit, 1)

        elif st == "python_script":
            py_col = QVBoxLayout()
            py_col.setSpacing(4)

            py_top_row = QHBoxLayout()
            py_top_row.setSpacing(6)
            lbl = QLabel(t("step_row_python_script"))
            lbl.setFont(make_font(11, QFont.Weight.ExtraBold))
            lbl.setStyleSheet(f"color: {self.cat_hex}; background: transparent;")

            self.py_preset_combo = SearchCombo(
                get_python_script_dropdown_items(),
                selected_key="preset:human_3d_keys",
            )
            self.py_preset_combo.setMinimumWidth(210)
            self.py_preset_combo.setFixedHeight(28)
            self.py_preset_combo.currentChanged.connect(self._on_step_py_preset_selected)

            self.py_script_name_edit = QLineEdit()
            self.py_script_name_edit.setPlaceholderText(t("ph_py_script_name"))
            self.py_script_name_edit.setFixedWidth(135)
            self.py_script_name_edit.setFixedHeight(28)
            self.py_script_name_edit.setStyleSheet(
                f"QLineEdit {{ background: {T.token_hex('bg0')}; color: {T.token_hex('text')}; border: 1px solid {T.token_hex('border_hi')}; border-radius: 7px; padding: 2px 8px; font-family: '{get_lang_font_family()}'; font-size: 11px; font-weight: bold; }}"
            )

            btn_save_py = Button(t("btn_save_py_script"), variant="glass", icon_name="save", tint_hex=self.cat_hex)
            btn_save_py.setFixedHeight(28)
            btn_save_py.clicked.connect(self._on_step_save_py_script)

            btn_del_py = IconButton("trash", size=28, variant="danger", tooltip=t("tip_del_py_script"))
            btn_del_py.clicked.connect(self._on_step_delete_py_script)

            py_top_row.addWidget(lbl)
            py_top_row.addWidget(self.py_preset_combo, 1)
            py_top_row.addWidget(self.py_script_name_edit)
            py_top_row.addWidget(btn_save_py)
            py_top_row.addWidget(btn_del_py)
            py_col.addLayout(py_top_row)

            self.py_edit = QPlainTextEdit(self.step.get("value", ""))
            self.py_edit.setLayoutDirection(Qt.LayoutDirection.LeftToRight)
            self.py_edit.setPlaceholderText(t("ph_python_script"))
            self.py_edit.setFixedHeight(84)
            self.py_edit.setStyleSheet(
                f"QPlainTextEdit {{ background: {T.token_hex('bg0')}; color: {T.token_hex('text')}; border: 1px solid {T.token_hex('border_hi')}; border-radius: 8px; padding: 4px 8px; font-family: 'Consolas', 'Courier New', monospace; font-size: 12px; }}"
            )
            self.py_edit.textChanged.connect(lambda: self._on_cmd_changed(self.py_edit.toPlainText()))
            py_col.addWidget(self.py_edit)
            lay.addLayout(py_col, 1)
            self.state.python_scripts_updated.connect(self._refresh_step_py_presets_combo)

        elif st in ("powershell", "cmd"):
            tag = t("step_row_ps") if st == "powershell" else t("step_row_cmd")
            lbl = QLabel(tag)
            lbl.setFont(make_font(12, QFont.Weight.ExtraBold))
            lbl.setStyleSheet(f"color: {self.cat_hex}; background: transparent;")
            lay.addWidget(lbl)

            self.cmd_edit = QLineEdit(self.step.get("value", ""))
            self.cmd_edit.setPlaceholderText(
                "Get-Process | ..." if st == "powershell" else "ipconfig /flushdns ..."
            )
            self.cmd_edit.setFixedHeight(32)
            self.cmd_edit.setStyleSheet(
                f"""
                QLineEdit {{
                    background: {T.token_hex('bg0')};
                    color: {T.token_hex('text')};
                    border: 1px solid {T.token_hex('border_hi')};
                    border-radius: 8px;
                    padding: 2px 10px;
                    font-family: 'Consolas', '{get_lang_font_family()}';
                    font-size: 13px;
                }}
                """
            )
            self.cmd_edit.textChanged.connect(self._on_cmd_changed)
            lay.addWidget(self.cmd_edit, 1)

        else:  # "key_tap", "key_hold", "key_down", "key_up"
            key_lbl_map = {
                "key_tap": t("step_row_key_tap"),
                "key_hold": t("step_row_key_tap"),
                "key_down": t("step_row_key_down"),
                "key_up": t("step_row_key_up"),
            }
            lbl = QLabel(key_lbl_map.get(st, t("step_row_key_tap")))
            lbl.setFont(make_font(12, QFont.Weight.Bold))
            lbl.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
            lay.addWidget(lbl)

            self.key_btn = KeyCaptureButton(
                current_key=self.step.get("value", "Space"),
                scan_code=int(self.step.get("scan_code", 0)),
                is_extended=bool(self.step.get("is_extended", False)),
                allow_chord=(st in ("key_tap", "key_hold")),
                state=self.state,
                large_hero=False,
            )
            self.key_btn.setFixedHeight(32)
            self.key_btn.chordCaptured.connect(self._on_step_chord_captured)
            lay.addWidget(self.key_btn)

            if st in ("key_tap", "key_hold"):
                lbl_hold = QLabel(t("lbl_hold_duration"))
                lbl_hold.setFont(make_font(11, QFont.Weight.Bold))
                lbl_hold.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
                default_hold = int(self.step.get("hold_ms", 0))
                self.step["hold_ms"] = default_hold
                self.num_hold = NumberInput(
                    value=float(default_hold),
                    min_val=0.0,
                    max_val=60000.0,
                    step=100.0,
                    decimals=0,
                    suffix="ms",
                )
                self.num_hold.setFixedSize(108, 32)
                self.num_hold.valueChanged.connect(self._on_hold_ms_changed)
                lay.addWidget(lbl_hold)
                lay.addWidget(self.num_hold)

        if st not in ("powershell", "cmd", "open_folder", "type_text", "python_script"):
            lay.addStretch(1)

        if st != "wait":
            lbl_d = QLabel(t("lbl_pre_delay"))
            lbl_d.setFont(make_font(11, QFont.Weight.Medium))
            lbl_d.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            self.num_delay = NumberInput(
                value=float(self.step.get("delay_ms", 20)),
                min_val=1.0,
                max_val=600000.0,
                step=25.0,
                decimals=0,
                suffix="ms",
            )
            self.num_delay.setFixedSize(108, 32)
            self.num_delay.valueChanged.connect(self._on_wait_ms_changed)
            lay.addWidget(lbl_d)
            lay.addWidget(self.num_delay)

        self.btn_note = StepNoteIconButton(str(self.step.get("note", "")), parent=self.card)
        self.btn_note.noteChanged.connect(self._on_step_note_changed)
        lay.addWidget(self.btn_note)

        b_test_one = IconButton(
            "play",
            size=28,
            variant="ok",
            tooltip=tl("تجربة هذه الخطوة وحدها مرة واحدة", "Test this step once"),
        )
        b_test_one.clicked.connect(lambda: self.testSingleStep.emit(self.index))

        b_up = IconButton("chevron_up", size=28, variant="ghost")
        b_up.clicked.connect(lambda: self.moveUp.emit(self.index))
        b_dn = IconButton("chevron_down", size=28, variant="ghost")
        b_dn.clicked.connect(lambda: self.moveDown.emit(self.index))
        b_del = IconButton("trash", size=28, variant="danger", tooltip=t("btn_delete"))
        b_del.clicked.connect(lambda: self.removed.emit(self.index))

        lay.addWidget(b_test_one)
        lay.addWidget(b_up)
        lay.addWidget(b_dn)
        lay.addWidget(b_del)

    def set_step_index(self, new_idx: int) -> None:
        self.index = new_idx
        self.step_num.setText(f"#{fmt_num(new_idx + 1)}")
        self.card.setIndex(new_idx)

    def _on_step_enabled_toggled(self, enabled: bool) -> None:
        self.step["enabled"] = bool(enabled)
        self.card.setStepEnabled(bool(enabled))
        self.lbl_paused.setVisible(not bool(enabled))
        self.changed.emit()

    def _on_step_note_changed(self, note_txt: str) -> None:
        self.step["note"] = note_txt
        self.changed.emit()

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

    def _browse_folder(self) -> None:
        cur = self.step.get("value", "") or "C:\\"
        picked = QFileDialog.getExistingDirectory(self, t("lbl_open_folder_step"), cur)
        if picked:
            self.folder_edit.setText(picked)
            self.step["value"] = picked
            self.changed.emit()

    def _browse_program_or_file(self) -> None:
        cur = self.step.get("value", "") or "C:\\"
        picked, _ = QFileDialog.getOpenFileName(
            self,
            t("btn_browse_app_file"),
            cur,
            "Programs & Files (*.exe *.bat *.cmd *.lnk *.py *.*);;All Files (*.*)",
        )
        if picked:
            self.folder_edit.setText(picked)
            self.step["value"] = picked
            self.changed.emit()

    def _refresh_step_py_presets_combo(self, select_key: str = "") -> None:
        if not hasattr(self, "py_preset_combo"):
            return
        cur = select_key or self.py_preset_combo.currentKey()
        self.py_preset_combo.setItems(get_python_script_dropdown_items(), selected_key=cur)

    def _on_step_py_preset_selected(self, script_key: str) -> None:
        code = get_python_script_code_by_key(script_key)
        if code is not None and hasattr(self, "py_edit"):
            self.py_edit.setPlainText(code)
            self.step["value"] = code
            self.changed.emit()
        saved_nm = get_python_script_name_by_key(script_key)
        if saved_nm and hasattr(self, "py_script_name_edit"):
            self.py_script_name_edit.setText(saved_nm)

    def _on_step_save_py_script(self) -> None:
        if not hasattr(self, "py_edit"):
            return
        nm = self.py_script_name_edit.text().strip()
        code = self.py_edit.toPlainText()
        cur_k = self.py_preset_combo.currentKey()
        saved_id, saved_nm = upsert_user_python_script(nm, code, existing_key=cur_k if not nm else "")
        self.py_script_name_edit.setText(saved_nm)
        self._refresh_step_py_presets_combo(select_key=saved_id)
        self.state.python_scripts_updated.emit()
        self.state.toast_requested.emit(f"{t('toast_py_script_saved')} «{saved_nm}»", "ok")

    def _on_step_delete_py_script(self) -> None:
        if not hasattr(self, "py_preset_combo"):
            return
        cur_k = self.py_preset_combo.currentKey()
        if delete_user_python_script(cur_k):
            self.py_script_name_edit.clear()
            self._refresh_step_py_presets_combo(select_key="preset:human_3d_keys")
            self.state.python_scripts_updated.emit()
            self.state.toast_requested.emit(t("toast_py_script_deleted"), "info")

    def _on_step_chord_captured(self, chord: List[Tuple[int, bool]], lbl: str) -> None:
        if not lbl:
            return
        self.step["value"] = lbl
        if chord:
            self.step["scan_code"] = chord[-1][0]
            self.step["is_extended"] = chord[-1][1]
        self.changed.emit()

    def _on_wait_ms_changed(self, ms: float) -> None:
        self.step["delay_ms"] = int(round(ms))
        self.changed.emit()

    def _on_hold_ms_changed(self, ms: float) -> None:
        self.step["hold_ms"] = int(round(ms))
        self.changed.emit()

    def _on_scroll_amt_changed(self, val: float) -> None:
        self.step["scroll_amount"] = max(1, int(round(val)))
        self.changed.emit()

    def _on_mouse_btn_changed(self, btn: str) -> None:
        self.step["value"] = btn
        self.changed.emit()

    def _on_x_changed(self, x_val: float) -> None:
        self.step["x"] = int(round(x_val))
        self.step["use_coords"] = True
        self.changed.emit()

    def _on_y_changed(self, y_val: float) -> None:
        self.step["y"] = int(round(y_val))
        self.step["use_coords"] = True
        self.changed.emit()

    def _on_cmd_changed(self, txt: str) -> None:
        self.step["value"] = txt
        self.changed.emit()

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


class CombosPage(QWidget):
    """
    Main Smart Keys & Macros Studio Page with Collision-Free Step Adder Toolbar,
    Unified Key Press + Hold Duration, 5 Mouse Buttons, 4-Way Scroll, and 1-to-1 Converted Recording Support!
    """

    LEFT_PANEL_WIDTH = 264

    def __init__(self, state: AppState, engine: MacroEngine, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.state = state
        self.engine = engine
        self._selected_idx: int = 0
        self._step_rows: List[_StepRowWidget] = []
        self._last_deleted_combo: Optional[Dict[str, Any]] = None
        self._sidebar_collapsed: bool = bool(getattr(self.state, "combos_sidebar_collapsed", False))
        self._steps_editor_visible: bool = False
        self._undo_steps_stack: List[List[Dict[str, Any]]] = []
        self._redo_steps_stack: List[List[Dict[str, Any]]] = []

        self._build_ui()
        self._connect_signals()
        QShortcut(QKeySequence("Ctrl+Z"), self, activated=self._undo_combo_steps)
        QShortcut(QKeySequence("Ctrl+Y"), self, activated=self._redo_combo_steps)
        self._refresh_left_list()
        self._load_selected_into_editor()

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(16, 8, 16, 12)
        root.setSpacing(10)

        top_card = GlassCard(accent_tint=True, parent=self)
        t_lay = QHBoxLayout(top_card)
        t_lay.setContentsMargins(18, 10, 18, 10)
        t_lay.setSpacing(12)

        t_col = QVBoxLayout()
        t_col.setSpacing(2)
        title = QLabel(t("combo_title"))
        title.setFont(make_font(16, QFont.Weight.ExtraBold))
        title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        sub = QLabel(t("combo_subtitle"))
        sub.setFont(make_font(12, QFont.Weight.Medium))
        sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        t_col.addWidget(title)
        t_col.addWidget(sub)
        t_lay.addLayout(t_col, 1)

        sw_lbl = QLabel(t("combo_master_switch"))
        sw_lbl.setFont(make_font(13, QFont.Weight.Bold))
        sw_lbl.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        self.master_tog = ToggleSwitch(self.state.combos_active)
        self.master_tog.toggled.connect(lambda _: self.engine.toggle_combos_master())

        self.btn_toggle_sidebar = Button(
            t("btn_show_macros_sidebar") if self._sidebar_collapsed else t("btn_hide_macros_sidebar"),
            variant="glass",
            icon_name="dashboard",
            tint_hex=T.token_hex("accent"),
        )
        self.btn_toggle_sidebar.setFixedHeight(32)
        self.btn_toggle_sidebar.clicked.connect(self._toggle_macros_sidebar)

        t_lay.addWidget(sw_lbl)
        t_lay.addWidget(self.master_tog)
        t_lay.addSpacing(6)
        t_lay.addWidget(self.btn_toggle_sidebar)
        root.addWidget(top_card)

        body = QHBoxLayout()
        body.setSpacing(12)

        # Left Column
        self.left_card = GlassCard(parent=self)
        self.left_card.setFixedWidth(self.LEFT_PANEL_WIDTH)
        l_lay = QVBoxLayout(self.left_card)
        l_lay.setContentsMargins(10, 12, 10, 12)
        l_lay.setSpacing(8)

        top_btns = QHBoxLayout()
        top_btns.setSpacing(6)
        self.btn_new = Button(t("btn_new_smart_key"), variant="primary", icon_name="plus", fixed_compact=True)
        self.btn_new.clicked.connect(self._create_new_blank_combo)
        self.btn_undo = IconButton("chevron_up", size=34, variant="ghost", tooltip=t("btn_undo_delete"))
        self.btn_undo.clicked.connect(self._undo_last_delete)
        self.btn_undo.hide()
        top_btns.addWidget(self.btn_new, 1)
        top_btns.addWidget(self.btn_undo)
        l_lay.addLayout(top_btns)

        list_scroll = QScrollArea()
        list_scroll.setWidgetResizable(True)
        list_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        list_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        list_scroll.setStyleSheet(modern_scroll_style())
        self.list_container = QWidget()
        self.list_container.setStyleSheet("background: transparent;")
        self.list_vbox = QVBoxLayout(self.list_container)
        self.list_vbox.setContentsMargins(0, 0, 2, 0)
        self.list_vbox.setSpacing(6)
        list_scroll.setWidget(self.list_container)
        l_lay.addWidget(list_scroll, 1)

        bk_row = QHBoxLayout()
        bk_row.setSpacing(6)
        btn_exp_all = Button(t("btn_export_all"), variant="glass", icon_name="save", fixed_compact=True)
        btn_exp_all.setFixedHeight(32)
        btn_exp_all.clicked.connect(self._export_full_backup)
        btn_imp_all = Button(t("btn_import_all"), variant="ghost", icon_name="folder", fixed_compact=True)
        btn_imp_all.setFixedHeight(32)
        btn_imp_all.clicked.connect(self._import_full_backup)
        bk_row.addWidget(btn_exp_all, 1)
        bk_row.addWidget(btn_imp_all, 1)
        l_lay.addLayout(bk_row)

        self.left_card.setVisible(not self._sidebar_collapsed)
        body.addWidget(self.left_card)

        # Empty-State Card
        self.empty_state_card = GlassCard(accent_tint=True, parent=self)
        em_lay = QVBoxLayout(self.empty_state_card)
        em_lay.setContentsMargins(40, 40, 40, 40)
        em_lay.setAlignment(Qt.AlignmentFlag.AlignCenter)
        em_lay.setSpacing(16)

        em_logo = QLabel()
        em_logo.setPixmap(logo_pixmap(72, 2.0))
        em_logo.setAlignment(Qt.AlignmentFlag.AlignCenter)
        em_lay.addWidget(em_logo)

        em_title = QLabel(t("empty_macros_title"))
        em_title.setFont(make_font(20, QFont.Weight.ExtraBold))
        em_title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        em_title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        em_lay.addWidget(em_title)

        em_sub = QLabel(t("empty_macros_subtitle"))
        em_sub.setFont(make_font(13, QFont.Weight.Medium))
        em_sub.setWordWrap(True)
        em_sub.setAlignment(Qt.AlignmentFlag.AlignCenter)
        em_sub.setFixedWidth(440)
        em_sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        em_lay.addWidget(em_sub, 0, Qt.AlignmentFlag.AlignCenter)

        em_lay.addSpacing(8)
        self.btn_empty_new = Button(t("btn_new_smart_key"), variant="primary", icon_name="plus")
        self.btn_empty_new.setFixedSize(220, 46)
        self.btn_empty_new.clicked.connect(self._create_new_blank_combo)
        em_lay.addWidget(self.btn_empty_new, 0, Qt.AlignmentFlag.AlignCenter)

        self.empty_state_card.hide()
        body.addWidget(self.empty_state_card, 1)

        # Right Editor Card
        self.right_card = GlassCard(parent=self)
        r_lay = QVBoxLayout(self.right_card)
        r_lay.setContentsMargins(16, 10, 16, 10)
        r_lay.setSpacing(7)

        # STEP ①
        s1_hdr_row = QHBoxLayout()
        s1_lbl = QLabel(t("step1_header"))
        s1_lbl.setFont(make_font(13, QFont.Weight.ExtraBold))
        s1_lbl.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        s1_hdr_row.addWidget(s1_lbl, 1)

        self.btn_no_key = Button(t("btn_clear_hotkey"), variant="ghost", tint_hex="#64748B")
        self.btn_no_key.setFixedHeight(28)
        self.btn_no_key.clicked.connect(self._clear_trigger_for_manual_mode)
        s1_hdr_row.addWidget(self.btn_no_key)
        r_lay.addLayout(s1_hdr_row)

        key_and_name_row = QHBoxLayout()
        key_and_name_row.setSpacing(10)

        self.name_edit = QLineEdit()
        self.name_edit.setPlaceholderText(t("ph_macro_title_opt"))
        self.name_edit.setFixedHeight(40)
        self.name_edit.setFixedWidth(185)
        self.name_edit.setStyleSheet(
            f"""
            QLineEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('text')};
                border: 1px solid {T.token_hex('border_hi')};
                border-radius: 11px;
                padding: 4px 12px;
                font-family: '{get_lang_font_family()}';
                font-size: 13px;
                font-weight: bold;
            }}
            """
        )
        self.name_edit.textEdited.connect(self._on_custom_name_edited)

        self.hero_key_btn = KeyCaptureButton(allow_chord=True, state=self.state, large_hero=True)
        self.hero_key_btn.setFixedHeight(40)
        self.hero_key_btn.chordCaptured.connect(self._on_trigger_chord_captured)

        key_and_name_row.addWidget(self.name_edit)
        key_and_name_row.addWidget(self.hero_key_btn, 1)
        r_lay.addLayout(key_and_name_row)

        # Row 2: Target App + Execution Mode + Re-Trigger Policy ALL on ONE clean Drop-Panel Row!
        target_row = QHBoxLayout()
        target_row.setSpacing(8)

        app_lbl = QLabel(t("lbl_target_app"))
        app_lbl.setFont(make_font(12, QFont.Weight.Bold))
        app_lbl.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

        self.app_combo = SearchCombo(list_running_window_apps_with_paths(), selected_key="")
        self.app_combo.setMinimumWidth(165)
        self.app_combo.currentChanged.connect(self._on_target_app_changed)

        btn_refresh_apps = IconButton("search", size=34, variant="glass")
        btn_refresh_apps.clicked.connect(self._refresh_running_apps_dropdown)

        mode_lbl = QLabel(t("lbl_repeat_mode"))
        mode_lbl.setFont(make_font(12, QFont.Weight.Bold))
        mode_lbl.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

        self.mode_seg = SearchCombo(
            [
                ("once", t("mode_once")),
                ("repeat_hold", t("mode_repeat_hold")),
                ("toggle_loop", t("mode_toggle_loop")),
            ],
            selected_key="once",
        )
        self.mode_seg.setMinimumWidth(145)
        self.mode_seg.currentChanged.connect(self._on_mode_changed)

        self.retrig_lbl = QLabel(t("lbl_retrigger_policy"))
        self.retrig_lbl.setFont(make_font(12, QFont.Weight.Bold))
        self.retrig_lbl.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")

        self.retrigger_seg = SearchCombo(
            [
                ("ignore", t("retrigger_ignore")),
                ("restart", t("retrigger_restart")),
                ("queue", t("retrigger_queue")),
            ],
            selected_key="ignore",
        )
        self.retrigger_seg.setMinimumWidth(145)
        self.retrigger_seg.currentChanged.connect(self._on_retrigger_policy_changed)

        self.lbl_stop_hk = QLabel(t("lbl_combo_stop_hotkey"))
        self.lbl_stop_hk.setFont(make_font(12, QFont.Weight.ExtraBold))
        self.lbl_stop_hk.setStyleSheet(f"color: {T.token_hex('danger')}; background: transparent;")

        self.stop_hk_btn = KeyCaptureButton(
            current_key="",
            allow_chord=True,
            state=self.state,
            large_hero=False,
        )
        self.stop_hk_btn.setFixedHeight(32)
        self.stop_hk_btn.setMinimumWidth(140)
        self.stop_hk_btn.chordCaptured.connect(self._on_stop_hotkey_captured)

        target_row.addWidget(app_lbl)
        target_row.addWidget(self.app_combo, 2)
        target_row.addWidget(btn_refresh_apps)
        target_row.addSpacing(4)
        target_row.addWidget(mode_lbl)
        target_row.addWidget(self.mode_seg, 1)
        target_row.addSpacing(4)
        target_row.addWidget(self.retrig_lbl)
        target_row.addWidget(self.retrigger_seg, 1)
        target_row.addWidget(self.lbl_stop_hk)
        target_row.addWidget(self.stop_hk_btn, 1)
        r_lay.addLayout(target_row)

        # Row 2b: Multi-Application Exclusion Bar (`إستثناء:` + Wide Drop Panel + 1-Icon Add Button + Distinct Framed Horizontal Scroll)
        excl_row = QHBoxLayout()
        excl_row.setSpacing(8)

        excl_lbl = QLabel(t("lbl_excluded_apps"))
        excl_lbl.setFont(make_font(12, QFont.Weight.ExtraBold))
        excl_lbl.setStyleSheet(f"color: {T.token_hex('danger')}; background: transparent;")

        self.exclude_app_combo = SearchCombo([], selected_key="")
        self.exclude_app_combo.setMinimumWidth(240)
        self.exclude_app_combo.setFixedHeight(32)

        self.btn_add_excl_app = IconButton(
            "plus",
            size=32,
            variant="danger",
            tooltip=t("btn_add_excluded_app"),
        )
        self.btn_add_excl_app.clicked.connect(self._on_add_excluded_app)

        self.excluded_scroll_box = TintedScrollBox(tint_hex="#EF4444")
        self.excluded_scroll_box.setFixedHeight(40)
        ex_box_lay = QHBoxLayout(self.excluded_scroll_box)
        ex_box_lay.setContentsMargins(6, 2, 6, 2)
        ex_box_lay.setSpacing(0)

        self.excluded_chips_scroll = QScrollArea()
        self.excluded_chips_scroll.setWidgetResizable(True)
        self.excluded_chips_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.excluded_chips_scroll.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.excluded_chips_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.excluded_chips_scroll.setStyleSheet(modern_scroll_style())

        self.excluded_chips_w = QWidget()
        self.excluded_chips_w.setStyleSheet("background: transparent;")
        self.excluded_chips_lay = QHBoxLayout(self.excluded_chips_w)
        self.excluded_chips_lay.setContentsMargins(4, 1, 4, 1)
        self.excluded_chips_lay.setSpacing(6)
        self.excluded_chips_scroll.setWidget(self.excluded_chips_w)
        ex_box_lay.addWidget(self.excluded_chips_scroll)

        self.btn_clear_excl_apps = IconButton(
            "trash",
            size=32,
            variant="ghost",
            tooltip=t("btn_clear_excluded_apps"),
        )
        self.btn_clear_excl_apps.clicked.connect(self._on_clear_excluded_apps)
        self.btn_clear_excl_apps.hide()

        excl_row.addWidget(excl_lbl)
        excl_row.addWidget(self.exclude_app_combo, 2)
        excl_row.addWidget(self.btn_add_excl_app)
        excl_row.addWidget(self.excluded_scroll_box, 2)
        excl_row.addWidget(self.btn_clear_excl_apps)
        r_lay.addLayout(excl_row)

        # Steps Hidden Summary Holder (takes stretch=1 when steps are hidden so top panels NEVER stretch!)
        self.combo_steps_hidden_holder = QWidget()
        self.combo_steps_hidden_holder.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.combo_steps_hidden_holder.setStyleSheet("background: transparent;")
        csh_outer = QVBoxLayout(self.combo_steps_hidden_holder)
        csh_outer.setContentsMargins(0, 0, 0, 0)
        csh_outer.setSpacing(0)

        self.combo_steps_hidden_card = GlassCard()
        self.combo_steps_hidden_card.setFixedHeight(62)
        csh_lay = QHBoxLayout(self.combo_steps_hidden_card)
        csh_lay.setContentsMargins(16, 10, 16, 10)
        csh_lay.setSpacing(12)
        self.lbl_combo_steps_hidden = QLabel(t("lbl_steps_hidden_hint"))
        self.lbl_combo_steps_hidden.setWordWrap(True)
        self.lbl_combo_steps_hidden.setFont(make_font(12, QFont.Weight.DemiBold))
        self.lbl_combo_steps_hidden.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        self.btn_combo_open_steps = Button(t("btn_edit_steps"), variant="primary", icon_name="bolt")
        self.btn_combo_open_steps.setFixedHeight(34)
        self.btn_combo_open_steps.clicked.connect(lambda: self._set_combo_steps_visible(True))
        csh_lay.addWidget(self.lbl_combo_steps_hidden, 1)
        csh_lay.addWidget(self.btn_combo_open_steps)
        csh_outer.addWidget(self.combo_steps_hidden_card)
        csh_outer.addStretch(1)
        r_lay.addWidget(self.combo_steps_hidden_holder, 1)

        # Edit & History Toolbar (shown when editing steps)
        self.combo_edit_toolbar = GlassCard(accent_tint=True)
        self.combo_edit_toolbar.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        cet_lay = QHBoxLayout(self.combo_edit_toolbar)
        cet_lay.setContentsMargins(12, 6, 12, 6)
        cet_lay.setSpacing(6)

        self.s2_lbl = QLabel(t("step2_header"))
        self.s2_lbl.setFont(make_font(12, QFont.Weight.ExtraBold))
        self.s2_lbl.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.btn_combo_undo = Button(t("btn_undo"), variant="ghost", icon_name="chevron_up")
        self.btn_combo_undo.setFixedHeight(30)
        self.btn_combo_undo.clicked.connect(self._undo_combo_steps)

        self.btn_combo_redo = Button(t("btn_redo"), variant="ghost", icon_name="chevron_down")
        self.btn_combo_redo.setFixedHeight(30)
        self.btn_combo_redo.clicked.connect(self._redo_combo_steps)

        self.btn_combo_save_curr = Button(t("btn_save_changes"), variant="primary", icon_name="save")
        self.btn_combo_save_curr.setFixedHeight(30)
        self.btn_combo_save_curr.clicked.connect(self._save_combo_changes_now)

        self.btn_combo_save_new = Button(t("btn_save_as_new"), variant="glass", icon_name="plus", tint_hex="#10B981")
        self.btn_combo_save_new.setFixedHeight(30)
        self.btn_combo_save_new.clicked.connect(self._save_combo_as_new)

        self.btn_combo_hide_steps = Button(t("btn_hide_steps"), variant="ghost", icon_name="close")
        self.btn_combo_hide_steps.setFixedHeight(30)
        self.btn_combo_hide_steps.clicked.connect(lambda: self._set_combo_steps_visible(False))

        cet_lay.addWidget(self.s2_lbl, 1)
        cet_lay.addWidget(self.btn_combo_undo)
        cet_lay.addWidget(self.btn_combo_redo)
        cet_lay.addWidget(self.btn_combo_save_curr)
        cet_lay.addWidget(self.btn_combo_save_new)
        cet_lay.addWidget(self.btn_combo_hide_steps)
        r_lay.addWidget(self.combo_edit_toolbar)

        b_add_key = Button(
            t("step_add_key_tap"),
            variant="glass",
            icon_name="keyboard",
            tint_hex=STEP_CATEGORY_COLORS["key_tap"],
        )
        b_add_key.clicked.connect(lambda: self._add_step("key_tap", "W", 20, hold_ms=0))

        b_add_wait = Button(
            t("step_add_wait"),
            variant="glass",
            icon_name="clock",
            tint_hex=STEP_CATEGORY_COLORS["wait"],
        )
        b_add_wait.clicked.connect(lambda: self._add_step("wait", "Wait", 1000))

        b_add_click = Button(
            t("step_add_mouse_click"),
            variant="glass",
            icon_name="mouse",
            tint_hex=STEP_CATEGORY_COLORS["mouse_click"],
        )
        b_add_click.clicked.connect(lambda: self._add_step("mouse_click", "left", 25))

        b_add_scroll = Button(
            t("step_add_mouse_scroll"),
            variant="glass",
            icon_name="mouse",
            tint_hex=STEP_CATEGORY_COLORS["mouse_scroll"],
        )
        b_add_scroll.clicked.connect(lambda: self._add_step("mouse_scroll", "down", 25, scroll_amount=3))

        b_add_folder = Button(
            t("step_add_folder"),
            variant="glass",
            icon_name="folder",
            tint_hex=STEP_CATEGORY_COLORS["open_folder"],
        )
        b_add_folder.clicked.connect(lambda: self._add_step("open_folder", "C:\\", 20))

        b_add_ps = Button(
            t("step_add_ps"),
            variant="glass",
            icon_name="terminal",
            tint_hex=STEP_CATEGORY_COLORS["powershell"],
        )
        b_add_ps.clicked.connect(lambda: self._add_step("powershell", "Write-Host 'Hello'", 20))

        b_add_cmd = Button(
            t("step_add_cmd"),
            variant="glass",
            icon_name="terminal",
            tint_hex=STEP_CATEGORY_COLORS["cmd"],
        )
        b_add_cmd.clicked.connect(lambda: self._add_step("cmd", "echo Hello", 20))

        b_add_type_text = Button(
            t("step_add_type_text"),
            variant="glass",
            icon_name="keyboard",
            tint_hex=STEP_CATEGORY_COLORS["type_text"],
        )
        b_add_type_text.clicked.connect(lambda: self._add_step("type_text", "Hello V-MacroPlus", 20))

        b_add_py = Button(
            t("step_add_python_script"),
            variant="glass",
            icon_name="bolt",
            tint_hex=STEP_CATEGORY_COLORS["python_script"],
        )
        b_add_py.clicked.connect(
            lambda: self._add_step(
                "python_script",
                'for i in range(3):\n    print(f"Step {i+1}")\n    press("Enter")\n    wait(200)',
                20,
            )
        )

        self.step_adder_bar = ResponsiveFlowBar(
            [
                b_add_key,
                b_add_wait,
                b_add_click,
                b_add_scroll,
                b_add_type_text,
                b_add_py,
                b_add_folder,
                b_add_ps,
                b_add_cmd,
            ],
            row_height=32,
            h_spacing=6,
            v_spacing=6,
            parent=self.right_card,
        )
        r_lay.addWidget(self.step_adder_bar)

        # Scrollable Steps Area
        steps_scroll = QScrollArea()
        self.steps_scroll = steps_scroll
        steps_scroll.setWidgetResizable(True)
        steps_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        steps_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        steps_scroll.setStyleSheet(modern_scroll_style())
        self.steps_container = QWidget()
        self.steps_container.setStyleSheet("background: transparent;")
        self.steps_vbox = QVBoxLayout(self.steps_container)
        self.steps_vbox.setContentsMargins(0, 0, 6, 0)
        self.steps_vbox.setSpacing(6)
        steps_scroll.setWidget(self.steps_container)
        r_lay.addWidget(steps_scroll, 1)
        self._set_combo_steps_visible(False)

        # STEP ③: Footer Bar
        s3_bar = QHBoxLayout()
        s3_bar.setSpacing(10)

        s3_lbl = QLabel(t("step3_header"))
        s3_lbl.setFont(make_font(13, QFont.Weight.ExtraBold))
        s3_lbl.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.combo_enable_tog = ToggleSwitch(False)
        self.combo_enable_tog.toggled.connect(self._on_current_enabled_toggled)

        after_lbl = QLabel(t("lbl_run_after"))
        after_lbl.setFont(make_font(12, QFont.Weight.Bold))
        after_lbl.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")

        self.num_start_delay = NumberInput(
            value=1000.0,
            min_val=0.0,
            max_val=60000.0,
            step=500.0,
            decimals=0,
            suffix="ms",
        )
        self.num_start_delay.setFixedWidth(132)
        self.num_start_delay.valueChanged.connect(self._on_start_delay_changed)

        self.btn_del_combo = Button(t("btn_delete"), variant="danger", icon_name="trash")
        self.btn_del_combo.clicked.connect(self._delete_selected_combo)

        s3_bar.addWidget(s3_lbl)
        s3_bar.addWidget(self.combo_enable_tog)
        s3_bar.addStretch(1)
        s3_bar.addWidget(after_lbl)
        s3_bar.addWidget(self.num_start_delay)
        s3_bar.addWidget(self.btn_del_combo)
        r_lay.addLayout(s3_bar)

        body.addWidget(self.right_card, 1)
        root.addLayout(body, 1)

    def _toggle_macros_sidebar(self) -> None:
        self._sidebar_collapsed = not self._sidebar_collapsed
        setattr(self.state, "combos_sidebar_collapsed", self._sidebar_collapsed)
        self.left_card.setVisible(not self._sidebar_collapsed)
        lbl = t("btn_show_macros_sidebar") if self._sidebar_collapsed else t("btn_hide_macros_sidebar")
        self.btn_toggle_sidebar.setText(lbl)

    def _connect_signals(self) -> None:
        self.state.combos_list_updated.connect(self._on_combos_updated)
        self.state.combos_master_changed.connect(
            lambda v: self.master_tog.setChecked(v, emit_signal=False)
        )

    def _export_full_backup(self) -> None:
        path, _ = QFileDialog.getSaveFileName(
            self,
            t("btn_export_all"),
            "V-MacroPlus_Full_Backup.json",
            "JSON Backup (*.json)",
        )
        if path:
            self.engine.export_all_backup(path)

    def _import_full_backup(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self,
            t("btn_import_all"),
            "",
            "JSON Backup (*.json)",
        )
        if path:
            self.engine.import_all_backup(path)

    def _on_combos_updated(self) -> None:
        self._refresh_left_list()
        self._load_selected_into_editor()

    def _current_combo_dict(self) -> Optional[Dict[str, Any]]:
        if 0 <= self._selected_idx < len(self.state.smart_combos):
            return self.state.smart_combos[self._selected_idx]
        return None

    def _auto_save(self) -> None:
        self.engine.save_combos_async()
        self._refresh_left_list()

    def _refresh_left_list(self) -> None:
        while self.list_vbox.count():
            it = self.list_vbox.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.deleteLater()

        is_ar = is_rtl()
        for idx, c_dict in enumerate(self.state.smart_combos):
            sc_obj = SmartCombo.from_dict(c_dict)
            short_title = sc_obj.sidebar_label(is_ar=is_ar)

            row_w = QWidget()
            rl = QHBoxLayout(row_w)
            rl.setContentsMargins(0, 1, 0, 1)
            rl.setSpacing(6)

            tog = ToggleSwitch(bool(c_dict.get("enabled", False)))
            tog.toggled.connect(lambda v, i=idx: self._toggle_list_item(i, v))

            btn = Button(
                short_title,
                variant="primary" if idx == self._selected_idx else "ghost",
                fixed_compact=True,
            )
            btn.setFixedHeight(34)
            btn.clicked.connect(lambda i=idx: self._select_combo(i))

            btn_run_sidebar = IconButton(
                "play",
                size=32,
                variant="ok",
                tooltip=tl(f"تشغيل «{short_title}» مرة واحدة الآن", f"Run '{short_title}' once now"),
            )
            btn_run_sidebar.clicked.connect(lambda i=idx: self._run_combo_from_sidebar(i))

            rl.addWidget(tog)
            rl.addWidget(btn, 1)
            rl.addWidget(btn_run_sidebar)
            self.list_vbox.addWidget(row_w)

        self.list_vbox.addStretch(1)

    def _run_combo_from_sidebar(self, idx: int) -> None:
        if not (0 <= idx < len(self.state.smart_combos)):
            return
        c = self.state.smart_combos[idx]
        if not c.get("steps"):
            return
        if str(c.get("mode", "once")) in ("repeat_hold", "toggle_loop") and not str(c.get("stop_hotkey", "")).strip():
            self._select_combo(idx)
            self.state.toast_requested.emit(t("err_combo_stop_hotkey_required"), "danger")
            self.stop_hk_btn._start_capture()
            return
        delay = int(c.get("start_delay_ms", 1000))
        sc_obj = SmartCombo.from_dict(c)
        title = sc_obj.sidebar_label(is_ar=is_rtl())
        self.state.toast_requested.emit(
            tl(f"سيتم تشغيل «{title}» مرة واحدة بعد {delay} ms...", f"Running '{title}' once in {delay} ms..."),
            "ok",
        )
        self.engine.execute_smart_combo(c, initial_delay_ms=delay)

    def _push_combo_undo(self) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        self._undo_steps_stack.append(copy.deepcopy(c.get("steps", [])))
        if len(self._undo_steps_stack) > 40:
            self._undo_steps_stack.pop(0)
        self._redo_steps_stack.clear()
        self._sync_combo_undo_redo()

    def _sync_combo_undo_redo(self) -> None:
        if hasattr(self, "btn_combo_undo"):
            self.btn_combo_undo.setEnabled(bool(self._undo_steps_stack))
        if hasattr(self, "btn_combo_redo"):
            self.btn_combo_redo.setEnabled(bool(self._redo_steps_stack))

    def _undo_combo_steps(self) -> None:
        c = self._current_combo_dict()
        if not self._undo_steps_stack:
            if self._last_deleted_combo is not None:
                self._undo_last_delete()
            return
        if c is not None:
            self._redo_steps_stack.append(copy.deepcopy(c.get("steps", [])))
            c["steps"] = self._undo_steps_stack.pop()
            self._rebuild_steps_ui()
            self._auto_save()
            self._sync_combo_undo_redo()

    def _redo_combo_steps(self) -> None:
        c = self._current_combo_dict()
        if not self._redo_steps_stack or c is None:
            return
        self._undo_steps_stack.append(copy.deepcopy(c.get("steps", [])))
        c["steps"] = self._redo_steps_stack.pop()
        self._rebuild_steps_ui()
        self._auto_save()
        self._sync_combo_undo_redo()

    def _set_combo_steps_visible(self, visible: bool) -> None:
        self._steps_editor_visible = bool(visible)
        self.combo_steps_hidden_holder.setVisible(not visible)
        self.combo_edit_toolbar.setVisible(visible)
        self.step_adder_bar.setVisible(visible)
        self.steps_scroll.setVisible(visible)
        c = self._current_combo_dict()
        cnt = len(c.get("steps", [])) if c else 0
        if not visible:
            self.lbl_combo_steps_hidden.setText(
                f"{t('lbl_steps_hidden_hint')} ({fmt_num(cnt)} {t('unit_steps')})"
            )
        else:
            self._sync_combo_undo_redo()

    def _save_combo_changes_now(self) -> None:
        self._auto_save()
        self.state.toast_requested.emit(t("toast_changes_saved"), "ok")

    def _save_combo_as_new(self) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        dup = copy.deepcopy(c)
        dup["id"] = f"combo_{int(time.time() * 1000)}"
        dup["name"] = f"{c.get('name') or 'Macro'} (Copy)"
        dup["trigger_key"] = ""
        dup["trigger_scan"] = 0
        dup["trigger_chord"] = []
        dup["enabled"] = False
        self.state.smart_combos.insert(0, dup)
        self._selected_idx = 0
        self._auto_save()
        self._load_selected_into_editor()
        self.state.toast_requested.emit(t("toast_changes_saved"), "ok")

    def _select_combo(self, idx: int) -> None:
        self._selected_idx = idx
        self._undo_steps_stack.clear()
        self._redo_steps_stack.clear()
        self._steps_editor_visible = False
        self._refresh_left_list()
        self._load_selected_into_editor()

    def _toggle_list_item(self, idx: int, enabled: bool) -> None:
        if 0 <= idx < len(self.state.smart_combos):
            c = self.state.smart_combos[idx]
            if enabled and not c.get("trigger_key"):
                self.state.toast_requested.emit(
                    tl("هذا الماكرو يدوي بدون زرار — اضغط على زر ▶ الأخضر بجانبه لتشغيله!", "Manual macro — click the green ▶ button next to it to run!"),
                    "info",
                )
                self._select_combo(idx)
                self._refresh_left_list()
                return
            if enabled and str(c.get("mode", "once")) in ("repeat_hold", "toggle_loop") and not str(c.get("stop_hotkey", "")).strip():
                self.state.toast_requested.emit(t("err_combo_stop_hotkey_required"), "danger")
                self._select_combo(idx)
                self._refresh_left_list()
                self.stop_hk_btn._start_capture()
                return
            c["enabled"] = enabled
            if idx == self._selected_idx:
                self.combo_enable_tog.setChecked(enabled, emit_signal=False)
            self.engine.save_combos_async()

    def _load_selected_into_editor(self) -> None:
        c = self._current_combo_dict()
        if c is None:
            self.right_card.hide()
            self.empty_state_card.show()
            return

        self.empty_state_card.hide()
        self.right_card.show()

        self.name_edit.setText(str(c.get("name", "")).replace("Key_0", "Calculator"))
        self.hero_key_btn.setKeyData(
            str(c.get("trigger_key", "")),
            int(c.get("trigger_scan", 0)),
            bool(c.get("trigger_extended", False)),
            c.get("trigger_chord", []),
        )
        cur_mode = str(c.get("mode", "once"))
        stop_hk = str(c.get("stop_hotkey", "")).strip()
        if cur_mode in ("repeat_hold", "toggle_loop") and not stop_hk and c.get("enabled", False):
            c["enabled"] = False
        self.combo_enable_tog.setChecked(bool(c.get("enabled", False)), emit_signal=False)
        self.mode_seg.setSelectedKey(cur_mode, emit_signal=False)
        self.retrigger_seg.setSelectedKey(str(c.get("retrigger_policy", "ignore")), emit_signal=False)
        self.stop_hk_btn.setKeyData(stop_hk, 0, False, [])
        self._sync_mode_stop_hotkey_ui(cur_mode)
        self.num_start_delay.setValue(float(c.get("start_delay_ms", 1000)), emit_signal=False)
        self._refresh_running_apps_dropdown()
        self._rebuild_steps_ui()
        self._set_combo_steps_visible(self._steps_editor_visible)

    def _rebuild_steps_ui(self) -> None:
        self._step_rows.clear()
        while self.steps_vbox.count():
            it = self.steps_vbox.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.deleteLater()

        c = self._current_combo_dict()
        if c is None:
            return

        steps = c.get("steps", [])
        for idx, st in enumerate(steps):
            row = _StepRowWidget(idx, st, self.state, self.steps_container)
            row.changed.connect(self._auto_save)
            row.removed.connect(self._remove_step_at)
            row.moveUp.connect(self._move_step_up)
            row.moveDown.connect(self._move_step_down)
            row.testSingleStep.connect(self._test_single_step_at)
            row.dragMoveRequested.connect(self._on_step_drag_moved)
            row.dragFinished.connect(self._auto_save)
            self.steps_vbox.addWidget(row)
            self._step_rows.append(row)

        self.steps_vbox.addStretch(1)

    def _on_step_drag_moved(self, dragged_row: _StepRowWidget, global_y: int) -> None:
        c = self._current_combo_dict()
        if c is None or dragged_row not in self._step_rows:
            return
        steps = c.get("steps", [])
        cur_idx = self._step_rows.index(dragged_row)
        local_y = self.steps_container.mapFromGlobal(QPoint(0, global_y)).y()

        target_idx = cur_idx
        for i, rw in enumerate(self._step_rows):
            ry = get_row_logical_y(rw)
            rh = max(24, rw.height())
            if i < cur_idx and local_y < (ry + rh * 0.55):
                target_idx = i
                break
            elif i > cur_idx and local_y > (ry + rh * 0.45):
                target_idx = i

        if target_idx != cur_idx and 0 <= target_idx < len(steps):
            if smooth_reorder_vbox_widget(self.steps_vbox, self._step_rows, cur_idx, target_idx):
                self._push_combo_undo()
                item = steps.pop(cur_idx)
                steps.insert(target_idx, item)
                for idx_i, rw in enumerate(self._step_rows):
                    rw.set_step_index(idx_i)

    def _on_custom_name_edited(self, txt: str) -> None:
        c = self._current_combo_dict()
        if c is not None:
            c["name"] = txt.strip()
            self._auto_save()

    def _on_trigger_chord_captured(self, chord: List[Tuple[int, bool]], lbl: str) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        c["trigger_key"] = lbl
        c["trigger_chord"] = [[sc, 1 if ext else 0] for sc, ext in chord if sc > 0]
        if chord:
            c["trigger_scan"] = chord[-1][0]
            c["trigger_extended"] = chord[-1][1]
            is_rep = str(c.get("mode", "once")) in ("repeat_hold", "toggle_loop")
            has_stop = bool(str(c.get("stop_hotkey", "")).strip())
            can_en = (not is_rep) or has_stop
            c["enabled"] = can_en
            self.combo_enable_tog.setChecked(can_en, emit_signal=False)
            if is_rep and not has_stop:
                self.state.toast_requested.emit(t("err_combo_stop_hotkey_required"), "danger")
                self.stop_hk_btn._start_capture()
            if not c.get("name"):
                c["name"] = lbl
                self.name_edit.setText(lbl)
        else:
            c["trigger_scan"] = 0
            c["trigger_extended"] = False
            c["enabled"] = False
            self.combo_enable_tog.setChecked(False, emit_signal=False)
        self._auto_save()

    def _clear_trigger_for_manual_mode(self) -> None:
        self.hero_key_btn.clearKey()
        c = self._current_combo_dict()
        if c is not None:
            c["trigger_key"] = ""
            c["trigger_scan"] = 0
            c["trigger_extended"] = False
            c["trigger_chord"] = []
            c["enabled"] = False
            if not c.get("name"):
                c["name"] = tl("ماكرو يدوي", "Manual Macro")
                self.name_edit.setText(c["name"])
            self.combo_enable_tog.setChecked(False, emit_signal=False)
            self._auto_save()

    def _add_step(
        self,
        step_type: str,
        value: str,
        delay_ms: int,
        hold_ms: int = 0,
        scroll_amount: int = 3,
    ) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        self._push_combo_undo()
        sc, ext = scancode_for_label(value) if step_type in ("key_tap", "key_hold", "key_down", "key_up") else (0, False)
        cx, cy = get_cursor_pos() if step_type in ("mouse_click", "mouse_down", "mouse_up", "mouse_move") else (500, 500)
        c.setdefault("steps", []).append(
            ComboStep(
                step_type=step_type,
                value=value,
                scan_code=sc,
                is_extended=ext,
                delay_ms=delay_ms,
                hold_ms=hold_ms,
                scroll_amount=scroll_amount,
                x=cx,
                y=cy,
                use_coords=True,
            ).to_dict()
        )
        self._rebuild_steps_ui()
        self._auto_save()

    def _remove_step_at(self, idx: int) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        steps = c.get("steps", [])
        if 0 <= idx < len(steps):
            self._push_combo_undo()
            steps.pop(idx)
            self._rebuild_steps_ui()
            self._auto_save()

    def _move_step_up(self, idx: int) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        steps = c.get("steps", [])
        if 0 < idx < len(steps):
            self._push_combo_undo()
            steps[idx - 1], steps[idx] = steps[idx], steps[idx - 1]
            smooth_reorder_vbox_widget(self.steps_vbox, self._step_rows, idx, idx - 1)
            for idx_i, rw in enumerate(self._step_rows):
                rw.set_step_index(idx_i)
            self._auto_save()

    def _move_step_down(self, idx: int) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        steps = c.get("steps", [])
        if 0 <= idx < len(steps) - 1:
            self._push_combo_undo()
            steps[idx + 1], steps[idx] = steps[idx], steps[idx + 1]
            smooth_reorder_vbox_widget(self.steps_vbox, self._step_rows, idx, idx + 1)
            for idx_i, rw in enumerate(self._step_rows):
                rw.set_step_index(idx_i)
            self._auto_save()

    def _test_single_step_at(self, idx: int) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        steps = c.get("steps", [])
        if 0 <= idx < len(steps):
            delay = int(self.num_start_delay.value())
            self.state.toast_requested.emit(
                tl(f"سيتم تنفيذ الخطوة #{idx + 1} بعد {delay} ms...", f"Running step #{idx + 1} in {delay} ms..."),
                "info",
            )
            self.engine.execute_single_step_after(steps[idx], delay_ms=delay)

    def _on_start_delay_changed(self, ms: float) -> None:
        c = self._current_combo_dict()
        if c is not None:
            c["start_delay_ms"] = int(round(ms))
            self._auto_save()

    def _create_new_blank_combo(self) -> None:
        w_sc, w_ext = scancode_for_label("W")
        sp_sc, sp_ext = scancode_for_label("Space")
        new_c = SmartCombo(
            id=f"combo_{int(time.time() * 1000)}",
            name=f"Macro {len(self.state.smart_combos) + 1}",
            trigger_key="",
            trigger_scan=0,
            trigger_extended=False,
            trigger_chord=[],
            mode="once",
            target_process="",
            start_delay_ms=1000,
            enabled=False,
            steps=[
                ComboStep(step_type="key_tap", value="W", scan_code=w_sc, is_extended=w_ext, delay_ms=20, hold_ms=0),
                ComboStep(step_type="wait", value="Wait", delay_ms=1000),
                ComboStep(step_type="key_tap", value="Space", scan_code=sp_sc, is_extended=sp_ext, delay_ms=20, hold_ms=0),
            ],
        ).to_dict()
        self.state.smart_combos.insert(0, new_c)
        self._selected_idx = 0
        self._load_selected_into_editor()
        self._auto_save()

    def _delete_selected_combo(self) -> None:
        if not (0 <= self._selected_idx < len(self.state.smart_combos)):
            return
        self._last_deleted_combo = self.state.smart_combos.pop(self._selected_idx)
        self.btn_undo.show()
        self._selected_idx = max(0, min(len(self.state.smart_combos) - 1, self._selected_idx))
        self.engine.save_combos_async()
        self._refresh_left_list()
        self._load_selected_into_editor()

    def _undo_last_delete(self) -> None:
        if self._last_deleted_combo is not None:
            self.state.smart_combos.insert(self._selected_idx, self._last_deleted_combo)
            self._last_deleted_combo = None
            self.btn_undo.hide()
            self.engine.save_combos_async()
            self._refresh_left_list()
            self._load_selected_into_editor()

    def _refresh_running_apps_dropdown(self) -> None:
        c = self._current_combo_dict()
        cur_app = str(c.get("target_process", "")) if c else ""
        apps = list_running_window_apps_with_paths()
        if cur_app and cur_app not in [k for k, _, _ in apps]:
            apps.append((cur_app, cur_app, ""))
        self.app_combo.setItems(apps, selected_key=cur_app)

        only_specific = [(k, lbl, p) for (k, lbl, p) in apps if k]
        self.exclude_app_combo.setItems(
            only_specific if only_specific else [("", t("lbl_no_excluded_apps"), "")],
            selected_key=only_specific[0][0] if only_specific else "",
        )
        self._rebuild_excluded_apps_chips()

    def _rebuild_excluded_apps_chips(self) -> None:
        while self.excluded_chips_lay.count():
            it = self.excluded_chips_lay.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.deleteLater()

        c = self._current_combo_dict()
        excl_list = [str(x).strip() for x in (c.get("excluded_processes", []) if c else []) if str(x).strip()]
        self.btn_clear_excl_apps.setVisible(len(excl_list) >= 2)

        if not excl_list:
            lbl_none = QLabel(t("lbl_no_excluded_apps"))
            lbl_none.setFont(make_font(11, QFont.Weight.Medium))
            lbl_none.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            self.excluded_chips_lay.addWidget(lbl_none)
            self.excluded_chips_lay.addStretch(1)
            return

        for exe_name in excl_list:
            chip_btn = Button(
                f"🚫 {exe_name}  ×",
                variant="glass",
                tint_hex="#EF4444",
            )
            chip_btn.setFixedHeight(26)
            chip_btn.setFixedWidth(chip_btn.naturalCompactWidth() + 6)
            chip_btn.clicked.connect(lambda ex=exe_name: self._on_remove_excluded_app(ex))
            self.excluded_chips_lay.addWidget(chip_btn)
        self.excluded_chips_lay.addStretch(1)

    def _on_add_excluded_app(self) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        exe = (self.exclude_app_combo.currentKey() or "").strip()
        if not exe:
            return
        excl = [str(x).strip() for x in c.get("excluded_processes", []) if str(x).strip()]
        if exe.lower() not in [x.lower() for x in excl]:
            excl.append(exe)
            c["excluded_processes"] = excl
            self._rebuild_excluded_apps_chips()
            self._auto_save()

    def _on_remove_excluded_app(self, exe_name: str) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        excl = [str(x).strip() for x in c.get("excluded_processes", []) if str(x).strip()]
        c["excluded_processes"] = [x for x in excl if x.lower() != exe_name.lower()]
        self._rebuild_excluded_apps_chips()
        self._auto_save()

    def _on_clear_excluded_apps(self) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        c["excluded_processes"] = []
        self._rebuild_excluded_apps_chips()
        self._auto_save()

    def _on_target_app_changed(self, exe_name: str) -> None:
        c = self._current_combo_dict()
        if c is not None:
            c["target_process"] = exe_name
            self._auto_save()

    def _sync_mode_stop_hotkey_ui(self, mode_key: str) -> None:
        is_rep = mode_key in ("repeat_hold", "toggle_loop")
        self.lbl_stop_hk.setVisible(is_rep)
        self.stop_hk_btn.setVisible(is_rep)
        self.retrig_lbl.setVisible(not is_rep)
        self.retrigger_seg.setVisible(not is_rep)

    def _on_stop_hotkey_captured(self, chord: List[Tuple[int, bool]], lbl: str) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        clean = (lbl or "").strip()
        c["stop_hotkey"] = clean
        if clean and str(c.get("mode", "once")) in ("repeat_hold", "toggle_loop") and c.get("trigger_key"):
            c["enabled"] = True
            self.combo_enable_tog.setChecked(True, emit_signal=False)
        elif not clean and str(c.get("mode", "once")) in ("repeat_hold", "toggle_loop"):
            c["enabled"] = False
            self.combo_enable_tog.setChecked(False, emit_signal=False)
        self._auto_save()

    def _on_mode_changed(self, mode_key: str) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        c["mode"] = mode_key
        self._sync_mode_stop_hotkey_ui(mode_key)
        if mode_key in ("repeat_hold", "toggle_loop") and not str(c.get("stop_hotkey", "")).strip():
            c["enabled"] = False
            self.combo_enable_tog.setChecked(False, emit_signal=False)
            self.state.toast_requested.emit(t("err_combo_stop_hotkey_required"), "danger")
            self.stop_hk_btn._start_capture()
        self._auto_save()

    def _on_retrigger_policy_changed(self, policy: str) -> None:
        if 0 <= self._selected_idx < len(self.state.smart_combos):
            self.state.smart_combos[self._selected_idx]["retrigger_policy"] = policy
            self.engine.save_combos_async()

    def _on_current_enabled_toggled(self, val: bool) -> None:
        c = self._current_combo_dict()
        if c is None:
            return
        if val and not c.get("trigger_key"):
            self.combo_enable_tog.setChecked(False, emit_signal=False)
            self.state.toast_requested.emit(
                tl("هذا الماكرو يدوي بدون زرار — اضغط على زر ▶ الأخضر بجانبه في القائمة لتشغيله!", "Manual macro — click the green ▶ button next to it in the sidebar!"),
                "info",
            )
            return
        if val and str(c.get("mode", "once")) in ("repeat_hold", "toggle_loop") and not str(c.get("stop_hotkey", "")).strip():
            self.combo_enable_tog.setChecked(False, emit_signal=False)
            self.state.toast_requested.emit(t("err_combo_stop_hotkey_required"), "danger")
            self.stop_hk_btn._start_capture()
            return
        c["enabled"] = val
        self._auto_save()
