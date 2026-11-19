[🌐 Todos los Idiomas](../README.md) | [🏠 Inicio](01-Home-and-Getting-Started.md) | ← [Mini-Scripts de Python y Humanización](06-Python-Mini-Scripting.md) | [Configuración, Temas y Atajos](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 Drivers Kernel y Microcontroladores USB HID

Para juegos 3D o aplicaciones que bloquean entradas de software (`LLKHF_INJECTED = 1`), V-MacroPlus ofrece hardware real (`LLKHF_INJECTED = 0`):

## 1. Raspberry Pi Pico (Método 21 — Instalación en 1 Clic)
1. Mantén pulsado **BOOTSEL** al conectar el Pico por un **cable USB de datos** (aparece la unidad `RPI-RP2`).
2. Copia el archivo `.uf2` oficial de **CircuitPython** en `RPI-RP2`; se reiniciará como **`CIRCUITPY`**.
3. En **V-MacroPlus -> Configuración**, haz clic en **Instalación en 1 clic en Pico (`CIRCUITPY`)**.
4. Reconecta el cable, pulsa **Detectar y Probar Placa** y elige el **Método 21**.

## 2. Arduino Leonardo / Pro Micro (Método 22 — ATmega32U4)
1. Abre `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` en **Arduino IDE**.
2. Selecciona **Board: `Arduino Leonardo`** y el puerto COM, luego pulsa **Upload**.
3. En **V-MacroPlus -> Configuración**, pulsa **Detectar y Probar Placa** y elige el **Método 22**.

## 3. Interception (Método 7) e InpOutx64 PS/2 (Método 8)
Descarga los drivers oficiales, vincula `interception.dll` (64-bit) o `inpoutx64.dll` en la carpeta `drivers/` desde Configuración y reinicia Windows una vez tras instalar `install-interception.exe`.

---

[🌐 Todos los Idiomas](../README.md) | [🏠 Inicio](01-Home-and-Getting-Started.md) | ← [Mini-Scripts de Python y Humanización](06-Python-Mini-Scripting.md) | [Configuración, Temas y Atajos](08-Settings-Themes-and-Hotkeys.md) →
