[🌐 جميع اللغات](../README.md) | [🏠 الرئيسية](01-Home-and-Getting-Started.md) | ← [الإعدادات والألوان والاختصارات](08-Settings-Themes-and-Hotkeys.md) | [استكشاف الأخطاء والأسئلة الشائعة](10-Troubleshooting-and-FAQ.md) →

# 🛠️ البناء من الكود المصدري (Windows و Linux)

## أولاً: نسخة C++20 / Qt 6 (الموصى بها — الأسرع والأخف)

### المتطلبات على Windows 10/11 (64-bit)
- **Qt 6.5+ / 6.12.0** (حزمة `MinGW 64-bit` أو `MSVC 2022`).
- **MinGW-w64 GCC 13+ / 15.1.0** و**CMake 3.21+** و**Ninja** (تأتي مدمجة مع مثبت Qt).

### البناء بنقرة واحدة عبر `build_windows.bat`
افتح PowerShell داخل مجلد `V-MacroPlus-CPP` وشغّل:
```powershell
.\build_windows.bat
```
يقوم السكريبت تلقائياً باكتشاف `C:\Qt\6.12.0\mingw_64` وأدوات MinGW وNinja، وتجميع جميع الملفات بـ `C++20`، وتشغيل `windeployqt.exe` لإخراج نسخة محمولة جاهزة في `dist\V-MacroPlus\V-MacroPlus.exe`.

### البناء عبر Qt Creator
1. افتح **Qt Creator** -> **File -> Open File or Project...** واختر `CMakeLists.txt`.
2. اختر طقم **Desktop Qt 6.x MinGW 64-bit** بوضع **Release**.
3. اضغط **`Ctrl + B`** للبناء أو **`Ctrl + R`** للتشغيل مباشرة.

### البناء على Linux (Ubuntu / Debian / Fedora / Arch)
ثبّت حزم التطوير أولاً:
```bash
# Ubuntu / Debian
sudo apt-get update && sudo apt-get install -y build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool
```
ثم نفّذ سكريبت البناء:
```bash
chmod +x build_linux.sh
./build_linux.sh
./dist/V-MacroPlus/V-MacroPlus
```

---

## ثانياً: نسخة Python / PySide6
تتطلب **Python 3.12+** (أو `3.10+`):
```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install -r requirements.txt
python main.py
```
ولإنشاء ملف `.exe` مستقل لنسخة البايثون:
```powershell
python build_exe.py
```

---

[🌐 جميع اللغات](../README.md) | [🏠 الرئيسية](01-Home-and-Getting-Started.md) | ← [الإعدادات والألوان والاختصارات](08-Settings-Themes-and-Hotkeys.md) | [استكشاف الأخطاء والأسئلة الشائعة](10-Troubleshooting-and-FAQ.md) →
