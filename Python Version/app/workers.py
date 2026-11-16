# -*- coding: utf-8 -*-
"""Zero-freeze background worker pool using QThreadPool + QRunnable + Qt Signals."""

from __future__ import annotations
import logging
import traceback
from typing import Any, Callable, Optional
from PySide6.QtCore import QObject, QRunnable, QThreadPool, Signal, Slot

logger = logging.getLogger(__name__)


class WorkerSignals(QObject):
    finished = Signal(object)
    error = Signal(str)


class AsyncTask(QRunnable):
    """Runnable wrapper executing a callable off the GUI thread."""

    def __init__(self, fn: Callable[..., Any], *args: Any, **kwargs: Any) -> None:
        super().__init__()
        self.fn = fn
        self.args = args
        self.kwargs = kwargs
        self.signals = WorkerSignals()
        self.setAutoDelete(True)

    @Slot()
    def run(self) -> None:
        try:
            result = self.fn(*self.args, **self.kwargs)
            self.signals.finished.emit(result)
        except Exception as exc:
            tb = traceback.format_exc()
            logger.error("AsyncTask error: %s\n%s", exc, tb)
            self.signals.error.emit(str(exc))


_ACTIVE_TASKS: set[AsyncTask] = set()


def run_async(
    fn: Callable[..., Any],
    on_done: Optional[Callable[[Any], None]] = None,
    on_error: Optional[Callable[[str], None]] = None,
    *args: Any,
    **kwargs: Any,
) -> AsyncTask:
    """Execute `fn(*args, **kwargs)` on global QThreadPool and invoke callbacks on GUI thread."""
    task = AsyncTask(fn, *args, **kwargs)
    _ACTIVE_TASKS.add(task)

    def _cleanup_done(res: Any) -> None:
        _ACTIVE_TASKS.discard(task)
        if on_done is not None:
            try:
                on_done(res)
            except Exception as exc:
                logger.error("Error in on_done callback: %s", exc)

    def _cleanup_err(err_msg: str) -> None:
        _ACTIVE_TASKS.discard(task)
        if on_error is not None:
            try:
                on_error(err_msg)
            except Exception as exc:
                logger.error("Error in on_error callback: %s", exc)

    task.signals.finished.connect(_cleanup_done)
    task.signals.error.connect(_cleanup_err)
    QThreadPool.globalInstance().start(task)
    return task
