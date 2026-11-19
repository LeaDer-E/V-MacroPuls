[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Settings, Themes & Global Hotkeys](08-Settings-Themes-and-Hotkeys.md) | [Troubleshooting & FAQ](10-Troubleshooting-and-FAQ.md) →

# 🛠️ Building from Source (Windows & Linux)

## C++20 / Qt 6 Edition (Recommended)

### Windows 10/11 (64-bit)
**Requirements:** Qt 6.5+ / 6.12.0 (`MinGW 64-bit` or `MSVC 2022`), CMake 3.21+, and Ninja.

**One-Click PowerShell Build:**
```powershell
cd V-MacroPlus-CPP
.\build_windows.bat
```
`build_windows.bat` auto-detects Qt 6, MinGW GCC 15.1.0, and Ninja, compiles all C++20 units, embeds fonts/icons via `resources.qrc`, and runs `windeployqt.exe` to produce a standalone portable folder in `dist\V-MacroPlus\`.

**Qt Creator:**
1. Open **Qt Creator** -> **File -> Open File or Project...** -> select `CMakeLists.txt`.
2. Configure the **Desktop Qt 6.x MinGW 64-bit** kit in **Release** mode.
3. Press **`Ctrl + B`** to build or **`Ctrl + R`** to run.

### Linux (Ubuntu / Debian / Fedora / Arch)
```bash
# Ubuntu / Debian dependencies
sudo apt-get update && sudo apt-get install -y build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool

chmod +x build_linux.sh
./build_linux.sh
./dist/V-MacroPlus/V-MacroPlus
```

## Python / PySide6 Edition
Requires **Python 3.12+** (or `3.10+`):
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install -r requirements.txt
python main.py
```
Build standalone executable: `python build_exe.py`.

---

[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Settings, Themes & Global Hotkeys](08-Settings-Themes-and-Hotkeys.md) | [Troubleshooting & FAQ](10-Troubleshooting-and-FAQ.md) →
