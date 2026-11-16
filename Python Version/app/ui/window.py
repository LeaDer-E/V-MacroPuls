# -*- coding: utf-8 -*-
"""
MainWindow, Integrated TopHeaderBar, Edge/Corner Window Resizer & Floating Recording Stop Indicator:
- Registers `self.rec_floating_pill` screen rect with `AppState.register_stop_button_rect_provider`
  and calls `self.engine.stop_recording(stopped_via_mouse=True)` when clicked so clicking the floating
  Stop button is NEVER recorded!
"""

from __future__ import annotations
from typing import List, Optional, Tuple
from PySide6.QtCore import (
    QEvent,
    QObject,
    QPoint,
    QPointF,
    QRect,
    QRectF,
    Qt,
    QTimer,
    Signal,
)
from PySide6.QtGui import (
    QAction,
    QColor,
    QFont,
    QFontMetrics,
    QLinearGradient,
    QMouseEvent,
    QPainter,
    QPen,
)
from PySide6.QtWidgets import (
    QApplication,
    QHBoxLayout,
    QMessageBox,
    QMainWindow,
    QMenu,
    QSystemTrayIcon,
    QVBoxLayout,
    QWidget,
)
from app import APP_NAME
from app.domain.macro_engine import MacroEngine
from app.domain.win32_input import is_running_as_admin, relaunch_as_admin
from app.i18n import SUPPORTED_LANGUAGES, bidi_text, fmt_num, is_rtl, t
from app.notifier import NOTIFIER
from app.state import AppState
from app.theme import T
from app.ui.about_dialog import AboutOverlay
from app.ui.icons import draw_icon, logo_icon, logo_pixmap, make_qicon
from app.ui.notification import NotificationPopup
from app.ui.onboarding import OnboardingOverlay
from app.ui.pages.autopresser_page import AutoKeyPresserPage
from app.ui.pages.combos_page import CombosPage
from app.ui.pages.recorder_page import RecorderPage
from app.ui.pages.test_page import TestPage
from app.ui.settings_page import SettingsPage
from app.ui.widgets import IconButton, SlideStack, ToastOverlay, _SearchPopup, make_font, refresh_widget_tree_theme


class RecordingFloatingIndicator(QWidget):
    """
    Always-on-top floating pill in the bottom-right corner of the screen during recording.
    Displays a pulsing red dot and `Stop (<Hotkey>)` / `إيقاف (<Hotkey>)` so the user can see
    recording is active across any application or game and click it (or press the hotkey) to stop cleanly.
    """

    stopClicked = Signal()

    def __init__(self, state: AppState) -> None:
        super().__init__(
            None,
            Qt.WindowType.Tool
            | Qt.WindowType.FramelessWindowHint
            | Qt.WindowType.WindowStaysOnTopHint,
        )
        self.state = state
        self._hovered = False
        self._pulse_on = True
        self._event_count = 0

        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground, True)
        self.setAttribute(Qt.WidgetAttribute.WA_ShowWithoutActivating, True)
        self.setMouseTracking(True)
        self.setCursor(Qt.CursorShape.PointingHandCursor)

        self._pulse_timer = QTimer(self)
        self._pulse_timer.setInterval(520)
        self._pulse_timer.timeout.connect(self._toggle_pulse)

        # Register screen geometry provider so clicks on this floating pill are never recorded!
        self.state.register_stop_button_rect_provider(self._get_screen_rect)

    def _get_screen_rect(self) -> Optional[Tuple[int, int, int, int]]:
        if not self.isVisible():
            return None
        g = self. geometry()
        return (g.left(), g.top(), g.right(), g.bottom())

    def _toggle_pulse(self) -> None:
        self._pulse_on = not self._pulse_on
        self.update()

    def show_indicator(self) -> None:
        self._event_count = len(self.state.recorded_events)
        self._recalc_size_and_position()
        self._pulse_timer.start()
        self.show()
        self.raise_()

    def hide_indicator(self) -> None:
        self._pulse_timer.stop()
        self.hide()

    def update_count(self, count: int) -> None:
        self._event_count = count
        if self.isVisible():
            self._recalc_size_and_position()
            self.update()

    def _label_text(self) -> str:
        if self.state.is_playing and not self.state.is_recording:
            hk_play = self.state.config.hotkey_playback_toggle or "F10"
            hk_stop = self.state.config.hotkey_panic_stop or "Esc"
            hk_disp = hk_play if hk_play.upper() == hk_stop.upper() else f"{hk_play} / {hk_stop}"
            return f"{t('btn_stop_play')} ({hk_disp})"
        hk = self.state.config.hotkey_record_toggle or "F9"
        if self._event_count > 0:
            return f"{t('btn_stop_rec')} ({hk})  •  {fmt_num(self._event_count)}"
        return f"{t('btn_stop_rec')} ({hk})" 

    def _recalc_size_and_position(self) -> None:
        font = make_font(13, QFont.Weight.ExtraBold)
        fm = QFontMetrics(font)
        txt = bidi_text(self._label_text())
        w = max(188, fm.horizontalAdvance(txt) + 56)
        h = 46
        self.setFixedSize(w, h)

        screen = QApplication.primaryScreen()
        if screen is not None:
            avail = screen.availableGeometry()
            x = avail.right() - w - 18
            y = avail.bottom() - h - 18
            self.move(x, y)

    def enterEvent(self, event) -> None:
        self._hovered = True
        self.update()

    def leaveEvent(self, event) -> None:
        self._hovered = False
        self.update()

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            self.stopClicked.emit()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)

        r = QRectF(self.rect()).adjusted(2.0, 2.0, -2.0, -2.0)

        is_dk = T.is_dark()
        bg_col = QColor(18, 20, 32, 245) if is_dk else QColor(255, 255, 255, 248)
        border_col = T.c("danger") if self._hovered else T.c("accent")

        if self._hovered:
            bg_col = T.c("danger")

        p.setBrush(bg_col)
        p.setPen(QPen(border_col, 2.0))
        p.drawRoundedRect(r, 11.0, 11.0)

        dot_x = r.left() + 18.0
        dot_y = r.center().y()
        dot_col = QColor("#FFFFFF") if self._hovered else T.c("danger")
        if self._pulse_on:
            glow_col = QColor(dot_col.red(), dot_col.green(), dot_col.blue(), 80)
            p.setBrush(glow_col)
            p.setPen(Qt.PenStyle.NoPen)
            p.drawEllipse(QPointF(dot_x, dot_y), 8.0, 8.0)

        p.setBrush(dot_col)
        p.setPen(Qt.PenStyle.NoPen)
        p.drawEllipse(QPointF(dot_x, dot_y), 4.8, 4.8)

        p.setFont(make_font(13, QFont.Weight.ExtraBold))
        fg_col = QColor("#FFFFFF") if self._hovered else (QColor("#F8FAFC") if is_dk else QColor("#0F172A"))
        p.setPen(fg_col)
        text_rect = QRectF(r.left() + 32.0, r.top(), r.width() - 40.0, r.height())
        p.drawText(
            text_rect,
            int(Qt.AlignmentFlag.AlignVCenter | Qt.AlignmentFlag.AlignCenter),
            bidi_text(self._label_text()),
        )
        p.end()


