[🌐 Alle Sprachen](../README.md) | [🏠 Startseite](01-Home-and-Getting-Started.md) | ← [Einstellungen, Designs & Hotkeys](08-Settings-Themes-and-Hotkeys.md) | [Fehlerbehebung & FAQ](10-Troubleshooting-and-FAQ.md) →

# 🛠️ Aus dem Quellcode kompilieren

## Native C++20 / Qt 6 Edition
- **Windows:** Erfordert Qt 6.5+ / 6.12.0 (MinGW 64-Bit), CMake 3.21+ und Ninja. `.\build_windows.bat` in PowerShell ausführen oder `CMakeLists.txt` direkt in **Qt Creator** öffnen (`Ctrl+B` / `Ctrl+R`).
- **Linux:** `build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool` installieren und `./build_linux.sh` ausführen.

## Python / PySide6 Edition
```bash
python -m venv .venv
python -m pip install -r requirements.txt
python main.py
```

---

[🌐 Alle Sprachen](../README.md) | [🏠 Startseite](01-Home-and-Getting-Started.md) | ← [Einstellungen, Designs & Hotkeys](08-Settings-Themes-and-Hotkeys.md) | [Fehlerbehebung & FAQ](10-Troubleshooting-and-FAQ.md) →
