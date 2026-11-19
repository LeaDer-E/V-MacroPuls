[🌐 همه زبان‌ها](../README.md) | [🏠 خانه](01-Home-and-Getting-Started.md) | ← [تنظیمات، تم‌ها و میانبرها](08-Settings-Themes-and-Hotkeys.md) | [عیب‌یابی و سوالات متداول](10-Troubleshooting-and-FAQ.md) →

# 🛠️ ساخت از سورس‌کد

## نسخه بومی C++20 / Qt 6
- **Windows:** نیازمند Qt 6.5+ / 6.12.0 (MinGW 64-bit)، CMake 3.21+ و Ninja. در PowerShell دستور `.\build_windows.bat` را اجرا کنید یا `CMakeLists.txt` را در **Qt Creator** باز کنید.
- **Linux:** بسته‌های `build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool` را نصب کرده و `./build_linux.sh` را اجرا کنید.

## نسخه Python / PySide6
```bash
python -m venv .venv
python -m pip install -r requirements.txt
python main.py
```

---

[🌐 همه زبان‌ها](../README.md) | [🏠 خانه](01-Home-and-Getting-Started.md) | ← [تنظیمات، تم‌ها و میانبرها](08-Settings-Themes-and-Hotkeys.md) | [عیب‌یابی و سوالات متداول](10-Troubleshooting-and-FAQ.md) →
