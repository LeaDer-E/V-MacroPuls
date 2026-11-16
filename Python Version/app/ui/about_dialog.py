# -*- coding: utf-8 -*-
"""
In-window modal glass overlay About Dialog for V-MacroPlus:
- Dynamically adapts to BOTH Light Mode and Dark Mode (`T.is_dark()`)!
- Clean vertical spacing below the Cyber-Tech V-Logo badge so `V-MacroPlus` and the current version
  (`v3.0.0`) sit comfortably below the logo with zero overlap!
- Pure per-language localization (`DEVELOPER` vs `المطور`, etc.).
"""

from __future__ import annotations
from typing import Optional
from PySide6.QtCore import QPointF, QRectF, Qt, QUrl
from PySide6.QtGui import (
    QColor,
    QDesktopServices,
    QFont,
    QGuiApplication,
    QLinearGradient,
    QRadialGradient,
    QPainter,
    QPainterPath,
    QPen,
    QPolygonF,
)
from PySide6.QtWidgets import QHBoxLayout, QLabel, QVBoxLayout, QWidget
from app import APP_NAME, APP_VERSION
from app.i18n import bidi_text, fmt_num, ltr_value, t
from app.notifier import NOTIFIER
from app.sound import SOUND
from app.theme import T
from app.ui.icons import draw_icon, logo_pixmap
from app.ui.widgets import IconButton, make_font

ABOUT_VERSION = APP_VERSION
DEV_NAME = "Eslam Mustafa"
DEV_EMAIL = "eslaam.mustafa@gmail.com"
DEV_PHONE = "+20 128 344 7065"


class _DiamondDivider(QWidget):
    """Paints a sleek horizontal gradient rule with a glowing center tech node (`─── ◆ ───`)."""

    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.setFixedHeight(16)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        w, h = float(self.width()), float(self.height())
        cx, cy = w * 0.5, h * 0.5

        acc = T.c("accent")
        line_grad = QLinearGradient(QPointF(w * 0.08, cy), QPointF(w * 0.92, cy))
        c0 = QColor(acc.red(), acc.green(), acc.blue(), 0)
        cm = QColor(acc.red(), acc.green(), acc.blue(), 110)
        line_grad.setColorAt(0.0, c0)
        line_grad.setColorAt(0.5, cm)
        line_grad.setColorAt(1.0, c0)

        p.setPen(QPen(line_grad, 1.1))
        p.drawLine(QPointF(w * 0.08, cy), QPointF(cx - 12, cy))
        p.drawLine(QPointF(cx + 12, cy), QPointF(w * 0.92, cy))

        diamond = QPolygonF([
            QPointF(cx, cy - 4.2),
            QPointF(cx + 4.2, cy),
            QPointF(cx, cy + 4.2),
            QPointF(cx - 4.2, cy),
        ])
        p.setPen(Qt.PenStyle.NoPen)
        p.setBrush(acc)
        p.drawPolygon(diamond)
        p.end()


class _GradientIconBadge(QWidget):
    """38x38 rounded gradient icon badge for Email and Phone pills."""

    def __init__(self, icon_name: str, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self._icon_name = icon_name
        self.setFixedSize(38, 38)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)
        grad = QLinearGradient(r.topLeft(), r.bottomRight())
        grad.setColorAt(0.0, T.c("accent_hi"))
        grad.setColorAt(1.0, T.c("accent"))
        p.setPen(Qt.PenStyle.NoPen)
        p.setBrush(grad)
        p.drawRoundedRect(r, 11.0, 11.0)
        draw_icon(p, self._icon_name, r.adjusted(9, 9, -9, -9), color=QColor("#FFFFFF"), stroke_width=2.1)
        p.end()


