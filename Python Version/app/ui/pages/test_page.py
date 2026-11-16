# -*- coding: utf-8 -*-
"""
Interactive Test Lab & 20-Method Hardware/Background Input Studio Page:
- 100% localized across all 10 supported languages (`t(...)`).
- Supports Background Testing Mode (`فحص في الخلفية`) without focusing or switching to the target window.
- Provides 6 Test Action Modes including 3 full 20-Method Sweep Modes:
  1. `macro`: Test a saved Smart Key or Recording using the currently selected method.
  2. `type_text`: Type text or a Smart Variable Template (`key="Enter" {method_num}: ... {method_id} \\n`) via the selected method.
  3. `simple_key`: Tap a single key via the selected method.
  4. `sweep_20`: Sweep all 20 methods sequentially using an Editable Python Per-Method Script (`QPlainTextEdit`).
  5. `sweep_20_template`: Sweep all 20 methods sequentially using a Smart Key & Variable Template (`key="Enter"`, `\\n`, `wait=100`, `{method_num}`, `{method_id}`, `{method_name}`, `{category}`, `{target_app}`, `{target_hwnd}`, `{mode}`, `{time}`, `{date}`).
  6. `sweep_20_macro`: Sweep all 20 methods sequentially by running any user Smart Key / Saved Recording macro strictly through each method `1..20` (with optional variable template prefix).
- Two-line structured, BiDi-clean diagnostic log cards that re-render dynamically when the UI language changes.
"""

