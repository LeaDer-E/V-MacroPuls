# -*- coding: utf-8 -*-
"""
10-Language Welcome / Onboarding Overlay (`OnboardingOverlay`):
- Displays ALL 10 supported languages (`SUPPORTED_LANGUAGES`) in a spacious 2x5 interactive grid
  so the user can select any language on first launch without any text or button clipping
  even on two-line titles/summaries (such as French and Russian).
- Automatically resizes to fill the parent window as soon as the window is shown (`showEvent`).
"""

from __future__ import annotations
from typing import Dict
from PySide6.QtCore import QRectF, Qt, Signal
from PySide6.QtGui import QColor, QFont, QPainter, QPen
from PySide6.QtWidgets import QGridLayout, QHBoxLayout, QLabel, QSizePolicy, QVBoxLayout, QWidget
from app.i18n import SUPPORTED_LANGUAGES, get_language, t
from app.state import AppState
from app.theme import T
from app.ui.icons import logo_pixmap
from app.ui.widgets import Button, GlassCard, make_font


class OnboardingOverlay(QWidget):
    """Welcome overlay supporting all 10 languages in a 2x5 grid on first launch with zero clipping."""

    finished = Signal()

    def __init__(self, state: AppState, parent: QWidget) -> None:
        super().__init__(parent)
        self.state = state
        self._lang_buttons: Dict[str, Button] = {}
        self._build_ui()

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(24, 20, 24, 20)
        root.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.card = QWidget(self)
        self.card.setMinimumSize(760, 490)
        self.card.setMaximumSize(820, 540)
        self.card.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.MinimumExpanding)
        c_lay = QVBoxLayout(self.card)
        c_lay.setContentsMargins(32, 22, 32, 22)
        c_lay.setSpacing(12)

        hdr = QHBoxLayout()
        hdr.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.logo_lbl = QLabel()
        self.logo_lbl.setPixmap(logo_pixmap(56, 2.0))
        hdr.addWidget(self.logo_lbl)
        c_lay.addLayout(hdr)

        self.lbl_title = QLabel(t("onb_title"))
        self.lbl_title.setFont(make_font(17, QFont.Weight.ExtraBold))
        self.lbl_title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.lbl_title.setWordWrap(True)
        self.lbl_title.setMinimumHeight(46)
        self.lbl_title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        c_lay.addWidget(self.lbl_title)

        summary_card = GlassCard(accent_tint=True, parent=self.card)
        summary_card.setMinimumHeight(64)
        s_lay = QVBoxLayout(summary_card)
        s_lay.setContentsMargins(18, 10, 18, 10)
        self.lbl_steps = QLabel(t("onb_step_summary"))
        self.lbl_steps.setFont(make_font(12, QFont.Weight.Bold))
        self.lbl_steps.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.lbl_steps.setWordWrap(True)
        self.lbl_steps.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        s_lay.addWidget(self.lbl_steps)
        c_lay.addWidget(summary_card)

        self.lbl_pick_lang = QLabel(t("onb_pick_lang"))
        self.lbl_pick_lang.setFont(make_font(13, QFont.Weight.Bold))
        self.lbl_pick_lang.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.lbl_pick_lang.setFixedHeight(24)
        self.lbl_pick_lang.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        c_lay.addWidget(self.lbl_pick_lang)

        # All 10 Supported Languages in a spacious 2x5 Grid with guaranteed vertical height
        grid_w = QWidget(self.card)
        grid_w.setMinimumHeight(92)
        grid = QGridLayout(grid_w)
        grid.setContentsMargins(4, 2, 4, 2)
        grid.setHorizontalSpacing(10)
        grid.setVerticalSpacing(10)

        cur_lang = get_language()
        for idx, (code, label, _, _) in enumerate(SUPPORTED_LANGUAGES):
            btn = Button(
                label,
                variant="primary" if code == cur_lang else "glass",
                fixed_compact=False,
            )
            btn.setFixedHeight(38)
            btn.setMinimumWidth(132)
            btn.clicked.connect(lambda c=code: self._on_lang_changed(c))
            row_i = idx // 5
            col_i = idx % 5
            grid.addWidget(btn, row_i, col_i)
            grid.setColumnStretch(col_i, 1)
            self._lang_buttons[code] = btn

        c_lay.addWidget(grid_w)
        c_lay.addSpacing(4)

        bot = QHBoxLayout()
        bot.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.btn_start = Button(t("btn_finish"), variant="primary", icon_name="bolt")
        self.btn_start.setFixedSize(220, 44)
        self.btn_start.clicked.connect(self._finish)
        bot.addWidget(self.btn_start)
        c_lay.addLayout(bot)

        root.addWidget(self.card)

    def _on_lang_changed(self, lang: str) -> None:
        self.state.set_lang(lang)
        self.refresh_texts()

    def refresh_texts(self) -> None:
        cur = get_language()
        self.lbl_title.setText(t("onb_title"))
        self.lbl_steps.setText(t("onb_step_summary"))
        self.lbl_pick_lang.setText(t("onb_pick_lang"))
        self.btn_start.setText(t("btn_finish"))
        for code, btn in self._lang_buttons.items():
            btn.setVariant("primary" if code == cur else "glass")
        self.card.adjustSize()
        self.update()

    def _finish(self) -> None:
        self.state.update_setting("first_run_completed", True)
        self.hide()
        self.finished.emit()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        p.fillRect(self.rect(), QColor(6, 9, 16, 195))
        cr = QRectF(self.card.geometry())
        p.setBrush(T.c("bg1"))
        p.setPen(QPen(T.c("border_hi"), 1.5))
        p.drawRoundedRect(cr, 26.0, 26.0)
        p.end()
