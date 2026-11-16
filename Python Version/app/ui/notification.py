# -*- coding: utf-8 -*-
"""Custom frameless bottom-right slide-in desktop notification popup (420ms OutCubic)."""

from __future__ import annotations
from typing import Optional
from PySide6.QtCore import QEasingCurve, QPoint, QPropertyAnimation, QRectF, Qt, QTimer
from PySide6.QtGui import QFont, QGuiApplication, QPainter, QPen
from PySide6.QtWidgets import QHBoxLayout, QLabel, QVBoxLayout, QWidget
from app.i18n import bidi_text, is_rtl, t
from app.notifier import AlertItem, NOTIFIER
from app.sound import SOUND
from app.theme import T
from app.ui.icons import logo_pixmap
from app.ui.widgets import Button, IconButton, make_font


class NotificationPopup(QWidget):
    """Frameless bottom-right slide-in notification card with action buttons and custom sound."""

    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(
            parent,
            Qt.WindowType.Tool
            | Qt.WindowType.FramelessWindowHint
            | Qt.WindowType.WindowStaysOnTopHint
            | Qt.WindowType.WindowDoesNotAcceptFocus,
        )
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground, True)
        self.setAttribute(Qt.WidgetAttribute.WA_ShowWithoutActivating, True)
        self.setFixedSize(370, 118)

        self._current_action_id = ""
        self._kind = "accent"

        root = QVBoxLayout(self)
        root.setContentsMargins(18, 14, 18, 14)
        root.setSpacing(6)

        top = QHBoxLayout()
        top.setSpacing(10)
        self.lbl_title = QLabel("")
        self.lbl_title.setFont(make_font(14, QFont.Weight.Bold))
        self.btn_dismiss = IconButton("close", size=26, variant="ghost")
        self.btn_dismiss.clicked.connect(self.hide)
        top.addWidget(self.lbl_title, 1)
        top.addWidget(self.btn_dismiss)
        root.addLayout(top)

        self.lbl_msg = QLabel("")
        self.lbl_msg.setWordWrap(True)
        self.lbl_msg.setFont(make_font(13, QFont.Weight.Medium))
        root.addWidget(self.lbl_msg, 1)

        bot = QHBoxLayout()
        bot.addStretch(1)
        self.btn_action = Button("", variant="primary")
        self.btn_action.setFixedHeight(28)
        self.btn_action.clicked.connect(self._on_action_clicked)
        self.btn_action.hide()
        bot.addWidget(self.btn_action)
        root.addLayout(bot)

        self._anim = QPropertyAnimation(self, b"pos", self)
        self._anim.setDuration(420)
        self._anim.setEasingCurve(QEasingCurve.Type.OutCubic)

        self._hide_timer = QTimer(self)
        self._hide_timer.setSingleShot(True)
        self._hide_timer.timeout.connect(self.hide)

    def present(self, item: AlertItem) -> None:
        self._kind = item.kind
        self._current_action_id = item.action_id
        self.lbl_title.setText(bidi_text(item.title))
        self.lbl_msg.setText(bidi_text(item.message))
        self.lbl_title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        self.lbl_msg.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")

        if item.action_label:
            self.btn_action.setText(item.action_label)
            self.btn_action.show()
            self.setFixedHeight(138)
        else:
            self.btn_action.hide()
            self.setFixedHeight(108)

        screen = QGuiApplication.primaryScreen()
        if screen is not None:
            avail = screen.availableGeometry()
            target_x = avail.right() - self.width() - 20
            target_y = avail.bottom() - self.height() - 20
            start_pos = QPoint(target_x, avail.bottom() + 30)
            end_pos = QPoint(target_x, target_y)
            self.move(start_pos)
            self.show()
            self._anim.stop()
            self._anim.setStartValue(start_pos)
            self._anim.setEndValue(end_pos)
            self._anim.start()
        else:
            self.show()

        self.update()
        self._hide_timer.start(4200)

    def _on_action_clicked(self) -> None:
        if self._current_action_id:
            NOTIFIER.action_triggered.emit(self._current_action_id)
        self.hide()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)
        r = QRectF(self.rect()).adjusted(2, 2, -2, -2)

        p.setBrush(T.c("bg1"))
        border_c = (
            T.c("ok")
            if self._kind == "ok"
            else T.c("danger")
            if self._kind == "danger"
            else T.c("accent_hi")
        )
        p.setPen(QPen(border_c, 1.5))
        p.drawRoundedRect(r, 28.0, 28.0)

        # Draw miniature 24px V-logo watermark in corner
        pix = logo_pixmap(size=22, dpr=2.0)
        if is_rtl():
            p.drawPixmap(int(r.left() + 14), int(r.bottom() - 32), pix)
        else:
            p.drawPixmap(int(r.right() - 36), int(r.bottom() - 32), pix)
        p.end()
