# -*- coding: utf-8 -*-
"""Pure logic event and alert notification dispatcher using Qt Signals."""

from __future__ import annotations
from dataclasses import dataclass
from PySide6.QtCore import QObject, Signal


@dataclass
class AlertItem:
    title: str
    message: str
    kind: str = "info"          # "info", "ok", "danger", "accent"
    action_label: str = ""
    action_id: str = ""


class NotifierEngine(QObject):
    """Pure logic notification engine emitting signals to Toast and Desktop Popup."""

    toast_requested = Signal(str, str)        # (message, kind)
    popup_requested = Signal(object)          # AlertItem
    action_triggered = Signal(str)            # action_id

    def __init__(self) -> None:
        super().__init__()
        self.enabled: bool = True

    def toast(self, message: str, kind: str = "info") -> None:
        self.toast_requested.emit(message, kind)

    def notify(
        self,
        title: str,
        message: str,
        kind: str = "accent",
        action_label: str = "",
        action_id: str = "",
    ) -> None:
        if not self.enabled:
            return
        item = AlertItem(
            title=title,
            message=message,
            kind=kind,
            action_label=action_label,
            action_id=action_id,
        )
        self.popup_requested.emit(item)


NOTIFIER = NotifierEngine()
