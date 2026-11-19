[🌐 Todos los Idiomas](../README.md) | [🏠 Inicio](01-Home-and-Getting-Started.md) | ← [Configuración, Temas y Atajos](08-Settings-Themes-and-Hotkeys.md) | [Solución de Problemas y Preguntas Frecuentes](10-Troubleshooting-and-FAQ.md) →

# 🛠️ Compilación desde el Código Fuente

## Edición Nativa C++20 / Qt 6
- **Windows:** Requiere Qt 6.5+ / 6.12.0 (MinGW 64-bit), CMake 3.21+ y Ninja. Ejecuta `.\build_windows.bat` en PowerShell o abre `CMakeLists.txt` directamente en **Qt Creator** (`Ctrl+B` / `Ctrl+R`).
- **Linux:** Instala `build-essential cmake ninja-build qt6-base-dev libgl1-mesa-dev libx11-dev libxtst-dev libxi-dev xdotool` y ejecuta `./build_linux.sh`.

## Edición Python / PySide6
```bash
python -m venv .venv
python -m pip install -r requirements.txt
python main.py
```

---

[🌐 Todos los Idiomas](../README.md) | [🏠 Inicio](01-Home-and-Getting-Started.md) | ← [Configuración, Temas y Atajos](08-Settings-Themes-and-Hotkeys.md) | [Solución de Problemas y Preguntas Frecuentes](10-Troubleshooting-and-FAQ.md) →