class _HeaderStatusAndAdminPill(QWidget):
    """Compact Admin / Standard User Privilege Badge in the top header bar (status pill removed per user request)."""

    def __init__(self, state: AppState, parent: QWidget) -> None:
        super().__init__(parent)
        self.state = state
        self._is_admin = is_running_as_admin()
        if not self._is_admin:
            self.setCursor(Qt.CursorShape.PointingHandCursor)
            self.setToolTip(t("btn_relaunch_admin"))
        self.setFixedHeight(32)
        self._recalc_width()

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton and not self._is_admin:
            event.accept()
            return
        super().mousePressEvent(event)

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton and not self._is_admin:
            event.accept()
            if self.rect().contains(event.pos()):
                ans = QMessageBox.question(
                    self.window(),
                    t("dialog_admin_title"),
                    t("dialog_admin_body"),
                    QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No,
                    QMessageBox.StandardButton.No,
                )
                if ans == QMessageBox.StandardButton.Yes:
                    relaunch_as_admin()
            return
        super().mouseReleaseEvent(event)

    def _recalc_width(self) -> None:
        fm = QFontMetrics(make_font(11, QFont.Weight.Bold))
        adm_txt = t("badge_admin" if self._is_admin else "badge_user")
        w_adm = fm.horizontalAdvance(bidi_text(adm_txt)) + 38
        self.setFixedWidth(max(106, w_adm))

    def refresh(self) -> None:
        self._is_admin = is_running_as_admin()
        self._recalc_width()
        self.update()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        font = make_font(11, QFont.Weight.Bold)
        p.setFont(font)

        adm_txt = t("badge_admin" if self._is_admin else "badge_user")
        adm_r = QRectF(0, 2, float(self.width()), 28)
        adm_col = T.c("ok") if self._is_admin else T.c("text3")
        p.setBrush(QColor(adm_col.red(), adm_col.green(), adm_col.blue(), 32))
        p.setPen(QPen(QColor(adm_col.red(), adm_col.green(), adm_col.blue(), 115), 1.0))
        p.drawRoundedRect(adm_r, 9.0, 9.0)

        draw_icon(p, "shield", QRectF(adm_r.left() + 8, adm_r.center().y() - 7, 14, 14), color=adm_col, stroke_width=2.0)
        p.setPen(adm_col)
        p.drawText(
            adm_r.adjusted(24, 0, -6, 0),
            int(Qt.AlignmentFlag.AlignVCenter | Qt.AlignmentFlag.AlignCenter),
            bidi_text(adm_txt),
        )
        p.end()


