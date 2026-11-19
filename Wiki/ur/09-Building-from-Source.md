[🌐 تمام زبانیں](../README.md) | [🏠 ہوم](01-Home-and-Getting-Started.md) | ← [ترتیبات، تھیمز اور شارٹ کٹس](08-Settings-Themes-and-Hotkeys.md) | [مسائل کا حل اور عمومی سوالات](10-Troubleshooting-and-FAQ.md) →

# 🛠️ سورس کوڈ سے بلڈ کرنا

## نیٹو C++20 / Qt 6 ایڈیشن
- **Windows:** Qt 6.5+ / 6.12.0 (MinGW 64-bit)، CMake 3.21+ اور Ninja درکار ہیں۔ PowerShell میں `.\build_windows.bat` چلائیں یا `CMakeLists.txt` کو **Qt Creator** میں کھولیں۔
- **Linux:** `build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool` انسٹال کریں اور `./build_linux.sh` چلائیں۔

## Python / PySide6 ایڈیشن
```bash
python -m venv .venv
python -m pip install -r requirements.txt
python main.py
```

---

[🌐 تمام زبانیں](../README.md) | [🏠 ہوم](01-Home-and-Getting-Started.md) | ← [ترتیبات، تھیمز اور شارٹ کٹس](08-Settings-Themes-and-Hotkeys.md) | [مسائل کا حل اور عمومی سوالات](10-Troubleshooting-and-FAQ.md) →
