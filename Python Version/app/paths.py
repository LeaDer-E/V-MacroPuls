# -*- coding: utf-8 -*-
"""
Path resolution utilities supporting both source run (`python main.py`) and
packaged PyInstaller >= 6 executables (`V-MacroPlus.exe` with `v_runtime`).
"""

from __future__ import annotations
import os
import sys
from pathlib import Path
from typing import List

try:
    from PySide6.QtCore import QStandardPaths
except Exception:
    QStandardPaths = None


def is_frozen_exe() -> bool:
    """Return True when running inside a packaged PyInstaller executable."""
    return bool(getattr(sys, "frozen", False))


def get_exe_dir() -> Path:
    """
    Return the directory containing `V-MacroPlus.exe` when frozen,
    or the project root directory when running from Python source.
    """
    if is_frozen_exe():
        return Path(sys.executable).resolve().parent
    return Path(__file__).resolve().parent.parent


def get_base_dir() -> Path:
    """Return the base bundle directory (handles PyInstaller `sys._MEIPASS` / `v_runtime`)."""
    if is_frozen_exe() and hasattr(sys, "_MEIPASS"):
        return Path(getattr(sys, "_MEIPASS")).resolve()
    return Path(__file__).resolve().parent.parent


def get_assets_dir() -> Path:
    """Return path to bundled assets directory."""
    d = get_base_dir() / "assets"
    d.mkdir(parents=True, exist_ok=True)
    return d


def get_fonts_dir() -> Path:
    """Return path to bundled static Tajawal TTF fonts directory."""
    d = get_assets_dir() / "fonts"
    d.mkdir(parents=True, exist_ok=True)
    return d


def get_appdata_dir() -> Path:
    """Return user-writable AppData directory for V-MacroPlus."""
    base = (
        QStandardPaths.writableLocation(QStandardPaths.StandardLocation.AppDataLocation)
        if QStandardPaths is not None
        else ""
    )
    if not base:
        base = os.path.expanduser("~/.v_macropulse")
    path = Path(base)
    if path.name.lower() not in ("v-macropulse", "v-macroplus"):
        path = path / "V-MacroPulse"
    path.mkdir(parents=True, exist_ok=True)
    return path


def get_drivers_dir() -> Path:
    """
    Return the primary user-accessible `drivers` directory:
    - When running as `V-MacroPlus.exe`: `<folder_of_exe>/drivers`
    - When running from source: `<project_root>/assets/drivers`
    Automatically creates a helpful guide file if the directory is empty.
    """
    if is_frozen_exe():
        d = get_exe_dir() / "drivers"
    else:
        d = get_assets_dir() / "drivers"
    try:
        d.mkdir(parents=True, exist_ok=True)
    except Exception:
        d = get_appdata_dir() / "drivers"
        d.mkdir(parents=True, exist_ok=True)

    readme = d / "README_DRIVERS.txt"
    if not readme.exists():
        try:
            readme.write_text(
                "======================================================================\n"
                "  V-MacroPlus — Ring-0 Kernel & Hardware Port Drivers Directory\n"
                "======================================================================\n\n"
                "Place your 64-bit hardware driver bridge files directly in this folder:\n\n"
                "1) Method 7 (Ring-0 Kernel Driver — Interception):\n"
                "   - Download Interception.zip from: https://github.com/oblitum/Interception/releases\n"
                "   - Required file here: interception.dll (64-bit from library/x64/interception.dll)\n"
                "   - System kernel driver (keyboard.sys & mouse.sys):\n"
                "     Run 'install-interception.exe /install' as Administrator once,\n"
                "     then RESTART Windows.\n\n"
                "2) Method 8 (Hardware I/O Port 0x60/0x64 — i8042 PS/2 Controller):\n"
                "   - Required file here: inpoutx64.dll (64-bit)\n"
                "   - Run V-MacroPlus as Administrator.\n\n"
                "After placing the files here, open V-MacroPlus -> Settings ->\n"
                "click 'Check Interception & Kernel Status Now' to verify 100% readiness.\n",
                encoding="utf-8",
            )
        except Exception:
            pass
    return d


def get_driver_search_dirs() -> List[Path]:
    """
    Return all candidate directories where `V-MacroPlus` and `V-MacroPlus.exe` search
    for `interception.dll`, `install-interception.exe`, and `inpoutx64.dll`.
    """
    candidates: List[Path] = [
        get_drivers_dir(),
        get_exe_dir() / "drivers",
        get_exe_dir() / "assets" / "drivers",
        get_exe_dir(),
        get_assets_dir() / "drivers",
        get_appdata_dir() / "drivers",
    ]
    if sys.platform.startswith("win"):
        sys_root = Path(os.environ.get("SystemRoot", r"C:\Windows"))
        candidates.append(sys_root / "System32")

    unique: List[Path] = []
    seen = set()
    for p in candidates:
        try:
            rp = str(p.resolve())
        except Exception:
            rp = str(p)
        if rp not in seen:
            seen.add(rp)
            unique.append(p)
    return unique


def get_config_path() -> Path:
    """Return path to settings.json in AppData."""
    return get_appdata_dir() / "settings.json"


def get_macros_dir() -> Path:
    """Return path to saved macros directory in AppData."""
    d = get_appdata_dir() / "macros"
    d.mkdir(parents=True, exist_ok=True)
    return d


def get_combos_path() -> Path:
    """Return path to saved key combo triggers JSON file."""
    return get_combos_path_internal()


def get_combos_path_internal() -> Path:
    return get_appdata_dir() / "combos.json"


def get_log_path() -> Path:
    """Return path to application log file."""
    d = get_appdata_dir() / "logs"
    d.mkdir(parents=True, exist_ok=True)
    return d / "v_macropulse.log"