class _CenterTabsBar(QWidget):
    """Sleek 5-Tab Pill Widget (`Smart Keys` | `Recordings` | `Test Lab` | `Settings` | `About`)."""

    tabSelected = Signal(str)
    aboutClicked = Signal()

    def __init__(self, parent: QWidget) -> None:
        super().__init__(parent)
        self._active_tab = "combos"
        self._hover_tab = -1
        self.setFixedSize(620, 36)
        self.setMouseTracking(True)

        self._tabs: List[Tuple[str, str, str]] = [
            ("combos", "bolt", "nav_combos"),
            ("autopresser", "keyboard", "nav_autopresser"),
            ("recorder", "recorder", "nav_recorder"),
            ("test", "test", "nav_test"),
            ("settings", "settings", "nav_settings"),
            ("about", "about", "nav_about"),
        ]

    def setActiveTab(self, tab_id: str) -> None:
        self._active_tab = tab_id
        self.update()

    def _tab_rects(self) -> List[Tuple[str, QRectF]]:
        w = float(self.width()) - 6.0
        seg_w = w / len(self._tabs)
        ordered = list(reversed(self._tabs)) if is_rtl() else self._tabs
        return [(tid, QRectF(3.0 + i * seg_w, 2.5, seg_w, 31.0)) for i, (tid, _, _) in enumerate(ordered)]

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        pos = event.position()
        hit = -1
        for i, (_, r) in enumerate(self._tab_rects()):
            if r.contains(pos):
                hit = i
                break
        if hit != self._hover_tab:
            self._hover_tab = hit
            self.setCursor(Qt.CursorShape.PointingHandCursor if hit >= 0 else Qt.CursorShape.ArrowCursor)
            self.update()

    def leaveEvent(self, event) -> None:
        self._hover_tab = -1
        self.update()

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            pos = event.position()
            for tid, r in self._tab_rects():
                if r.contains(pos):
                    if tid == "about":
                        self.aboutClicked.emit()
                    else:
                        self._active_tab = tid
                        self.update()
                        self.tabSelected.emit(tid)
                    return

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        bar_r = QRectF(self.rect()).adjusted(0.5, 0.5, -0.5, -0.5)
        p.setBrush(T.c("bg0", 175))
        p.setPen(QPen(T.c("border"), 1.0))
        p.drawRoundedRect(bar_r, 11.0, 11.0)

        tab_meta = {tid: (ic, tr_k) for tid, ic, tr_k in self._tabs}
        for i, (tid, tr) in enumerate(self._tab_rects()):
            _, tr_key = tab_meta[tid]
            active = tid == self._active_tab
            hovered = i == self._hover_tab

            if active:
                grad = QLinearGradient(tr.topLeft(), tr.bottomRight())
                grad.setColorAt(0.0, T.c("accent"))
                grad.setColorAt(1.0, T.c("accent2"))
                p.setBrush(grad)
                p.setPen(QPen(T.c("accent_hi", 120), 1.0))
                p.drawRoundedRect(tr.adjusted(1, 0.5, -1, -0.5), 8.5, 8.5)
                fg = T.c("on_accent")
            elif hovered:
                p.setBrush(T.c("surface_hi", 140))
                p.setPen(Qt.PenStyle.NoPen)
                p.drawRoundedRect(tr.adjusted(1, 0.5, -1, -0.5), 8.5, 8.5)
                fg = T.c("text")
            else:
                fg = T.c("text2")

            p.setFont(make_font(12, QFont.Weight.Bold if active else QFont.Weight.DemiBold))
            p.setPen(fg)
            p.drawText(
                tr,
                int(Qt.AlignmentFlag.AlignAbsolute | Qt.AlignmentFlag.AlignCenter),
                bidi_text(t(tr_key)),
            )
        p.end()


