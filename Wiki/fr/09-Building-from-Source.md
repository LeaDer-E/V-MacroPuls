[🌐 Toutes les Langues](../README.md) | [🏠 Accueil](01-Home-and-Getting-Started.md) | ← [Paramètres, Thèmes et Raccourcis](08-Settings-Themes-and-Hotkeys.md) | [Dépannage et FAQ](10-Troubleshooting-and-FAQ.md) →

# 🛠️ Compilation depuis les Sources

## Édition Native C++20 / Qt 6
- **Windows :** Requiert Qt 6.5+ / 6.12.0 (MinGW 64-bit), CMake 3.21+ et Ninja. Lancez `.\build_windows.bat` dans PowerShell ou ouvrez `CMakeLists.txt` dans **Qt Creator** (`Ctrl+B` / `Ctrl+R`).
- **Linux :** Installez `build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool` puis lancez `./build_linux.sh`.

## Édition Python / PySide6
```bash
python -m venv .venv
python -m pip install -r requirements.txt
python main.py
```

---

[🌐 Toutes les Langues](../README.md) | [🏠 Accueil](01-Home-and-Getting-Started.md) | ← [Paramètres, Thèmes et Raccourcis](08-Settings-Themes-and-Hotkeys.md) | [Dépannage et FAQ](10-Troubleshooting-and-FAQ.md) →