class _ContactPillCard(QWidget):
    """Sleek rounded contact pill adapting to Light and Dark modes."""

    def __init__(
        self,
        icon_name: str,
        value_text: str,
        show_mailto: bool = False,
        parent: Optional[QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._value = value_text
        self.setFixedHeight(50)

        lay = QHBoxLayout(self)
        lay.setContentsMargins(10, 5, 12, 5)
        lay.setSpacing(12)
        self.setLayoutDirection(Qt.LayoutDirection.LeftToRight)

        self.badge = _GradientIconBadge(icon_name, self)
        lay.addWidget(self.badge)

        self.lbl_val = QLabel(ltr_value(value_text))
        self.lbl_val.setFont(make_font(14, QFont.Weight.Bold))
        lay.addWidget(self.lbl_val, 1)

        if show_mailto:
            self.btn_mail = IconButton("mail", size=32, variant="ghost", tooltip=value_text)
            self.btn_mail.clicked.connect(
                lambda: QDesktopServices.openUrl(QUrl(f"mailto:{self._value}"))
            )
            lay.addWidget(self.btn_mail)

        self.btn_copy = IconButton("copy", size=32, variant="ghost")
        self.btn_copy.clicked.connect(self._copy_val)
        lay.addWidget(self.btn_copy)

    def _copy_val(self) -> None:
        cb = QGuiApplication.clipboard()
        if cb is not None:
            cb.setText(self._value)
        SOUND.play("click")
        NOTIFIER.toast(f"{t('copied_toast')}: {self._value}", "ok")

    def refresh_styles(self) -> None:
        self.lbl_val.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        self.btn_copy.setToolTip(t("btn_copy"))
        self.update()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)
        p.setBrush(T.c("surface_hi", 210))
        p.setPen(QPen(T.c("border_hi"), 1.1))
        p.drawRoundedRect(r, 14.0, 14.0)
        p.end()


class _AboutCardWidget(QWidget):
    """
    Custom-painted About Card that dynamically adapts to Light Mode and Dark Mode (`T.is_dark()`),
    with generous breathing room below the V-Logo badge so the title never touches the logo!
    """

    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)
        self.setFixedSize(506, 636)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.SmoothPixmapTransform)
        r = QRectF(self.rect()).adjusted(1.5, 1.5, -1.5, -1.5)
        radius = 24.0

        path = QPainterPath()
        path.addRoundedRect(r, radius, radius)
        p.setClipPath(path)

        dark = T.is_dark()
        bg0 = T.c("bg0")
        bg1 = T.c("bg1")
        acc = T.c("accent")
        acc_hi = T.c("accent_hi")

        # Main Card Surface (Light in Light Mode, Dark in Dark Mode!)
        bg_grad = QLinearGradient(r.topLeft(), r.bottomLeft())
        if dark:
            bg_grad.setColorAt(0.0, bg1)
            bg_grad.setColorAt(0.28, bg0)
            bg_grad.setColorAt(1.0, bg0.darker(115))
        else:
            bg_grad.setColorAt(0.0, QColor("#FFFFFF"))
            bg_grad.setColorAt(0.35, bg0)
            bg_grad.setColorAt(1.0, bg1)
        p.fillPath(path, bg_grad)

        # Premium Cyber-Studio Hero Cover Banner (top 102px) — High-contrast in BOTH Dark & Light modes
        banner_h = 102.0
        banner_rect = QRectF(r.left(), r.top(), r.width(), banner_h)
        b_grad = QLinearGradient(banner_rect.topLeft(), banner_rect.bottomRight())
        b_grad.setColorAt(0.0, QColor("#0B0719"))
        b_grad.setColorAt(0.35, QColor("#1D1040"))
        b_grad.setColorAt(0.68, QColor("#2D145E"))
        b_grad.setColorAt(1.0, QColor("#0E172E"))
        p.fillRect(banner_rect, b_grad)

        # Radial neon aura centered behind the V+ metallic logo
        cx_banner = self.width() * 0.5
        cy_logo = 68.0
        aura = QRadialGradient(QPointF(cx_banner, cy_logo), 135.0)
        aura.setColorAt(0.0, QColor(acc_hi.red(), acc_hi.green(), acc_hi.blue(), 115))
        aura.setColorAt(0.55, QColor(6, 182, 212, 48))
        aura.setColorAt(1.0, QColor(11, 7, 25, 0))
        p.fillRect(banner_rect, aura)

        # Subtle diagonal cyber-circuit lines & glowing nodes
        p.setPen(QPen(QColor(139, 92, 246, 40), 1.0))
        for gx in range(36, int(self.width()), 48):
            p.drawLine(QPointF(gx, 0), QPointF(gx - 18, banner_h))

        # Smooth dual neon horizon wave + bottom accent border on the cover banner
        wave1 = QPainterPath()
        wave1.moveTo(0, 88)
        wave1.cubicTo(110, 62, 165, 98, cx_banner - 40, 76)
        wave1.lineTo(cx_banner + 40, 76)
        wave1.cubicTo(self.width() - 165, 98, self.width() - 110, 62, self.width(), 88)
        p.setBrush(Qt.BrushStyle.NoBrush)
        p.setPen(QPen(QColor("#06B6D4"), 1.8))
        p.drawPath(wave1)

        p.setPen(QPen(QColor(acc_hi.red(), acc_hi.green(), acc_hi.blue(), 170), 1.4))
        p.drawLine(QPointF(r.left(), banner_h), QPointF(r.right(), banner_h))

        # Pure Text-Free Macro Automation Artwork:
        # Left Wing: 3D Tactile Mechanical Keycaps & Signal Bus Traces leading into the V+ Core
        key_specs = [
            (22.0, 16.0, 34.0, 28.0, "#8B5CF6"),
            (62.0, 16.0, 34.0, 28.0, "#06B6D4"),
            (102.0, 16.0, 34.0, 28.0, "#10B981"),
            (36.0, 50.0, 86.0, 22.0, "#A855F7"),
        ]
        for kx, ky, kw, kh, khex in key_specs:
            kc = QColor(khex)
            kr = QRectF(kx, ky, kw, kh)
            p.setBrush(QColor(kc.red(), kc.green(), kc.blue(), 34))
            p.setPen(QPen(QColor(kc.red(), kc.green(), kc.blue(), 155), 1.2))
            p.drawRoundedRect(kr, 6.5, 6.5)
            inner_kr = kr.adjusted(4.0, 3.5, -4.0, -5.5)
            p.setBrush(QColor(kc.red(), kc.green(), kc.blue(), 22))
            p.setPen(QPen(QColor(kc.red(), kc.green(), kc.blue(), 95), 0.9))
            p.drawRoundedRect(inner_kr, 4.0, 4.0)

        # Signal trace lines from Left Keycaps -> Center V+ Core
        p.setPen(QPen(QColor(6, 182, 212, 130), 1.4))
        p.drawLine(QPointF(136, 30), QPointF(178, 30))
        p.drawLine(QPointF(178, 30), QPointF(198, 52))
        p.drawLine(QPointF(122, 61), QPointF(195, 61))
        p.setBrush(QColor("#06B6D4"))
        p.setPen(Qt.PenStyle.NoPen)
        p.drawEllipse(QPointF(178, 30), 2.5, 2.5)
        p.drawEllipse(QPointF(195, 61), 2.5, 2.5)

        # Right Wing: Precision Mouse Click Target Ripples + Vector Cursor Pointer
        target_cx, target_cy = self.width() - 92.0, 48.0
        for rad, alph, col_hex in [(28.0, 75, "#8B5CF6"), (19.0, 120, "#06B6D4"), (10.0, 175, "#10B981")]:
            rc = QColor(col_hex)
            p.setBrush(Qt.BrushStyle.NoBrush)
            p.setPen(QPen(QColor(rc.red(), rc.green(), rc.blue(), alph), 1.4))
            p.drawEllipse(QPointF(target_cx, target_cy), rad, rad)
        # Crosshair ticks
        p.setPen(QPen(QColor(16, 185, 129, 170), 1.2))
        p.drawLine(QPointF(target_cx - 34, target_cy), QPointF(target_cx - 22, target_cy))
        p.drawLine(QPointF(target_cx + 22, target_cy), QPointF(target_cx + 34, target_cy))
        p.drawLine(QPointF(target_cx, target_cy - 34), QPointF(target_cx, target_cy - 22))
        p.drawLine(QPointF(target_cx, target_cy + 22), QPointF(target_cx, target_cy + 34))

        # Vector Cursor Arrow on Target Center
        cursor_poly = QPolygonF([
            QPointF(target_cx + 2, target_cy + 2),
            QPointF(target_cx + 2, target_cy + 22),
            QPointF(target_cx + 8, target_cy + 17),
            QPointF(target_cx + 13, target_cy + 25),
            QPointF(target_cx + 16, target_cy + 23),
            QPointF(target_cx + 11, target_cy + 15),
            QPointF(target_cx + 18, target_cy + 14),
        ])
        p.setBrush(QColor("#F8FAFC"))
        p.setPen(QPen(QColor("#8B5CF6"), 1.2))
        p.drawPolygon(cursor_poly)

        # Signal trace lines from Center V+ Core -> Right Target
        p.setPen(QPen(QColor(168, 85, 247, 130), 1.4))
        p.drawLine(QPointF(cx_banner + 42, 52), QPointF(target_cx - 38, 52))
        p.setBrush(QColor("#A855F7"))
        p.setPen(Qt.PenStyle.NoPen)
        p.drawEllipse(QPointF(target_cx - 38, 52), 2.5, 2.5)

        # Glowing outer halo ring + Center 72px Cyber-Tech V+ Logo badge at y=36 (bottom at y=108)
        logo_s = 72
        lx = int((self.width() - logo_s) * 0.5)
        ly = 36
        halo_r = QRectF(lx - 5, ly - 5, logo_s + 10, logo_s + 10)
        p.setBrush(QColor(15, 10, 30, 210))
        p.setPen(QPen(QColor("#8B5CF6"), 2.0))
        p.drawRoundedRect(halo_r, 20.0, 20.0)
        p.drawPixmap(lx, ly, logo_pixmap(logo_s, 2.0))

        # Outer border rim
        p.setClipping(False)
        p.setBrush(Qt.BrushStyle.NoBrush)
        p.setPen(QPen(T.c("border_hi"), 1.5))
        p.drawRoundedRect(r, radius, radius)
        p.end()