class TopHeaderBar(QWidget):
    """Integrated 52px Top Header Bar."""

    tabSelected = Signal(str)
    aboutClicked = Signal()
    minimizeClicked = Signal()
    maximizeClicked = Signal()
    closeClicked = Signal()

    def __init__(self, state: AppState, parent: QWidget) -> None:
        super().__init__(parent)
        self.state = state
        self.setFixedHeight(52)
        self.setLayoutDirection(Qt.LayoutDirection.LeftToRight)

        lay = QHBoxLayout(self)
        lay.setContentsMargins(24, 6, 20, 4)
        lay.setSpacing(6)

        lay.addSpacing(148)
        lay.addStretch(1)

        self.tabs_bar = _CenterTabsBar(self)
        self.tabs_bar.tabSelected.connect(self.tabSelected.emit)
        self.tabs_bar.aboutClicked.connect(self.aboutClicked.emit)
        lay.addWidget(self.tabs_bar)

        lay.addStretch(1)

        self.status_pill = _HeaderStatusAndAdminPill(self.state, self)
        lay.addWidget(self.status_pill)

        self.btn_lang = IconButton("globe", size=30, variant="ghost", tooltip=t("lbl_language"))
        self.btn_lang.clicked.connect(self._open_language_drop_panel)

        self.lang_popup = _SearchPopup(self)
        self.lang_popup.itemSelected.connect(lambda code, _: self.state.set_lang(code))

        self.btn_about = IconButton("about", size=30, variant="ghost", tooltip=t("nav_about"))
        self.btn_about.clicked.connect(self.aboutClicked.emit)

        self.btn_min = IconButton("minimize", size=30, variant="ghost")
        self.btn_min.clicked.connect(self.minimizeClicked.emit)

        self.btn_max = IconButton("maximize", size=30, variant="ghost")
        self.btn_max.clicked.connect(self.maximizeClicked.emit)

        self.btn_close = IconButton("close", size=30, variant="danger")
        self.btn_close.clicked.connect(self.closeClicked.emit)

        lay.addWidget(self.btn_lang)
        lay.addWidget(self.btn_about)
        lay.addWidget(self.btn_min)
        lay.addWidget(self.btn_max)
        lay.addWidget(self.btn_close)

    def _open_language_drop_panel(self) -> None:
        items = [(code, label, None) for code, label, _, _ in SUPPORTED_LANGUAGES]
        self.lang_popup.populate(items)
        self.lang_popup.setFixedSize(230, 290)
        gpos = self.btn_lang.mapToGlobal(QPoint(-140, self.btn_lang.height() + 6))
        self.lang_popup.move(gpos)
        self.lang_popup.show()
        self.lang_popup.search_edit.setFocus()

    def setActiveTab(self, tab_id: str) -> None:
        self.tabs_bar.setActiveTab(tab_id)

    def refresh_status(self) -> None:
        self.btn_lang.setToolTip(t("lbl_language"))
        self.btn_about.setToolTip(t("nav_about"))
        self.lang_popup.search_edit.setPlaceholderText(t("search_placeholder"))
        self.status_pill.refresh()
        self.tabs_bar.update()
        self.update()

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            if self.childAt(event.position().toPoint()) is None:
                win = self.window().windowHandle()
                if win is not None:
                    win.startSystemMove()
        super().mousePressEvent(event)

    def mouseDoubleClickEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            self.maximizeClicked.emit()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)

        hdr_r = QRectF(16.0, 5.0, float(self.width()) - 32.0, float(self.height()) - 7.0)
        p.setBrush(T.c("surface", 165))
        p.setPen(QPen(T.c("border"), 1.0))
        p.drawRoundedRect(hdr_r, 14.0, 14.0)

        pix = logo_pixmap(size=28, dpr=2.0)
        p.drawPixmap(26, int(hdr_r.center().y() - 14), pix)
        p.setFont(make_font(15, QFont.Weight.ExtraBold))
        p.setPen(T.c("text"))
        p.drawText(
            QRectF(62, hdr_r.top(), 130, hdr_r.height()),
            int(Qt.AlignmentFlag.AlignVCenter | Qt.AlignmentFlag.AlignLeft),
            APP_NAME,
        )
        p.end()


class _CornerResizeGrip(QWidget):
    """Visible diagonal corner resize grip placed at the bottom-right and bottom-left of MainWindow."""

    def __init__(self, main_win: "MainWindow", is_left: bool = False) -> None:
        super().__init__(main_win)
        self._main_win = main_win
        self._is_left = is_left
        self._dragging = False
        self._drag_start_gpos = QPoint()
        self._drag_start_geom = QRect()
        self.setFixedSize(22, 22)
        self.setCursor(
            Qt.CursorShape.SizeBDiagCursor if is_left else Qt.CursorShape.SizeFDiagCursor
        )

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton and not self._main_win.isMaximized():
            edges = (
                (Qt.Edge.BottomEdge | Qt.Edge.LeftEdge)
                if self._is_left
                else (Qt.Edge.BottomEdge | Qt.Edge.RightEdge)
            )
            wh = self._main_win.windowHandle()
            if wh is not None and wh.startSystemResize(edges):
                event.accept()
                return
            self._dragging = True
            self._drag_start_gpos = event.globalPosition().toPoint()
            self._drag_start_geom = self._main_win.geometry()
            event.accept()

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        if self._dragging:
            gpos = event.globalPosition().toPoint()
            self._main_win._apply_manual_resize_delta(
                gpos - self._drag_start_gpos,
                self._drag_start_geom,
                (Qt.Edge.BottomEdge | Qt.Edge.LeftEdge)
                if self._is_left
                else (Qt.Edge.BottomEdge | Qt.Edge.RightEdge),
            )
            event.accept()

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if self._dragging:
            self._dragging = False
            event.accept()

    def paintEvent(self, event) -> None:
        if self._main_win.isMaximized():
            return
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        pen = QPen(T.c("accent_hi", 150), 1.6)
        pen.setCapStyle(Qt.PenCapStyle.RoundCap)
        p.setPen(pen)
        if self._is_left:
            p.drawLine(QPointF(4, 14), QPointF(8, 18))
            p.drawLine(QPointF(4, 9), QPointF(13, 18))
        else:
            p.drawLine(QPointF(18, 14), QPointF(14, 18))
            p.drawLine(QPointF(18, 9), QPointF(9, 18))
        p.end()


