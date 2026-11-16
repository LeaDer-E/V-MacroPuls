# -*- coding: utf-8 -*-
"""
V-MacroPulse Main Entry Point:
- Rotating/file logging initialization
- QLockFile single-instance guard
- Bundled static Tajawal TTF fonts (500/600/700/800) registration
- AppConfig & AppState initialization
- Launch Frameless MainWindow
"""

from __future__ import annotations
import logging
import os
import sys
from PySide6.QtCore import QDir, QLockFile, Qt
from PySide6.QtGui import QFont, QFontDatabase
from PySide6.QtWidgets import QApplication
from app import APP_NAME
from app.config import AppConfig
from app.domain.macro_engine import MacroEngine
from app.i18n import set_language
from app.paths import get_appdata_dir, get_fonts_dir, get_log_path
from app.state import AppState
from app.ui.window import MainWindow


def setup_logging() -> None:
    log_file = get_log_path()
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s [%(levelname)s] %(name)s: %(message)s",
        handlers=[
            logging.FileHandler(log_file, encoding="utf-8"),
            logging.StreamHandler(sys.stdout),
        ],
    )


def load_bundled_fonts() -> None:
    """Load static Tajawal TTF weights (500/600/700/800) if present in assets/fonts."""
    fonts_dir = get_fonts_dir()
    if not fonts_dir.exists():
        return
    for ttf in sorted(fonts_dir.glob("*.ttf")):
        QFontDatabase.addApplicationFont(str(ttf))


def main() -> int:
    # High-DPI scale policy
    QApplication.setHighDpiScaleFactorRoundingPolicy(
        Qt.HighDpiScaleFactorRoundingPolicy.PassThrough
    )

    app = QApplication(sys.argv)
    app.setApplicationName(APP_NAME)
    app.setOrganizationName("V-Studio")
    app.setQuitOnLastWindowClosed(False)

    setup_logging()
    logger = logging.getLogger("main")
    logger.info("Starting %s...", APP_NAME)

    # Single-instance lock via QLockFile
    lock_path = os.path.join(QDir.tempPath(), "v_macropulse_single_instance.lock")
    lock_file = QLockFile(lock_path)
    lock_file.setStaleLockTime(0)
    lock_timeout = 3500 if "--relaunch-admin" in sys.argv else 150
    if not lock_file.tryLock(lock_timeout):
        logger.warning("Another instance of %s is already running. Exiting.", APP_NAME)
        return 0

    load_bundled_fonts()
    default_font = QFont("Tajawal", 11)
    default_font.setWeight(QFont.Weight.Medium)
    app.setFont(default_font)

    # Load Config + State + Domain Engine
    cfg = AppConfig.load()
    set_language(cfg.language)
    state = AppState(cfg)
    engine = MacroEngine(state)

    window = MainWindow(state, engine)
    window.show()

    exit_code = app.exec()
    engine.shutdown()
    lock_file.unlock()
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
