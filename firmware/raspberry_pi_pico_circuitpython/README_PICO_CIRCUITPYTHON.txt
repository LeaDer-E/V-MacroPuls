================================================================================
V-MacroPlus v3.9.0 — Hardware USB HID Microcontroller Guide
(Raspberry Pi Pico RP2040/RP2350 & Arduino Leonardo / Pro Micro ATmega32U4)
================================================================================

Why use a Raspberry Pi Pico or Arduino Leonardo / Pro Micro?
------------------------------------------------------------
When connected via USB, the Raspberry Pi Pico (Method 21) or Arduino Leonardo /
Pro Micro (Method 22) enumerates on Windows/Linux as a genuine physical hardware
USB Composite Keyboard + Mouse. Because keystrokes and mouse movements enter the
operating system from an external USB controller cable, 3D game engines (DirectInput,
RawInput) and anti-cheat filters see 100% physical hardware input with zero
software injection flags.

--------------------------------------------------------------------------------
OPTION A: Raspberry Pi Pico — CircuitPython (Easiest: 60 Seconds, No Compiler!)
--------------------------------------------------------------------------------
1. Hold the white BOOTSEL button on your Raspberry Pi Pico while plugging it into USB.
   A removable drive named `RPI-RP2` will appear on your computer.
2. Download the official CircuitPython `.uf2` file for Raspberry Pi Pico from:
   https://circuitpython.org/board/raspberry_pi_pico/
   and drag-and-drop the `.uf2` file onto the `RPI-RP2` drive.
3. The Pico will reboot automatically as a USB drive named `CIRCUITPY`.
4. Copy `boot.py` and `code.py` from `raspberry_pi_pico_circuitpython/` directly
   onto the `CIRCUITPY` drive (or click "Install to Pico Drive (CIRCUITPY)" inside
   V-MacroPlus -> Settings).
5. Unplug and replug the Pico once. Open V-MacroPlus -> Settings, click
   "Auto-Detect & Test Piece", and select Method 21!

--------------------------------------------------------------------------------
OPTION B: Raspberry Pi Pico — C++ Arduino IDE Sketch (`pico_hid_bridge.ino`)
--------------------------------------------------------------------------------
1. Open `raspberry_pi_pico_arduino_cpp/pico_hid_bridge.ino` in Arduino IDE.
2. Select Board: "Raspberry Pi Pico" (Earle Philhower core or Arduino Mbed RP2040).
3. Click Upload. Once uploaded, V-MacroPlus will auto-detect it on Method 21!

--------------------------------------------------------------------------------
OPTION C: Arduino Leonardo / Pro Micro (ATmega32U4) (`arduino_hid_bridge.ino`)
--------------------------------------------------------------------------------
1. Open `arduino_leonardo_promicro/arduino_hid_bridge.ino` in Arduino IDE.
2. Select Tools -> Board -> Arduino AVR Boards -> "Arduino Leonardo" (or Pro Micro).
3. Click Upload. Once uploaded, V-MacroPlus will auto-detect it on Method 22!
================================================================================
