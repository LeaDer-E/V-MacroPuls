[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Python Mini-Scripting & Humanization](06-Python-Mini-Scripting.md) | [Settings, Themes & Global Hotkeys](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 Kernel Drivers & External USB HID Microcontrollers

Protected applications and 3D games sometimes ignore software-injected inputs flagged with `LLKHF_INJECTED = 1`. V-MacroPlus supports true hardware-level input (`LLKHF_INJECTED = 0`) via three options:

## 1. Raspberry Pi Pico (Method 21 — Easiest 60-Second Setup)
When connected over a USB **data cable**, a **Raspberry Pi Pico** (`RP2040` / `RP2350`) enumerates as a genuine physical USB keyboard and mouse.
1. Hold the white **BOOTSEL** button while plugging in the Pico — it appears as a USB drive named `RPI-RP2`.
2. Download the official **CircuitPython `.uf2`** for Raspberry Pi Pico and copy it onto `RPI-RP2`. It reboots as **`CIRCUITPY`**.
3. Open **V-MacroPlus -> Settings -> External USB HID Microcontroller Bridge** and click **1-Click Install to Pico Drive (`CIRCUITPY`)** (or copy `boot.py` and `code.py` from `firmware/raspberry_pi_pico_circuitpython/`).
4. Unplug and replug once, click **Auto-Detect & Test Board Now**, and select **Method 21**.

## 2. Arduino Leonardo / Pro Micro (Method 22 — Compact & Affordable)
Powered by the **`ATmega32U4`** chip with native USB HID support (note: requires Leonardo or Pro Micro, not Uno/Nano).
1. Open `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` in **Arduino IDE**.
2. Select **Tools -> Board -> `Arduino Leonardo`** and your COM port under **Tools -> Port**, then click **Upload**.
3. In **V-MacroPlus -> Settings**, click **Auto-Detect & Test Board Now** and select **Method 22**.

> All microcontroller probes run asynchronously on background threads so the UI never freezes even when no board is connected.

## 3. Ring-0 Kernel Interception Driver (Method 7)
1. Download `Interception.zip` (`v1.0.1`) from GitHub and extract it.
2. In **V-MacroPlus -> Settings -> Interception Diagnostics**, link `install-interception.exe` (runs as Admin) and copy `library/x64/interception.dll` into `drivers/`.
3. **Reboot Windows once**, then click **Check Interception & Kernel Status Now**.

## 4. Motherboard PS/2 Port 0x60/0x64 (Method 8 — InpOutx64)
1. Download `InpOutBinaries_1501.zip` and link `x64/inpoutx64.dll` via Settings.
2. Run V-MacroPlus as Administrator and click **Check PS/2 Port & InpOutx64 Now**.

---

[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Python Mini-Scripting & Humanization](06-Python-Mini-Scripting.md) | [Settings, Themes & Global Hotkeys](08-Settings-Themes-and-Hotkeys.md) →
