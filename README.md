<p align="center">
<img src="assets/icon.png" alt="V-MacroPlus Logo" width="130" />
</p>
<h1 align="center">
V-MacroPlus v3.9.0 (C++20 / Qt 6 Edition)
</h1>

<div align="center">
High-precision desktop macro automation, smart key chord studio, multi-tab background auto-presser, 240 FPS input recorder, and 22-method hardware/kernel/MCU input testing suite built natively in **C++20** and **Qt 6** for **Windows** and **Linux**, with an embedded **Python Mini-Script Runner** for user-written automation scripts and per-method sweeps.
</div>

<br>

<p align="center">
  <img src="https://img.shields.io/badge/version-3.9.0-8B5CF6?style=flat-square" alt="Version" />
  <img src="https://img.shields.io/badge/C%2B%2B-20-3B82F6?style=flat-square" alt="C++" />
  <img src="https://img.shields.io/badge/UI-Qt%206.12%2B-10B981?style=flat-square" alt="UI Framework" />
  <img src="https://img.shields.io/badge/platform-Windows%2010%2F11%20%7C%20Linux%20%7C%20macOS%20(fallback)-06B6D4?style=flat-square" alt="Platform" />
  <img src="https://img.shields.io/badge/license-MIT%20(proposed)-F59E0B?style=flat-square" alt="License" />
</p>

<!-- Uncomment once docs/screenshots/hero-smart-keys-dark-ar.png is captured:
<br />
<img src="docs/screenshots/hero-smart-keys-dark-ar.png" alt="V-MacroPlus Smart Keys Studio" width="900" />
-->

</div>

