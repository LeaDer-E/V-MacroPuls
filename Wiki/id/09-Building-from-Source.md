[🌐 Semua Bahasa](../README.md) | [🏠 Beranda](01-Home-and-Getting-Started.md) | ← [Pengaturan, Tema & Pintasan Global](08-Settings-Themes-and-Hotkeys.md) | [Pemecahan Masalah & FAQ](10-Troubleshooting-and-FAQ.md) →

# 🛠️ Kompilasi dari Kode Sumber

## Edisi Natif C++20 / Qt 6
- **Windows:** Membutuhkan Qt 6.5+ / 6.12.0 (MinGW 64-bit), CMake 3.21+, dan Ninja. Jalankan `.\build_windows.bat` di PowerShell atau buka `CMakeLists.txt` di **Qt Creator**.
- **Linux:** Pasang `build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool`, lalu jalankan `./build_linux.sh`.

## Edisi Python / PySide6
```bash
python -m venv .venv
python -m pip install -r requirements.txt
python main.py
```

---

[🌐 Semua Bahasa](../README.md) | [🏠 Beranda](01-Home-and-Getting-Started.md) | ← [Pengaturan, Tema & Pintasan Global](08-Settings-Themes-and-Hotkeys.md) | [Pemecahan Masalah & FAQ](10-Troubleshooting-and-FAQ.md) →
