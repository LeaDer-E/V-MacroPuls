[🌐 Semua Bahasa](../README.md) | [🏠 Beranda](01-Home-and-Getting-Started.md) | ← [Mini-Skrip Python & Humanisasi](06-Python-Mini-Scripting.md) | [Pengaturan, Tema & Pintasan Global](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 Driver Kernel & Mikrokontroler USB HID

Untuk game 3D yang memblokir input perangkat lunak (`LLKHF_INJECTED = 1`), gunakan input perangkat keras asli (`LLKHF_INJECTED = 0`):

## 1. Raspberry Pi Pico (Metode 21 — Pasang 1 Klik)
1. Tahan tombol **BOOTSEL** saat mencolokkan Pico dengan **kabel data USB** (`RPI-RP2`).
2. Salin `.uf2` resmi **CircuitPython** ke `RPI-RP2` (akan menyala ulang sebagai **`CIRCUITPY`**).
3. Di **V-MacroPlus -> Pengaturan**, klik **Pasang 1 Klik ke Pico (`CIRCUITPY`)**.
4. Cabut dan colokkan ulang, uji papan, lalu pilih **Metode 21**.

## 2. Arduino Leonardo / Pro Micro (Metode 22 — ATmega32U4)
1. Buka `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` di **Arduino IDE**.
2. Pilih **Board: `Arduino Leonardo`** dan port COM, lalu klik **Upload**.
3. Uji papan di Pengaturan dan pilih **Metode 22**.

## 3. Interception (Metode 7) & InpOutx64 PS/2 (Metode 8)
Tautkan `interception.dll` (64-bit) atau `inpoutx64.dll` ke `drivers/` melalui Pengaturan dan mulai ulang Windows sekali setelah `install-interception.exe`.

---

[🌐 Semua Bahasa](../README.md) | [🏠 Beranda](01-Home-and-Getting-Started.md) | ← [Mini-Skrip Python & Humanisasi](06-Python-Mini-Scripting.md) | [Pengaturan, Tema & Pintasan Global](08-Settings-Themes-and-Hotkeys.md) →
