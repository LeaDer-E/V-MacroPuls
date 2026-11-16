# -*- coding: utf-8 -*-
"""
Calm, Unified Settings Page («الإعدادات») for V-MacroPlus v3.2.0:
- Drop-down panel (`SearchCombo`) for selecting from 10 full languages.
- Input Simulation Method selector (10 Hardware Keyboard & Win32/Linux Input Engines)
  + Live **Interception & Ring-0 Kernel Diagnostic Panel** (`diagnose_kernel_drivers(run_live_probe=True)`)
  allowing the user to verify `interception.dll`, `keyboard.sys` UpperFilters, active Ring-0 Hardware Device ID,
  and live `LLKHF_INJECTED = 0` status directly from Settings, plus 1-click import/install of `interception.dll` / `install-interception.exe`!
- Dedicated Full Backup Card (`Export All` & `Import All` for Smart Keys, Custom Macros, and Recordings).
- Windows Startup toggle (`Run on Windows Startup` via `HKCU\\...\\Run`).
- Multi-Key Chord support on all 4 global shortcuts (`F9`, `F10`, `Esc`, `F11`).
"""

from __future__ import annotations
from typing import Callable, Optional
from PySide6.QtCore import QPointF, QRectF, Qt, Signal
from PySide6.QtGui import QColor, QFont, QPainter, QPen
from PySide6.QtWidgets import (
    QColorDialog,
    QFileDialog,
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QScrollArea,
    QVBoxLayout,
    QWidget,
)
from app.config import AppConfig
from app.domain.macro_engine import MacroEngine
from app.domain.win32_input import (
    diagnose_kernel_drivers,
    diagnose_ps2_port_driver,
    format_kernel_diagnostic_report,
    format_ps2_diagnostic_report,
    install_or_import_interception_file,
    install_or_import_ps2_file,
    is_running_as_admin,
    relaunch_as_admin,
    is_windows_startup_enabled,
    open_folder_or_path,
    set_windows_startup_enabled,
)
from app.i18n import SUPPORTED_LANGUAGES, get_language, is_rtl, t, tl
from app.paths import get_drivers_dir
from app.state import AppState
from app.theme import PALETTES, T
from app.ui.icons import draw_icon
from app.ui.pages.test_page import get_input_method_options
from app.ui.widgets import (
    Button,
    GlassCard,
    KeyCaptureButton,
    ResponsiveFlowBar,
    SearchCombo,
    SegmentedControl,
    ToggleSwitch,
    make_font,
    modern_scroll_style,
)

CALM_PALETTE_IDS = list(PALETTES.keys())


