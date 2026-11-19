[🌐 Semua Bahasa](../README.md) | [🏠 Beranda](01-Home-and-Getting-Started.md) | ← [Lab Uji & 22 Metode Input](05-Test-Lab-and-22-Input-Methods.md) | [Driver Kernel & Mikrokontroler USB HID](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 Mini-Skrip Python & Humanisasi

**Tombol Pintar** dan **Lab Uji (Mode 4)** berbagi interpreter bawaan, 6 preset siap pakai, dan pustaka skrip pengguna (`python_scripts.json`).

## Fungsi Bawaan
- `press(key, hold_ms=65, human=False)` / `human_press(key, min_hold, max_hold, min_wait, max_wait)`
- `hold_key(key, duration_ms=300)` — menahan tombol gerak `WASD` di game 3D.
- `rand(min_val, max_val)`, `wait(ms)`, `human_wait(min_ms, max_ms)`
- `print(text, enter=False, human=False)` / `type_text(...)`
- `click(button="left", x=None, y=None, hold_ms=68, human=False, in_game_3d=False)`
- `move_rel(dx, dy, steps=6, duration_ms=60)` — rotasi kamera 3D relatif yang halus.

---

[🌐 Semua Bahasa](../README.md) | [🏠 Beranda](01-Home-and-Getting-Started.md) | ← [Lab Uji & 22 Metode Input](05-Test-Lab-and-22-Input-Methods.md) | [Driver Kernel & Mikrokontroler USB HID](07-Hardware-Drivers-and-Microcontrollers.md) →