class MainWindow(QMainWindow):
    """Frameless outer shell with safe minimum size (`1060x640`), 8-direction resizing, and floating stop indicator."""

    OUTER_MARGIN = 10
    RESIZE_BORDER = 11

    def __init__(self, state: AppState, engine: MacroEngine) -> None:
        super().__init__()
        self.state = state
        self.engine = engine

        self._manual_resizing = False
        self._resize_edges = Qt.Edges()
        self._resize_start_gpos = QPoint()
        self._resize_start_geom = QRect()
        self._edge_cursor_override = False

        self.setWindowTitle(APP_NAME)
        self.setWindowIcon(logo_icon(64))
        self.setMinimumSize(1180, 700)
        self.resize(1240, 760)
        self.setMouseTracking(True)
        self.setWindowFlags(
            Qt.WindowType.FramelessWindowHint
            | Qt.WindowType.Window
            | Qt.WindowType.WindowSystemMenuHint
            | Qt.WindowType.WindowMinMaxButtonsHint
        )
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground, True)

        if self.state.config.always_on_top:
            self.setWindowFlag(Qt.WindowType.WindowStaysOnTopHint, True)

        self.central = QWidget(self)
        self.central.setMouseTracking(True)
        self.setCentralWidget(self.central)

        self.root_layout = QVBoxLayout(self.central)
        self.root_layout.setContentsMargins(
            self.OUTER_MARGIN, self.OUTER_MARGIN, self.OUTER_MARGIN, self.OUTER_MARGIN
        )
        self.root_layout.setSpacing(0)

        self.header_bar = TopHeaderBar(self.state, self.central)
        self.header_bar.tabSelected.connect(self.navigate_to)
        self.header_bar.aboutClicked.connect(self._show_about)
        self.header_bar.minimizeClicked.connect(self.showMinimized)
        self.header_bar.maximizeClicked.connect(self._toggle_maximize)
        self.header_bar.closeClicked.connect(self._on_close_clicked)
        self.root_layout.addWidget(self.header_bar)

        self.page_stack = SlideStack(self.central)
        self.root_layout.addWidget(self.page_stack, 1)

        self.toast = ToastOverlay(self.central)
        self.about_overlay = AboutOverlay(self.central)
        self.onboarding = OnboardingOverlay(self.state, self.central)
        self.onboarding.hide()
        self.popup = NotificationPopup()

        self.grip_br = _CornerResizeGrip(self, is_left=False)
        self.grip_bl = _CornerResizeGrip(self, is_left=True)
        self._reposition_corner_grips()

        self.rec_floating_pill = RecordingFloatingIndicator(self.state)
        self.rec_floating_pill.stopClicked.connect(self._on_floating_stop_clicked)

        self._page_map: dict[str, int] = {}
        self._rebuild_pages()
        self._setup_tray()

        app_inst = QApplication.instance()
        if app_inst is not None:
            app_inst.installEventFilter(self)

        T.theme_changed.connect(self._on_theme_changed)
        self.state.language_changed.connect(lambda _: self._on_language_changed())
        self.state.navigation_requested.connect(self.navigate_to)
        self.state.recording_state_changed.connect(self._on_recording_state_changed)
        self.state.recording_count_changed.connect(self.rec_floating_pill.update_count)
        self.state.playback_state_changed.connect(self._on_playback_state_changed)
        self.state.combos_master_changed.connect(lambda _: self._refresh_status_surfaces())
        NOTIFIER.toast_requested.connect(self.toast.show_toast)
        NOTIFIER.popup_requested.connect(self.popup.present)
        self.state.activate_window_requested.connect(self._restore_from_tray)

    def showEvent(self, event) -> None:
        super().showEvent(event)
        self._reposition_corner_grips()
        if not self.state.config.first_run_completed:
            QTimer.singleShot(0, self._present_onboarding_full)

    def _present_onboarding_full(self) -> None:
        if not self.state.config.first_run_completed:
            self.onboarding.setGeometry(self.central.rect())
            self.onboarding.refresh_texts()
            self.onboarding.show()
            self.onboarding.raise_()

    def _reposition_corner_grips(self) -> None:
        m = 0 if self.isMaximized() else self.OUTER_MARGIN
        self.grip_br.move(self.width() - m - 24, self.height() - m - 24)
        self.grip_bl.move(m + 2, self.height() - m - 24)
        self.grip_br.raise_()
        self.grip_bl.raise_()

    def _on_recording_state_changed(self, active: bool) -> None:
        self._refresh_status_surfaces()
        show_play_pill = bool(getattr(self.state.config, "show_floating_playback_pill", False))
        if active or (self.state.is_playing and show_play_pill):
            self.rec_floating_pill.show_indicator()
        else:
            self.rec_floating_pill.hide_indicator()

    def _on_playback_state_changed(self, active: bool) -> None:
        self._refresh_status_surfaces()
        show_play_pill = bool(getattr(self.state.config, "show_floating_playback_pill", False))
        if (active and show_play_pill) or self.state.is_recording:
            self.rec_floating_pill.show_indicator()
        else:
            self.rec_floating_pill.hide_indicator()

    def _on_floating_stop_clicked(self) -> None:
        if self.state.is_recording:
            self.engine.stop_recording(stopped_via_mouse=True)
        elif self.state.is_playing:
            self.engine.stop_all()

    def _rebuild_pages(self) -> None:
        valid_ids = ("combos", "autopresser", "recorder", "test", "settings")
        active_id = self.state.current_page_id if self.state.current_page_id in valid_ids else "combos"
        self.setUpdatesEnabled(False)
        try:
            while self.page_stack.count():
                w = self.page_stack.widget(0)
                w.hide()
                self.page_stack.removeWidget(w)
                w.deleteLater()

            self._page_map.clear()
            pages: List[Tuple[str, QWidget]] = [
                ("combos", CombosPage(self.state, self.engine, self.page_stack)),
                ("autopresser", AutoKeyPresserPage(self.state, self.engine, self.page_stack)),
                ("recorder", RecorderPage(self.state, self.engine, self.page_stack)),
                ("test", TestPage(self.state, self.engine, self.page_stack)),
                ("settings", SettingsPage(self.state, self.engine, on_about=self._show_about, parent=self.page_stack)),
            ]
            for idx, (pid, widget) in enumerate(pages):
                self.page_stack.addWidget(widget)
                self._page_map[pid] = idx

            self.page_stack.setCurrentIndex(self._page_map.get(active_id, 0))
            self.header_bar.setActiveTab(active_id)
            self._reposition_corner_grips()
        finally:
            self.setUpdatesEnabled(True)

    def _on_theme_changed(self) -> None:
        """Instant zero-flicker in-place theme update (never destroys/recreates pages or opens windows!)."""
        icon = logo_icon(64)
        self.setWindowIcon(icon)
        if hasattr(self, "tray") and self.tray is not None:
            self.tray.setIcon(icon)
            self._rebuild_tray_menu()
        refresh_widget_tree_theme(self.central)
        self.about_overlay.refresh_styles()
        self.onboarding.refresh_texts()
        self.header_bar.refresh_status()
        if self.rec_floating_pill.isVisible():
            self.rec_floating_pill.update()
        self.update()

    def _on_language_changed(self) -> None:
        """Rebuild localized text across pages cleanly with updates suspended to prevent any flicker."""
        icon = logo_icon(64)
        self.setWindowIcon(icon)
        if hasattr(self, "tray") and self.tray is not None:
            self.tray.setIcon(icon)
            self._rebuild_tray_menu()
        self._rebuild_pages()
        self.about_overlay.refresh_styles()
        self.onboarding.refresh_texts()
        if self.onboarding.isVisible():
            self.onboarding.setGeometry(self.central.rect())
            self.onboarding.raise_()
        self.header_bar.refresh_status()
        if self.rec_floating_pill.isVisible():
            self.rec_floating_pill._recalc_size_and_position()
            self.rec_floating_pill.update()
        self.update()

    def _on_theme_or_lang_changed(self) -> None:
        self._on_language_changed()

    def navigate_to(self, page_id: str) -> None:
        if page_id in self._page_map:
            self.state.current_page_id = page_id
            self.header_bar.setActiveTab(page_id)
            self.page_stack.setCurrentIndexSmooth(self._page_map[page_id])

    def _show_about(self) -> None:
        self.about_overlay.show_modal()

    def _toggle_maximize(self) -> None:
        if self.isMaximized():
            self.showNormal()
            self.root_layout.setContentsMargins(
                self.OUTER_MARGIN, self.OUTER_MARGIN, self.OUTER_MARGIN, self.OUTER_MARGIN
            )
        else:
            self.root_layout.setContentsMargins(0, 0, 0, 0)
            self.showMaximized()
        self._reposition_corner_grips()
        self.update()

    def _on_close_clicked(self) -> None:
        if self.state.config.minimize_to_tray and hasattr(self, "tray") and self.tray.isSystemTrayAvailable():
            self.hide()
        else:
            self._quit_application()

    def _quit_application(self) -> None:
        self.rec_floating_pill.hide_indicator()
        self.engine.shutdown()
        if hasattr(self, "tray") and self.tray is not None:
            self.tray.hide()
        QApplication.quit()

    def _setup_tray(self) -> None:
        self.tray = QSystemTrayIcon(logo_icon(64), self)
        self.tray.setToolTip(APP_NAME)
        self.tray_menu = QMenu()
        self._rebuild_tray_menu()
        self.tray.setContextMenu(self.tray_menu)
        self.tray.activated.connect(self._on_tray_activated)
        if QSystemTrayIcon.isSystemTrayAvailable():
            self.tray.show()

    def _rebuild_tray_menu(self) -> None:
        self.tray_menu.clear()
        self.tray_menu.setStyleSheet(
            f"""
            QMenu {{
                background-color: {T.token_hex('bg1')};
                color: {T.token_hex('text')};
                border: 1px solid {T.token_hex('border_hi')};
                border-radius: 12px;
                padding: 6px;
                font-size: 13px;
            }}
            QMenu::item {{
                padding: 7px 16px;
                border-radius: 8px;
            }}
            QMenu::item:selected {{
                background-color: {T.token_hex('surface_hi')};
                color: {T.token_hex('accent_hi')};
            }}
            """
        )

        st_label = f"{APP_NAME} — {t('status_combos_active' if self.state.combos_active else 'status_idle')}"
        act_status = QAction(logo_icon(20), st_label, self.tray_menu)
        act_status.setEnabled(False)
        self.tray_menu.addAction(act_status)
        self.tray_menu.addSeparator()

        act_show = QAction(make_qicon("bolt", color=T.c("accent_hi")), t("tray_show"), self.tray_menu)
        act_show.triggered.connect(self._restore_from_tray)
        self.tray_menu.addAction(act_show)

        combo_st = t("state_on") if self.state.combos_active else t("state_off")
        act_combos = QAction(
            make_qicon("bolt", color=T.c("ok" if self.state.combos_active else "text3")),
            f"{t('tray_toggle_combos')}{combo_st}",
            self.tray_menu,
        )
        act_combos.triggered.connect(self.engine.toggle_combos_master)
        self.tray_menu.addAction(act_combos)

        act_rec = QAction(make_qicon("record", color=T.c("danger")), t("tray_quick_rec"), self.tray_menu)
        act_rec.triggered.connect(lambda: self.engine.toggle_recording(via_mouse=True))
        self.tray_menu.addAction(act_rec)

        act_play = QAction(make_qicon("play", color=T.c("ok")), t("tray_quick_play"), self.tray_menu)
        act_play.triggered.connect(self.engine.toggle_playback)
        self.tray_menu.addAction(act_play)

        act_about = QAction(make_qicon("about", color=T.c("text")), t("nav_about"), self.tray_menu)
        act_about.triggered.connect(self._show_about_from_tray)
        self.tray_menu.addAction(act_about)

        self.tray_menu.addSeparator()
        act_quit = QAction(make_qicon("close", color=T.c("danger")), t("tray_quit"), self.tray_menu)
        act_quit.triggered.connect(self._quit_application)
        self.tray_menu.addAction(act_quit)

    def _show_about_from_tray(self) -> None:
        self._restore_from_tray()
        self._show_about()

    def _restore_from_tray(self) -> None:
        self.showNormal()
        self.raise_()
        self.activateWindow()

    def _on_tray_activated(self, reason: QSystemTrayIcon.ActivationReason) -> None:
        if reason == QSystemTrayIcon.ActivationReason.Trigger:
            self._restore_from_tray()

    def _refresh_status_surfaces(self) -> None:
        self.header_bar.refresh_status()
        if hasattr(self, "tray_menu"):
            self._rebuild_tray_menu()

    def resizeEvent(self, event) -> None:
        super().resizeEvent(event)
        self._reposition_corner_grips()
        if self.about_overlay.isVisible():
            self.about_overlay.setGeometry(self.central.rect())
        if self.onboarding.isVisible():
            self.onboarding.setGeometry(self.central.rect())

    def _hit_test_edges(self, pos: QPoint) -> Qt.Edges:
        if self.isMaximized():
            return Qt.Edges()
        b = self.OUTER_MARGIN + self.RESIZE_BORDER
        w, h = self.width(), self.height()
        edges = Qt.Edges()
        if pos.x() <= b:
            edges |= Qt.Edge.LeftEdge
        elif pos.x() >= w - b:
            edges |= Qt.Edge.RightEdge
        if pos.y() <= b:
            edges |= Qt.Edge.TopEdge
        elif pos.y() >= h - b:
            edges |= Qt.Edge.BottomEdge
        return edges

    @staticmethod
    def _cursor_for_edges(edges: Qt.Edges) -> Optional[Qt.CursorShape]:
        if not edges:
            return None
        left = bool(edges & Qt.Edge.LeftEdge)
        right = bool(edges & Qt.Edge.RightEdge)
        top = bool(edges & Qt.Edge.TopEdge)
        bottom = bool(edges & Qt.Edge.BottomEdge)
        if (left and top) or (right and bottom):
            return Qt.CursorShape.SizeFDiagCursor
        if (right and top) or (left and bottom):
            return Qt.CursorShape.SizeBDiagCursor
        if left or right:
            return Qt.CursorShape.SizeHorCursor
        if top or bottom:
            return Qt.CursorShape.SizeVerCursor
        return None

    def _apply_manual_resize_delta(self, delta: QPoint, start_geom: QRect, edges: Qt.Edges) -> None:
        min_w, min_h = self.minimumWidth(), self.minimumHeight()
        x, y, w, h = start_geom.x(), start_geom.y(), start_geom.width(), start_geom.height()

        if edges & Qt.Edge.RightEdge:
            w = max(min_w, start_geom.width() + delta.x())
        elif edges & Qt.Edge.LeftEdge:
            new_w = max(min_w, start_geom.width() - delta.x())
            x = start_geom.right() - new_w + 1
            w = new_w

        if edges & Qt.Edge.BottomEdge:
            h = max(min_h, start_geom.height() + delta.y())
        elif edges & Qt.Edge.TopEdge:
            new_h = max(min_h, start_geom.height() - delta.y())
            y = start_geom.bottom() - new_h + 1
            h = new_h

        self.setGeometry(x, y, w, h)

    def eventFilter(self, watched: QObject, event: QEvent) -> bool:
        etype = event.type()
        if etype not in (
            QEvent.Type.MouseMove,
            QEvent.Type.MouseButtonPress,
            QEvent.Type.MouseButtonRelease,
        ):
            return False
        if not self.isVisible() or self.isMaximized():
            return False
        if not self._manual_resizing and not self._edge_cursor_override:
            if watched not in (self, self.central, self.header_bar):
                return False
        if etype == QEvent.Type.MouseMove and isinstance(event, QMouseEvent):
            gpos = event.globalPosition().toPoint()
            if self._manual_resizing:
                self._apply_manual_resize_delta(
                    gpos - self._resize_start_gpos,
                    self._resize_start_geom,
                    self._resize_edges,
                )
                return True

            local_pos = self.mapFromGlobal(gpos)
            if self.rect().contains(local_pos):
                edges = self._hit_test_edges(local_pos)
                c_shape = self._cursor_for_edges(edges)
                if c_shape is not None:
                    if not self._edge_cursor_override:
                        QApplication.setOverrideCursor(c_shape)
                        self._edge_cursor_override = True
                    else:
                        QApplication.changeOverrideCursor(c_shape)
                elif self._edge_cursor_override:
                    QApplication.restoreOverrideCursor()
                    self._edge_cursor_override = False
            elif self._edge_cursor_override:
                QApplication.restoreOverrideCursor()
                self._edge_cursor_override = False

        elif etype == QEvent.Type.MouseButtonPress and isinstance(event, QMouseEvent):
            if event.button() == Qt.MouseButton.LeftButton:
                gpos = event.globalPosition().toPoint()
                local_pos = self.mapFromGlobal(gpos)
                if self.rect().contains(local_pos):
                    edges = self._hit_test_edges(local_pos)
                    if edges:
                        wh = self.windowHandle()
                        if wh is not None and wh.startSystemResize(edges):
                            return True
                        self._manual_resizing = True
                        self._resize_edges = edges
                        self._resize_start_gpos = gpos
                        self._resize_start_geom = self.geometry()
                        return True

        elif etype == QEvent.Type.MouseButtonRelease and isinstance(event, QMouseEvent):
            if self._manual_resizing and event.button() == Qt.MouseButton.LeftButton:
                self._manual_resizing = False
                self._resize_edges = Qt.Edges()
                if self._edge_cursor_override:
                    QApplication.restoreOverrideCursor()
                    self._edge_cursor_override = False
                return True

        return super().eventFilter(watched, event)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)

        if self.isMaximized():
            p.fillRect(self.rect(), T.c("bg0"))
            p.end()
            return

        p.fillRect(self.rect(), QColor(0, 0, 0, 3))

        m = float(self.OUTER_MARGIN)
        shell_rect = QRectF(m, m, self.width() - 2 * m, self.height() - 2 * m)
        radius = 18.0

        for spread, alpha in ((2.0, 32), (5.0, 14)):
            sr = shell_rect.adjusted(-spread, -spread + 1.5, spread, spread + 1.5)
            p.setBrush(Qt.BrushStyle.NoBrush)
            p.setPen(QPen(QColor(0, 0, 0, alpha), 2.5))
            p.drawRoundedRect(sr, radius + spread * 0.4, radius + spread * 0.4)

        grad = QLinearGradient(shell_rect.topLeft(), shell_rect.bottomRight())
        grad.setColorAt(0.0, T.c("bg0"))
        grad.setColorAt(1.0, T.c("bg1"))
        p.setBrush(grad)
        p.setPen(QPen(T.c("border_hi"), 1.1))
        p.drawRoundedRect(shell_rect, radius, radius)
        p.end()
