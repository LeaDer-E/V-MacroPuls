[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Ayarlar, Temalar ve Kısayollar](08-Settings-Themes-and-Hotkeys.md) | [Sorun Giderme ve SSS](10-Troubleshooting-and-FAQ.md) →

# 🛠️ Kaynak Koddan Derleme

## Yerel C++20 / Qt 6 Sürümü
- **Windows:** Qt 6.5+ / 6.12.0 (MinGW 64-bit), CMake 3.21+ ve Ninja gerekir. PowerShell'de `.\build_windows.bat` çalıştırın veya `CMakeLists.txt` dosyasını **Qt Creator** ile açın.
- **Linux:** `build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool` paketlerini kurup `./build_linux.sh` çalıştırın.

## Python / PySide6 Sürümü
```bash
python -m venv .venv
python -m pip install -r requirements.txt
python main.py
```

---

[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Ayarlar, Temalar ve Kısayollar](08-Settings-Themes-and-Hotkeys.md) | [Sorun Giderme ve SSS](10-Troubleshooting-and-FAQ.md) →