from __future__ import annotations
from datetime import datetime
import threading
import time
from typing import Any, Dict, List, Optional, Tuple
from PySide6.QtCore import QRectF, Qt, Signal
from PySide6.QtGui import QFont, QKeyEvent, QMouseEvent, QPainter, QPen
from PySide6.QtWidgets import (
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QPlainTextEdit,
    QScrollArea,
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
from app.domain.models import SmartCombo
from app.domain.autopresser_engine import (
    AUTOPRESSER_KEY_METHODS,
    BACKGROUND_HWND_KEY_METHODS,
    DEFAULT_PYTHON_SWEEP_SCRIPT,
    DEFAULT_TEMPLATE_SWEEP_STRING,
    SWEEP_TEMPLATE_VARIABLES,
    _find_best_input_child_hwnd,
    _get_window_class_name,
    dispatch_autopresser_key,
    execute_per_method_macro_test,
    execute_per_method_python_test,
    expand_and_execute_method_template,
    focus_and_bring_hwnd_to_front,
    format_diagnostic_reason,
    list_detailed_windows_for_picker,
)
from app.domain.win32_input import (
    IS_WINDOWS,
    label_for_scancode,
    resolve_effective_input_engine,
    usb_hid_usage_for_scancode,
)
from app.i18n import fmt_num, get_lang_font_family, is_rtl, t
from app.state import AppState
from app.theme import T
from app.ui.widgets import (
    Button,
    GlassCard,
    IconButton,
    KeyCaptureButton,
    NumberInput,
    SearchCombo,
    StepCard,
    TintedScrollBox,
    ToggleSwitch,
    make_font,
    modern_scroll_style,
)


def get_input_method_options() -> List[Tuple[str, str]]:
    return [
        ("sendinput_scancode", t("method_scancode")),
        ("sendinput_hybrid", t("method_hybrid")),
        ("keybd_mouse_event", t("method_keybd_event")),
        ("unicode", t("akp_method_unicode")),
        ("cpp_stealth_humanized", t("method_cpp_stealth")),
        ("cpp_vhid_ioctl_bridge", t("method_cpp_vhid")),
        ("cpp_interception_driver", t("method_cpp_interception")),
        ("cpp_winring0_ps2_port", t("method_cpp_ps2_port")),
        ("cpp_sendinput_batch_atomic", t("method_cpp_batch_atomic")),
        ("linux_uinput_xtest", t("method_linux_uinput")),
        ("postmessage", t("akp_method_postmessage")),
        ("sendmessage", t("akp_method_sendmessage")),
        ("post_child", t("akp_method_post_child")),
        ("post_children", t("akp_method_post_children")),
        ("post_active", t("akp_method_post_active")),
        ("attach_state", t("akp_method_attach_state")),
        ("guithread_focus", t("akp_method_guithread_focus")),
        ("wm_char_triplet", t("akp_method_wm_char_triplet")),
        ("ime_pipeline", t("akp_method_ime_pipeline")),
        ("sendnotify_tree", t("akp_method_sendnotify_tree")),
        ("pico_usb_hid", t("method_pico_hid")),
        ("arduino_usb_hid", t("method_arduino_hid")),
    ]


class _LiveInputSandboxBox(QWidget):
    """
    Compact single-row interactive focusable sandbox surface that captures and logs every
    key press and mouse click received while testing macros or pressing keys directly.
    """

    def __init__(self, state: AppState, on_log_entry, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.state = state
        self._on_log_entry = on_log_entry
        self.setFixedHeight(48)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.setCursor(Qt.CursorShape.PointingHandCursor)

        lay = QHBoxLayout(self)
        lay.setContentsMargins(14, 6, 14, 6)
        lay.setSpacing(10)

        self.test_input = QLineEdit(self)
        self.test_input.setPlaceholderText(t("sandbox_input_placeholder"))
        self.test_input.setFixedHeight(32)
        self.test_input.setMinimumWidth(290)
        self.test_input.setStyleSheet(
            f"""
            QLineEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('text')};
                border: 1px solid {T.token_hex('border_hi')};
                border-radius: 8px;
                padding: 3px 10px;
                font-family: '{get_lang_font_family()}';
                font-size: 12px;
                font-weight: bold;
            }}
            """
        )

        self.lbl_status = QLabel(t("sandbox_hint_status"))
        self.lbl_status.setFont(make_font(11, QFont.Weight.ExtraBold))
        self.lbl_status.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        lay.addWidget(self.test_input, 1)
        lay.addWidget(self.lbl_status, 1)

    def mousePressEvent(self, event: QMouseEvent) -> None:
        self.setFocus()
        btn_name = (
            "Left Click"
            if event.button() == Qt.MouseButton.LeftButton
            else "Right Click"
            if event.button() == Qt.MouseButton.RightButton
            else "Middle Click"
        )
        pos = event.globalPosition().toPoint()
        eff_info = getattr(self, '_cached_eff_info', None) or resolve_effective_input_engine(self.state.config.input_method, is_rtl())
        self._cached_eff_info = eff_info
        eff = eff_info["effective_method"]
        fb = f" ({t('log_fallback_from')} {eff_info['requested_method']})" if eff_info["is_fallback"] else ""
        plain_msg = (
            f"{t('log_recv_mouse')} {t('log_mouse_click_at')} ({btn_name}) "
            f"{t('log_at_coords')} ({pos.x()}, {pos.y()}) — {t('log_actual_engine')} {eff}{fb}"
        )
        self.lbl_status.setText(plain_msg)
        self._on_log_entry(
            {
                "kind": "mouse",
                "btn_name": btn_name,
                "x": pos.x(),
                "y": pos.y(),
                "requested_method": self.state.config.input_method,
            },
            "#10B981",
        )
        super().mousePressEvent(event)

    def keyPressEvent(self, event: QKeyEvent) -> None:
        hw_sc = int(event.nativeScanCode()) & 0xFF if hasattr(event, "nativeScanCode") else 0
        lbl = label_for_scancode(hw_sc, False) if hw_sc > 0 else (event.text() or "Key")
        hid_usage = usb_hid_usage_for_scancode(hw_sc, False)
        eff_info = getattr(self, '_cached_eff_info', None) or resolve_effective_input_engine(self.state.config.input_method, is_rtl())
        self._cached_eff_info = eff_info
        eff = eff_info["effective_method"]
        fb = f" ({t('log_fallback_from')} {eff_info['requested_method']})" if eff_info["is_fallback"] else ""
        plain_msg = (
            f"{t('log_recv_key')} [ {lbl} ] (ScanCode: 0x{hw_sc:02X} | USB HID: 0x{hid_usage:02X}) "
            f"— {t('log_actual_engine')} {eff}{fb}"
        )
        self.lbl_status.setText(plain_msg)
        self._on_log_entry(
            {
                "kind": "key",
                "lbl": lbl,
                "hw_sc": hw_sc,
                "hid_usage": hid_usage,
                "requested_method": self.state.config.input_method,
            },
            "#8B5CF6",
        )
        super().keyPressEvent(event)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(1.5, 1.5, -1.5, -1.5)
        focused = self.hasFocus() or self.test_input.hasFocus()
        p.setBrush(T.c("surface_hi" if focused else "bg1", 215))
        p.setPen(QPen(T.c("accent_hi" if focused else "border_hi"), 1.6 if focused else 1.1))
        p.drawRoundedRect(r, 12.0, 12.0)
        p.end()


class TestPage(QWidget):
    """
    Dedicated Test Lab & 20-Engine Hardware/Background Input Page:
    - Thread-safe `_thread_log_signal` for background test & sweep threads.
    - Background Test Mode (`فحص في الخلفية`) toggle so testing never steals focus when enabled.
    - 3 Complete 20-Method Sweep Modes:
      1. `sweep_20`: Editable Python Per-Method Script (`QPlainTextEdit`)
      2. `sweep_20_template`: Smart Key & Variable Template (`key="Enter" {method_num}: ... {method_id} \\n`)
      3. `sweep_20_macro`: Run any saved Smart Key or Recording across all 20 methods with optional template prefix
    """

    _thread_log_signal = Signal(dict, str)
    _sweep_finished_signal = Signal()

    def __init__(self, state: AppState, engine: MacroEngine, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.state = state
        if not hasattr(self.state, "test_lab_logs"):
            self.state.test_lab_logs = []
        self.engine = engine
        self._log_count = 0
        self._test_win_cache: List[Dict[str, Any]] = []
        self._sweep_running = False
        self._thread_log_signal.connect(self._add_structured_log)
        self._sweep_finished_signal.connect(self._on_sweep_finished_main_thread)

        self._build_ui()
        self._refresh_test_target_apps()
        self._populate_test_targets()
        self._restore_persistent_logs()

        self.state.combos_list_updated.connect(self._populate_test_targets)
        self.state.macros_library_updated.connect(self._populate_test_targets)
        self.state.python_scripts_updated.connect(self._refresh_py_presets_combo)

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(16, 6, 16, 10)
        root.setSpacing(6)

        # Header Card
        top_card = GlassCard(accent_tint=True, parent=self)
        t_lay = QHBoxLayout(top_card)
        t_lay.setContentsMargins(16, 7, 16, 7)
        t_lay.setSpacing(12)

        t_col = QVBoxLayout()
        t_col.setSpacing(1)
        title = QLabel(t("test_title"))
        title.setFont(make_font(14, QFont.Weight.ExtraBold))
        title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        sub = QLabel(t("test_subtitle"))
        sub.setFont(make_font(11, QFont.Weight.Medium))
        sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        t_col.addWidget(title)
        t_col.addWidget(sub)
        t_lay.addLayout(t_col, 1)

        btn_clear_log = Button(t("btn_clear_test_log"), variant="ghost", icon_name="trash")
        btn_clear_log.setFixedHeight(30)
        btn_clear_log.clicked.connect(self._clear_log)
        t_lay.addWidget(btn_clear_log)
        root.addWidget(top_card)

        # Method & Target Selection Card
        ctrl_card = GlassCard(parent=self)
        c_lay = QVBoxLayout(ctrl_card)
        c_lay.setContentsMargins(16, 10, 16, 10)
        c_lay.setSpacing(6)

        # Row 1: Input Simulation Method Selector (20 Engines)
        r1 = QHBoxLayout()
        r1.setSpacing(8)
        lbl_m = QLabel(t("lbl_input_method"))
        lbl_m.setFont(make_font(12, QFont.Weight.ExtraBold))
        lbl_m.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.method_combo = SearchCombo(
            get_input_method_options(),
            selected_key=self.state.config.input_method,
        )
        self.method_combo.setMinimumWidth(380)
        self.method_combo.currentChanged.connect(self._on_method_changed)

        init_eff = resolve_effective_input_engine(self.state.config.input_method, is_rtl())
        init_col = "#F59E0B" if init_eff["is_fallback"] else T.token_hex("success")
        self.lbl_hw_status = QLabel("⚡ " + init_eff["reason"])
        self.lbl_hw_status.setFont(make_font(11, QFont.Weight.Bold))
        self.lbl_hw_status.setStyleSheet(f"color: {init_col}; background: transparent;")

        r1.addWidget(lbl_m)
        r1.addWidget(self.method_combo, 1)
        r1.addWidget(self.lbl_hw_status)
        c_lay.addLayout(r1)

        # Row 1b: Optional Target Application / Window Selector + Switch Button + Background Test Toggle
        r_app = QHBoxLayout()
        r_app.setSpacing(8)

        lbl_tapp = QLabel(t("lbl_test_target_app_opt"))
        lbl_tapp.setFont(make_font(12, QFont.Weight.Bold))
        lbl_tapp.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

        self.test_app_combo = SearchCombo([], selected_key="")
        self.test_app_combo.setMinimumWidth(250)

        btn_ref_tapp = IconButton("search", size=30, variant="glass", tooltip=t("toast_apps_refreshed"))
        btn_ref_tapp.clicked.connect(self._refresh_test_target_apps)

        self.btn_test_switch = Button(
            t("akp_btn_switch_win"),
            variant="glass",
            icon_name="",
            tint_hex="#F59E0B",
        )
        self.btn_test_switch.setFixedHeight(30)
        self.btn_test_switch.clicked.connect(self._on_switch_to_test_app)

        lbl_bg_test = QLabel(t("lbl_test_bg_mode"))
        lbl_bg_test.setFont(make_font(11, QFont.Weight.Bold))
        lbl_bg_test.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        self.tog_test_bg = ToggleSwitch(False)

        r_app.addWidget(lbl_tapp)
        r_app.addWidget(self.test_app_combo, 1)
        r_app.addWidget(btn_ref_tapp)
        r_app.addWidget(self.btn_test_switch)
        r_app.addSpacing(6)
        r_app.addWidget(lbl_bg_test)
        r_app.addWidget(self.tog_test_bg)
        c_lay.addLayout(r_app)

        # Row 2: Action Mode Selector + Delay + Run Button
        r2_mode = QHBoxLayout()
        r2_mode.setSpacing(8)

        lbl_am = QLabel(t("lbl_test_action_mode"))
        lbl_am.setFont(make_font(12, QFont.Weight.Bold))
        lbl_am.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.action_mode_combo = SearchCombo(
            [
                ("macro", t("test_mode_macro")),
                ("type_text", t("test_mode_type_text")),
                ("simple_key", t("test_mode_simple_key")),
                ("sweep_20", t("test_mode_sweep_20")),
                ("sweep_20_template", t("test_mode_sweep_20_template")),
                ("sweep_20_macro", t("test_mode_sweep_20_macro")),
            ],
            selected_key="macro",
        )
        self.action_mode_combo.setMinimumWidth(340)
        self.action_mode_combo.currentChanged.connect(self._on_action_mode_changed)

        lbl_delay = QLabel(t("lbl_run_after"))
        lbl_delay.setFont(make_font(11, QFont.Weight.Bold))
        lbl_delay.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")

        self.num_test_delay = NumberInput(
            value=1000.0,
            min_val=0.0,
            max_val=30000.0,
            step=500.0,
            decimals=0,
            suffix="ms",
        )
        self.num_test_delay.setFixedSize(118, 32)

        self.btn_run_test = Button(t("btn_run_test_now"), variant="primary", icon_name="play")
        self.btn_run_test.setFixedHeight(34)
        self.btn_run_test.clicked.connect(self._run_selected_test)

        r2_mode.addWidget(lbl_am)
        r2_mode.addWidget(self.action_mode_combo, 1)
        r2_mode.addWidget(lbl_delay)
        r2_mode.addWidget(self.num_test_delay)
        r2_mode.addWidget(self.btn_run_test)
        c_lay.addLayout(r2_mode)

        # Row 2b: Dynamic Payload Row (Macro Picker / Custom Text Box / Simple Key Capture)
        self.payload_row_w = QWidget(ctrl_card)
        r2 = QHBoxLayout(self.payload_row_w)
        r2.setContentsMargins(0, 0, 0, 0)
        r2.setSpacing(8)

        self.lbl_target = QLabel(t("lbl_test_select_macro"))
        self.lbl_target.setFont(make_font(12, QFont.Weight.Bold))
        self.lbl_target.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

        self.target_combo = SearchCombo([], selected_key="")
        self.target_combo.setMinimumWidth(240)

        self.tog_macro_sweep_prefix = ToggleSwitch(True)
        self.lbl_macro_sweep_prefix = QLabel(t("chk_macro_sweep_prefix"))
        self.lbl_macro_sweep_prefix.setFont(make_font(11, QFont.Weight.Bold))
        self.lbl_macro_sweep_prefix.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.lbl_custom_txt = QLabel(t("lbl_test_custom_text"))
        self.lbl_custom_txt.setFont(make_font(12, QFont.Weight.Bold))
        self.lbl_custom_txt.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        self.custom_text_edit = QLineEdit('key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \\n')
        self.custom_text_edit.setPlaceholderText(t("ph_test_custom_text"))
        self.custom_text_edit.setLayoutDirection(Qt.LayoutDirection.LeftToRight)
        self.custom_text_edit.setFixedHeight(32)
        self.custom_text_edit.setStyleSheet(
            f"QLineEdit {{ background: {T.token_hex('bg0')}; color: {T.token_hex('text')}; border: 1px solid {T.token_hex('border_hi')}; border-radius: 8px; padding: 3px 10px; font-family: 'Consolas', '{get_lang_font_family()}'; font-size: 12px; font-weight: bold; }}"
        )

        self.tog_press_enter = ToggleSwitch(True)
        self.lbl_press_enter = QLabel(t("chk_test_press_enter"))
        self.lbl_press_enter.setFont(make_font(11, QFont.Weight.Bold))
        self.lbl_press_enter.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

        self.lbl_simple_k = QLabel(t("lbl_test_simple_key"))
        self.lbl_simple_k.setFont(make_font(12, QFont.Weight.Bold))
        self.lbl_simple_k.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        self.simple_key_btn = KeyCaptureButton("Enter", allow_chord=False, state=self.state)
        self.simple_key_btn.setFixedSize(160, 32)

        r2.addWidget(self.lbl_target)
        r2.addWidget(self.target_combo, 1)
        r2.addWidget(self.tog_macro_sweep_prefix)
        r2.addWidget(self.lbl_macro_sweep_prefix)
        r2.addWidget(self.lbl_custom_txt)
        r2.addWidget(self.custom_text_edit, 1)
        r2.addWidget(self.tog_press_enter)
        r2.addWidget(self.lbl_press_enter)
        r2.addWidget(self.lbl_simple_k)
        r2.addWidget(self.simple_key_btn)
        c_lay.addWidget(self.payload_row_w)

        # Row 2c: Smart Variable Template Container (Visible in `sweep_20_template` and `sweep_20_macro`)
        self.template_sweep_container = QWidget(ctrl_card)
        tpl_lay = QVBoxLayout(self.template_sweep_container)
        tpl_lay.setContentsMargins(0, 2, 0, 0)
        tpl_lay.setSpacing(4)

        tpl_hdr = QHBoxLayout()
        tpl_hdr.setSpacing(8)
        self.lbl_tpl_box = QLabel(t("lbl_template_sweep_box"))
        self.lbl_tpl_box.setFont(make_font(11, QFont.Weight.ExtraBold))
        self.lbl_tpl_box.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.btn_reset_tpl = Button(t("btn_reset_template_sweep"), variant="ghost", icon_name="refresh")
        self.btn_reset_tpl.setFixedHeight(26)
        self.btn_reset_tpl.clicked.connect(self._on_reset_template_sweep)

        tpl_hdr.addWidget(self.lbl_tpl_box, 1)
        tpl_hdr.addWidget(self.btn_reset_tpl)
        tpl_lay.addLayout(tpl_hdr)

        saved_tpl = getattr(self.state, "test_lab_template_sweep_str", "") or DEFAULT_TEMPLATE_SWEEP_STRING
        self.template_sweep_edit = QLineEdit(saved_tpl, self.template_sweep_container)
        self.template_sweep_edit.setLayoutDirection(Qt.LayoutDirection.LeftToRight)
        self.template_sweep_edit.setFixedHeight(32)
        self.template_sweep_edit.setStyleSheet(
            f"""
            QLineEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('text')};
                border: 1.4px solid {T.token_hex('accent')};
                border-radius: 8px;
                padding: 4px 10px;
                font-family: 'Consolas', 'Courier New', monospace;
                font-size: 12px;
                font-weight: bold;
            }}
            """
        )
        self.template_sweep_edit.textChanged.connect(self._on_template_sweep_changed)
        tpl_lay.addWidget(self.template_sweep_edit)

        # Quick-insert variable chips inside a horizontal TintedScrollBox + QScrollArea (Zero Overlap)
        self.tpl_chips_scroll_box = TintedScrollBox(tint_hex="")
        self.tpl_chips_scroll_box.setFixedHeight(42)
        tc_box_lay = QHBoxLayout(self.tpl_chips_scroll_box)
        tc_box_lay.setContentsMargins(6, 2, 6, 2)
        tc_box_lay.setSpacing(0)

        self.tpl_chips_scroll = QScrollArea()
        self.tpl_chips_scroll.setWidgetResizable(True)
        self.tpl_chips_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.tpl_chips_scroll.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.tpl_chips_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.tpl_chips_scroll.setStyleSheet(modern_scroll_style())

        self.tpl_chips_container = QWidget()
        self.tpl_chips_container.setLayoutDirection(Qt.LayoutDirection.LeftToRight)
        self.tpl_chips_container.setStyleSheet("background: transparent;")
        chips_row = QHBoxLayout(self.tpl_chips_container)
        chips_row.setContentsMargins(4, 2, 4, 2)
        chips_row.setSpacing(8)

        total_chips_w = 16
        for token_str, disp_lbl in SWEEP_TEMPLATE_VARIABLES:
            chip_btn = Button(f"+ {disp_lbl}", variant="glass", icon_name="")
            chip_btn._force_ltr_text = True
            chip_btn.setFixedHeight(26)
            btn_w = chip_btn.naturalCompactWidth() + 14
            chip_btn.setFixedWidth(btn_w)
            total_chips_w += btn_w + 8
            chip_btn.clicked.connect(lambda _=False, tk=token_str: self._insert_template_token(tk))
            chips_row.addWidget(chip_btn)
        chips_row.addStretch(1)
        self.tpl_chips_container.setMinimumWidth(total_chips_w)
        self.tpl_chips_scroll.setWidget(self.tpl_chips_container)
        tc_box_lay.addWidget(self.tpl_chips_scroll)
        tpl_lay.addWidget(self.tpl_chips_scroll_box)
        c_lay.addWidget(self.template_sweep_container)

        # Row 2d: Editable Python Per-Method Test Script Container (Visible when `sweep_20` is selected)
        self.py_sweep_container = QWidget(ctrl_card)
        py_lay = QVBoxLayout(self.py_sweep_container)
        py_lay.setContentsMargins(0, 2, 0, 0)
        py_lay.setSpacing(4)

        py_hdr_row = QHBoxLayout()
        py_hdr_row.setSpacing(6)
        self.lbl_py_sweep = QLabel(t("lbl_py_sweep_editor"))
        self.lbl_py_sweep.setFont(make_font(11, QFont.Weight.ExtraBold))
        self.lbl_py_sweep.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

        self.py_preset_combo = SearchCombo(
            get_python_script_dropdown_items(),
            selected_key="preset:chat_20_sweep",
        )
        self.py_preset_combo.setMinimumWidth(260)
        self.py_preset_combo.setFixedHeight(28)
        self.py_preset_combo.currentChanged.connect(self._on_py_preset_selected)

        self.py_script_name_edit = QLineEdit()
        self.py_script_name_edit.setPlaceholderText(t("ph_py_script_name"))
        self.py_script_name_edit.setFixedWidth(150)
        self.py_script_name_edit.setFixedHeight(28)
        self.py_script_name_edit.setStyleSheet(
            f"QLineEdit {{ background: {T.token_hex('bg0')}; color: {T.token_hex('text')}; border: 1px solid {T.token_hex('border_hi')}; border-radius: 7px; padding: 2px 8px; font-family: '{get_lang_font_family()}'; font-size: 11px; font-weight: bold; }}"
        )

        self.btn_save_py_script = Button(t("btn_save_py_script"), variant="glass", icon_name="save")
        self.btn_save_py_script.setFixedHeight(28)
        self.btn_save_py_script.clicked.connect(self._on_save_py_script_clicked)

        self.btn_del_py_script = IconButton("trash", size=28, variant="danger", tooltip=t("tip_del_py_script"))
        self.btn_del_py_script.clicked.connect(self._on_delete_py_script_clicked)

        self.btn_reset_py = Button(t("btn_reset_py_sweep"), variant="ghost", icon_name="refresh")
        self.btn_reset_py.setFixedHeight(28)
        self.btn_reset_py.clicked.connect(self._on_reset_py_sweep_script)

        py_hdr_row.addWidget(self.lbl_py_sweep)
        py_hdr_row.addWidget(self.py_preset_combo, 1)
        py_hdr_row.addWidget(self.py_script_name_edit)
        py_hdr_row.addWidget(self.btn_save_py_script)
        py_hdr_row.addWidget(self.btn_del_py_script)
        py_hdr_row.addWidget(self.btn_reset_py)
        py_lay.addLayout(py_hdr_row)

        self.py_sweep_editor = QPlainTextEdit(self.py_sweep_container)
        self.py_sweep_editor.setLayoutDirection(Qt.LayoutDirection.LeftToRight)
        saved_script = getattr(self.state, "test_lab_py_sweep_script", "") or DEFAULT_PYTHON_SWEEP_SCRIPT
        self.py_sweep_editor.setPlainText(saved_script)
        self.py_sweep_editor.setFixedHeight(88)
        self.py_sweep_editor.setStyleSheet(
            f"""
            QPlainTextEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('text')};
                border: 1.4px solid {T.token_hex('accent')};
                border-radius: 9px;
                padding: 5px 10px;
                font-family: 'Consolas', 'Courier New', monospace;
                font-size: 12px;
                font-weight: 600;
            }}
            """
        )
        self.py_sweep_editor.textChanged.connect(self._on_py_sweep_text_changed)
        py_lay.addWidget(self.py_sweep_editor)

        c_lay.addWidget(self.py_sweep_container)
        root.addWidget(ctrl_card)
        self._on_action_mode_changed("macro")

        # Live Sandbox Box + Event Log
        sandbox_card = GlassCard(parent=self)
        s_lay = QVBoxLayout(sandbox_card)
        s_lay.setContentsMargins(14, 8, 14, 8)
        s_lay.setSpacing(6)

        lbl_sb = QLabel(t("lbl_test_sandbox"))
        lbl_sb.setFont(make_font(11, QFont.Weight.ExtraBold))
        lbl_sb.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        s_lay.addWidget(lbl_sb)

        self.sandbox_box = _LiveInputSandboxBox(self.state, self._add_structured_log, self)
        s_lay.addWidget(self.sandbox_box)

        self.log_scroll = QScrollArea()
        self.log_scroll.setMinimumHeight(185)
        self.log_scroll.setWidgetResizable(True)
        self.log_scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.log_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.log_scroll.setStyleSheet(modern_scroll_style())

        self.log_container = QWidget()
        self.log_container.setStyleSheet("background: transparent;")
        self.log_vbox = QVBoxLayout(self.log_container)
        self.log_vbox.setContentsMargins(0, 0, 4, 0)
        self.log_vbox.setSpacing(6)
        self.log_vbox.addStretch(1)
        self.log_scroll.setWidget(self.log_container)
        s_lay.addWidget(self.log_scroll, 1)

        root.addWidget(sandbox_card, 1)

    def _insert_template_token(self, token_str: str) -> None:
        cur = self.template_sweep_edit.text()
        sep = " " if (cur and not cur.endswith(" ")) else ""
        self.template_sweep_edit.setText(f"{cur}{sep}{token_str} ")
        self.template_sweep_edit.setFocus()

    def _on_template_sweep_changed(self, txt: str) -> None:
        self.state.test_lab_template_sweep_str = txt

    def _on_reset_template_sweep(self) -> None:
        self.template_sweep_edit.setText(DEFAULT_TEMPLATE_SWEEP_STRING)
        self.state.test_lab_template_sweep_str = DEFAULT_TEMPLATE_SWEEP_STRING

    def _refresh_py_presets_combo(self, select_key: str = "") -> None:
        cur = select_key or (self.py_preset_combo.currentKey() if hasattr(self, "py_preset_combo") else "preset:default_sweep")
        self.py_preset_combo.setItems(get_python_script_dropdown_items(), selected_key=cur)

    def _on_py_preset_selected(self, script_key: str) -> None:
        code = get_python_script_code_by_key(script_key)
        if code is not None:
            self.py_sweep_editor.setPlainText(code)
            self.state.test_lab_py_sweep_script = code
        saved_nm = get_python_script_name_by_key(script_key)
        if saved_nm:
            self.py_script_name_edit.setText(saved_nm)

    def _on_save_py_script_clicked(self) -> None:
        nm = self.py_script_name_edit.text().strip()
        code = self.py_sweep_editor.toPlainText()
        cur_k = self.py_preset_combo.currentKey()
        saved_id, saved_nm = upsert_user_python_script(nm, code, existing_key=cur_k if not nm else "")
        self.py_script_name_edit.setText(saved_nm)
        self._refresh_py_presets_combo(select_key=saved_id)
        self.state.python_scripts_updated.emit()
        self.state.toast_requested.emit(f"{t('toast_py_script_saved')} «{saved_nm}»", "ok")

    def _on_delete_py_script_clicked(self) -> None:
        cur_k = self.py_preset_combo.currentKey()
        if delete_user_python_script(cur_k):
            self.py_script_name_edit.clear()
            self._refresh_py_presets_combo(select_key="preset:default_sweep")
            self.state.python_scripts_updated.emit()
            self.state.toast_requested.emit(t("toast_py_script_deleted"), "info")

    def _on_py_sweep_text_changed(self) -> None:
        self.state.test_lab_py_sweep_script = self.py_sweep_editor.toPlainText()

    def _on_reset_py_sweep_script(self) -> None:
        self.py_sweep_editor.setPlainText(DEFAULT_PYTHON_SWEEP_SCRIPT)
        self.state.test_lab_py_sweep_script = DEFAULT_PYTHON_SWEEP_SCRIPT

    def _refresh_test_target_apps(self) -> None:
        cur_sel = self.test_app_combo.currentKey() if hasattr(self, "test_app_combo") else ""
        self._test_win_cache = list_detailed_windows_for_picker()
        items: List[Tuple] = [("", t("test_target_app_sandbox_free"), "", "")]
        seen = set()
        for w in self._test_win_cache:
            pname = w["proc_name"]
            k_low = pname.lower()
            if k_low in seen:
                continue
            seen.add(k_low)
            disp = f"{pname} — {w['title'][:42]} (PID: {w['pid']})"
            items.append((pname, disp, w["exe_path"], "#10B981" if cur_sel and k_low == cur_sel.lower() else ""))
        self.test_app_combo.setItems(items, selected_key=cur_sel)

    def _resolve_selected_test_hwnd(self) -> int:
        sel = (self.test_app_combo.currentKey() or "").strip().lower()
        if not sel:
            return 0
        for w in self._test_win_cache:
            if w["proc_name"].lower() == sel:
                return int(w["hwnd"])
        for w in list_detailed_windows_for_picker():
            if w["proc_name"].lower() == sel:
                return int(w["hwnd"])
        return 0

    def _on_switch_to_test_app(self) -> None:
        sel = (self.test_app_combo.currentKey() or "").strip()
        if not sel:
            self.state.toast_requested.emit(t("akp_err_must_select_window"), "info")
            return
        hwnd = self._resolve_selected_test_hwnd()
        if hwnd > 0:
            focus_and_bring_hwnd_to_front(hwnd)
        else:
            self.state.toast_requested.emit(t("akp_err_window_not_found"), "danger")

    def _populate_test_targets(self) -> None:
        items: List[Tuple[str, str]] = [
            ("quick_demo", t("test_quick_demo_label"))
        ]
        is_ar = is_rtl()
        for idx, c in enumerate(self.state.smart_combos):
            sc = SmartCombo.from_dict(c)
            title = sc.sidebar_label(is_ar=is_ar)
            items.append((f"combo:{idx}", f"{t('test_prefix_smart_key')} {title}"))
        for idx, m in enumerate(self.state.saved_macros):
            nm = m.get("name", f"Recording {idx + 1}")
            items.append((f"rec:{idx}", f"{t('test_prefix_saved_rec')} {nm}"))
        self.target_combo.setItems(items, selected_key=self.target_combo.currentKey() or "quick_demo")

    def _on_method_changed(self, method_key: str) -> None:
        self.state.update_setting("input_method", method_key)
        if hasattr(self, 'sandbox_box'):
            self.sandbox_box._cached_eff_info = None
        eff = resolve_effective_input_engine(method_key, is_rtl())
        col = "#F59E0B" if eff["is_fallback"] else T.token_hex("success")
        self.lbl_hw_status.setText("⚡ " + eff["reason"])
        self.lbl_hw_status.setStyleSheet(f"color: {col}; background: transparent;")
        self._add_structured_log(
            {"kind": "method_switch", "requested_method": method_key},
            "#F59E0B" if eff["is_fallback"] else "#06B6D4",
        )

    def _format_effective_engine_badge(self, requested_method: str) -> Tuple[str, str]:
        eff = resolve_effective_input_engine(requested_method, is_rtl())
        eff_id = eff["effective_method"]
        eff_name = next((lbl for k, lbl in get_input_method_options() if k == eff_id), eff_id)
        if eff["is_fallback"]:
            html = (
                f"<b style='color:#D97706;'>{eff_name}</b> "
                f"<span style='color:#EF4444;'>({t('log_auto_fallback_from')} {eff['requested_method']})</span>"
            )
            return html, "#F59E0B"
        html = f"<b style='color:#10B981;'>{eff_name}</b>"
        return html, "#10B981"

    def _render_log_entry_html(self, entry: Dict[str, Any]) -> str:
        kind = entry.get("kind", "raw")
        if kind == "sweep_item":
            ok = bool(entry.get("ok", False))
            idx = int(entry.get("idx", 1))
            total = int(entry.get("total", 20))
            tr_k = str(entry.get("method_tr_key", ""))
            m_id = str(entry.get("method_id", ""))
            m_name = t(tr_k) if tr_k else m_id
            dt_ms = fmt_num(entry.get("dt_ms", 1))
            badge_col = "#10B981" if ok else "#EF4444"
            badge_txt = t("akp_log_sweep_item_ok") if ok else t("akp_log_sweep_item_fail")
            target_app = str(entry.get("target_app", "Sandbox"))
            hwnd_str = str(entry.get("hwnd", "0x0"))
            child_cls = str(entry.get("child_cls", "TopWindow"))
            mode_lbl = t("log_lbl_mode_bg") if entry.get("is_bg") else t("log_lbl_mode_fg")
            action_desc = str(entry.get("action_desc", entry.get("key", "Enter")))
            reason_str = format_diagnostic_reason(entry)

            line1 = (
                f"<span dir='ltr' style='color:#06B6D4; font-weight:800;'>[#{idx:02d}/{total}]</span> "
                f"<b style='color:{badge_col};'>{badge_txt}</b> — "
                f"<b>{m_name}</b> "
                f"<span dir='ltr' style='color:#3B82F6; font-weight:bold;'>({dt_ms} ms)</span>"
            )
            line2 = (
                f"<span style='color:#94A3B8; font-size:11px;'>"
                f"🎯 <b>{t('log_lbl_target')}</b> <span dir='ltr'>{target_app} [{hwnd_str} &rarr; {child_cls}]</span> ({mode_lbl}) "
                f"&nbsp;|&nbsp; ⚡ <b>{t('log_lbl_action')}</b> <span dir='ltr' style='color:#A78BFA;'>{action_desc}</span> "
                f"&nbsp;|&nbsp; 📌 <b>{t('log_lbl_details')}</b> <span style='color:{badge_col};'>{reason_str}</span>"
                f"</span>"
            )
            return f"<div>{line1}<br>{line2}</div>"

        if kind == "mouse":
            eff_info = resolve_effective_input_engine(entry.get("requested_method", ""), is_rtl())
            eff = eff_info["effective_method"]
            fb = f" ({t('log_fallback_from')} {eff_info['requested_method']})" if eff_info["is_fallback"] else ""
            eff_col = "#D97706" if eff_info["is_fallback"] else "#10B981"
            return (
                f"{t('log_recv_mouse')} <b style='color:#10B981;'>{t('log_mouse_click_at')} ({entry.get('btn_name', '')})</b> "
                f"{t('log_at_coords')} <b dir='ltr' style='color:#3B82F6;'>({entry.get('x', 0)}, {entry.get('y', 0)})</b> "
                f"— {t('log_actual_engine')} <b style='color:{eff_col};'>{eff}{fb}</b>"
            )
        if kind == "key":
            eff_info = resolve_effective_input_engine(entry.get("requested_method", ""), is_rtl())
            eff = eff_info["effective_method"]
            fb = f" ({t('log_fallback_from')} {eff_info['requested_method']})" if eff_info["is_fallback"] else ""
            eff_col = "#D97706" if eff_info["is_fallback"] else "#10B981"
            hw_sc = int(entry.get("hw_sc", 0))
            hid = int(entry.get("hid_usage", 0))
            return (
                f"{t('log_recv_key')} <b dir='ltr' style='color:#8B5CF6;'>[ {entry.get('lbl', '')} ]</b> "
                f"&nbsp;|&nbsp; <span dir='ltr' style='color:#06B6D4; font-weight:bold;'>ScanCode: 0x{hw_sc:02X}</span> "
                f"&nbsp;|&nbsp; <span dir='ltr' style='color:#3B82F6; font-weight:bold;'>USB HID: 0x{hid:02X}</span> "
                f"— {t('log_actual_engine')} <b style='color:{eff_col};'>{eff}{fb}</b>"
            )
        if kind == "method_switch":
            req = entry.get("requested_method", "")
            req_lbl = next((lbl for k, lbl in get_input_method_options() if k == req), req)
            eff = resolve_effective_input_engine(req, is_rtl())
            badge_col = "#D97706" if eff["is_fallback"] else "#10B981"
            return (
                f"{t('log_selected_prefix')} <b style='color:#8B5CF6;'>{req_lbl}</b><br>"
                f"<span style='color:{badge_col}; font-weight:bold;'>{t('log_actual_status')} {eff['reason']}</span>"
            )
        if kind == "run_combo":
            eff_html, _ = self._format_effective_engine_badge(entry.get("requested_method", ""))
            delay_ms = int(entry.get("delay_ms", 0))
            return (
                f"{t('log_run_combo_prefix')} <b style='color:#3B82F6;'>{fmt_num(delay_ms)} ms</b> "
                f"{t('log_via_actual_engine')} {eff_html}"
            )
        if kind == "run_rec":
            eff_html, _ = self._format_effective_engine_badge(entry.get("requested_method", ""))
            nm = entry.get("target_name", "")
            return (
                f"{t('log_run_rec_prefix')} <b style='color:#8B5CF6;'>«{nm}»</b> "
                f"{t('log_via_actual_engine')} {eff_html}"
            )
        if kind == "run_quick":
            eff_html, _ = self._format_effective_engine_badge(entry.get("requested_method", ""))
            delay_ms = int(entry.get("delay_ms", 0))
            return (
                f"{t('log_run_quick_prefix')} <b style='color:#8B5CF6;'>(W -> Space)</b> "
                f"{t('log_in_delay')} <b style='color:#3B82F6;'>{fmt_num(delay_ms)} ms</b> "
                f"{t('log_via_actual_engine')} {eff_html}"
            )
        return str(entry.get("text", ""))

    def _on_action_mode_changed(self, mode_key: str) -> None:
        show_macro_picker = mode_key in ("macro", "sweep_20_macro")
        self.lbl_target.setVisible(show_macro_picker)
        self.target_combo.setVisible(show_macro_picker)
        self.tog_macro_sweep_prefix.setVisible(mode_key == "sweep_20_macro")
        self.lbl_macro_sweep_prefix.setVisible(mode_key == "sweep_20_macro")

        self.lbl_custom_txt.setVisible(mode_key == "type_text")
        self.custom_text_edit.setVisible(mode_key == "type_text")
        self.tog_press_enter.setVisible(mode_key == "type_text")
        self.lbl_press_enter.setVisible(mode_key == "type_text")

        self.lbl_simple_k.setVisible(mode_key == "simple_key")
        self.simple_key_btn.setVisible(mode_key == "simple_key")

        self.payload_row_w.setVisible(mode_key in ("macro", "type_text", "simple_key", "sweep_20_macro"))
        self.template_sweep_container.setVisible(mode_key in ("sweep_20_template", "sweep_20_macro"))
        self.py_sweep_container.setVisible(mode_key == "sweep_20")

    def _resolve_selected_macro_steps(self) -> Tuple[str, List[Dict[str, Any]], bool]:
        """Return `(macro_display_name, steps_or_events_list, is_recording)` for `self.target_combo`."""
        key = self.target_combo.currentKey() or "quick_demo"
        if key.startswith("combo:"):
            idx = int(key.split(":")[1])
            if 0 <= idx < len(self.state.smart_combos):
                c = self.state.smart_combos[idx]
                return str(c.get("name") or c.get("trigger_key") or f"Macro {idx + 1}"), list(c.get("steps", [])), False
        elif key.startswith("rec:"):
            idx = int(key.split(":")[1])
            if 0 <= idx < len(self.state.saved_macros):
                m = self.state.saved_macros[idx]
                return str(m.get("name") or f"Recording {idx + 1}"), list(m.get("events", [])), True

        demo_steps = [
            {"step_type": "key_tap", "value": "W", "scan_code": 0x11, "is_extended": False, "delay_ms": 40, "hold_ms": 20},
            {"step_type": "key_tap", "value": "Space", "scan_code": 0x39, "is_extended": False, "delay_ms": 40, "hold_ms": 20},
        ]
        return "Quick Demo (W -> Space)", demo_steps, False

    def _run_selected_test(self) -> None:
        action_mode = self.action_mode_combo.currentKey() if hasattr(self, "action_mode_combo") else "macro"
        delay_ms = int(self.num_test_delay.value())
        req_method = self.state.config.input_method
        run_in_bg = self.tog_test_bg.isChecked()

        sel_app = (self.test_app_combo.currentKey() or "").strip()
        target_hwnd = 0
        if sel_app:
            target_hwnd = self._resolve_selected_test_hwnd()
            if target_hwnd <= 0 and IS_WINDOWS:
                self.state.toast_requested.emit(t("akp_err_window_not_found"), "danger")
                return
            if target_hwnd > 0 and not run_in_bg:
                focus_and_bring_hwnd_to_front(target_hwnd)
        else:
            if run_in_bg:
                self.state.toast_requested.emit(t("akp_err_must_select_window"), "info")
                return
            self.sandbox_box.test_input.setFocus()
            try:
                target_hwnd = int(self.sandbox_box.test_input.winId())
            except Exception:
                target_hwnd = 0

        allow_self_test = not bool(sel_app)
        target_app_label = sel_app or "Sandbox"
        best_child = _find_best_input_child_hwnd(target_hwnd) if target_hwnd else 0
        child_cls = _get_window_class_name(best_child) if best_child else "SandboxEdit"

        # Mode 2: Type Custom Text or Smart Variable Template (Single Method)
        if action_mode == "type_text":
            txt = self.custom_text_edit.text()
            press_enter = self.tog_press_enter.isChecked()
            m_idx = next((i for i, (k, _) in enumerate(AUTOPRESSER_KEY_METHODS, start=1) if k == req_method), 1)
            m_tr = next((tr for k, tr in AUTOPRESSER_KEY_METHODS if k == req_method), "")
            m_title = t(m_tr) if m_tr else req_method

            def _bg_type() -> None:
                if delay_ms > 0:
                    time.sleep(delay_ms / 1000.0)
                t0 = time.perf_counter()
                ok_t, reason_t, preview_t = expand_and_execute_method_template(
                    txt,
                    method_num=m_idx,
                    method_id=req_method,
                    method_name=m_title,
                    target_hwnd=target_hwnd,
                    target_app=target_app_label,
                    run_in_background=run_in_bg,
                    extra_enter=press_enter,
                    allow_self_test=allow_self_test,
                )
                dt_ms = max(1, int((time.perf_counter() - t0) * 1000))
                self._thread_log_signal.emit(
                    {
                        "kind": "sweep_item",
                        "idx": m_idx,
                        "total": len(AUTOPRESSER_KEY_METHODS),
                        "method_id": req_method,
                        "method_tr_key": m_tr,
                        "target_app": target_app_label,
                        "hwnd": f"0x{target_hwnd:X}" if target_hwnd else "0x0",
                        "child_cls": child_cls,
                        "is_bg": run_in_bg,
                        "action_desc": f"Text: {preview_t[:38]}",
                        "ok": ok_t,
                        "dt_ms": dt_ms,
                        "reason_code": ("delivered_bg" if req_method in BACKGROUND_HWND_KEY_METHODS else "delivered_fg") if ok_t else "",
                        "reason": reason_t,
                    },
                    "#10B981" if ok_t else "#EF4444",
                )

            threading.Thread(target=_bg_type, daemon=True).start()
            return

        # Mode 3: Single Key Press
        if action_mode == "simple_key":
            k_lbl = self.simple_key_btn.keyString() or "Enter"
            m_idx = next((i for i, (k, _) in enumerate(AUTOPRESSER_KEY_METHODS, start=1) if k == req_method), 1)
            m_tr = next((tr for k, tr in AUTOPRESSER_KEY_METHODS if k == req_method), "")

            def _bg_simple_key() -> None:
                if delay_ms > 0:
                    time.sleep(delay_ms / 1000.0)
                t0 = time.perf_counter()
                ok_k, reason_k = dispatch_autopresser_key(
                    k_lbl,
                    method=req_method,
                    hold_ms=25,
                    target_hwnd=target_hwnd,
                    run_in_background=run_in_bg,
                    strict_no_fallback=True,
                    allow_self_test=allow_self_test,
                )
                dt_ms = max(1, int((time.perf_counter() - t0) * 1000))
                self._thread_log_signal.emit(
                    {
                        "kind": "sweep_item",
                        "idx": m_idx,
                        "total": len(AUTOPRESSER_KEY_METHODS),
                        "method_id": req_method,
                        "method_tr_key": m_tr,
                        "target_app": target_app_label,
                        "hwnd": f"0x{target_hwnd:X}" if target_hwnd else "0x0",
                        "child_cls": child_cls,
                        "is_bg": run_in_bg,
                        "action_desc": f"Key [{k_lbl}]",
                        "ok": ok_k,
                        "dt_ms": dt_ms,
                        "reason_code": ("delivered_bg" if req_method in BACKGROUND_HWND_KEY_METHODS else "delivered_fg") if ok_k else "",
                        "reason": reason_k,
                    },
                    "#10B981" if ok_k else "#EF4444",
                )

            threading.Thread(target=_bg_simple_key, daemon=True).start()
            return

        # Modes 4, 5, 6: Sweep All 20 Methods (Python Script / Smart Variable Template / Saved Macro)
        if action_mode in ("sweep_20", "sweep_20_template", "sweep_20_macro"):
            if self._sweep_running:
                return
            self._sweep_running = True
            self.btn_run_test.setBusy(True)
            py_code = self.py_sweep_editor.toPlainText()
            tpl_str = self.template_sweep_edit.text()
            use_macro_prefix = self.tog_macro_sweep_prefix.isChecked()
            macro_name, macro_steps, is_rec = self._resolve_selected_macro_steps()

            def _bg_sweep_all_20() -> None:
                try:
                    if delay_ms > 0:
                        time.sleep(delay_ms / 1000.0)
                    mode_tag = t("log_lbl_mode_bg") if run_in_bg else t("log_lbl_mode_fg")
                    sweep_kind_lbl = (
                        "Python Per-Method Script"
                        if action_mode == "sweep_20"
                        else f"Smart Template ({tpl_str[:32]}...)"
                        if action_mode == "sweep_20_template"
                        else f"Macro Sweep «{macro_name}» ({len(macro_steps)} steps)"
                    )
                    self._thread_log_signal.emit(
                        {
                            "kind": "raw",
                            "text": (
                                f"<b style='color:#06B6D4;'>⚡ {t('akp_log_sweep_header')} "
                                f"<span dir='ltr'>[{target_app_label} | HWND: 0x{target_hwnd:X} &rarr; {child_cls}]</span> "
                                f"({mode_tag}) — {sweep_kind_lbl}</b>"
                            ),
                        },
                        "#06B6D4",
                    )
                    ok_c = 0
                    fail_c = 0
                    for idx, (m_id, tr_k) in enumerate(AUTOPRESSER_KEY_METHODS, start=1):
                        m_title = t(tr_k)
                        t0 = time.perf_counter()
                        if action_mode == "sweep_20":
                            ok, reason = execute_per_method_python_test(
                                script_code=py_code,
                                method_num=idx,
                                method_id=m_id,
                                method_name=m_title,
                                target_hwnd=target_hwnd,
                                run_in_background=run_in_bg,
                                target_app=target_app_label,
                                allow_self_test=allow_self_test,
                            )
                            act_desc = f"Python (#{idx} {m_id})"
                        elif action_mode == "sweep_20_template":
                            ok, reason, prev = expand_and_execute_method_template(
                                tpl_str,
                                method_num=idx,
                                method_id=m_id,
                                method_name=m_title,
                                target_hwnd=target_hwnd,
                                target_app=target_app_label,
                                run_in_background=run_in_bg,
                                extra_enter=False,
                                allow_self_test=allow_self_test,
                            )
                            act_desc = f"Template: {prev[:32]}"
                        else:
                            ok, reason, step_cnt = execute_per_method_macro_test(
                                macro_steps,
                                is_recording=is_rec,
                                method_num=idx,
                                method_id=m_id,
                                method_name=m_title,
                                target_hwnd=target_hwnd,
                                target_app=target_app_label,
                                run_in_background=run_in_bg,
                                prefix_template=tpl_str if use_macro_prefix else "",
                                allow_self_test=allow_self_test,
                            )
                            act_desc = f"Macro «{macro_name}» ({step_cnt} ops)"

                        dt_ms = max(1, int((time.perf_counter() - t0) * 1000))
                        if ok:
                            ok_c += 1
                        else:
                            fail_c += 1

                        self._thread_log_signal.emit(
                            {
                                "kind": "sweep_item",
                                "idx": idx,
                                "total": len(AUTOPRESSER_KEY_METHODS),
                                "method_id": m_id,
                                "method_tr_key": tr_k,
                                "target_app": target_app_label,
                                "hwnd": f"0x{target_hwnd:X}" if target_hwnd else "0x0",
                                "child_cls": child_cls,
                                "is_bg": run_in_bg,
                                "action_desc": act_desc,
                                "ok": ok,
                                "dt_ms": dt_ms,
                                "reason_code": ("delivered_bg" if m_id in BACKGROUND_HWND_KEY_METHODS else "delivered_fg") if ok else "",
                                "reason": reason,
                            },
                            "#10B981" if ok else "#EF4444",
                        )
                        time.sleep(0.04)

                    summary_msg = (
                        t("akp_log_sweep_summary")
                        .replace("{ok}", fmt_num(ok_c))
                        .replace("{fail}", fmt_num(fail_c))
                    )
                    self._thread_log_signal.emit(
                        {
                            "kind": "raw",
                            "text": f"<b style='color:#10B981;'>🏁 {summary_msg} — <span dir='ltr'>{target_app_label}</span> ({mode_tag})</b>",
                        },
                        "#10B981" if ok_c > 0 else "#F59E0B",
                    )
                finally:
                    self._sweep_finished_signal.emit()

            threading.Thread(target=_bg_sweep_all_20, daemon=True).start()
            return

        self._run_selected_test_macro_impl(run_in_bg=run_in_bg)

    def _on_sweep_finished_main_thread(self) -> None:
        self._sweep_running = False
        self.btn_run_test.setBusy(False)

    def _run_selected_test_macro_impl(self, run_in_bg: bool = False) -> None:
        key = self.target_combo.currentKey()
        delay_ms = int(self.num_test_delay.value())
        req_method = self.state.config.input_method
        _, eff_accent = self._format_effective_engine_badge(req_method)

        sel_app = (self.test_app_combo.currentKey() or "").strip()
        allow_self_test = not bool(sel_app)
        if sel_app:
            hwnd = self._resolve_selected_test_hwnd()
            if hwnd > 0 and not run_in_bg:
                focus_and_bring_hwnd_to_front(hwnd)
        else:
            self.sandbox_box.test_input.setFocus()

        if key.startswith("combo:"):
            idx = int(key.split(":")[1])
            if 0 <= idx < len(self.state.smart_combos):
                c = self.state.smart_combos[idx]
                self._add_structured_log(
                    {"kind": "run_combo", "delay_ms": delay_ms, "requested_method": req_method},
                    eff_accent,
                )
                self.engine.execute_smart_combo(c, initial_delay_ms=delay_ms, allow_self_test=allow_self_test)
                return
        elif key.startswith("rec:"):
            idx = int(key.split(":")[1])
            if 0 <= idx < len(self.state.saved_macros):
                m = self.state.saved_macros[idx]
                self._add_structured_log(
                    {
                        "kind": "run_rec",
                        "target_name": m.get("name", ""),
                        "requested_method": req_method,
                    },
                    eff_accent,
                )
                self.engine.start_playback(events=m.get("events", []), repeat_count=1, allow_self_test=allow_self_test)
                return

        demo_combo = {
            "id": "test_lab_quick_demo",
            "name": "Quick Test",
            "mode": "once",
            "steps": [
                {"step_type": "key_tap", "value": "W", "scan_code": 0x11, "is_extended": False, "delay_ms": 40},
                {"step_type": "key_tap", "value": "Space", "scan_code": 0x39, "is_extended": False, "delay_ms": 40},
            ],
        }
        self._add_structured_log(
            {"kind": "run_quick", "delay_ms": delay_ms, "requested_method": req_method},
            eff_accent,
        )
        self.engine.execute_smart_combo(demo_combo, initial_delay_ms=delay_ms, allow_self_test=allow_self_test)

    def _add_structured_log(self, entry: Dict[str, Any], accent_hex: str = "#8B5CF6") -> None:
        item = dict(entry)
        item["ts"] = datetime.now().strftime("%H:%M:%S")
        item["accent"] = accent_hex
        self.state.test_lab_logs.insert(0, item)
        if len(self.state.test_lab_logs) > 200:
            self.state.test_lab_logs.pop()
        self._insert_log_card(item, at_top=True)

    def _restore_persistent_logs(self) -> None:
        logs = getattr(self.state, "test_lab_logs", [])
        for item in reversed(logs):
            self._insert_log_card(item, at_top=True)

    def _insert_log_card(self, item: Dict[str, Any], at_top: bool = True) -> None:
        accent_hex = str(item.get("accent", "#8B5CF6"))
        ts = str(item.get("ts", ""))
        html = self._render_log_entry_html(item)

        card = StepCard(index=self._log_count, accent_hex=accent_hex, parent=self.log_container)
        self._log_count += 1
        lay = QHBoxLayout(card)
        lay.setContentsMargins(12, 6, 12, 6)
        lay.setSpacing(10)

        lbl_ts = QLabel(ts)
        lbl_ts.setFont(make_font(11, QFont.Weight.Bold))
        lbl_ts.setStyleSheet(f"color: {accent_hex}; background: transparent;")

        lbl_msg = QLabel(html)
        lbl_msg.setTextFormat(Qt.TextFormat.RichText)
        lbl_msg.setWordWrap(True)
        lbl_msg.setFont(make_font(11, QFont.Weight.DemiBold))
        lbl_msg.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

        lay.addWidget(lbl_ts)
        lay.addWidget(lbl_msg, 1)

        if at_top:
            self.log_vbox.insertWidget(0, card)
        else:
            self.log_vbox.insertWidget(max(0, self.log_vbox.count() - 1), card)

    def _clear_log(self) -> None:
        self.state.test_lab_logs.clear()
        self.sandbox_box.test_input.clear()
        self.sandbox_box.lbl_status.setText(t("sandbox_hint_status"))
        while self.log_vbox.count() > 1:
            it = self.log_vbox.takeAt(0)
            w = it.widget()
            if w is not None:
                w.deleteLater()
