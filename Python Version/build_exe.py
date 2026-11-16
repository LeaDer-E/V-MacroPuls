# -*- coding: utf-8 -*-
"""
PyInstaller >= 6 build script for V-MacroPlus:
1. Automatically generates multi-size `assets/icon.ico` and `assets/icon.png` from the 3D Metallic V+ Logo.
2. Ensures `assets/drivers/` exists and is bundled both inside `v_runtime/assets/drivers` AND copied to
   `dist/V-MacroPlus/drivers/` right next to `V-MacroPlus.exe` so `interception.dll` / `install-interception.exe`
   / `inpoutx64.dll` work seamlessly in the compiled `.exe`.
3. Uses `--contents-directory=v_runtime` to rename the default `_internal` folder to `v_runtime`.
4. Builds with standard user privileges (`asInvoker`) by default so `V-MacroPlus.exe` can be launched without Administrator prompts (pass `--uac-admin` only if you want forced UAC elevation).
"""

from __future__ import annotations
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent


def build() -> None:
    print("=== [1/3] Generating multi-size 3D Metallic V+ Logo icon assets & drivers folder ===")
    os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")
    from tools.make_icon import main as generate_icons
    from app.paths import get_drivers_dir

    generate_icons()
    src_drivers = get_drivers_dir()

    icon_ico = ROOT / "assets" / "icon.ico"
    assets_dir = ROOT / "assets"
    sep = ";" if sys.platform.startswith("win") else ":"

    cmd = [
        sys.executable,
        "-m",
        "PyInstaller",
        "--noconfirm",
        "--clean",
        "--windowed",
        "--name=V-MacroPlus",
        "--contents-directory=v_runtime",
        f"--icon={icon_ico}",
        f"--add-data={assets_dir}{sep}assets",
    ]
    if sys.platform.startswith("win") and "--uac-admin" in sys.argv:
        cmd.append("--uac-admin")
    cmd.append("main.py")

    print("=== [2/3] Running PyInstaller >= 6 Build (Runtime folder: v_runtime) ===")
    print("Command:", " ".join(cmd))
    subprocess.run(cmd, cwd=str(ROOT), check=True)

    print("=== [3/3] Staging external drivers/ folder next to dist/V-MacroPlus/V-MacroPlus.exe ===")
    dist_app_dir = ROOT / "dist" / "V-MacroPlus"
    if dist_app_dir.exists():
        dist_drivers = dist_app_dir / "drivers"
        dist_drivers.mkdir(parents=True, exist_ok=True)
        if src_drivers.exists():
            for item in src_drivers.iterdir():
                if item.is_file():
                    shutil.copy2(item, dist_drivers / item.name)
    print("=== Build Complete! Executable is in dist/V-MacroPlus/V-MacroPlus.exe ===")


if __name__ == "__main__":
    build()
