[🌐 Semua Bahasa](../README.md) | [🏠 Beranda](01-Home-and-Getting-Started.md) | ← [Perekam Mouse & Keyboard](04-Recorder-and-Playback.md) | [Mini-Skrip Python & Humanisasi](06-Python-Mini-Scripting.md) →

# 🧪 Lab Uji & 22 Metode Input

Uji satu metode atau jalankan pemindaian ketat (`strict_no_fallback`) pada seluruh **22 metode input** di mode Foreground atau Background.

## 6 Mode Pengujian
1. **Makro / Demo Cepat (`W -> Space`)**
2. **Teks / Template Pintar**
3. **Tombol Tunggal**
4. **Sapuan — Skrip Python yang Dapat Diedit**
5. **Sapuan — Template Variabel** (`key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n`)
6. **Sapuan — Makro Tersimpan**

## Ringkasan 22 Metode
- **1–4:** `SendInput` (ScanCode, Hybrid, Unicode) & `keybd_event`.
- **5–6:** `Stealth Humanized` & `8-Byte 6KRO USB HID`.
- **7–8:** Driver Kernel Ring-0 `Interception` (`LLKHF_INJECTED=0`) & Port PS/2 `0x60/0x64` (`InpOutx64`).
- **9–10:** Burst Atomik `INPUT[2]` & Linux `/dev/uinput` / `XTest`.
- **11–20:** Pipeline `HWND` Latar Belakang (`PostMessageW`, `SendMessageTimeoutW`, kontrol anak `Scintilla`/`Edit`, `AttachThreadInput`, `WM_CHAR`, `WM_IME_KEYDOWN`).
- **21–22:** Perangkat Keras USB HID Eksternal (**Raspberry Pi Pico** & **Arduino Leonardo / Pro Micro**).

---

[🌐 Semua Bahasa](../README.md) | [🏠 Beranda](01-Home-and-Getting-Started.md) | ← [Perekam Mouse & Keyboard](04-Recorder-and-Playback.md) | [Mini-Skrip Python & Humanisasi](06-Python-Mini-Scripting.md) →