Repository: [https://github.com/LeaDer-E/V-AutoKeyPresser](https://github.com/LeaDer-E/V-AutoKeyPresser)


## Key Features

- **22 Unified Input Methods**:
  - **Methods 1-10 (Foreground & Hardware/Kernel)**: `sendinput_scancode`, `sendinput_vk`, `keybd_mouse_event`, `direct_scan`, `cpp_stealth_humanized`, `cpp_direct_hardware`, `cpp_interception_driver` (Ring-0 Interception), `cpp_ps2_port_0x60` (InpOutx64 PS/2 port), `cpp_raw_hid_device`, `cpp_kernel_filter_hook`.
  - **Methods 11-20 (Background Window / HWND)**: `postmessage`, `sendmessage`, `post_child`, `send_child`, `post_active`, `send_notify`, `guithread_focus`, `attach_state`, `ime_pipeline`, `uia_invoke`.
  - **Method 21 (`pico_usb_hid`)**: Physical USB HID keyboard & mouse injection via **Raspberry Pi Pico** (CircuitPython or Arduino C++ firmware included in `firmware/`).
  - **Method 22 (`arduino_usb_hid`)**: Physical USB HID keyboard & mouse injection via **Arduino Leonardo / Pro Micro (ATmega32U4)** (firmware included in `firmware/`).
- **8 Mouse Injection Modes**: `auto`, `sendinput_abs`, `sendinput_rel`, `mouse_event`, `postmessage_client`, `sendmessage_client`, `interception_mouse`, `usb_hid_mouse`.
- **Embedded Python Script Runner (`PythonScriptRunner`)**:
  - Run user-written Python scripts inside **Smart Key `python_script` steps** and **Test Lab Per-Method Sweeps** (`sweep_22`).
  - Exposes `print(text, enter=False, human=False, char_delay_ms=0)`, `type_text(text)`, `press(key, hold_ms=20)`, `human_press(key, min_ms=65, max_ms=95)`, `hold_key(key, ms=300)`, `key_down(key)`, `key_up(key)`, `click(x=-1, y=-1, button="left", hold_ms=20)`, `wait(ms)`, and metadata variables (`method_num`, `method_id`, `method_name`, `category`, `target_app`, `target_hwnd`, `mode`).
  - Includes **6 built-in presets** and persistent user script library (`python_scripts.json`).
- **Smart Keys Studio**:
  - Multi-key chord & media key triggers, process whitelist/blacklist, mandatory `stop_hotkey` for `repeat_hold` and `toggle_loop` modes, `startDelayMs`, Edit-Mode Isolation (`Edit Steps`, `Save Changes`, `Save as New`), full `Ctrl+Z` / `Ctrl+Y` Undo/Redo, smooth animated step reordering, per-step Enable/Pause toggle, and instant RTL/LTR Step Notes.
- **240 FPS Recorder**:
  - Low-level `WH_KEYBOARD_LL` / `WH_MOUSE_LL` (Windows) and X11/XTest (Linux) capture with keyboard, click, scroll, and mouse-path filters, `once` / `count` / `duration` repeat modes, `playbackStartDelayMs`, optional floating stop pill, in-place `updateSavedMacro`, and 1-to-1 Smart Key conversion.
- **Non-Blocking Microcontroller Bridge (`MicrocontrollerBridge`)**:
  - Asynchronous serial port scanning (`auto` or manual COM/`ttyACM` port), `PING`/`PONG` connection diagnostics, 1-click `CIRCUITPY` installer for Raspberry Pi Pico, and 1-click `firmware/` bundle extractor.
- **Strict Self-App Immunity**:
  - Automated triggers never fire on V-MacroPlus itself (`isOwnAppForeground`, `isHwndOwnProcess`), except inside the Test Lab sandbox when explicitly testing.
- **10 Languages & 36 Themes**:
  - Full RTL/LTR support (`ar`, `en`, `es`, `fr`, `de`, `tr`, `ru`, `id`, `fa`, `ur`), Dark/Light modes, 36 curated color palettes, and custom accent color picker.

---

## Building with Qt Creator (Recommended)

### Prerequisites
- **Qt 6.5+** (`Qt6::Core`, `Qt6::Gui`, `Qt6::Widgets` — no extra `Qt6::SerialPort` or `Qt6::Svg` modules required)
- **CMake 3.20+** and **Ninja** (or MinGW Makefiles)
- **Windows**: MinGW GCC 13+ (or MSVC 2022) via Qt Online Installer
- **Linux**: GCC 11+ / Clang 14+, `libx11-dev`, `libxtst-dev` (for X11/XTest input injection)
- **Runtime (Optional, for Python Mini-Scripts)**: Python 3.8+ on `PATH` (`python` or `python3`)

### Steps in Qt Creator
1. Open **Qt Creator**.
2. Select **File -> Open File or Project...** and choose `V-MacroPlus-CPP/CMakeLists.txt`.
3. Select your **Desktop Qt 6 (MinGW 64-bit)** kit on Windows or **Desktop Qt 6 (GCC 64-bit)** kit on Linux, then click **Configure Project**.
4. Switch the build configuration to **Release** and press **Build** (`Ctrl+B`) or **Run** (`Ctrl+R`).

### Command-Line Build

**Windows (`build_windows.bat`):**
```bat
build_windows.bat
```

**Linux (`build_linux.sh`):**
```bash
chmod +x build_linux.sh
./build_linux.sh
```

---

## Hardware & Driver Setup

### 1. Ring-0 Interception Driver (Method 7)
1. Download the official Interception release from [https://github.com/oblitum/Interception/releases](https://github.com/oblitum/Interception/releases).
2. Run `install-interception.exe /install` from an Administrator Command Prompt and reboot.
3. Place `interception.dll` into `drivers/` next to `V-MacroPlus.exe` (or import it directly from **Settings -> Kernel Drivers**).

### 2. PS/2 Port 0x60 Driver (Method 8)
1. Download `InpOutx64.dll` from [https://www.highrez.co.uk/downloads/inpout32/](https://www.highrez.co.uk/downloads/inpout32/).
2. Place `inpoutx64.dll` into `drivers/` or import it from **Settings -> Kernel Drivers** while running as Administrator.

### 3. Raspberry Pi Pico (Method 21) & Arduino Leonardo / Pro Micro (Method 22)
- **Raspberry Pi Pico (CircuitPython - Easiest)**:
  1. Flash CircuitPython UF2 from [https://circuitpython.org/board/raspberry_pi_pico/](https://circuitpython.org/board/raspberry_pi_pico/).
  2. Connect the Pico so the `CIRCUITPY` drive appears.
  3. In V-MacroPlus **Settings -> Microcontroller USB HID Bridge**, click **Install to CIRCUITPY** (copies `boot.py`, `code.py`, and `adafruit_hid` automatically), or extract `firmware/` manually.
- **Arduino Leonardo / Pro Micro (ATmega32U4)**:
  1. Open [Arduino IDE](https://www.arduino.cc/en/software).
  2. Flash `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` to your board.
  3. Select `auto` (or the detected COM/`/dev/ttyACM*` port) in **Settings** and click **Scan & Test Connection**.

---

## Credits

- **Developer**: LeaDer-E (Mohamed)
- **Architecture & Engineering**: Argon AI Architecture
- **License**: MIT License
