# -*- coding: utf-8 -*-
"""
100% Custom QPainter Widgets for V-MacroPlus v3.1.0:
- `_SearchPopup.populate()` dynamically updates `self.search_edit.setPlaceholderText(t("search_placeholder"))`
  so it NEVER shows Arabic `بحث...` when English or another language is active!
- `DragGripHandle` (`⠿`) and live drag-and-drop highlighting on `StepCard` (`setIndex`, `setDragging`).
- `KeyCaptureButton` relies on `Win32KeyboardHook` on Windows so Media / Calculator keys never become `Key_0`.
"""

from __future__ import annotations
import os
from typing import Dict, List, Optional, Tuple
from PySide6.QtCore import (
    QEasingCurve,
    QFileInfo,
    QPoint,
    QPointF,
    Property,
    QPropertyAnimation,
    QRectF,
    QSize,
    Qt,
    QTimer,
    Signal,
)
from PySide6.QtGui import (
    QColor,
    QFont,
    QFontMetrics,
    QIcon,
    QKeyEvent,
    QLinearGradient,
    QMouseEvent,
    QPainter,
    QPen,
    QPixmap,
)
from PySide6.QtWidgets import (
    QFileIconProvider,
    QHBoxLayout,
    QLineEdit,
    QScrollArea,
    QSizePolicy,
    QStackedWidget,
    QVBoxLayout,
    QWidget,
)
from app.domain.win32_input import (
    IS_WINDOWS,
    is_modifier_scancode,
    label_for_scancode,
    scancode_for_label,
)
from app.i18n import bidi_text, fmt_num, get_lang_font_family, is_rtl, t, tl
from app.theme import T
from app.ui.icons import draw_icon

_ACTIVE_KEY_CAPTURE_WIDGET: Optional["KeyCaptureButton"] = None
_ICON_PROVIDER: Optional[QFileIconProvider] = None
_APP_ICON_CACHE: Dict[str, QPixmap] = {}


def framed_horizontal_scroll_style(tint_hex: str = "") -> str:
    """Distinct tinted rectangular container style for horizontal scroll strips (Tabs & Excluded Apps)."""
    accent = tint_hex or T.token_hex("accent")
    qc = QColor(accent)
    bg_alpha = 36 if T.is_dark() else 24
    border_alpha = 165 if T.is_dark() else 145
    bg_rgba = f"rgba({qc.red()}, {qc.green()}, {qc.blue()}, {bg_alpha})"
    border_rgba = f"rgba({qc.red()}, {qc.green()}, {qc.blue()}, {border_alpha})"
    thumb_rgba = f"rgba({qc.red()}, {qc.green()}, {qc.blue()}, 195)"
    return f"""
        QScrollArea {{
            background: {bg_rgba};
            border: 1.5px solid {border_rgba};
            border-radius: 10px;
            padding: 1px 4px;
        }}
        QScrollBar:horizontal {{
            background: transparent;
            height: 6px;
            margin: 0px 4px 1px 4px;
            border-radius: 3px;
        }}
        QScrollBar::handle:horizontal {{
            background: {thumb_rgba};
            min-width: 28px;
            border-radius: 3px;
        }}
        QScrollBar::handle:horizontal:hover {{
            background: {accent};
        }}
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {{
            width: 0px;
            background: none;
        }}
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {{
            background: transparent;
        }}
    """



def refresh_widget_tree_theme(root: QWidget) -> None:
    """
    Fast in-place theme updater for standard & custom widgets inside `root`.
    Updates QScrollArea, QLineEdit, QPlainTextEdit, QLabel inline stylesheets and Button accent tints
    without destroying or recreating any page or spawning any top-level OS window!
    """
    old_map = getattr(T, "prev_tokens", {}) or {}
    new_map = T.current_tokens() if hasattr(T, "current_tokens") else {}
    # Build case-insensitive color replacement pairs for token hex values
    replacements: List[Tuple[str, str]] = []
    for k in ("bg0", "bg1", "bg2", "surface", "surface_hi", "border", "border_hi", "text", "text2", "text3", "accent", "accent2", "accent_hi", "ok", "danger"):
        old_hx = old_map.get(k, "")
        new_hx = new_map.get(k, "")
        if old_hx and new_hx and old_hx.lower() != new_hx.lower():
            replacements.append((old_hx, new_hx))

    old_acc = old_map.get("accent", "").lower()
    new_acc = new_map.get("accent", "")

    root.setUpdatesEnabled(False)
    try:
        all_widgets = [root] + root.findChildren(QWidget)
        for w in all_widgets:
            if isinstance(w, QScrollArea):
                w.setStyleSheet(modern_scroll_style())
            else:
                ss = w.styleSheet()
                if ss and replacements:
                    new_ss = ss
                    for o_hx, n_hx in replacements:
                        new_ss = new_ss.replace(o_hx, n_hx).replace(o_hx.lower(), n_hx).replace(o_hx.upper(), n_hx)
                    if new_ss != ss:
                        w.setStyleSheet(new_ss)
                if hasattr(w, "_tint_hex") and getattr(w, "_tint_hex", "").lower() == old_acc and new_acc:
                    setattr(w, "_tint_hex", new_acc)
            w.update()
    finally:
        root.setUpdatesEnabled(True)


def modern_scroll_style() -> str:
    thumb = T.token_hex("accent")
    hover = T.token_hex("accent_hi")
    track = "rgba(128, 128, 128, 28)"
    return f"""
        QScrollArea {{
            background: transparent;
            border: none;
        }}
        QScrollBar:vertical {{
            background: {track};
            width: 10px;
            margin: 3px 1px 3px 1px;
            border-radius: 5px;
        }}
        QScrollBar::handle:vertical {{
            background: {thumb};
            min-height: 28px;
            border-radius: 3px;
        }}
        QScrollBar::handle:vertical:hover {{
            background: {hover};
        }}
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {{
            height: 0px;
            background: none;
        }}
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {{
            background: transparent;
        }}
        QScrollBar:horizontal {{
            background: transparent;
            height: 6px;
            margin: 1px 4px 1px 4px;
            border-radius: 3px;
        }}
        QScrollBar::handle:horizontal {{
            background: {thumb};
            min-width: 28px;
            border-radius: 3px;
        }}
        QScrollBar::handle:horizontal:hover {{
            background: {hover};
        }}
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {{
            width: 0px;
            background: none;
        }}
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {{
            background: transparent;
        }}
    """


def get_exe_icon_pixmap(exe_path: str, size: int = 20) -> Optional[QPixmap]:
    global _ICON_PROVIDER
    if not exe_path or not os.path.exists(exe_path):
        return None
    cache_key = f"{exe_path}:{size}"
    if cache_key in _APP_ICON_CACHE:
        return _APP_ICON_CACHE[cache_key]
    try:
        if _ICON_PROVIDER is None:
            _ICON_PROVIDER = QFileIconProvider()
        icon: QIcon = _ICON_PROVIDER.icon(QFileInfo(exe_path))
        if not icon.isNull():
            pix = icon.pixmap(QSize(size, size))
            if not pix.isNull():
                _APP_ICON_CACHE[cache_key] = pix
                return pix
    except Exception:
        pass
    return None


def make_font(size_px: int = 14, weight: QFont.Weight = QFont.Weight.Medium) -> QFont:
    f = QFont(get_lang_font_family())
    f.setPixelSize(size_px)
    f.setWeight(weight)
    return f


def align_flag(prefer_start: bool = True, vcenter: bool = True) -> int:
    h_flag = (
        (Qt.AlignmentFlag.AlignRight if is_rtl() else Qt.AlignmentFlag.AlignLeft)
        if prefer_start
        else (Qt.AlignmentFlag.AlignLeft if is_rtl() else Qt.AlignmentFlag.AlignRight)
    )
    v_flag = Qt.AlignmentFlag.AlignVCenter if vcenter else Qt.AlignmentFlag.AlignTop
    return int(Qt.AlignmentFlag.AlignAbsolute | h_flag | v_flag)



