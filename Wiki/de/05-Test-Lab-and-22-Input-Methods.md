[🌐 Alle Sprachen](../README.md) | [🏠 Startseite](01-Home-and-Getting-Started.md) | ← [Maus- & Tastatur-Rekorder](04-Recorder-and-Playback.md) | [Python Mini-Skripte & Humanisierung](06-Python-Mini-Scripting.md) →

# 🧪 Testlabor & 22 Eingabemethoden

Teste einzelne Methoden oder führe einen strikten Komplett-Sweep (`strict_no_fallback`) über alle **22 Eingabemethoden** im Vorder- oder Hintergrund durch.

## Die 6 Testmodi
1. **Makro / Schnelltest (`W -> Space`)**
2. **Text / Variablen-Vorlage**
3. **Einzelne Taste**
4. **Sweep — Editierbares Python-Skript**
5. **Sweep — Smart-Variablen-Vorlage** (`key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n`)
6. **Sweep — Gespeichertes Makro**

## Übersicht der 22 Methoden
- **1–4:** `SendInput` (ScanCode, Hybrid, Unicode) & `keybd_event`.
- **5–6:** `Stealth Humanized` & `8-Byte 6KRO USB HID`.
- **7–8:** Ring-0-Kernel `Interception` (`LLKHF_INJECTED=0`) & PS/2-Port `0x60/0x64` (`InpOutx64`).
- **9–10:** Atomarer `INPUT[2]`-Burst & Linux `/dev/uinput` / `XTest`.
- **11–20:** Hintergrund-`HWND`-Pipelines (`PostMessageW`, `SendMessageTimeoutW`, Kind-Controls `Scintilla`/`Edit`, `AttachThreadInput`, `WM_CHAR`, `WM_IME_KEYDOWN`).
- **21–22:** Externe USB-HID-Hardware (**Raspberry Pi Pico** & **Arduino Leonardo / Pro Micro**).

---

[🌐 Alle Sprachen](../README.md) | [🏠 Startseite](01-Home-and-Getting-Started.md) | ← [Maus- & Tastatur-Rekorder](04-Recorder-and-Playback.md) | [Python Mini-Skripte & Humanisierung](06-Python-Mini-Scripting.md) →
