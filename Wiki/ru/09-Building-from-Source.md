[🌐 Все Языки](../README.md) | [🏠 Главная](01-Home-and-Getting-Started.md) | ← [Настройки, Темы и Горячие Клавиши](08-Settings-Themes-and-Hotkeys.md) | [Устранение Неполадок и FAQ](10-Troubleshooting-and-FAQ.md) →

# 🛠️ Сборка из Исходного Кода

## Нативная версия C++20 / Qt 6
- **Windows:** Требуется Qt 6.5+ / 6.12.0 (MinGW 64-bit), CMake 3.21+ и Ninja. Запустите `.\build_windows.bat` в PowerShell или откройте `CMakeLists.txt` в **Qt Creator**.
- **Linux:** Установите `build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool` и выполните `./build_linux.sh`.

## Версия Python / PySide6
```bash
python -m venv .venv
python -m pip install -r requirements.txt
python main.py
```

---

[🌐 Все Языки](../README.md) | [🏠 Главная](01-Home-and-Getting-Started.md) | ← [Настройки, Темы и Горячие Клавиши](08-Settings-Themes-and-Hotkeys.md) | [Устранение Неполадок и FAQ](10-Troubleshooting-and-FAQ.md) →