class AboutOverlay(QWidget):
    """Full-window modal About dialog supporting both Light Mode and Dark Mode seamlessly."""

    def __init__(self, parent: QWidget) -> None:
        super().__init__(parent)
        self.hide()
        self._build_ui()

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(16, 16, 16, 16)
        root.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.card = _AboutCardWidget(self)
        c_lay = QVBoxLayout(self.card)
        c_lay.setContentsMargins(32, 14, 32, 18)
        c_lay.setSpacing(6)

        top_bar = QHBoxLayout()
        top_bar.setDirection(QHBoxLayout.Direction.LeftToRight)
        top_bar.addStretch(1)
        self.btn_close_x = IconButton("close", size=34, variant="glass")
        self.btn_close_x.clicked.connect(self.hide)
        top_bar.addWidget(self.btn_close_x)
        c_lay.addLayout(top_bar)

        # Generous 76px spacing below the top bar so `lbl_app_name` starts at y=130 (18px clear below logo bottom y=112!)
        c_lay.addSpacing(76)

        self.lbl_app_name = QLabel(APP_NAME)
        self.lbl_app_name.setFont(make_font(24, QFont.Weight.ExtraBold))
        self.lbl_app_name.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_app_name)

        self.lbl_sub_ver = QLabel("")
        self.lbl_sub_ver.setFont(make_font(12, QFont.Weight.Bold))
        self.lbl_sub_ver.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_sub_ver)

        c_lay.addSpacing(4)

        self.lbl_desc = QLabel("")
        self.lbl_desc.setWordWrap(True)
        self.lbl_desc.setFont(make_font(13, QFont.Weight.Medium))
        self.lbl_desc.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_desc)

        c_lay.addWidget(_DiamondDivider(self.card))

        self.lbl_tech = QLabel("")
        self.lbl_tech.setWordWrap(True)
        self.lbl_tech.setFont(make_font(13, QFont.Weight.Bold))
        self.lbl_tech.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_tech)

        self.lbl_badge = QLabel("")
        self.lbl_badge.setFont(make_font(12, QFont.Weight.ExtraBold))
        self.lbl_badge.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_badge)

        c_lay.addWidget(_DiamondDivider(self.card))

        self.lbl_dev_hdr = QLabel("")
        self.lbl_dev_hdr.setFont(make_font(11, QFont.Weight.Bold))
        self.lbl_dev_hdr.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_dev_hdr)

        self.lbl_dev_name = QLabel(DEV_NAME)
        self.lbl_dev_name.setFont(make_font(19, QFont.Weight.ExtraBold))
        self.lbl_dev_name.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_dev_name)

        c_lay.addSpacing(4)

        self.pill_email = _ContactPillCard("mail", DEV_EMAIL, show_mailto=True, parent=self.card)
        c_lay.addWidget(self.pill_email)

        self.pill_phone = _ContactPillCard("phone", DEV_PHONE, show_mailto=False, parent=self.card)
        c_lay.addWidget(self.pill_phone)

        c_lay.addSpacing(4)

        self.lbl_arch_footer = QLabel("")
        self.lbl_arch_footer.setWordWrap(True)
        self.lbl_arch_footer.setFont(make_font(12, QFont.Weight.DemiBold))
        self.lbl_arch_footer.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_arch_footer)

        self.lbl_assisted = QLabel("")
        self.lbl_assisted.setFont(make_font(11, QFont.Weight.Medium))
        self.lbl_assisted.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_assisted)

        self.lbl_care = QLabel("")
        self.lbl_care.setFont(make_font(11, QFont.Weight.DemiBold))
        self.lbl_care.setAlignment(Qt.AlignmentFlag.AlignCenter)
        c_lay.addWidget(self.lbl_care)

        root.addWidget(self.card)
        self.refresh_styles()

    def refresh_styles(self) -> None:
        txt_col = T.token_hex("text")
        txt2_col = T.token_hex("text2")
        txt3_col = T.token_hex("text3")
        acc_col = T.token_hex("accent_hi" if T.is_dark() else "accent")
        ok_col = T.token_hex("ok")

        self.lbl_app_name.setText(APP_NAME)
        self.lbl_app_name.setStyleSheet(f"color: {txt_col}; background: transparent;")
        ver_str = fmt_num(ABOUT_VERSION)
        self.lbl_sub_ver.setText(f"{t('about_subtitle_caps')}  ·  {t('about_version_prefix')} {ver_str}")
        self.lbl_sub_ver.setStyleSheet(f"color: {acc_col}; background: transparent;")

        self.lbl_desc.setText(bidi_text(t("about_desc_body")))
        self.lbl_desc.setStyleSheet(f"color: {txt2_col}; background: transparent;")

        self.lbl_tech.setText(bidi_text(t("about_tech_highlights")))
        self.lbl_tech.setStyleSheet(f"color: {acc_col}; background: transparent;")

        self.lbl_badge.setText(bidi_text(t("about_tech_badge")))
        self.lbl_badge.setStyleSheet(f"color: {ok_col}; background: transparent;")

        self.lbl_dev_hdr.setText(t("about_dev_header"))
        self.lbl_dev_hdr.setStyleSheet(f"color: {txt3_col}; background: transparent;")

        self.lbl_dev_name.setStyleSheet(f"color: {txt_col}; background: transparent;")

        self.pill_email.refresh_styles()
        self.pill_phone.refresh_styles()

        self.lbl_arch_footer.setText(bidi_text(t("about_arch_footer")))
        self.lbl_arch_footer.setStyleSheet(f"color: {txt2_col}; background: transparent;")

        self.lbl_assisted.setText(bidi_text(t("about_assisted_by")))
        self.lbl_assisted.setStyleSheet(f"color: {txt3_col}; background: transparent;")

        self.lbl_care.setText(bidi_text(t("about_made_with_care")))
        self.lbl_care.setStyleSheet(f"color: {acc_col}; background: transparent;")

        self.btn_close_x.setToolTip(t("btn_close"))
        self.card.update()
        self.update()

    def show_modal(self) -> None:
        if self.parentWidget():
            self.setGeometry(self.parentWidget().rect())
        self.refresh_styles()
        self.show()
        self.raise_()

    def mousePressEvent(self, event) -> None:
        if not self.card.geometry().contains(event.pos()):
            self.hide()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        backdrop_alpha = 180 if T.is_dark() else 120
        p.fillRect(self.rect(), QColor(5, 7, 14, backdrop_alpha))
        p.end()
