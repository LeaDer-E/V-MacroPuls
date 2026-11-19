[🌐 Todos los Idiomas](../README.md) | [🏠 Inicio](01-Home-and-Getting-Started.md) | ← [Grabadora de Ratón y Teclado](04-Recorder-and-Playback.md) | [Mini-Scripts de Python y Humanización](06-Python-Mini-Scripting.md) →

# 🧪 Laboratorio de Pruebas y 22 Métodos de Entrada

Evalúa un método individual o ejecuta un barrido diagnóstico estricto (`strict_no_fallback`) sobre los **22 métodos de entrada** en modo Primer Plano o Segundo Plano.

## Los 6 Modos de Prueba
1. **Macro / Demo rápida (`W -> Space`)**
2. **Texto o Plantilla Inteligente**
3. **Tecla única**
4. **Barrido con Script Python editable**
5. **Barrido con Plantilla de Variables** (`key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n`)
6. **Barrido con Macro guardada**

## Resumen de los 22 Métodos
- **Métodos 1–4:** `SendInput` (ScanCode, Hybrid, Unicode) y `keybd_event`.
- **Métodos 5–6:** `Stealth Humanized` y `8-Byte 6KRO USB HID`.
- **Métodos 7–8:** Kernel Ring-0 `Interception` (`LLKHF_INJECTED=0`) y puerto PS/2 `0x60/0x64` (`InpOutx64`).
- **Métodos 9–10:** Ráfaga atómica `INPUT[2]` y Linux `/dev/uinput` + `XTest`.
- **Métodos 11–20:** Mensajes `HWND` en segundo plano (`PostMessageW`, `SendMessageTimeoutW`, controles hijos `Scintilla`/`Edit`, `AttachThreadInput`, `WM_CHAR`, `WM_IME_KEYDOWN`).
- **Métodos 21–22:** Microcontroladores físicos USB HID (**Raspberry Pi Pico** y **Arduino Leonardo / Pro Micro**).

---

[🌐 Todos los Idiomas](../README.md) | [🏠 Inicio](01-Home-and-Getting-Started.md) | ← [Grabadora de Ratón y Teclado](04-Recorder-and-Playback.md) | [Mini-Scripts de Python y Humanización](06-Python-Mini-Scripting.md) →