class TintedScrollBox(QWidget):
    """
    Distinct custom-painted rectangular container frame for horizontal scroll strips
    (such as the Multi-Tab bar in AutoKeyPresserPage and Excluded Apps bar in CombosPage).
    Guarantees a crisp colored fill and border in both Dark and Light themes across all OS styles.
    """

    def __init__(self, tint_hex: str = "", parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self._tint_hex = tint_hex
        T.theme_changed.connect(self.update)

    def setTintHex(self, tint_hex: str) -> None:
        self._tint_hex = tint_hex
        self.update()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(1.0, 1.0, -1.0, -1.0)
        qc = QColor(self._tint_hex) if self._tint_hex else T.c("accent")
        bg = QColor(qc.red(), qc.green(), qc.blue(), 42 if T.is_dark() else 28)
        border = QColor(qc.red(), qc.green(), qc.blue(), 185 if T.is_dark() else 160)
        p.setBrush(bg)
        p.setPen(QPen(border, 1.6))
        p.drawRoundedRect(r, 10.0, 10.0)
        p.end()


class DragGripHandle(QWidget):
    """
    6-dot Drag Grip Handle (`⠿`) placed at the start of every step row.
    Allows dragging any step up or down with live real-time reordering!
    """

    dragStarted = Signal()
    dragMoved = Signal(int)
    dragFinished = Signal()

    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self._hover = False
        self._dragging = False
        self.setFixedSize(22, 30)
        self.setCursor(Qt.CursorShape.OpenHandCursor)
        self.setToolTip(tl("اسحب بالماوس لنقل الخطوة فوراً لأي مكان", "Drag to move this step live"))

    def enterEvent(self, event) -> None:
        self._hover = True
        self.update()

    def leaveEvent(self, event) -> None:
        self._hover = False
        self.update()

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            self._dragging = True
            self.setCursor(Qt.CursorShape.ClosedHandCursor)
            self.dragStarted.emit()
            self.update()
            event.accept()

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        if self._dragging:
            gy = int(event.globalPosition().y())
            self.dragMoved.emit(gy)
            event.accept()

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if self._dragging:
            self._dragging = False
            self.setCursor(Qt.CursorShape.OpenHandCursor)
            self.dragFinished.emit()
            self.update()
            event.accept()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect())
        if self._hover or self._dragging:
            p.setBrush(T.c("surface_hi", 160))
            p.setPen(Qt.PenStyle.NoPen)
            p.drawRoundedRect(r.adjusted(1, 2, -1, -2), 6.0, 6.0)
        col = T.c("accent_hi") if (self._hover or self._dragging) else T.c("text3")
        draw_icon(p, "grip", r.adjusted(2, 5, -2, -5), color=col, stroke_width=2.0)
        p.end()