class _ColorSwatchButton(QWidget):
    clicked = Signal(str)

    def __init__(self, palette_id: str, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.palette_id = palette_id
        self.setFixedSize(34, 34)
        self.setCursor(Qt.CursorShape.PointingHandCursor)
        pal = PALETTES.get(self.palette_id)
        if pal:
            self.setToolTip(f"{pal.ar_name if is_rtl() else pal.en_name} ({pal.accent_hex})")

    def mouseReleaseEvent(self, event) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            self.clicked.emit(self.palette_id)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        pal = PALETTES.get(self.palette_id)
        if not pal:
            p.end()
            return
        c = QColor(pal.accent_hex)
        active = T.palette_id == self.palette_id
        center = QPointF(self.width() / 2, self.height() / 2)

        if active:
            p.setBrush(Qt.BrushStyle.NoBrush)
            p.setPen(QPen(c, 2.2))
            p.drawEllipse(center, 16, 16)

        p.setBrush(c)
        p.setPen(Qt.PenStyle.NoPen)
        p.drawEllipse(center, 12, 12)

        if active:
            draw_icon(p, "check", QRectF(center.x() - 6, center.y() - 6, 12, 12), color=QColor("#FFFFFF"), stroke_width=2.4)
        p.end()


class SettingsPage(QWidget):
    """Unified Settings page with Live Interception & Ring-0 Kernel Diagnostics, 10-Language Drop Panel, Full Backup, and 4 Global Hotkeys."""

    aboutRequested = Signal()

    def __init__(
        self,
        state: AppState,
        engine: Optional[MacroEngine] = None,
        on_about: Optional[Callable[[], None]] = None,
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self.state = state
        self.engine = engine
        if on_about is not None:
            self.aboutRequested.connect(on_about)
        self._build_ui()
        self.state.settings_changed.connect(self._on_external_settings_changed)
        self._refresh_kernel_diagnostics(run_live_probe=False, show_toast=False)
        self._refresh_ps2_diagnostics(run_live_probe=False, show_toast=False)

    def _build_ui(self) -> None:
        outer = QVBoxLayout(self)
        outer.setContentsMargins(18, 8, 18, 12)
        outer.setSpacing(10)

        hdr = GlassCard(accent_tint=True)
        h_lay = QHBoxLayout(hdr)
        h_lay.setContentsMargins(18, 12, 18, 12)

        t_col = QVBoxLayout()
        t_col.setSpacing(2)
        title = QLabel(t("settings_title"))
        title.setFont(make_font(17, QFont.Weight.ExtraBold))
        title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        sub = QLabel(t("settings_subtitle"))
        sub.setFont(make_font(12, QFont.Weight.Medium))
        sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        t_col.addWidget(title)
        t_col.addWidget(sub)
        h_lay.addLayout(t_col, 1)

        btn_test_lab = Button(t("nav_test"), variant="primary", icon_name="test")
        btn_test_lab.clicked.connect(lambda: self.state.navigation_requested.emit("test"))

        btn_about = Button(t("btn_about_app"), variant="glass", icon_name="about")
        btn_about.clicked.connect(self.aboutRequested.emit)
        btn_reset = Button(t("btn_reset_settings"), variant="ghost", icon_name="trash")
        btn_reset.clicked.connect(self._reset_defaults)
        h_lay.addWidget(btn_test_lab)
        h_lay.addWidget(btn_about)
        h_lay.addWidget(btn_reset)
        outer.addWidget(hdr)

        scroll = QScrollArea(self)
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        scroll.setStyleSheet(modern_scroll_style())

        content = QWidget()
        content.setStyleSheet("background: transparent;")
        c_lay = QVBoxLayout(content)
        c_lay.setContentsMargins(0, 0, 4, 12)
        c_lay.setSpacing(12)

        # 2. Language Drop Panel & Appearance Card
        card_app = GlassCard()
        a_lay = QVBoxLayout(card_app)
        a_lay.setContentsMargins(20, 14, 20, 14)
        a_lay.setSpacing(12)

        lang_items = [(code, label) for code, label, _, _ in SUPPORTED_LANGUAGES]
        self.lang_combo = SearchCombo(lang_items, selected_key=get_language())
        self.lang_combo.setFixedWidth(240)
        self.lang_combo.currentChanged.connect(self.state.set_lang)
        self._add_row(a_lay, t("lbl_language"), self.lang_combo)

        mode_seg = SegmentedControl(
            [("dark", t("mode_dark")), ("light", t("mode_light"))],
            selected_key="light" if not T.is_dark() else "dark",
        )
        mode_seg.changed.connect(lambda m: self.state.set_theme(mode=m))
        self._add_row(a_lay, t("lbl_theme_mode"), mode_seg)

        swatch_w = QWidget()
        sw_outer = QVBoxLayout(swatch_w)
        sw_outer.setContentsMargins(0, 0, 0, 0)
        sw_outer.setSpacing(8)

        sw_grid = QGridLayout()
        sw_grid.setContentsMargins(0, 0, 0, 0)
        sw_grid.setHorizontalSpacing(8)
        sw_grid.setVerticalSpacing(8)
        self._swatches: list[_ColorSwatchButton] = []
        cols = 12
        for idx, pid in enumerate(CALM_PALETTE_IDS):
            sw = _ColorSwatchButton(pid, swatch_w)
            sw.clicked.connect(self._pick_palette)
            sw_grid.addWidget(sw, idx // cols, idx % cols)
            self._swatches.append(sw)
        sw_outer.addLayout(sw_grid)

        custom_col_row = QHBoxLayout()
        custom_col_row.setContentsMargins(0, 0, 0, 0)
        self.btn_custom_accent = Button(
            t("btn_custom_color"),
            variant="glass",
            icon_name="palette",
            tint_hex=T.token_hex("accent"),
        )
        self.btn_custom_accent.setFixedHeight(32)
        self.btn_custom_accent.clicked.connect(self._pick_custom_color_dialog)
        custom_col_row.addWidget(self.btn_custom_accent)
        custom_col_row.addStretch(1)
        sw_outer.addLayout(custom_col_row)

        self._add_row(a_lay, t("lbl_color_accent"), swatch_w)

        c_lay.addWidget(card_app)

        # 1. Input Simulation Method & Live Interception / Ring-0 Kernel Diagnostic Card
        card_input = GlassCard(accent_tint=True)
        in_lay = QVBoxLayout(card_input)
        in_lay.setContentsMargins(20, 14, 20, 14)
        in_lay.setSpacing(10)

        lbl_method = QLabel(t("lbl_input_method"))
        lbl_method.setFont(make_font(13, QFont.Weight.ExtraBold))
        lbl_method.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        in_lay.addWidget(lbl_method)

        self.method_combo = SearchCombo(
            get_input_method_options(),
            selected_key=self.state.config.input_method,
        )
        self.method_combo.setMinimumWidth(520)
        self.method_combo.setFixedHeight(38)
        self.method_combo.currentChanged.connect(self._on_input_method_changed)
        in_lay.addWidget(self.method_combo)

        # Dedicated Interception & Kernel Driver Diagnostic Box inside Settings
        diag_box = GlassCard()
        d_lay = QVBoxLayout(diag_box)
        d_lay.setContentsMargins(16, 12, 16, 12)
        d_lay.setSpacing(8)

        lbl_diag_hdr = QLabel(t("lbl_kernel_diag_title"))
        lbl_diag_hdr.setFont(make_font(13, QFont.Weight.ExtraBold))
        lbl_diag_hdr.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        d_lay.addWidget(lbl_diag_hdr)

        self.lbl_kernel_headline = QLabel("")
        self.lbl_kernel_headline.setWordWrap(True)
        self.lbl_kernel_headline.setFont(make_font(13, QFont.Weight.ExtraBold))
        d_lay.addWidget(self.lbl_kernel_headline)

        self.lbl_kernel_details = QLabel("")
        self.lbl_kernel_details.setWordWrap(True)
        self.lbl_kernel_details.setFont(make_font(12, QFont.Weight.DemiBold))
        self.lbl_kernel_details.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        d_lay.addWidget(self.lbl_kernel_details)

        self.btn_check_interception = Button(
            t("btn_check_interception"),
            variant="primary",
            icon_name="shield",
        )
        self.btn_check_interception.clicked.connect(
            lambda: self._refresh_kernel_diagnostics(run_live_probe=True, show_toast=True)
        )

        self.btn_install_interception = Button(
            t("btn_install_interception"),
            variant="glass",
            icon_name="bolt",
            tint_hex="#10B981",
        )
        self.btn_install_interception.clicked.connect(self._on_install_or_link_interception)

        self.btn_open_drivers = Button(
            t("btn_open_drivers_dir"),
            variant="ghost",
            icon_name="folder",
        )
        self.btn_open_drivers.clicked.connect(lambda: open_folder_or_path(str(get_drivers_dir())))

        diag_btns_bar = ResponsiveFlowBar(
            [
                self.btn_check_interception,
                self.btn_install_interception,
                self.btn_open_drivers,
            ],
            row_height=34,
            h_spacing=8,
            v_spacing=8,
            parent=diag_box,
        )
        d_lay.addWidget(diag_btns_bar)

        in_lay.addWidget(diag_box)

        # Dedicated PS/2 Port 0x60/0x64 (InpOutx64 / i8042prt) Diagnostic Box inside Settings
        ps2_box = GlassCard()
        p2_lay = QVBoxLayout(ps2_box)
        p2_lay.setContentsMargins(16, 12, 16, 12)
        p2_lay.setSpacing(8)

        lbl_ps2_hdr = QLabel(t("lbl_ps2_diag_title"))
        lbl_ps2_hdr.setFont(make_font(13, QFont.Weight.ExtraBold))
        lbl_ps2_hdr.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        p2_lay.addWidget(lbl_ps2_hdr)

        self.lbl_ps2_headline = QLabel("")
        self.lbl_ps2_headline.setWordWrap(True)
        self.lbl_ps2_headline.setFont(make_font(13, QFont.Weight.ExtraBold))
        p2_lay.addWidget(self.lbl_ps2_headline)

        self.lbl_ps2_details = QLabel("")
        self.lbl_ps2_details.setWordWrap(True)
        self.lbl_ps2_details.setFont(make_font(12, QFont.Weight.DemiBold))
        self.lbl_ps2_details.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        p2_lay.addWidget(self.lbl_ps2_details)

        self.btn_check_ps2 = Button(
            t("btn_check_ps2"),
            variant="primary",
            icon_name="shield",
        )
        self.btn_check_ps2.clicked.connect(
            lambda: self._refresh_ps2_diagnostics(run_live_probe=True, show_toast=True)
        )

        self.btn_install_ps2 = Button(
            t("btn_install_ps2"),
            variant="glass",
            icon_name="bolt",
            tint_hex="#06B6D4",
        )
        self.btn_install_ps2.clicked.connect(self._on_install_or_link_ps2)

        self.btn_open_ps2_drivers = Button(
            t("btn_open_drivers_dir"),
            variant="ghost",
            icon_name="folder",
        )
        self.btn_open_ps2_drivers.clicked.connect(lambda: open_folder_or_path(str(get_drivers_dir())))

        ps2_btns_bar = ResponsiveFlowBar(
            [
                self.btn_check_ps2,
                self.btn_install_ps2,
                self.btn_open_ps2_drivers,
            ],
            row_height=34,
            h_spacing=8,
            v_spacing=8,
            parent=ps2_box,
        )
        p2_lay.addWidget(ps2_btns_bar)
        in_lay.addWidget(ps2_box)
        c_lay.addWidget(card_input)

        # 3. Full Backup Card (Export All & Import All Smart Keys, Macros, and Recordings)
        card_backup = GlassCard(accent_tint=True)
        bk_lay = QHBoxLayout(card_backup)
        bk_lay.setContentsMargins(20, 14, 20, 14)
        bk_lay.setSpacing(12)

        bk_lbl = QLabel(t("lbl_backup_section"))
        bk_lbl.setFont(make_font(13, QFont.Weight.ExtraBold))
        bk_lbl.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        bk_lay.addWidget(bk_lbl, 1)

        btn_exp_all = Button(t("btn_export_all"), variant="primary", icon_name="save")
        btn_exp_all.setFixedHeight(36)
        btn_exp_all.clicked.connect(self._export_full_backup)

        btn_imp_all = Button(t("btn_import_all"), variant="glass", icon_name="folder")
        btn_imp_all.setFixedHeight(36)
        btn_imp_all.clicked.connect(self._import_full_backup)

        bk_lay.addWidget(btn_exp_all)
        bk_lay.addWidget(btn_imp_all)
        c_lay.addWidget(card_backup)

        # 4. System Behavior, Windows Startup & Multi-Key Global Hotkeys Card
        card_beh = GlassCard()
        b_lay = QVBoxLayout(card_beh)
        b_lay.setContentsMargins(20, 14, 20, 14)
        b_lay.setSpacing(12)

        admin_ok = is_running_as_admin()
        admin_row = QHBoxLayout()
        admin_row.setSpacing(10)
        admin_txt = t("lbl_admin_ok") if admin_ok else t("lbl_admin_warn")
        admin_lbl = QLabel(admin_txt)
        admin_lbl.setWordWrap(True)
        admin_lbl.setFont(make_font(12, QFont.Weight.Bold))
        admin_lbl.setStyleSheet(f"color: {T.token_hex('ok' if admin_ok else 'accent_hi')}; background: transparent;")
        admin_row.addWidget(admin_lbl, 1)
        if not admin_ok:
            btn_elevate = Button(t("btn_relaunch_admin"), variant="glass", icon_name="shield", tint_hex="#10B981")
            btn_elevate.setFixedHeight(32)
            btn_elevate.clicked.connect(relaunch_as_admin)
            admin_row.addWidget(btn_elevate)
        b_lay.addLayout(admin_row)

        startup_active = is_windows_startup_enabled() or self.state.config.run_on_windows_startup
        tog_startup = ToggleSwitch(startup_active)
        tog_startup.toggled.connect(self._on_startup_toggled)
        self._add_row(b_lay, t("lbl_run_on_startup"), tog_startup)

        tog_sup = ToggleSwitch(self.state.config.suppress_trigger_key)
        tog_sup.toggled.connect(lambda v: self.state.update_setting("suppress_trigger_key", v))
        self._add_row(b_lay, t("lbl_suppress_key"), tog_sup)

        tog_tray = ToggleSwitch(self.state.config.minimize_to_tray)
        tog_tray.toggled.connect(lambda v: self.state.update_setting("minimize_to_tray", v))
        self._add_row(b_lay, t("lbl_min_to_tray"), tog_tray)

        tog_top = ToggleSwitch(self.state.config.always_on_top)
        tog_top.toggled.connect(lambda v: self.state.update_setting("always_on_top", v))
        self._add_row(b_lay, t("lbl_always_on_top"), tog_top)

        tog_snd = ToggleSwitch(self.state.config.sound_effects_enabled)
        tog_snd.toggled.connect(lambda v: self.state.update_setting("sound_effects_enabled", v))
        self._add_row(b_lay, t("lbl_sounds"), tog_snd)

        hk_rec = KeyCaptureButton(self.state.config.hotkey_record_toggle, allow_chord=True, state=self.state)
        hk_rec.setFixedWidth(170)
        hk_rec.keyCaptured.connect(lambda k: self.state.update_setting("hotkey_record_toggle", k or "F9"))
        self._add_row(b_lay, t("lbl_hk_rec"), hk_rec)

        hk_play = KeyCaptureButton(self.state.config.hotkey_playback_toggle, allow_chord=True, state=self.state)
        hk_play.setFixedWidth(170)
        hk_play.keyCaptured.connect(lambda k: self.state.update_setting("hotkey_playback_toggle", k or "F10"))
        self._add_row(b_lay, t("lbl_hk_play"), hk_play)

        hk_combo = KeyCaptureButton(self.state.config.hotkey_combo_master_toggle, allow_chord=True, state=self.state)
        hk_combo.setFixedWidth(170)
        hk_combo.keyCaptured.connect(lambda k: self.state.update_setting("hotkey_combo_master_toggle", k or "F11"))
        self._add_row(b_lay, t("lbl_hk_combo"), hk_combo)

        c_lay.addWidget(card_beh)

        # 5. Dedicated Auto Key Presser Studio Hotkeys Card (Synchronized with Auto Presser Tab)
        card_akp_hk = GlassCard(accent_tint=True)
        akp_hk_lay = QVBoxLayout(card_akp_hk)
        akp_hk_lay.setContentsMargins(20, 14, 20, 14)
        akp_hk_lay.setSpacing(10)

        lbl_akp_hk_title = QLabel(t("settings_akp_hotkeys_title"))
        lbl_akp_hk_title.setFont(make_font(13, QFont.Weight.ExtraBold))
        lbl_akp_hk_title.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        akp_hk_lay.addWidget(lbl_akp_hk_title)

        self.hk_akp_stop_all = KeyCaptureButton(
            self.state.config.hotkey_autopresser_stop_all,
            allow_chord=True,
            state=self.state,
        )
        self.hk_akp_stop_all.setFixedWidth(220)
        self.hk_akp_stop_all.keyCaptured.connect(
            lambda k: self.state.update_setting("hotkey_autopresser_stop_all", k or "F7")
        )
        self._add_row(akp_hk_lay, t("akp_lbl_stop_all_hotkey"), self.hk_akp_stop_all)

        lbl_akp_note = QLabel(t("settings_akp_hotkeys_note"))
        lbl_akp_note.setWordWrap(True)
        lbl_akp_note.setFont(make_font(12, QFont.Weight.DemiBold))
        lbl_akp_note.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        akp_hk_lay.addWidget(lbl_akp_note)

        c_lay.addWidget(card_akp_hk)

        # 6. Dedicated Standalone Emergency Stop Card at the Very Bottom of Settings
        panic_card = GlassCard()
        panic_card.setStyleSheet(
            f"GlassCard {{ border: 1.5px solid {T.token_hex('danger')}; border-radius: 14px; }}"
        )
        p_outer = QVBoxLayout(panic_card)
        p_outer.setContentsMargins(20, 14, 20, 14)
        p_outer.setSpacing(8)

        lbl_panic_hdr = QLabel("⛔ " + t("lbl_panic_section_title"))
        lbl_panic_hdr.setFont(make_font(14, QFont.Weight.ExtraBold))
        lbl_panic_hdr.setStyleSheet(f"color: {T.token_hex('danger')}; background: transparent;")
        p_outer.addWidget(lbl_panic_hdr)

        lbl_panic_desc = QLabel(t("lbl_panic_section_desc"))
        lbl_panic_desc.setWordWrap(True)
        lbl_panic_desc.setFont(make_font(12, QFont.Weight.Medium))
        lbl_panic_desc.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        p_outer.addWidget(lbl_panic_desc)

        p_lay = QHBoxLayout()
        p_lay.setSpacing(12)
        lbl_panic = QLabel("⛔ " + t("lbl_hk_panic_all"))
        lbl_panic.setFont(make_font(13, QFont.Weight.ExtraBold))
        lbl_panic.setStyleSheet(f"color: {T.token_hex('danger')}; background: transparent;")
        hk_stop_play = KeyCaptureButton(self.state.config.hotkey_panic_stop, allow_chord=True, state=self.state)
        hk_stop_play.setFixedWidth(220)
        hk_stop_play.keyCaptured.connect(lambda k: self.state.update_setting("hotkey_panic_stop", k or "Ctrl + Shift + Alt"))
        p_lay.addWidget(lbl_panic, 1)
        p_lay.addWidget(hk_stop_play)
        p_outer.addLayout(p_lay)

        c_lay.addWidget(panic_card)
        c_lay.addStretch(1)

        scroll.setWidget(content)
        outer.addWidget(scroll, 1)

    def _on_external_settings_changed(self, key: str, val: object) -> None:
        if key == "hotkey_autopresser_stop_all" and hasattr(self, "hk_akp_stop_all"):
            self.hk_akp_stop_all.setKeyString(str(val or "F7"))
        elif key == "input_method" and hasattr(self, "method_combo"):
            self.method_combo.setSelectedKey(str(val), emit_signal=False)

    def _on_input_method_changed(self, method_key: str) -> None:
        self.state.update_setting("input_method", method_key)
        self._refresh_kernel_diagnostics(run_live_probe=False, show_toast=False)
        self._refresh_ps2_diagnostics(run_live_probe=False, show_toast=False)

    def _refresh_kernel_diagnostics(self, run_live_probe: bool = True, show_toast: bool = True) -> None:
        diag = diagnose_kernel_drivers(run_live_probe=run_live_probe)
        headline, details, kind = format_kernel_diagnostic_report(diag, is_arabic=is_rtl())
        col_hex = (
            T.token_hex("ok")
            if kind == "ok"
            else "#F59E0B"
            if kind == "warn"
            else T.token_hex("accent_hi")
        )
        self.lbl_kernel_headline.setText(headline)
        self.lbl_kernel_headline.setStyleSheet(f"color: {col_hex}; background: transparent;")
        self.lbl_kernel_details.setText(details)

        if show_toast:
            toast_kind = "ok" if kind == "ok" else "info"
            self.state.toast_requested.emit(headline, toast_kind)

    def _on_install_or_link_interception(self) -> None:
        drv_dir = get_drivers_dir()
        # Check if installer or dll is already in drivers_dir; otherwise let user browse for it
        has_files = any(
            (drv_dir / n).exists()
            for n in ("install-interception.exe", "interception.dll", "inpoutx64.dll")
        )
        picked_path = ""
        if not has_files:
            picked_path, _ = QFileDialog.getOpenFileName(
                self,
                t("btn_install_interception"),
                str(drv_dir),
                "Driver & Bridge Files (interception.dll install-interception.exe inpoutx64.dll *.dll *.exe);;All Files (*.*)",
            )
            if not picked_path:
                return

        ok, msg = install_or_import_interception_file(picked_path)
        self._refresh_kernel_diagnostics(run_live_probe=True, show_toast=False)
        self.state.toast_requested.emit(msg, "ok" if ok else "danger")

    def _refresh_ps2_diagnostics(self, run_live_probe: bool = True, show_toast: bool = True) -> None:
        diag = diagnose_ps2_port_driver(run_live_probe=run_live_probe)
        headline, details, kind = format_ps2_diagnostic_report(diag, is_arabic=is_rtl())
        col_hex = (
            T.token_hex("ok")
            if kind == "ok"
            else "#F59E0B"
            if kind == "warn"
            else T.token_hex("accent_hi")
        )
        self.lbl_ps2_headline.setText(headline)
        self.lbl_ps2_headline.setStyleSheet(f"color: {col_hex}; background: transparent;")
        self.lbl_ps2_details.setText(details)

        if show_toast:
            toast_kind = "ok" if kind == "ok" else "info"
            self.state.toast_requested.emit(headline, toast_kind)

    def _on_install_or_link_ps2(self) -> None:
        drv_dir = get_drivers_dir()
        has_files = any(
            (drv_dir / n).exists()
            for n in ("inpoutx64.dll", "InpOutx64.dll", "WinRing0x64.dll", "InstallDriver.exe")
        )
        picked_path = ""
        if not has_files:
            picked_path, _ = QFileDialog.getOpenFileName(
                self,
                t("btn_install_ps2"),
                str(drv_dir),
                "PS/2 & InpOutx64 Files (inpoutx64.dll InstallDriver.exe WinRing0x64.dll *.dll *.exe);;All Files (*.*)",
            )
            if not picked_path:
                return

        ok, msg = install_or_import_ps2_file(picked_path)
        self._refresh_ps2_diagnostics(run_live_probe=True, show_toast=False)
        self._refresh_kernel_diagnostics(run_live_probe=False, show_toast=False)
        self.state.toast_requested.emit(msg, "ok" if ok else "danger")

    def _export_full_backup(self) -> None:
        if self.engine is None:
            return
        path, _ = QFileDialog.getSaveFileName(
            self,
            t("btn_export_all"),
            "V-MacroPlus_Full_Backup.json",
            "JSON Backup (*.json)",
        )
        if path:
            self.engine.export_all_backup(path)

    def _import_full_backup(self) -> None:
        if self.engine is None:
            return
        path, _ = QFileDialog.getOpenFileName(
            self,
            t("btn_import_all"),
            "",
            "JSON Backup (*.json)",
        )
        if path:
            self.engine.import_all_backup(path)

    def _on_startup_toggled(self, val: bool) -> None:
        set_windows_startup_enabled(val)
        self.state.update_setting("run_on_windows_startup", val)

    def _pick_palette(self, pid: str) -> None:
        self.state.set_theme(palette_id=pid)
        for sw in self._swatches:
            sw.update()

    def _pick_custom_color_dialog(self) -> None:
        initial = QColor(T.token_hex("accent"))
        picked = QColorDialog.getColor(initial, self, t("btn_custom_color"))
        if picked.isValid():
            self.state.set_theme(palette_id="custom", custom_accent=picked.name())
            for sw in self._swatches:
                sw.update()

    def _add_row(self, parent_layout: QVBoxLayout, label_text: str, control: QWidget) -> None:
        row = QHBoxLayout()
        row.setSpacing(12)
        lbl = QLabel(label_text)
        lbl.setFont(make_font(13, QFont.Weight.DemiBold))
        lbl.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        row.addWidget(lbl, 1)
        row.addWidget(control)
        parent_layout.addLayout(row)

    def _reset_defaults(self) -> None:
        fresh = AppConfig()
        fresh.first_run_completed = True
        self.state.config = fresh
        self.state.save_config_async()
        self.state.set_theme(fresh.palette_id, fresh.theme_mode, fresh.custom_accent)
        self.state.toast_requested.emit(
            t("toast_settings_reset"),
            "ok",
        )
