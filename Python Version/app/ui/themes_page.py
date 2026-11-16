# -*- coding: utf-8 -*-
"""Live Theme & Palette Studio featuring 32+ palettes with mini-window preview cards + custom color picker."""

from __future__ import annotations
from PySide6.QtCore import QPointF, QRectF, Qt, Signal
from PySide6.QtGui import QColor, QFont, QLinearGradient, QPainter, QPen
from PySide6.QtWidgets import (
    QColorDialog,
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QScrollArea,
    QVBoxLayout,
    QWidget,
)
from app.i18n import bidi_text, get_language, is_rtl, t
from app.state import AppState
from app.theme import PaletteDef, T
from app.ui.icons import draw_icon
from app.ui.widgets import Button, GlassCard, SegmentedControl, align_flag, make_font


class PalettePreviewCard(QWidget):
    """Mini-window live preview card for a single palette."""

    selected = Signal(str)

    def __init__(self, palette: PaletteDef, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self.palette = palette
        self._hover = False
        self.setFixedSize(210, 128)
        self.setCursor(Qt.CursorShape.PointingHandCursor)

    def enterEvent(self, event) -> None:
        self._hover = True
        self.update()

    def leaveEvent(self, event) -> None:
        self._hover = False
        self.update()

    def mouseReleaseEvent(self, event) -> None:
        if event.button() == Qt.MouseButton.LeftButton and self.rect().contains(event.pos()):
            self.selected.emit(self.palette.id)

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        r = QRectF(self.rect()).adjusted(2, 2, -2, -2)

        is_active = T.palette_id == self.palette.id
        tokens = self.palette.dark_tokens if T.is_dark() else self.palette.light_tokens

        bg0 = QColor(tokens["bg0"])
        bg1 = QColor(tokens["bg1"])
        surf = QColor(tokens["surface"])
        acc = QColor(tokens["accent"])
        acc2 = QColor(tokens["accent2"])
        txt = QColor(tokens["text"])

        # Outer card shell
        p.setBrush(bg1)
        border_pen = QPen(acc if (is_active or self._hover) else QColor(tokens["border"]), 2.0 if is_active else 1.0)
        p.setPen(border_pen)
        p.drawRoundedRect(r, 18.0, 18.0)

        # Mini window preview viewport inside top half
        vp = QRectF(r.left() + 10, r.top() + 10, r.width() - 20, 68)
        p.setBrush(bg0)
        p.setPen(Qt.PenStyle.NoPen)
        p.drawRoundedRect(vp, 11.0, 11.0)

        # Mini sidebar & cards inside mini-window
        sb = QRectF(vp.right() - 28 if is_rtl() else vp.left() + 6, vp.top() + 6, 22, vp.height() - 12)
        p.setBrush(surf)
        p.drawRoundedRect(sb, 6.0, 6.0)

        # Accent pill inside mini-window
        pill = QRectF(
            vp.left() + 10 if is_rtl() else vp.left() + 34,
            vp.top() + 10,
            vp.width() - 46,
            18,
        )
        grad = QLinearGradient(pill.topLeft(), pill.bottomRight())
        grad.setColorAt(0.0, acc)
        grad.setColorAt(1.0, acc2)
        p.setBrush(grad)
        p.drawRoundedRect(pill, 6.0, 6.0)

        # Secondary mock card inside mini-window
        sub = QRectF(
            vp.left() + 10 if is_rtl() else vp.left() + 34,
            vp.top() + 34,
            vp.width() - 46,
            24,
        )
        p.setBrush(surf)
        p.drawRoundedRect(sub, 6.0, 6.0)

        # Palette Title & Swatch Dot
        name = self.palette.ar_name if get_language() == "ar" else self.palette.en_name
        p.setFont(make_font(13, QFont.Weight.Bold))
        p.setPen(txt)
        lbl_rect = QRectF(r.left() + 14, r.bottom() - 38, r.width() - 48, 30)
        p.drawText(lbl_rect, align_flag(prefer_start=True), bidi_text(name))

        # Swatch or active checkmark
        dot_center = QPointF(r.left() + 22 if is_rtl() else r.right() - 22, r.bottom() - 23)
        p.setBrush(acc)
        p.setPen(Qt.PenStyle.NoPen)
        p.drawEllipse(dot_center, 9.0, 9.0)
        if is_active:
            ir = QRectF(dot_center.x() - 6, dot_center.y() - 6, 12, 12)
            draw_icon(p, "check", ir, color=QColor("#FFFFFF"), stroke_width=2.4)

        p.end()


class ThemesPage(QWidget):
    """Dedicated Theme Studio page with Dark/Light/Auto selector, Custom Color Picker, and 32 palette cards."""

    def __init__(self, state: AppState, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self.state = state
        self._build_ui()

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(28, 20, 28, 24)
        root.setSpacing(18)

        # Header Row
        hdr = GlassCard(accent_tint=True)
        h_lay = QHBoxLayout(hdr)
        h_lay.setContentsMargins(24, 18, 24, 18)
        h_lay.setSpacing(16)

        titles = QVBoxLayout()
        titles.setSpacing(4)
        self.lbl_title = QLabel(t("themes_title"))
        self.lbl_title.setFont(make_font(22, QFont.Weight.ExtraBold))
        self.lbl_title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        self.lbl_sub = QLabel(t("themes_subtitle"))
        self.lbl_sub.setFont(make_font(13, QFont.Weight.Medium))
        self.lbl_sub.setWordWrap(True)
        self.lbl_sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        titles.addWidget(self.lbl_title)
        titles.addWidget(self.lbl_sub)
        h_lay.addLayout(titles, 1)

        controls = QHBoxLayout()
        controls.setSpacing(12)
        self.mode_seg = SegmentedControl(
            [
                ("dark", t("mode_dark")),
                ("light", t("mode_light")),
                ("auto", t("mode_auto")),
            ],
            selected_key=self.state.config.theme_mode,
        )
        self.mode_seg.changed.connect(self._on_mode_changed)

        self.btn_custom = Button(t("btn_custom_color"), variant="primary", icon_name="palette")
        self.btn_custom.clicked.connect(self._pick_custom_color)

        controls.addWidget(self.mode_seg)
        controls.addWidget(self.btn_custom)
        h_lay.addLayout(controls)
        root.addWidget(hdr)

        # Scrollable grid of 32 palettes
        scroll = QScrollArea(self)
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        scroll.setStyleSheet("background: transparent;")

        container = QWidget()
        container.setStyleSheet("background: transparent;")
        grid = QGridLayout(container)
        grid.setContentsMargins(2, 4, 2, 12)
        grid.setHorizontalSpacing(16)
        grid.setVerticalSpacing(16)

        palettes = T.all_palettes()
        cols = 4
        for idx, pal in enumerate(palettes):
            card = PalettePreviewCard(pal, container)
            card.selected.connect(self._on_palette_selected)
            grid.addWidget(card, idx // cols, idx % cols)

        scroll.setWidget(container)
        root.addWidget(scroll, 1)

    def _on_mode_changed(self, mode: str) -> None:
        self.state.set_theme(mode=mode)

    def _on_palette_selected(self, palette_id: str) -> None:
        self.state.set_theme(palette_id=palette_id)

    def _pick_custom_color(self) -> None:
        initial = QColor(self.state.config.custom_accent)
        color = QColorDialog.getColor(initial, self, t("btn_custom_color"))
        if color.isValid():
            self.state.set_theme(palette_id="custom", custom_accent=color.name())
