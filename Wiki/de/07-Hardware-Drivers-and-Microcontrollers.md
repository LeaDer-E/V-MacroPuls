[🌐 Alle Sprachen](../README.md) | [🏠 Startseite](01-Home-and-Getting-Started.md) | ← [Python Mini-Skripte & Humanisierung](06-Python-Mini-Scripting.md) | [Einstellungen, Designs & Hotkeys](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 Kernel-Treiber & USB-HID-Mikrocontroller

Für geschützte 3D-Spiele, die Software-Eingaben (`LLKHF_INJECTED = 1`) blockieren, erzeugt V-MacroPlus echte Hardware-Eingaben (`LLKHF_INJECTED = 0`):

## 1. Raspberry Pi Pico (Methode 21 — 1-Klick-Setup)
1. **BOOTSEL** beim Einstecken per **USB-Datenkabel** gedrückt halten (`RPI-RP2`).
2. Offizielle **CircuitPython `.uf2`** auf `RPI-RP2` kopieren — Laufwerk startet als **`CIRCUITPY`** neu.
3. In **V-MacroPlus -> Einstellungen** auf **1-Klick-Installation auf Pico (`CIRCUITPY`)** klicken.
4. Einmal neu anstecken, **Board erkennen & testen** klicken und **Methode 21** wählen.

## 2. Arduino Leonardo / Pro Micro (Methode 22 — ATmega32U4)
1. `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` in der **Arduino IDE** öffnen.
2. **Board: `Arduino Leonardo`** und COM-Port wählen, dann **Upload** klicken.
3. In **V-MacroPlus -> Einstellungen** testen und **Methode 22** wählen.

## 3. Interception (Methode 7) & InpOutx64 PS/2 (Methode 8)
`interception.dll` (64-Bit) bzw. `inpoutx64.dll` in den Ordner `drivers/` verknüpfen und nach `install-interception.exe` Windows einmal neu starten.

---

[🌐 Alle Sprachen](../README.md) | [🏠 Startseite](01-Home-and-Getting-Started.md) | ← [Python Mini-Skripte & Humanisierung](06-Python-Mini-Scripting.md) | [Einstellungen, Designs & Hotkeys](08-Settings-Themes-and-Hotkeys.md) →