class Button(QWidget):
    """Custom QPainter Button supporting `tint_hex` so category action buttons stand out."""

    clicked = Signal()

    def __init__(
        self,
        text: str = "",
        variant: str = "primary",
        icon_name: str = "",
        app_pixmap: Optional[QPixmap] = None,
        fixed_compact: bool = False,
        tint_hex: str = "",
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._text = text
        self._variant = variant
        self._icon_name = icon_name
        self._app_pixmap = app_pixmap
        self._fixed_compact = fixed_compact
        self._tint_hex = tint_hex
        self._hover = 0.0
        self._pressed = False
        self._busy = False
        self._spin_angle = 0

        self.setCursor(Qt.CursorShape.PointingHandCursor)
        self.setFixedHeight(38)
        self.setSizePolicy(
            QSizePolicy.Policy.Expanding if fixed_compact else QSizePolicy.Policy.Minimum,
            QSizePolicy.Policy.Fixed,
        )

        self._anim = QPropertyAnimation(self, b"hoverProgress", self)
        self._anim.setDuration(140)
        self._anim.setEasingCurve(QEasingCurve.Type.OutCubic)

        self._spin_timer = QTimer(self)
        self._spin_timer.setInterval(24)
        self._spin_timer.timeout.connect(self._tick_spin)
        self._recalc_min_width()

    def setFixedHeight(self, h: int) -> None:
        super().setFixedHeight(h)
        self._recalc_min_width()

    def naturalCompactWidth(self) -> int:
        is_small = self.height() <= 32
        f_sz = 12 if is_small else 13
        paint_font = make_font(f_sz, QFont.Weight.Bold if self._tint_hex else QFont.Weight.DemiBold)
        fm = QFontMetrics(paint_font)
        tw = fm.horizontalAdvance(bidi_text(self._text)) if self._text else 0
        has_icon = bool(self._icon_name or self._app_pixmap is not None)
        iw = (18 if is_small else 24) if has_icon else 0
        pad = 28 if is_small else 34
        return max(60 if is_small else 74, tw + iw + pad)

    def _recalc_min_width(self) -> None:
        if self._fixed_compact:
            self.setMinimumWidth(52)
            return
        self.setMinimumWidth(self.naturalCompactWidth())

    def getHoverProgress(self) -> float:
        return self._hover

    def setHoverProgress(self, val: float) -> None:
        self._hover = val
        self.update()

    hoverProgress = Property(float, getHoverProgress, setHoverProgress)

    def setText(self, text: str) -> None:
        self._text = text
        self._recalc_min_width()
        self.updateGeometry()
        self.update()

    def text(self) -> str:
        return self._text

    def setVariant(self, variant: str) -> None:
        self._variant = variant
        self.update()

    def setIconName(self, icon_name: str) -> None:
        self._icon_name = icon_name
        self._recalc_min_width()
        self.updateGeometry()
        self.update()

    def setAppPixmap(self, pix: Optional[QPixmap]) -> None:
        self._app_pixmap = pix
        self._recalc_min_width()
        self.updateGeometry()
        self.update()

    def setBusy(self, busy: bool) -> None:
        self._busy = busy
        if busy:
            self._spin_timer.start()
        else:
            self._spin_timer.stop()
        self.update()

    def _tick_spin(self) -> None:
        self._spin_angle = (self._spin_angle + 18) % 360
        self.update()

    def sizeHint(self) -> QSize:
        if self._fixed_compact:
            return QSize(110, self.height())
        return QSize(self.naturalCompactWidth(), self.height())

    def enterEvent(self, event) -> None:
        self._anim.stop()
        self._anim.setEndValue(1.0)
        self._anim.start()
        super().enterEvent(event)

    def leaveEvent(self, event) -> None:
        self._anim.stop()
        self._anim.setEndValue(0.0)
        self._anim.start()
        super().leaveEvent(event)

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton and self.isEnabled() and not self._busy:
            self._pressed = True
            self.update()

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if self._pressed:
            self._pressed = False
            self.update()
            if self.rect().contains(event.pos()) and self.isEnabled() and not self._busy:
                self.clicked.emit()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(
            QPainter.RenderHint.Antialiasing
            | QPainter.RenderHint.TextAntialiasing
            | QPainter.RenderHint.SmoothPixmapTransform
        )
        r = QRectF(self.rect()).adjusted(1.0, 1.0, -1.0, -1.0)
        radius = 11.0
        ic_color: Optional[QColor] = None

        if self._tint_hex:
            tc = QColor(self._tint_hex)
            if self._variant == "tinted_row":
                bg_alpha = int(78 + 52 * self._hover + (25 if self._pressed else 0))
                border_alpha = int(210 + 45 * self._hover)
                pen_w = 1.7
                fg = QColor("#FFFFFF") if T.is_dark() else tc.darker(145)
            else:
                bg_alpha = int(38 + 45 * self._hover + (25 if self._pressed else 0))
                border_alpha = int(125 + 85 * self._hover)
                pen_w = 1.3
                fg = tc.lighter(135) if T.is_dark() else tc.darker(125)
            p.setBrush(QColor(tc.red(), tc.green(), tc.blue(), bg_alpha))
            p.setPen(QPen(QColor(tc.red(), tc.green(), tc.blue(), border_alpha), pen_w))
            ic_color = tc.lighter(125)

        elif self._variant == "primary":
            grad = QLinearGradient(r.topLeft(), r.bottomRight())
            c1 = T.c("accent").lighter(int(100 + 12 * self._hover))
            c2 = T.c("accent2").lighter(int(100 + 12 * self._hover))
            if self._pressed:
                c1 = c1.darker(115)
                c2 = c2.darker(115)
            grad.setColorAt(0.0, c1)
            grad.setColorAt(1.0, c2)
            p.setBrush(grad)
            p.setPen(QPen(T.c("accent_hi", int(90 + 70 * self._hover)), 1.0))
            fg = T.c("on_accent")

        elif self._variant == "danger":
            base = T.c("danger")
            bg = base.lighter(int(100 + 12 * self._hover))
            if self._pressed:
                bg = bg.darker(118)
            p.setBrush(bg)
            p.setPen(QPen(base.lighter(135), 1.0))
            fg = QColor("#FFFFFF")

        elif self._variant == "ghost":
            alpha = int(42 * self._hover + (25 if self._pressed else 0))
            p.setBrush(T.c("surface_hi", alpha))
            p.setPen(QPen(T.c("border_hi" if self._hover > 0.3 else "border"), 1.0))
            fg = T.c("text")

        else:  # "glass"
            base = T.c("surface_hi" if self._hover > 0.4 else "surface")
            base.setAlpha(int(180 + 55 * self._hover))
            p.setBrush(base)
            p.setPen(QPen(T.c("border_hi" if self._hover > 0.2 else "border"), 1.0))
            fg = T.c("text")

        if not self.isEnabled():
            p.setOpacity(0.45)

        p.drawRoundedRect(r, radius, radius)

        if self._busy:
            cx, cy = r.center().x(), r.center().y()
            spin_rect = QRectF(cx - 9, cy - 9, 18, 18)
            pen = QPen(fg, 2.3)
            pen.setCapStyle(Qt.PenCapStyle.RoundCap)
            p.setPen(pen)
            p.drawArc(spin_rect, int(-self._spin_angle * 16), int(250 * 16))
            p.end()
            return

        is_small = self.height() <= 32
        font = make_font(12 if is_small else 13, QFont.Weight.Bold if self._tint_hex else QFont.Weight.DemiBold)
        p.setFont(font)
        p.setPen(fg)
        fm = QFontMetrics(font)

        has_icon = bool(self._icon_name or self._app_pixmap is not None)
        if has_icon and self._text:
            icon_s = 14.0 if is_small else 16.0
            pad_edge = 8.0 if is_small else 10.0
            if is_rtl():
                ir = QRectF(r.right() - pad_edge - icon_s, r.center().y() - icon_s / 2, icon_s, icon_s)
                tr = QRectF(r.left() + 6, r.top(), max(10.0, r.width() - icon_s - pad_edge - 10), r.height())
            else:
                ir = QRectF(r.left() + pad_edge, r.center().y() - icon_s / 2, icon_s, icon_s)
                tr = QRectF(r.left() + icon_s + pad_edge + 4, r.top(), max(10.0, r.width() - icon_s - pad_edge - 10), r.height())

            if self._app_pixmap is not None:
                p.drawPixmap(int(ir.x()), int(ir.y()), int(icon_s), int(icon_s), self._app_pixmap)
            else:
                draw_icon(p, self._icon_name, ir, color=ic_color or fg, stroke_width=2.0)

            elided = fm.elidedText(self._text, Qt.TextElideMode.ElideRight, int(tr.width()))
            align = align_flag(prefer_start=True) if (self._app_pixmap is not None or self._fixed_compact) else int(Qt.AlignmentFlag.AlignAbsolute | Qt.AlignmentFlag.AlignCenter)
            p.drawText(tr, align, bidi_text(elided))
        elif has_icon:
            ir = QRectF(r.center().x() - 9, r.center().y() - 9, 18, 18)
            if self._app_pixmap is not None:
                p.drawPixmap(int(ir.x()), int(ir.y()), 18, 18, self._app_pixmap)
            else:
                draw_icon(p, self._icon_name, ir, color=ic_color or fg, stroke_width=2.0)
        else:
            tr = r.adjusted(10, 0, -10, 0)
            elided = fm.elidedText(self._text, Qt.TextElideMode.ElideRight, int(tr.width()))
            disp_txt = f"\u200e{elided}\u200e" if getattr(self, "_force_ltr_text", False) else bidi_text(elided)
            p.drawText(
                tr,
                int(Qt.AlignmentFlag.AlignAbsolute | Qt.AlignmentFlag.AlignCenter),
                disp_txt,
            )
        p.end()




class ResponsiveFlowBar(QWidget):
    """
    Responsive button bar that places all buttons side-by-side on a single row
    whenever they fit the current window width, and wraps cleanly onto a second
    row ONLY when the window is resized narrower — preventing any overlap.
    """

    def __init__(
        self,
        buttons: List[Button],
        row_height: int = 32,
        h_spacing: int = 6,
        v_spacing: int = 6,
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._buttons: List[Button] = list(buttons)
        self._row_h = row_height
        self._h_space = h_spacing
        self._v_space = v_spacing
        self._last_avail_w = -1
        self._last_rtl = None
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        for b in self._buttons:
            b.setParent(self)
            b.setFixedHeight(row_height)
        self._relayout(920)

    def showEvent(self, event) -> None:
        super().showEvent(event)
        self._relayout(max(200, self.width()))

    def resizeEvent(self, event) -> None:
        super().resizeEvent(event)
        self._relayout(max(200, self.width()))

    def sizeHint(self) -> QSize:
        return QSize(600, self.height())

    def minimumSizeHint(self) -> QSize:
        return QSize(240, self.height())

    def _relayout(self, avail_w: int) -> None:
        if not self._buttons:
            self.setFixedHeight(self._row_h)
            return
        rtl = is_rtl()
        if abs(avail_w - self._last_avail_w) < 3 and rtl == self._last_rtl:
            return
        self._last_avail_w = avail_w
        self._last_rtl = rtl

        n = len(self._buttons)
        nat_widths = [max(54, b.naturalCompactWidth()) for b in self._buttons]

        total_gaps = self._h_space * (n - 1)
        total_nat = sum(nat_widths) + total_gaps
        rtl = is_rtl()

        # Case 1: All buttons fit at full natural width on a single row!
        if avail_w >= total_nat:
            extra_each = min(10, max(0, (avail_w - total_nat) // n))
            widths = [w + extra_each for w in nat_widths]

            x_cursor = avail_w if rtl else 0
            for b, bw in zip(self._buttons, widths):
                if rtl:
                    x_cursor -= bw
                    b.setGeometry(x_cursor, 0, bw, self._row_h)
                    x_cursor -= self._h_space
                else:
                    b.setGeometry(x_cursor, 0, bw, self._row_h)
                    x_cursor += bw + self._h_space

            if self.height() != self._row_h:
                self.setFixedHeight(self._row_h)
                self.updateGeometry()
            return

        # Case 2: Window is narrower than single-row minimum -> wrap onto 2 (or more) non-overlapping rows!
        rows: List[List[Tuple[Button, int]]] = []
        cur_row: List[Tuple[Button, int]] = []
        cur_w = 0

        for b, nw in zip(self._buttons, nat_widths):
            bw = min(nw, avail_w)
            needed = bw if not cur_row else (cur_w + self._h_space + bw)
            if cur_row and needed > avail_w:
                rows.append(cur_row)
                cur_row = [(b, bw)]
                cur_w = bw
            else:
                cur_row.append((b, bw))
                cur_w = needed
        if cur_row:
            rows.append(cur_row)

        for r_idx, row_items in enumerate(rows):
            y = r_idx * (self._row_h + self._v_space)
            x_cursor = avail_w if rtl else 0
            for b, bw in row_items:
                if rtl:
                    x_cursor -= bw
                    b.setGeometry(x_cursor, y, bw, self._row_h)
                    x_cursor -= self._h_space
                else:
                    b.setGeometry(x_cursor, y, bw, self._row_h)
                    x_cursor += bw + self._h_space

        total_h = len(rows) * self._row_h + max(0, len(rows) - 1) * self._v_space
        if self.height() != total_h:
            self.setFixedHeight(total_h)
            self.updateGeometry()


class IconButton(QWidget):
    """Compact square QPainter icon button."""

    clicked = Signal()

    def __init__(
        self,
        icon_name: str,
        size: int = 34,
        variant: str = "glass",
        tooltip: str = "",
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._icon_name = icon_name
        self._variant = variant
        self._hover = 0.0
        self._pressed = False
        self.setFixedSize(size, size)
        self.setCursor(Qt.CursorShape.PointingHandCursor)
        if tooltip:
            self.setToolTip(tooltip)

        self._anim = QPropertyAnimation(self, b"hoverProgress", self)
        self._anim.setDuration(140)

    def getHoverProgress(self) -> float:
        return self._hover

    def setHoverProgress(self, val: float) -> None:
        self._hover = val
        self.update()

    hoverProgress = Property(float, getHoverProgress, setHoverProgress)

    def setIconName(self, name: str) -> None:
        self._icon_name = name
        self.update()

    def enterEvent(self, event) -> None:
        self._anim.stop()
        self._anim.setEndValue(1.0)
        self._anim.start()

    def leaveEvent(self, event) -> None:
        self._anim.stop()
        self._anim.setEndValue(0.0)
        self._anim.start()

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            self._pressed = True
            self.update()

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if self._pressed:
            self._pressed = False
            self.update()
            if self.rect().contains(event.pos()):
                self.clicked.emit()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)

        if self._variant == "danger":
            bg = T.c("danger", int(35 + 165 * self._hover))
            fg = QColor("#FFFFFF") if self._hover > 0.5 else T.c("danger")
            border = T.c("danger", 110)
        elif self._variant == "ok":
            bg = T.c("ok", int(38 + 150 * self._hover))
            fg = QColor("#FFFFFF") if self._hover > 0.5 else T.c("ok")
            border = T.c("ok", 130)
        elif self._variant == "ghost":
            bg = T.c("surface_hi", int(140 * self._hover))
            fg = T.c("text" if self._hover > 0.3 else "text2")
            border = Qt.GlobalColor.transparent
        else:
            bg = T.c("surface_hi" if self._hover > 0.3 else "surface")
            fg = T.c("accent" if self._hover > 0.5 else "text")
            border = T.c("border_hi" if self._hover > 0.3 else "border")

        p.setBrush(bg)
        p.setPen(QPen(border, 1.0))
        p.drawRoundedRect(r, 9.0, 9.0)

        ir = QRectF(r.center().x() - 7.5, r.center().y() - 7.5, 15, 15)
        draw_icon(p, self._icon_name, ir, color=fg, stroke_width=1.9)
        p.end()


class ToggleSwitch(QWidget):
    """Animated OutBack Toggle Switch drawn with QPainter."""

    toggled = Signal(bool)

    def __init__(self, checked: bool = False, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self._checked = checked
        self._pos = 1.0 if checked else 0.0
        self.setFixedSize(44, 24)
        self.setCursor(Qt.CursorShape.PointingHandCursor)

        self._anim = QPropertyAnimation(self, b"thumbPos", self)
        self._anim.setDuration(220)
        self._anim.setEasingCurve(QEasingCurve.Type.OutBack)

    def getThumbPos(self) -> float:
        return self._pos

    def setThumbPos(self, val: float) -> None:
        self._pos = val
        self.update()

    thumbPos = Property(float, getThumbPos, setThumbPos)

    def isChecked(self) -> bool:
        return self._checked

    def setChecked(self, checked: bool, emit_signal: bool = False) -> None:
        if self._checked == checked:
            return
        self._checked = checked
        self._anim.stop()
        self._anim.setEndValue(1.0 if checked else 0.0)
        self._anim.start()
        if emit_signal:
            self.toggled.emit(self._checked)

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            event.accept()
            return
        super().mousePressEvent(event)

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            event.accept()
            if self.rect().contains(event.pos()):
                self.setChecked(not self._checked, emit_signal=True)
            return
        super().mouseReleaseEvent(event)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(1.5, 1.5, -1.5, -1.5)

        off_c = T.c("bg2")
        on_c = T.c("accent")
        t_val = max(0.0, min(1.0, self._pos))
        bg = QColor(
            int(off_c.red() + (on_c.red() - off_c.red()) * t_val),
            int(off_c.green() + (on_c.green() - off_c.green()) * t_val),
            int(off_c.blue() + (on_c.blue() - off_c.blue()) * t_val),
        )
        p.setBrush(bg)
        p.setPen(QPen(T.c("border_hi" if self._checked else "border"), 1.0))
        p.drawRoundedRect(r, r.height() / 2, r.height() / 2)

        thumb_d = r.height() - 6.0
        travel = r.width() - thumb_d - 6.0
        effective_pos = (1.0 - self._pos) if is_rtl() else self._pos
        tx = r.left() + 3.0 + travel * effective_pos
        ty = r.top() + 3.0

        p.setPen(Qt.PenStyle.NoPen)
        p.setBrush(QColor("#FFFFFF"))
        p.drawEllipse(QRectF(tx, ty, thumb_d, thumb_d))
        p.end()


class SegmentedControl(QWidget):
    """Animated thumb Segmented Control drawn with QPainter."""

    changed = Signal(str)

    def __init__(
        self,
        items: List[Tuple[str, str]],
        selected_key: str = "",
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._items = items
        self._selected_idx = 0
        for idx, (k, _) in enumerate(items):
            if k == selected_key:
                self._selected_idx = idx
                break
        self._thumb_idx = float(self._selected_idx)
        self.setFixedHeight(36)
        self._recalc_min_width()
        self.setCursor(Qt.CursorShape.PointingHandCursor)

    def _recalc_min_width(self) -> None:
        if not self._items:
            self.setMinimumWidth(165)
            return
        fm = QFontMetrics(make_font(12, QFont.Weight.DemiBold))
        max_tw = max((fm.horizontalAdvance(bidi_text(lbl)) for _, lbl in self._items), default=60)
        seg_w = max(108, max_tw + 38)
        self.setMinimumWidth(max(210, len(self._items) * seg_w + 14))

        self._anim = QPropertyAnimation(self, b"thumbIndex", self)
        self._anim.setDuration(200)
        self._anim.setEasingCurve(QEasingCurve.Type.OutCubic)

    def getThumbIndex(self) -> float:
        return self._thumb_idx

    def setThumbIndex(self, val: float) -> None:
        self._thumb_idx = val
        self.update()

    thumbIndex = Property(float, getThumbIndex, setThumbIndex)

    def setItems(self, items: List[Tuple[str, str]], selected_key: str = "") -> None:
        self._items = items
        for idx, (k, _) in enumerate(items):
            if k == selected_key:
                self._selected_idx = idx
                break
        self._thumb_idx = float(self._selected_idx)
        self._recalc_min_width()
        self.updateGeometry()
        self.update()

    def currentKey(self) -> str:
        if not self._items:
            return ""
        return self._items[self._selected_idx][0]

    def setSelectedKey(self, key: str, emit_signal: bool = False) -> None:
        for idx, (k, _) in enumerate(self._items):
            if k == key:
                self._selected_idx = idx
                self._anim.stop()
                self._anim.setEndValue(float(idx))
                self._anim.start()
                if emit_signal:
                    self.changed.emit(key)
                break

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if not self._items or event.button() != Qt.MouseButton.LeftButton:
            return
        n = len(self._items)
        seg_w = self.width() / n
        clicked_visual = int(event.position().x() // seg_w)
        clicked_visual = max(0, min(n - 1, clicked_visual))
        logical_idx = (n - 1 - clicked_visual) if is_rtl() else clicked_visual
        if logical_idx != self._selected_idx:
            self._selected_idx = logical_idx
            self._anim.stop()
            self._anim.setEndValue(float(logical_idx))
            self._anim.start()
            self.changed.emit(self._items[logical_idx][0])

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)
        p.setBrush(T.c("bg1"))
        p.setPen(QPen(T.c("border"), 1.0))
        p.drawRoundedRect(r, 11.0, 11.0)

        n = len(self._items)
        if n == 0:
            p.end()
            return

        seg_w = (r.width() - 6.0) / n
        visual_thumb = (n - 1 - self._thumb_idx) if is_rtl() else self._thumb_idx
        thumb_rect = QRectF(r.left() + 3.0 + visual_thumb * seg_w, r.top() + 3.0, seg_w, r.height() - 6.0)

        grad = QLinearGradient(thumb_rect.topLeft(), thumb_rect.bottomRight())
        grad.setColorAt(0.0, T.c("accent"))
        grad.setColorAt(1.0, T.c("accent2"))
        p.setBrush(grad)
        p.setPen(QPen(T.c("accent_hi", 110), 1.0))
        p.drawRoundedRect(thumb_rect, 8.0, 8.0)

        seg_font = make_font(12, QFont.Weight.DemiBold)
        p.setFont(seg_font)
        fm = QFontMetrics(seg_font)
        for idx, (_, label) in enumerate(self._items):
            vis_i = (n - 1 - idx) if is_rtl() else idx
            sr = QRectF(r.left() + 3.0 + vis_i * seg_w, r.top(), seg_w, r.height())
            inner_sr = sr.adjusted(4.0, 0.0, -4.0, 0.0)
            active = abs(idx - self._thumb_idx) < 0.45
            p.setPen(T.c("on_accent") if active else T.c("text2"))
            elided = fm.elidedText(label, Qt.TextElideMode.ElideRight, max(20, int(inner_sr.width())))
            p.drawText(
                inner_sr,
                int(Qt.AlignmentFlag.AlignAbsolute | Qt.AlignmentFlag.AlignCenter),
                bidi_text(elided),
            )
        p.end()


class NumberInput(QWidget):
    """Custom QPainter numeric input with direct keyboard typing + `-`/`+` buttons."""

    valueChanged = Signal(float)
    focused = Signal()

    def __init__(
        self,
        value: float = 1.0,
        min_val: float = 0.0,
        max_val: float = 999999.0,
        step: float = 1.0,
        decimals: int = 0,
        suffix: str = "",
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._val = value
        self._min = min_val
        self._max = max_val
        self._step = step
        self._decimals = decimals
        self._suffix = suffix
        self._hover_part = 0
        self.setFixedSize(130, 34)
        self.setMouseTracking(True)
        self.setCursor(Qt.CursorShape.PointingHandCursor)

        self._inline_edit = QLineEdit(self)
        self._inline_edit.hide()
        self._inline_edit.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self._inline_edit.returnPressed.connect(self._commit_inline_edit)
        self._inline_edit.editingFinished.connect(self._commit_inline_edit)

    def value(self) -> float:
        return self._val

    def setValue(self, val: float, emit_signal: bool = False) -> None:
        clamped = max(self._min, min(self._max, val))
        if self._decimals == 0:
            clamped = float(int(round(clamped)))
        else:
            clamped = round(clamped, self._decimals)
        if clamped != self._val:
            self._val = clamped
            self.update()
            if emit_signal:
                self.valueChanged.emit(self._val)

    def _start_inline_edit(self) -> None:
        self.focused.emit()
        raw_str = f"{int(self._val)}" if self._decimals == 0 else f"{self._val:.{self._decimals}f}"
        self._inline_edit.setGeometry(26, 2, self.width() - 52, self.height() - 4)
        self._inline_edit.setStyleSheet(
            f"""
            QLineEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('accent_hi')};
                border: 1px solid {T.token_hex('accent')};
                border-radius: 6px;
                font-family: '{get_lang_font_family()}';
                font-size: 13px;
                font-weight: bold;
            }}
            """
        )
        self._inline_edit.setText(raw_str)
        self._inline_edit.show()
        self._inline_edit.setFocus()
        self._inline_edit.selectAll()

    def _commit_inline_edit(self) -> None:
        if not self._inline_edit.isVisible():
            return
        txt = self._inline_edit.text().strip()
        for ar_d, en_d in zip("٠١٢٣٤٥٦٧٨٩", "0123456789"):
            txt = txt.replace(ar_d, en_d)
        for fa_d, en_d in zip("۰۱۲۳۴۵۶۷۸۹", "0123456789"):
            txt = txt.replace(fa_d, en_d)
        self._inline_edit.hide()
        try:
            v = float(txt)
            self.setValue(v, emit_signal=True)
        except Exception:
            self.update()

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        x = event.position().x()
        if x < 26:
            self._hover_part = 1
        elif x > self.width() - 26:
            self._hover_part = 2
        else:
            self._hover_part = 3
        self.update()

    def leaveEvent(self, event) -> None:
        self._hover_part = 0
        self.update()

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if event.button() != Qt.MouseButton.LeftButton:
            return
        x = event.position().x()
        if x < 26:
            self.setValue(self._val - self._step, emit_signal=True)
        elif x > self.width() - 26:
            self.setValue(self._val + self._step, emit_signal=True)
        else:
            self._start_inline_edit()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)
        p.setBrush(T.c("bg0", 210))
        p.setPen(QPen(T.c("border_hi" if self._hover_part == 3 else "border"), 1.0))
        p.drawRoundedRect(r, 9.0, 9.0)

        left_r = QRectF(r.left() + 2, r.top() + 2, 23, r.height() - 4)
        right_r = QRectF(r.right() - 25, r.top() + 2, 23, r.height() - 4)

        if self._hover_part == 1:
            p.setBrush(T.c("surface_hi"))
            p.setPen(Qt.PenStyle.NoPen)
            p.drawRoundedRect(left_r, 6.0, 6.0)
        elif self._hover_part == 2:
            p.setBrush(T.c("surface_hi"))
            p.setPen(Qt.PenStyle.NoPen)
            p.drawRoundedRect(right_r, 6.0, 6.0)

        draw_icon(p, "minimize", left_r.adjusted(4, 4, -4, -4), color=T.c("text2"))
        draw_icon(p, "plus", right_r.adjusted(4, 4, -4, -4), color=T.c("accent"))

        raw_num = f"{int(self._val)}" if self._decimals == 0 else f"{self._val:.{self._decimals}f}"
        disp = fmt_num(raw_num)
        if self._suffix:
            disp = f"{disp} {self._suffix}"
        p.setFont(make_font(12, QFont.Weight.Bold))
        p.setPen(T.c("text"))
        mid_r = QRectF(left_r.right(), r.top(), r.width() - 50, r.height())
        p.drawText(mid_r, int(Qt.AlignmentFlag.AlignAbsolute | Qt.AlignmentFlag.AlignCenter), disp)
        p.end()


class _SearchPopup(QWidget):
    itemSelected = Signal(str, str)

    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(
            parent,
            Qt.WindowType.Popup | Qt.WindowType.FramelessWindowHint | Qt.WindowType.NoDropShadowWindowHint,
        )
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground, True)
        self._items: List[Tuple[str, str, Optional[QPixmap]]] = []

        lay = QVBoxLayout(self)
        lay.setContentsMargins(10, 10, 10, 10)
        lay.setSpacing(6)

        self.search_edit = QLineEdit(self)
        self.search_edit.setPlaceholderText(t("search_placeholder"))
        self.search_edit.setFixedHeight(32)
        self.search_edit.textChanged.connect(self._on_filter)
        lay.addWidget(self.search_edit)

        self.scroll = QScrollArea(self)
        self.scroll.setWidgetResizable(True)
        self.scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        self.scroll.setStyleSheet(modern_scroll_style())
        self.list_container = QWidget()
        self.list_container.setStyleSheet("background: transparent;")
        self.list_layout = QVBoxLayout(self.list_container)
        self.list_layout.setContentsMargins(0, 0, 2, 0)
        self.list_layout.setSpacing(4)
        self.scroll.setWidget(self.list_container)
        lay.addWidget(self.scroll)

    def populate(self, items: List[Tuple[str, str, Optional[QPixmap]]]) -> None:
        self._items = items
        self.scroll.setStyleSheet(modern_scroll_style())
        # Dynamically refresh placeholder text to match current language!
        self.search_edit.setPlaceholderText(t("search_placeholder"))
        self.search_edit.setStyleSheet(
            f"""
            QLineEdit {{
                background: {T.token_hex('bg0')};
                color: {T.token_hex('text')};
                border: 1px solid {T.token_hex('border_hi')};
                border-radius: 8px;
                padding: 4px 10px;
                font-family: '{get_lang_font_family()}';
                font-size: 13px;
            }}
            """
        )
        self.search_edit.clear()
        self._rebuild_rows(self._items)

    def _on_filter(self, q: str) -> None:
        q_low = q.strip().lower()
        if not q_low:
            self._rebuild_rows(self._items)
        else:
            self._rebuild_rows(
                [row for row in self._items if q_low in row[0].lower() or q_low in row[1].lower()]
            )

    def _rebuild_rows(self, rows: List[Tuple]) -> None:
        while self.list_layout.count():
            it = self.list_layout.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.deleteLater()

        for row in rows:
            k, lbl, pix = row[0], row[1], row[2]
            row_tint = str(row[3]) if len(row) >= 4 and row[3] else ""
            btn = Button(
                lbl,
                variant="tinted_row" if row_tint else "ghost",
                app_pixmap=pix,
                fixed_compact=True,
                tint_hex=row_tint,
            )
            btn.setFixedHeight(36)
            btn.clicked.connect(lambda kk=k, ll=lbl: self._pick(kk, ll))
            self.list_layout.addWidget(btn)
        self.list_layout.addStretch(1)

    def _pick(self, k: str, lbl: str) -> None:
        self.hide()
        self.itemSelected.emit(k, lbl)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(2, 2, -2, -2)
        p.setBrush(T.c("surface"))
        p.setPen(QPen(T.c("border_hi"), 1.2))
        p.drawRoundedRect(r, 14.0, 14.0)
        p.end()


class SearchCombo(QWidget):
    """Custom QPainter searchable dropdown selector with Application Icon support."""

    currentChanged = Signal(str)

    def __init__(
        self,
        items: List[Tuple],
        selected_key: str = "",
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._items: List[Tuple[str, str, Optional[QPixmap]]] = []
        self._selected_key = ""
        self._hover = False
        self.setFixedHeight(34)
        self.setMinimumWidth(135)
        self.setCursor(Qt.CursorShape.PointingHandCursor)

        self._popup = _SearchPopup(self)
        self._popup.itemSelected.connect(self._on_selected)
        self.setItems(items, selected_key=selected_key)

    def setItems(self, items: List[Tuple], selected_key: str = "") -> None:
        normalized: List[Tuple[str, str, Optional[QPixmap], str]] = []
        for entry in items:
            if len(entry) >= 3:
                k, lbl, third = entry[0], entry[1], entry[2]
                fourth = str(entry[3]) if len(entry) >= 4 and entry[3] else ""
                if isinstance(third, QPixmap):
                    pix = third
                elif isinstance(third, str) and third:
                    pix = get_exe_icon_pixmap(third, 20)
                else:
                    pix = None
                normalized.append((str(k), str(lbl), pix, fourth))
            elif len(entry) == 2:
                normalized.append((str(entry[0]), str(entry[1]), None, ""))

        self._items = normalized
        keys = [row[0] for row in self._items]
        if selected_key in keys:
            self._selected_key = selected_key
        elif self._items and self._selected_key not in keys:
            self._selected_key = self._items[0][0]
        self.update()

    def currentKey(self) -> str:
        return self._selected_key

    def currentLabel(self) -> str:
        for row in self._items:
            if row[0] == self._selected_key:
                return row[1]
        return self._selected_key

    def currentPixmap(self) -> Optional[QPixmap]:
        for row in self._items:
            if row[0] == self._selected_key:
                return row[2]
        return None

    def currentTintHex(self) -> str:
        for row in self._items:
            if row[0] == self._selected_key and len(row) >= 4:
                return str(row[3] or "")
        return "" 

    def setSelectedKey(self, key: str, emit_signal: bool = False) -> None:
        self._selected_key = key
        self.update()
        if emit_signal:
            self.currentChanged.emit(key)

    def _on_selected(self, key: str, lbl: str) -> None:
        self._selected_key = key
        self.update()
        self.currentChanged.emit(key)

    def enterEvent(self, event) -> None:
        self._hover = True
        self.update()

    def leaveEvent(self, event) -> None:
        self._hover = False
        self.update()

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if event.button() == Qt.MouseButton.LeftButton:
            self._popup.populate(self._items)
            fm = QFontMetrics(make_font(12, QFont.Weight.DemiBold))
            longest_px = max((fm.horizontalAdvance(row[1]) + (92 if row[2] is not None else 64) for row in self._items), default=240)
            desired_w = max(280, self.width(), longest_px)
            screen = self.screen()
            if screen is not None:
                avail = screen.availableGeometry()
                popup_w = min(desired_w, max(280, avail.width() - 32))
                popup_h = min(320, max(200, len(self._items) * 40 + 68), avail.height() - 40)
                gpos = self.mapToGlobal(QPoint(0, self.height() + 4))
                if is_rtl():
                    gpos = self.mapToGlobal(QPoint(self.width() - popup_w, self.height() + 4))
                x = max(avail.left() + 8, min(gpos.x(), avail.right() - popup_w - 8))
                y = gpos.y()
                if y + popup_h > avail.bottom() - 8:
                    y = max(avail.top() + 8, self.mapToGlobal(QPoint(0, -popup_h - 4)).y())
            else:
                popup_w = min(desired_w, 780)
                popup_h = 280
                gpos = self.mapToGlobal(QPoint(0, self.height() + 4))
                x, y = gpos.x(), gpos.y()
            self._popup.setFixedSize(popup_w, popup_h)
            self._popup.move(x, y)
            self._popup.show()
            self._popup.search_edit.setFocus()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(
            QPainter.RenderHint.Antialiasing
            | QPainter.RenderHint.TextAntialiasing
            | QPainter.RenderHint.SmoothPixmapTransform
        )
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)
        sel_tint = self.currentTintHex()
        if sel_tint:
            tc = QColor(sel_tint)
            tc.setAlpha(45 if not self._hover else 68)
            p.setBrush(tc)
            p.setPen(QPen(QColor(sel_tint), 1.5))
        else:
            p.setBrush(T.c("surface_hi" if self._hover else "bg0", 210))
            p.setPen(QPen(T.c("border_hi" if self._hover else "border"), 1.0))
        p.drawRoundedRect(r, 9.0, 9.0)

        pix = self.currentPixmap()
        font = make_font(12, QFont.Weight.DemiBold)
        p.setFont(font)
        p.setPen(T.c("text"))
        fm = QFontMetrics(font)

        if is_rtl():
            chev_r = QRectF(r.left() + 8, r.center().y() - 7, 14, 14)
            if pix is not None:
                p.drawPixmap(int(r.right() - 28), int(r.center().y() - 9), 18, 18, pix)
                tr = QRectF(r.left() + 26, r.top(), r.width() - 58, r.height())
            else:
                tr = QRectF(r.left() + 26, r.top(), r.width() - 36, r.height())
        else:
            chev_r = QRectF(r.right() - 22, r.center().y() - 7, 14, 14)
            if pix is not None:
                p.drawPixmap(int(r.left() + 10), int(r.center().y() - 9), 18, 18, pix)
                tr = QRectF(r.left() + 34, r.top(), r.width() - 58, r.height())
            else:
                tr = QRectF(r.left() + 10, r.top(), r.width() - 36, r.height())

        elided = fm.elidedText(self.currentLabel(), Qt.TextElideMode.ElideRight, int(tr.width()))
        p.drawText(tr, align_flag(prefer_start=True), bidi_text(elided))
        draw_icon(p, "chevron_down", chev_r, color=T.c("text2"), stroke_width=2.0)
        p.end()


class KeyCaptureButton(QWidget):
    """Physical ScanCode Key & Multi-Key Chord Capture Widget."""

    keyCaptured = Signal(str)
    scanKeyCaptured = Signal(int, bool, str)
    chordCaptured = Signal(object, str)

    def __init__(
        self,
        current_key: str = "",
        scan_code: int = 0,
        is_extended: bool = False,
        chord: Optional[List[Tuple[int, bool]]] = None,
        allow_chord: bool = True,
        state: Optional[object] = None,
        large_hero: bool = False,
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._state = state
        self._large_hero = large_hero
        self._allow_chord = allow_chord
        self._listening = False
        self._hover = False

        self._chord: List[Tuple[int, bool]] = []
        self._pending_chord: List[Tuple[int, bool]] = []
        self._held_count: int = 0

        if chord:
            self._chord = [
                ((0x1B7, True) if int(sc) <= 0 else (int(sc), bool(ext)))
                for sc, ext in chord
            ]
        elif scan_code > 0:
            self._chord = [(int(scan_code), bool(is_extended))]
        elif current_key:
            parts = [p.strip() for p in current_key.replace("Key_0", "Calculator").split("+") if p.strip()]
            for p_lbl in parts:
                sc, ext = scancode_for_label(p_lbl)
                if sc > 0:
                    self._chord.append((sc, ext))

        self._key_str = self._format_chord_label(self._chord) if self._chord else current_key.replace("Key_0", "Calculator")

        self.setFixedHeight(44 if large_hero else 34)
        self.setMinimumWidth(200 if large_hero else 108)
        self.setCursor(Qt.CursorShape.PointingHandCursor)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)

        if self._state is not None:
            if hasattr(self._state, "hook_key_event"):
                self._state.hook_key_event.connect(self._on_hook_key_event)
            elif hasattr(self._state, "hook_key_captured"):
                self._state.hook_key_captured.connect(self._on_legacy_hook_captured)

    @staticmethod
    def _format_chord_label(chord: List[Tuple[int, bool]]) -> str:
        if not chord:
            return ""
        mods = [c for c in chord if is_modifier_scancode(c[0])]
        normals = [c for c in chord if not is_modifier_scancode(c[0])]
        ordered = mods + normals
        return " + ".join(label_for_scancode(sc, ext) for sc, ext in ordered)

    def keyString(self) -> str:
        return self._key_str

    def chordData(self) -> List[Tuple[int, bool]]:
        return list(self._chord)

    def scanCodeInfo(self) -> Tuple[int, bool, str]:
        if self._chord:
            sc, ext = self._chord[-1]
            return sc, ext, self._key_str
        return 0, False, ""

    def clearKey(self) -> None:
        self._chord.clear()
        self._pending_chord.clear()
        self._key_str = ""
        self._listening = False
        if self._state is not None:
            self._state.is_capturing_key = False
        self.update()
        self.keyCaptured.emit("")
        self.chordCaptured.emit([], "")

    def setChordData(self, chord: List[Tuple[int, bool]], label: str = "") -> None:
        cleaned: List[Tuple[int, bool]] = []
        for sc, ext in chord:
            s_val, e_val = (0x1B7, True) if int(sc) <= 0 else (int(sc), bool(ext))
            if (s_val, e_val) not in cleaned:
                cleaned.append((s_val, e_val))
        self._chord = cleaned
        self._key_str = self._format_chord_label(self._chord) if self._chord else label.replace("Key_0", "Calculator")
        self._listening = False
        self.update()

    def setKeyData(self, key_str: str, scan_code: int = 0, is_extended: bool = False, chord: Optional[List] = None) -> None:
        clean_str = key_str.replace("Key_0", "Calculator")
        if chord:
            parsed = [
                ((0x1B7, True) if int(item[0]) <= 0 else (int(item[0]), bool(item[1])))
                for item in chord
                if len(item) >= 2
            ]
            self.setChordData(parsed, clean_str)
            return
        if not clean_str and scan_code <= 0:
            self._chord.clear()
            self._key_str = ""
        elif "+" in clean_str:
            parsed = []
            for p_lbl in clean_str.split("+"):
                sc, ext = scancode_for_label(p_lbl.strip())
                if sc > 0 and (sc, ext) not in parsed:
                    parsed.append((sc, ext))
            self._chord = parsed
            self._key_str = self._format_chord_label(self._chord)
        else:
            sc, ext = (scan_code, is_extended) if scan_code > 0 else scancode_for_label(clean_str)
            self._chord = [(sc, ext)] if sc > 0 else []
            self._key_str = label_for_scancode(sc, ext) if sc > 0 else ""
        self._listening = False
        self.update()

    def setKeyString(self, k: str) -> None:
        self.setKeyData(k)

    def setKeyText(self, k: str) -> None:
        self.setKeyData(k)

    def enterEvent(self, event) -> None:
        self._hover = True
        self.update()

    def leaveEvent(self, event) -> None:
        self._hover = False
        self.update()

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        global _ACTIVE_KEY_CAPTURE_WIDGET
        if event.button() == Qt.MouseButton.LeftButton:
            self._listening = not self._listening
            if self._listening:
                _ACTIVE_KEY_CAPTURE_WIDGET = self
                self._pending_chord.clear()
                self._held_count = 0
                if self._state is not None:
                    self._state.is_capturing_key = True
                self.setFocus()
            else:
                if _ACTIVE_KEY_CAPTURE_WIDGET is self:
                    _ACTIVE_KEY_CAPTURE_WIDGET = None
                if self._state is not None:
                    self._state.is_capturing_key = False
            self.update()

    def _finalize_capture(self) -> None:
        global _ACTIVE_KEY_CAPTURE_WIDGET
        if not self._pending_chord:
            return
        _ACTIVE_KEY_CAPTURE_WIDGET = None
        self._listening = False
        if self._state is not None:
            self._state.is_capturing_key = False

        self._chord = list(self._pending_chord)
        self._pending_chord.clear()
        self._held_count = 0
        self._key_str = self._format_chord_label(self._chord)
        self.update()

        sc, ext = self._chord[-1]
        self.keyCaptured.emit(self._key_str)
        self.scanKeyCaptured.emit(sc, ext, self._key_str)
        self.chordCaptured.emit(list(self._chord), self._key_str)

    def _on_hook_key_event(self, scan_code: int, is_extended: bool, is_down: bool, label: str) -> None:
        global _ACTIVE_KEY_CAPTURE_WIDGET
        if not self._listening or _ACTIVE_KEY_CAPTURE_WIDGET is not self:
            return
        if scan_code <= 0:
            return
        kid = (scan_code, is_extended)
        if is_down:
            if kid not in self._pending_chord and len(self._pending_chord) < (3 if self._allow_chord else 1):
                self._pending_chord.append(kid)
                self._held_count += 1
                self.update()
                if not self._allow_chord or len(self._pending_chord) >= 3:
                    self._finalize_capture()
        else:
            if self._pending_chord:
                self._finalize_capture()

    def _on_legacy_hook_captured(self, scan_code: int, is_extended: bool, label: str) -> None:
        self._on_hook_key_event(scan_code, is_extended, True, label)
        self._on_hook_key_event(scan_code, is_extended, False, label)

    def keyPressEvent(self, event: QKeyEvent) -> None:
        if not self._listening:
            super().keyPressEvent(event)
            return
        if IS_WINDOWS:
            event.accept()
            return
        hw_sc = int(event.nativeScanCode()) if hasattr(event, "nativeScanCode") else 0
        if (hw_sc & 0xFF) > 0:
            sc = hw_sc & 0xFF
            ext = bool(hw_sc & 0x100)
            lbl = label_for_scancode(sc, ext)
        else:
            txt = event.text().strip().upper()
            lbl = txt if txt else "Space"
            sc, ext = scancode_for_label(lbl)
        self._on_hook_key_event(sc, ext, True, lbl)

    def keyReleaseEvent(self, event: QKeyEvent) -> None:
        if not self._listening:
            super().keyReleaseEvent(event)
            return
        if IS_WINDOWS:
            event.accept()
            return
        if self._pending_chord:
            self._finalize_capture()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        r = QRectF(self.rect()).adjusted(1.5, 1.5, -1.5, -1.5)
        rad = 12.0 if self._large_hero else 9.0

        if self._listening:
            p.setBrush(T.c("accent", 55))
            p.setPen(QPen(T.c("accent_hi"), 2.0))
            if self._pending_chord:
                live_lbl = self._format_chord_label(self._pending_chord)
                label = f"[ {live_lbl} + ... ]"
            else:
                label = (
                    tl("اضغط زراراً أو 2 أو 3 معاً الآن...", "Press 1, 2, or 3 keys together now...")
                    if self._allow_chord
                    else tl("اضغط الزرار الآن...", "Press key now...")
                )
            fg = T.c("accent_hi")
        elif not self._key_str:
            p.setBrush(T.c("surface_hi" if self._hover else "bg0", 210))
            p.setPen(QPen(T.c("accent_hi" if self._hover else "border_hi"), 1.4, Qt.PenStyle.DashLine))
            label = (
                tl("بدون زرار (تشغيل يدوي) — اضغط هنا لتحديد اختصار", "No Key (Manual Only) — Click to bind shortcut")
                if self._large_hero
                else tl("تحديد اختصار", "Bind Key")
            )
            fg = T.c("text2")
        else:
            p.setBrush(T.c("surface_hi" if self._hover else "bg0", 220))
            p.setPen(QPen(T.c("border_hi" if self._hover else "border"), 1.2))
            if self._large_hero:
                label = tl(
                    f"الاختصار المحدد:  [ {self._key_str} ]  (اضغط لتغييره)",
                    f"Bound Shortcut:  [ {self._key_str} ]  (Click to change)",
                )
            else:
                label = f"[ {self._key_str} ]"
            fg = T.c("text")

        p.drawRoundedRect(r, rad, rad)
        p.setFont(make_font(14 if self._large_hero else 12, QFont.Weight.ExtraBold if self._large_hero else QFont.Weight.Bold))
        p.setPen(fg)
        p.drawText(r, int(Qt.AlignmentFlag.AlignAbsolute | Qt.AlignmentFlag.AlignCenter), bidi_text(label))
        p.end()


class StepCard(QWidget):
    """Alternating Two-Tone Step Card with Live Drag-and-Drop Highlighting."""

    def __init__(self, index: int, accent_hex: str = "#8B5CF6", parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.index = index
        self.accent_hex = accent_hex
        self._dragging = False
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Preferred)

    def setIndex(self, new_index: int) -> None:
        if self.index != new_index:
            self.index = new_index
            self.update()

    def setDragging(self, dragging: bool) -> None:
        if self._dragging != dragging:
            self._dragging = dragging
            self.update()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)
        radius = 13.0

        is_odd_step = (self.index % 2) == 0
        stripe_c = QColor(self.accent_hex)

        if self._dragging:
            base = T.c("surface_hi")
            tint = QColor(
                int(base.red() * 0.72 + stripe_c.red() * 0.28),
                int(base.green() * 0.72 + stripe_c.green() * 0.28),
                int(base.blue() * 0.72 + stripe_c.blue() * 0.28),
                245,
            )
            border_c = stripe_c.lighter(125)
            border_w = 2.2
        elif is_odd_step:
            base = T.c("surface_hi")
            tint = QColor(
                int(base.red() * 0.85 + stripe_c.red() * 0.15),
                int(base.green() * 0.85 + stripe_c.green() * 0.15),
                int(base.blue() * 0.85 + stripe_c.blue() * 0.15),
                225 if T.is_dark() else 245,
            )
            border_c = QColor(stripe_c.red(), stripe_c.green(), stripe_c.blue(), 95)
            border_w = 1.1
        else:
            base = T.c("bg1")
            tint = QColor(base.red(), base.green(), base.blue(), 220 if T.is_dark() else 240)
            border_c = T.c("border_hi")
            border_w = 1.1

        p.setBrush(tint)
        p.setPen(QPen(border_c, border_w))
        p.drawRoundedRect(r, radius, radius)

        bar_w = 4.5
        if is_rtl():
            bar_r = QRectF(r.right() - bar_w - 2, r.top() + 6, bar_w, r.height() - 12)
        else:
            bar_r = QRectF(r.left() + 2, r.top() + 6, bar_w, r.height() - 12)
        p.setPen(Qt.PenStyle.NoPen)
        p.setBrush(stripe_c)
        p.drawRoundedRect(bar_r, 2.2, 2.2)
        p.end()


class GlassCard(QWidget):
    """Calm, clean rounded Glass Card container."""

    def __init__(self, accent_tint: bool = False, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self._accent_tint = accent_tint
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Preferred)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)
        radius = 18.0

        p.setBrush(T.c("glass"))
        p.setPen(QPen(T.c("border_hi" if self._accent_tint else "border"), 1.0))
        p.drawRoundedRect(r, radius, radius)

        if self._accent_tint:
            grad = QLinearGradient(r.topLeft(), QPointF(r.left(), r.top() + 55))
            grad.setColorAt(0.0, T.c("accent", 28))
            grad.setColorAt(1.0, T.c("accent", 0))
            p.setBrush(grad)
            p.setPen(Qt.PenStyle.NoPen)
            p.drawRoundedRect(r, radius, radius)
        p.end()


class ToastOverlay(QWidget):
    """In-window floating bottom-center Toast notification pill."""

    def __init__(self, parent: QWidget) -> None:
        super().__init__(parent)
        self._msg = ""
        self._kind = "info"
        self.setFixedHeight(42)
        self.hide()

        self._timer = QTimer(self)
        self._timer.setSingleShot(True)
        self._timer.timeout.connect(self.hide)

    def show_toast(self, message: str, kind: str = "info", duration_ms: int = 2400) -> None:
        self._msg = message
        self._kind = kind
        fm = QFontMetrics(make_font(13, QFont.Weight.Bold))
        w = max(240, min(600, fm.horizontalAdvance(message) + 74))
        self.setFixedWidth(w)
        if self.parentWidget():
            pw = self.parentWidget().width()
            ph = self.parentWidget().height()
            self.move((pw - w) // 2, ph - 62)
        self.show()
        self.raise_()
        self.update()
        self._timer.start(duration_ms)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        r = QRectF(self.rect()).adjusted(1.5, 1.5, -1.5, -1.5)

        p.setBrush(T.c("surface_hi"))
        border_c = T.c("ok") if self._kind == "ok" else T.c("danger") if self._kind == "danger" else T.c("accent_hi")
        p.setPen(QPen(border_c, 1.3))
        p.drawRoundedRect(r, 18.0, 18.0)

        icon_name = "check" if self._kind == "ok" else "info"
        ir = QRectF(r.left() + 14, r.center().y() - 8, 16, 16)
        draw_icon(p, icon_name, ir, color=border_c, stroke_width=2.2)

        p.setFont(make_font(13, QFont.Weight.Bold))
        p.setPen(T.c("text"))
        tr = QRectF(r.left() + 36, r.top(), r.width() - 48, r.height())
        p.drawText(
            tr,
            int(Qt.AlignmentFlag.AlignAbsolute | Qt.AlignmentFlag.AlignCenter),
            bidi_text(self._msg),
        )
        p.end()


class SlideStack(QStackedWidget):
    """Simple, calm stacked widget."""

    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

    def setCurrentIndexSmooth(self, idx: int) -> None:
        if idx == self.currentIndex() or idx < 0 or idx >= self.count():
            return
        self.setCurrentIndex(idx)
