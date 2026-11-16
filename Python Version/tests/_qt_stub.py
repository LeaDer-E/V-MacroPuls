# -*- coding: utf-8 -*-
"""Lightweight PySide6 fallback stub for headless test environments where PySide6 is not installed."""

from __future__ import annotations
import sys
import types


def install_qt_stubs_if_needed() -> None:
    try:
        import PySide6.QtCore  # noqa: F401
        return
    except ImportError:
        pass

    qtcore = types.ModuleType("PySide6.QtCore")

    class _SignalInstance:
        def __init__(self) -> None:
            self._cbs = []

        def connect(self, cb) -> None:
            self._cbs.append(cb)

        def emit(self, *a, **kw) -> None:
            for cb in list(self._cbs):
                cb(*a, **kw)

    class Signal:
        def __init__(self, *a) -> None:
            pass

        def __set_name__(self, owner, name: str) -> None:
            self.name = "_sig_" + name

        def __get__(self, obj, owner=None):
            if obj is None:
                return self
            if not hasattr(obj, self.name):
                setattr(obj, self.name, _SignalInstance())
            return getattr(obj, self.name)

    class QObject:
        def __init__(self, parent=None) -> None:
            pass

    class QPoint:
        def __init__(self, x: int = 0, y: int = 0) -> None:
            self._x = x
            self._y = y

        def x(self) -> int:
            return self._x

        def y(self) -> int:
            return self._y

    class Qt:
        class LayoutDirection:
            RightToLeft = 1
            LeftToRight = 0

    qtcore.QObject = QObject
    qtcore.QPoint = QPoint
    qtcore.Signal = Signal
    qtcore.Qt = Qt

    qtgui = types.ModuleType("PySide6.QtGui")

    class QCursor:
        @staticmethod
        def pos() -> QPoint:
            return QPoint(0, 0)

    qtgui.QCursor = QCursor

    qtwidgets = types.ModuleType("PySide6.QtWidgets")

    class QApplication:
        @staticmethod
        def instance():
            return None

    qtwidgets.QApplication = QApplication

    pyside = types.ModuleType("PySide6")
    pyside.QtCore = qtcore
    pyside.QtGui = qtgui
    pyside.QtWidgets = qtwidgets

    sys.modules["PySide6"] = pyside
    sys.modules["PySide6.QtCore"] = qtcore
    sys.modules["PySide6.QtGui"] = qtgui
    sys.modules["PySide6.QtWidgets"] = qtwidgets

    theme_mod = types.ModuleType("app.theme")

    class _StubT:
        palette_id = "cyber_violet"
        mode = "dark"
        custom_accent = "#8B5CF6"

        def configure(self, *a, **kw) -> None:
            pass

        def set_theme(self, *a, **kw) -> None:
            pass

        def is_dark(self) -> bool:
            return True

        def token_hex(self, token: str, force_dark: bool = False) -> str:
            return "#8B5CF6"

    theme_mod.T = _StubT()
    sys.modules.setdefault("app.theme", theme_mod)

    sound_mod = types.ModuleType("app.sound")

    class _StubSound:
        enabled = False
        volume = 60

        def set_config(self, *a, **kw) -> None:
            pass

        def play(self, *a, **kw) -> None:
            pass

    sound_mod.SOUND = _StubSound()
    sys.modules.setdefault("app.sound", sound_mod)

    workers_mod = types.ModuleType("app.workers")
    workers_mod.run_async = lambda fn, *a, **kw: None
    sys.modules.setdefault("app.workers", workers_mod)
