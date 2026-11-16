# -*- coding: utf-8 -*-
"""
Hardware USB HID Microcontroller Bridge for V-MacroPlus v3.9.0:
- Method 21 (`pico_usb_hid`): Raspberry Pi Pico / Pico W / Pico 2 (RP2040 / RP2350) External Hardware USB HID Keyboard & Mouse.
- Method 22 (`arduino_usb_hid`): Arduino Leonardo / Pro Micro (ATmega32U4) External Hardware USB HID Keyboard & Mouse.

Features:
1. Zero-Dependency Native Win32 Serial (`CreateFileW` + `SetCommState` + `COMMTIMEOUTS` + Registry `SERIALCOMM` & `USB` VID/PID scan)
   with automatic `pyserial` support if installed — works out-of-the-box in Python and standalone `.exe` without extra packages.
2. Smart Auto-Detection (`auto`) by USB Vendor ID (VID) / Product ID (PID) AND live `PING` -> `PONG:PICO` / `PONG:ARDUINO` handshake.
3. Bundled Ready-to-Flash Firmwares (CircuitPython `boot.py` + `code.py` for Pico, Arduino C++ `.ino` for Pico, and Arduino C++ `.ino`
   for Leonardo / Pro Micro) with 1-click folder extraction and 1-click `CIRCUITPY` USB drive auto-installation.
"""

from __future__ import annotations
import ctypes
from ctypes import wintypes
import glob
import os
from pathlib import Path
import string
import sys
import threading
import time
from typing import Any, Dict, List, Optional, Tuple

IS_WINDOWS = sys.platform.startswith("win")

# Known USB Vendor IDs (VID) for Auto-Detection
PICO_USB_VIDS = {"2E8A", "239A"}                  # Raspberry Pi Foundation (0x2E8A), Adafruit CircuitPython (0x239A)
ARDUINO_USB_VIDS = {"2341", "2A03", "1B4F"}       # Official Arduino (0x2341 / 0x2A03), SparkFun Pro Micro (0x1B4F)

# ===========================================================================
# Ready-to-Flash Firmware Source Code (Embedded & Written to firmware/)
# ===========================================================================

PICO_CIRCUITPYTHON_BOOT_PY = """# -*- coding: utf-8 -*-
# V-MacroPlus Hardware USB HID Bridge — Raspberry Pi Pico (CircuitPython boot.py)
# Copy BOTH boot.py and code.py to the root of your CIRCUITPY USB drive, then unplug and replug the Pico.

import usb_cdc
import usb_hid

# Enable BOTH console and secondary data serial channel + Hardware USB HID Keyboard & Mouse
usb_cdc.enable(console=True, data=True)
usb_hid.enable((usb_hid.Device.KEYBOARD, usb_hid.Device.MOUSE))
"""

PICO_CIRCUITPYTHON_CODE_PY = """# -*- coding: utf-8 -*-
# V-MacroPlus Hardware USB HID Bridge — Raspberry Pi Pico (CircuitPython code.py)
# Turns Raspberry Pi Pico (RP2040 / RP2350) into a 100% genuine physical USB HID Keyboard & Mouse.

import time
import usb_cdc
import usb_hid

# Raw 8-byte USB HID Keyboard Report & 4-byte USB HID Mouse Report (Zero external library dependencies!)
kb_device = None
ms_device = None
for dev in usb_hid.devices:
    if dev.usage_page == 0x01 and dev.usage == 0x06:
        kb_device = dev
    elif dev.usage_page == 0x01 and dev.usage == 0x02:
        ms_device = dev

kb_report = bytearray(8)   # [modifiers, reserved, k1, k2, k3, k4, k5, k6]
ms_report = bytearray(4)   # [buttons, dx, dy, wheel]
pressed_keys = set()
ms_buttons = 0


def _send_kb() -> None:
    if kb_device is None:
        return
    for i in range(2, 8):
        kb_report[i] = 0
    idx = 2
    for usage in pressed_keys:
        if idx < 8:
            kb_report[idx] = usage & 0xFF
            idx += 1
    try:
        kb_device.send_report(kb_report)
    except Exception:
        pass


def _key_down(usage: int, mod_mask: int = 0) -> None:
    kb_report[0] |= (mod_mask & 0xFF)
    if 0xE0 <= usage <= 0xE7:
        kb_report[0] |= (1 << (usage - 0xE0))
    elif usage > 0:
        pressed_keys.add(usage & 0xFF)
    _send_kb()


def _key_up(usage: int, mod_mask: int = 0) -> None:
    if mod_mask:
        kb_report[0] &= ~(mod_mask & 0xFF)
    if 0xE0 <= usage <= 0xE7:
        kb_report[0] &= ~(1 << (usage - 0xE0))
    elif usage > 0:
        pressed_keys.discard(usage & 0xFF)
    _send_kb()


def _release_all() -> None:
    global ms_buttons
    pressed_keys.clear()
    for i in range(8):
        kb_report[i] = 0
    _send_kb()
    ms_buttons = 0
    _send_mouse(0, 0, 0)


def _signed_byte(val: int) -> int:
    v = max(-127, min(127, int(val)))
    return v & 0xFF


def _send_mouse(dx: int = 0, dy: int = 0, wheel: int = 0) -> None:
    if ms_device is None:
        return
    ms_report[0] = ms_buttons & 0x1F
    ms_report[1] = _signed_byte(dx)
    ms_report[2] = _signed_byte(dy)
    ms_report[3] = _signed_byte(wheel)
    try:
        ms_device.send_report(ms_report)
    except Exception:
        pass


def _btn_bit(name: str) -> int:
    n = name.strip().lower()
    if n == "right":
        return 0x02
    if n == "middle":
        return 0x04
    if n in ("x1", "back"):
        return 0x08
    if n in ("x2", "forward"):
        return 0x10
    return 0x01


def handle_line(line: str) -> str:
    global ms_buttons
    cmd = line.strip()
    if not cmd:
        return ""
    if cmd == "PING":
        return "PONG:PICO_CIRCUITPYTHON_HID"
    if cmd == "REL_ALL":
        _release_all()
        return "OK"
    if cmd.startswith("KD:"):
        parts = cmd[3:].split(",")
        u = int(parts[0])
        m = int(parts[1]) if len(parts) > 1 else 0
        _key_down(u, m)
        return "OK"
    if cmd.startswith("KU:"):
        parts = cmd[3:].split(",")
        u = int(parts[0])
        m = int(parts[1]) if len(parts) > 1 else 0
        _key_up(u, m)
        return "OK"
    if cmd.startswith("KT:"):
        parts = cmd[3:].split(",")
        u = int(parts[0])
        hold_ms = int(parts[1]) if len(parts) > 1 else 25
        m = int(parts[2]) if len(parts) > 2 else 0
        _key_down(u, m)
        time.sleep(max(0.005, min(2.0, hold_ms / 1000.0)))
        _key_up(u, m)
        return "OK"
    if cmd.startswith("MD:"):
        ms_buttons |= _btn_bit(cmd[3:])
        _send_mouse(0, 0, 0)
        return "OK"
    if cmd.startswith("MU:"):
        ms_buttons &= ~_btn_bit(cmd[3:])
        _send_mouse(0, 0, 0)
        return "OK"
    if cmd.startswith("MC:"):
        parts = cmd[3:].split(",")
        b = _btn_bit(parts[0])
        hold_ms = int(parts[1]) if len(parts) > 1 else 25
        ms_buttons |= b
        _send_mouse(0, 0, 0)
        time.sleep(max(0.005, min(2.0, hold_ms / 1000.0)))
        ms_buttons &= ~b
        _send_mouse(0, 0, 0)
        return "OK"
    if cmd.startswith("MM:"):
        parts = cmd[3:].split(",")
        dx = int(parts[0])
        dy = int(parts[1]) if len(parts) > 1 else 0
        _send_mouse(dx, dy, 0)
        return "OK"
    if cmd.startswith("MS:"):
        parts = cmd[3:].split(",")
        wheel = int(parts[0])
        _send_mouse(0, 0, wheel)
        return "OK"
    return "ERR"


serial_port = usb_cdc.data or usb_cdc.console
buf = b""
while True:
    if serial_port is not None and serial_port.in_waiting > 0:
        chunk = serial_port.read(serial_port.in_waiting)
        if chunk:
            buf += chunk
            while b"\n" in buf:
                raw_line, buf = buf.split(b"\n", 1)
                try:
                    resp = handle_line(raw_line.decode("utf-8", "ignore"))
                    if resp:
                        serial_port.write((resp + "\n").encode("utf-8"))
                except Exception:
                    pass
    else:
        time.sleep(0.001)
"""

PICO_CPP_ARDUINO_INO = """/*
 * V-MacroPlus Hardware USB HID Bridge — Raspberry Pi Pico / Pico 2 (Arduino IDE C++ Sketch)
 * Board Core: Earle F. Philhower raspberrypi / Arduino Mbed OS RP2040
 * USB Stack: "Adafruit TinyUSB" or "Pico SDK USB (Keyboard + Mouse + CDC Serial)"
 * Baud Rate: 115200
 */

#include <Keyboard.h>
#include <Mouse.h>

static String inputBuffer = "";

uint8_t mapHidUsageToArduinoKey(uint8_t usage) {
  // Printable ASCII & Special Keys mapping for Arduino Keyboard.h
  if (usage >= 0x04 && usage <= 0x1D) return 'a' + (usage - 0x04);
  if (usage >= 0x1E && usage <= 0x26) return '1' + (usage - 0x1E);
  if (usage == 0x27) return '0';
  switch (usage) {
    case 0x28: return KEY_RETURN;
    case 0x29: return KEY_ESC;
    case 0x2A: return KEY_BACKSPACE;
    case 0x2B: return KEY_TAB;
    case 0x2C: return ' ';
    case 0x2D: return '-';
    case 0x2E: return '=';
    case 0x2F: return '[';
    case 0x30: return ']';
    case 0x31: return '\\';
    case 0x33: return ';';
    case 0x34: return '\'';
    case 0x35: return '`';
    case 0x36: return ',';
    case 0x37: return '.';
    case 0x38: return '/';
    case 0x39: return KEY_CAPS_LOCK;
    case 0x3A: return KEY_F1;
    case 0x3B: return KEY_F2;
    case 0x3C: return KEY_F3;
    case 0x3D: return KEY_F4;
    case 0x3E: return KEY_F5;
    case 0x3F: return KEY_F6;
    case 0x40: return KEY_F7;
    case 0x41: return KEY_F8;
    case 0x42: return KEY_F9;
    case 0x43: return KEY_F10;
    case 0x44: return KEY_F11;
    case 0x45: return KEY_F12;
    case 0x49: return KEY_INSERT;
    case 0x4A: return KEY_HOME;
    case 0x4B: return KEY_PAGE_UP;
    case 0x4C: return KEY_DELETE;
    case 0x4D: return KEY_END;
    case 0x4E: return KEY_PAGE_DOWN;
    case 0x4F: return KEY_RIGHT_ARROW;
    case 0x50: return KEY_LEFT_ARROW;
    case 0x51: return KEY_DOWN_ARROW;
    case 0x52: return KEY_UP_ARROW;
    case 0xE0: return KEY_LEFT_CTRL;
    case 0xE1: return KEY_LEFT_SHIFT;
    case 0xE2: return KEY_LEFT_ALT;
    case 0xE3: return KEY_LEFT_GUI;
    case 0xE4: return KEY_RIGHT_CTRL;
    case 0xE5: return KEY_RIGHT_SHIFT;
    case 0xE6: return KEY_RIGHT_ALT;
    case 0xE7: return KEY_RIGHT_GUI;
    default: return 0;
  }
}

uint8_t parseMouseButton(const String &btn) {
  if (btn == "right") return MOUSE_RIGHT;
  if (btn == "middle") return MOUSE_MIDDLE;
  return MOUSE_LEFT;
}

void applyModifiers(int modMask, bool down) {
  if (modMask & 0x01) { if (down) Keyboard.press(KEY_LEFT_CTRL); else Keyboard.release(KEY_LEFT_CTRL); }
  if (modMask & 0x02) { if (down) Keyboard.press(KEY_LEFT_SHIFT); else Keyboard.release(KEY_LEFT_SHIFT); }
  if (modMask & 0x04) { if (down) Keyboard.press(KEY_LEFT_ALT); else Keyboard.release(KEY_LEFT_ALT); }
  if (modMask & 0x08) { if (down) Keyboard.press(KEY_LEFT_GUI); else Keyboard.release(KEY_LEFT_GUI); }
}

void processCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "PING") {
    Serial.println("PONG:PICO_CPP_HID");
    return;
  }
  if (cmd == "REL_ALL") {
    Keyboard.releaseAll();
    Mouse.release(MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("KD:")) {
    int c1 = cmd.indexOf(',', 3);
    int usage = (c1 > 0) ? cmd.substring(3, c1).toInt() : cmd.substring(3).toInt();
    int mod = (c1 > 0) ? cmd.substring(c1 + 1).toInt() : 0;
    applyModifiers(mod, true);
    uint8_t k = mapHidUsageToArduinoKey((uint8_t)usage);
    if (k) Keyboard.press(k);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("KU:")) {
    int c1 = cmd.indexOf(',', 3);
    int usage = (c1 > 0) ? cmd.substring(3, c1).toInt() : cmd.substring(3).toInt();
    int mod = (c1 > 0) ? cmd.substring(c1 + 1).toInt() : 0;
    uint8_t k = mapHidUsageToArduinoKey((uint8_t)usage);
    if (k) Keyboard.release(k);
    applyModifiers(mod, false);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("KT:")) {
    int c1 = cmd.indexOf(',', 3);
    int c2 = (c1 > 0) ? cmd.indexOf(',', c1 + 1) : -1;
    int usage = (c1 > 0) ? cmd.substring(3, c1).toInt() : cmd.substring(3).toInt();
    int holdMs = (c1 > 0) ? ((c2 > 0) ? cmd.substring(c1 + 1, c2).toInt() : cmd.substring(c1 + 1).toInt()) : 25;
    int mod = (c2 > 0) ? cmd.substring(c2 + 1).toInt() : 0;
    applyModifiers(mod, true);
    uint8_t k = mapHidUsageToArduinoKey((uint8_t)usage);
    if (k) Keyboard.press(k);
    delay(max(5, min(2000, holdMs)));
    if (k) Keyboard.release(k);
    applyModifiers(mod, false);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MD:")) {
    Mouse.press(parseMouseButton(cmd.substring(3)));
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MU:")) {
    Mouse.release(parseMouseButton(cmd.substring(3)));
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MC:")) {
    int c1 = cmd.indexOf(',', 3);
    String bStr = (c1 > 0) ? cmd.substring(3, c1) : cmd.substring(3);
    int holdMs = (c1 > 0) ? cmd.substring(c1 + 1).toInt() : 25;
    uint8_t b = parseMouseButton(bStr);
    Mouse.press(b);
    delay(max(5, min(2000, holdMs)));
    Mouse.release(b);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MM:")) {
    int c1 = cmd.indexOf(',', 3);
    int dx = (c1 > 0) ? cmd.substring(3, c1).toInt() : 0;
    int dy = (c1 > 0) ? cmd.substring(c1 + 1).toInt() : 0;
    Mouse.move((signed char)constrain(dx, -127, 127), (signed char)constrain(dy, -127, 127), 0);
    Serial.println("OK");
    return;
  }
  if (cmd.startsWith("MS:")) {
    int c1 = cmd.indexOf(',', 3);
    int wheel = (c1 > 0) ? cmd.substring(3, c1).toInt() : cmd.substring(3).toInt();
    Mouse.move(0, 0, (signed char)constrain(wheel, -127, 127));
    Serial.println("OK");
    return;
  }
}

void setup() {
  Serial.begin(115200);
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      processCommand(inputBuffer);
      inputBuffer = "";
    } else if (c != '\r') {
      inputBuffer += c;
    }
  }
}
"""

ARDUINO_LEONARDO_PROMICRO_INO = (
    PICO_CPP_ARDUINO_INO
    .replace("Raspberry Pi Pico / Pico 2 (Arduino IDE C++ Sketch)", "Arduino Leonardo / Pro Micro (ATmega32U4 Native USB HID)")
    .replace("PONG:PICO_CPP_HID", "PONG:ARDUINO_32U4_HID")
)

FIRMWARE_README_TXT = """================================================================================
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
"""


def ensure_firmware_files_on_disk(base_dir: Optional[Path] = None) -> Path:
    """Write all ready-to-flash Pico (CircuitPython + C++) and Arduino (.ino) files into `firmware/`."""
    if base_dir is None:
        base_dir = Path(__file__).resolve().parents[2] / "firmware"
    cp_dir = base_dir / "raspberry_pi_pico_circuitpython"
    pico_cpp_dir = base_dir / "raspberry_pi_pico_arduino_cpp"
    ard_dir = base_dir / "arduino_leonardo_promicro"

    cp_dir.mkdir(parents=True, exist_ok=True)
    pico_cpp_dir.mkdir(parents=True, exist_ok=True)
    ard_dir.mkdir(parents=True, exist_ok=True)

    (cp_dir / "boot.py").write_text(PICO_CIRCUITPYTHON_BOOT_PY.lstrip(), encoding="utf-8")
    (cp_dir / "code.py").write_text(PICO_CIRCUITPYTHON_CODE_PY.lstrip().replace("\n pressed_keys =", "\npressed_keys ="), encoding="utf-8")
    (cp_dir / "README_PICO_CIRCUITPYTHON.txt").write_text(FIRMWARE_README_TXT.lstrip(), encoding="utf-8")
    (pico_cpp_dir / "pico_hid_bridge.ino").write_text(PICO_CPP_ARDUINO_INO.lstrip(), encoding="utf-8")
    (ard_dir / "arduino_hid_bridge.ino").write_text(ARDUINO_LEONARDO_PROMICRO_INO.lstrip(), encoding="utf-8")
    (base_dir / "README_FIRMWARE.txt").write_text(FIRMWARE_README_TXT.lstrip(), encoding="utf-8")
    return base_dir


# ===========================================================================
# Native Win32 (`CreateFileW` / `DCB`) & Cross-Platform Serial Port Transport
# ===========================================================================
GENERIC_READ = 0x80000000
GENERIC_WRITE = 0x40000000
OPEN_EXISTING = 3
INVALID_HANDLE_VALUE = ctypes.c_void_p(-1).value


class DCB(ctypes.Structure):
    _fields_ = [
        ("DCBlength", wintypes.DWORD),
        ("BaudRate", wintypes.DWORD),
        ("fFlags", wintypes.DWORD),
        ("wReserved", wintypes.WORD),
        ("XonLim", wintypes.WORD),
        ("XoffLim", wintypes.WORD),
        ("ByteSize", wintypes.BYTE),
        ("Parity", wintypes.BYTE),
        ("StopBits", wintypes.BYTE),
        ("XonChar", ctypes.c_char),
        ("XoffChar", ctypes.c_char),
        ("ErrorChar", ctypes.c_char),
        ("EofChar", ctypes.c_char),
        ("EvtChar", ctypes.c_char),
        ("wReserved1", wintypes.WORD),
    ]


class COMMTIMEOUTS(ctypes.Structure):
    _fields_ = [
        ("ReadIntervalTimeout", wintypes.DWORD),
        ("ReadTotalTimeoutMultiplier", wintypes.DWORD),
        ("ReadTotalTimeoutConstant", wintypes.DWORD),
        ("WriteTotalTimeoutMultiplier", wintypes.DWORD),
        ("WriteTotalTimeoutConstant", wintypes.DWORD),
    ]


class _NativeSerialConnection:
    """Lightweight, non-blocking Serial Port wrapper using Win32 `kernel32` (or `pyserial` if installed)."""

    def __init__(self, port: str, baudrate: int = 115200, timeout_ms: int = 45) -> None:
        self.port = port.strip()
        self.baudrate = baudrate
        self.timeout_ms = timeout_ms
        self._handle: Any = None
        self._pyserial_obj: Any = None
        self._lock = threading.Lock()

    def open(self) -> bool:
        try:
            import serial  # type: ignore
            self._pyserial_obj = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout_ms / 1000.0,
                write_timeout=self.timeout_ms / 1000.0,
            )
            return True
        except Exception:
            pass

        if not IS_WINDOWS:
            return False

        try:
            k32 = ctypes.windll.kernel32
            dev_path = f"\\\\.\\{self.port}"
            h = k32.CreateFileW(
                dev_path,
                GENERIC_READ | GENERIC_WRITE,
                0,
                None,
                OPEN_EXISTING,
                0,
                None,
            )
            if not h or h == INVALID_HANDLE_VALUE or h == -1:
                return False

            dcb = DCB()
            dcb.DCBlength = ctypes.sizeof(DCB)
            if k32.GetCommState(h, ctypes.byref(dcb)):
                dcb.BaudRate = self.baudrate
                dcb.ByteSize = 8
                dcb.Parity = 0
                dcb.StopBits = 0
                # EnablefBinary (0x0001) | DTR_CONTROL_ENABLE (0x0010) | RTS_CONTROL_ENABLE (0x1000)
                dcb.fFlags = 0x1011
                k32.SetCommState(h, ctypes.byref(dcb))

            timeouts = COMMTIMEOUTS(
                ReadIntervalTimeout=15,
                ReadTotalTimeoutMultiplier=1,
                ReadTotalTimeoutConstant=self.timeout_ms,
                WriteTotalTimeoutMultiplier=1,
                WriteTotalTimeoutConstant=self.timeout_ms,
            )
            k32.SetCommTimeouts(h, ctypes.byref(timeouts))
            k32.PurgeComm(h, 0x000C)  # PURGE_RXCLEAR | PURGE_TXCLEAR
            self._handle = h
            return True
        except Exception:
            self.close()
            return False

    def is_open(self) -> bool:
        if self._pyserial_obj is not None:
            return bool(getattr(self._pyserial_obj, "is_open", False))
        return bool(self._handle is not None and self._handle != INVALID_HANDLE_VALUE)

    def close(self) -> None:
        if self._pyserial_obj is not None:
            try:
                self._pyserial_obj.close()
            except Exception:
                pass
            self._pyserial_obj = None
        if self._handle is not None and IS_WINDOWS:
            try:
                ctypes.windll.kernel32.CloseHandle(self._handle)
            except Exception:
                pass
            self._handle = None

    def send_command(self, cmd_line: str, expect_reply: bool = False) -> Tuple[bool, str]:
        with self._lock:
            if not self.is_open():
                return False, ""
            payload = (cmd_line.strip() + "\n").encode("utf-8")
            if self._pyserial_obj is not None:
                try:
                    self._pyserial_obj.write(payload)
                    self._pyserial_obj.flush()
                    if expect_reply:
                        line = self._pyserial_obj.readline().decode("utf-8", "ignore").strip()
                        return True, line
                    return True, "OK"
                except Exception:
                    self.close()
                    return False, ""

            if IS_WINDOWS and self._handle is not None:
                try:
                    k32 = ctypes.windll.kernel32
                    written = wintypes.DWORD(0)
                    ok = k32.WriteFile(self._handle, payload, len(payload), ctypes.byref(written), None)
                    if not ok or written.value == 0:
                        self.close()
                        return False, ""
                    if not expect_reply:
                        return True, "OK"
                    buf = ctypes.create_string_buffer(128)
                    read_dw = wintypes.DWORD(0)
                    k32.ReadFile(self._handle, buf, 127, ctypes.byref(read_dw), None)
                    resp = buf.raw[: read_dw.value].decode("utf-8", "ignore").strip()
                    return True, resp
                except Exception:
                    self.close()
                    return False, ""
            return False, ""


def _scan_windows_usb_serial_vid_map() -> Dict[str, Tuple[str, str, str]]:
    r"""
    Fast, non-blocking Windows Registry lookup under `SYSTEM\CurrentControlSet\Enum\USB`
    mapping active USB Serial `PortName` (`COMx`) -> `(vid_hex, pid_hex, board_type)`.
    Never touches Bluetooth (`BTHENUM`) or legacy motherboard (`ACPI\PNP0501`) ports!
    """
    usb_port_map: Dict[str, Tuple[str, str, str]] = {}
    if not IS_WINDOWS:
        return usb_port_map
    try:
        import winreg
        usb_root = r"SYSTEM\CurrentControlSet\Enum\USB"
        with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, usb_root) as h_usb:
            i = 0
            while True:
                try:
                    dev_id = winreg.EnumKey(h_usb, i)
                    i += 1
                except OSError:
                    break
                up_id = dev_id.upper()
                if "VID_" not in up_id:
                    continue
                vid_hex = ""
                pid_hex = ""
                try:
                    v_pos = up_id.find("VID_")
                    if v_pos >= 0:
                        vid_hex = up_id[v_pos + 4 : v_pos + 8]
                    p_pos = up_id.find("PID_")
                    if p_pos >= 0:
                        pid_hex = up_id[p_pos + 4 : p_pos + 8]
                except Exception:
                    pass
                b_type = ""
                if vid_hex in PICO_USB_VIDS:
                    b_type = "pico"
                elif vid_hex in ARDUINO_USB_VIDS:
                    b_type = "arduino"
                elif vid_hex in {"1A86", "10C4", "0403"}:
                    b_type = "usb_uart"
                else:
                    continue

                try:
                    with winreg.OpenKey(h_usb, dev_id) as h_dev:
                        j = 0
                        while True:
                            try:
                                inst_id = winreg.EnumKey(h_dev, j)
                                j += 1
                            except OSError:
                                break
                            try:
                                with winreg.OpenKey(h_dev, f"{inst_id}\\Device Parameters") as h_dp:
                                    port_val, _ = winreg.QueryValueEx(h_dp, "PortName")
                                    p_str = str(port_val).strip().upper()
                                    if p_str.startswith("COM"):
                                        usb_port_map[p_str] = (vid_hex, pid_hex, b_type)
                            except OSError:
                                pass
                except OSError:
                    pass
    except Exception:
        pass
    return usb_port_map


def list_serial_ports_with_mcu_info() -> List[Dict[str, Any]]:
    """
    Enumerate active serial ports and classify whether each port is a genuine USB Raspberry Pi Pico,
    Arduino Leonardo / Pro Micro, or generic port without opening or blocking on any device handle.
    """
    ports_map: Dict[str, Dict[str, Any]] = {}

    # 1. Try pyserial list_ports if installed
    try:
        import serial.tools.list_ports  # type: ignore
        for p in serial.tools.list_ports.comports():
            dev = str(p.device).strip()
            hwid = str(getattr(p, "hwid", "") or "").upper()
            if "BTHENUM" in hwid:
                continue  # Never touch Bluetooth virtual COM ports (they block for seconds on open!)
            vid_hex = f"{p.vid:04X}" if getattr(p, "vid", None) else ""
            pid_hex = f"{p.pid:04X}" if getattr(p, "pid", None) else ""
            desc = str(getattr(p, "description", "") or "")
            board_guess = ""
            if vid_hex in PICO_USB_VIDS or "pico" in desc.lower() or "rp2040" in desc.lower():
                board_guess = "pico"
            elif vid_hex in ARDUINO_USB_VIDS or "arduino" in desc.lower() or "leonardo" in desc.lower() or "pro micro" in desc.lower():
                board_guess = "arduino"
            ports_map[dev.upper()] = {
                "port": dev,
                "description": desc or dev,
                "vid": vid_hex,
                "pid": pid_hex,
                "board_type": board_guess,
            }
    except Exception:
        pass

    # 2. Native Windows Registry scan (HARDWARE\DEVICEMAP\SERIALCOMM cross-referenced with USB VID/PID)
    if IS_WINDOWS:
        usb_vid_map = _scan_windows_usb_serial_vid_map()
        try:
            import winreg
            with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"HARDWARE\DEVICEMAP\SERIALCOMM") as key:
                idx = 0
                while True:
                    try:
                        val_name, port_str, _ = winreg.EnumValue(key, idx)
                        idx += 1
                        p_clean = str(port_str).strip()
                        p_up = p_clean.upper()
                        vn_low = val_name.lower()
                        # Skip Bluetooth virtual COM ports (BthModem) which hang CreateFileW!
                        if "bth" in vn_low or "bluetooth" in vn_low:
                            continue
                        if p_clean and p_up not in ports_map:
                            vid_h, pid_h, b_guess = usb_vid_map.get(p_up, ("", "", ""))
                            tag = val_name.split(chr(92))[-1]
                            if b_guess == "pico":
                                desc = f"{p_clean} — Raspberry Pi Pico USB (VID:{vid_h})"
                            elif b_guess == "arduino":
                                desc = f"{p_clean} — Arduino USB HID (VID:{vid_h})"
                            else:
                                desc = f"{p_clean} ({tag})"
                            ports_map[p_up] = {
                                "port": p_clean,
                                "description": desc,
                                "vid": vid_h,
                                "pid": pid_h,
                                "board_type": b_guess,
                            }
                    except OSError:
                        break
        except Exception:
            pass
    else:
        for pat in ("/dev/ttyACM*", "/dev/ttyUSB*"):
            for dev_p in sorted(glob.glob(pat)):
                if dev_p not in ports_map:
                    ports_map[dev_p] = {
                        "port": dev_p,
                        "description": dev_p,
                        "vid": "",
                        "pid": "",
                        "board_type": "",
                    }

    return list(ports_map.values())


class MicrocontrollerBridge:
    """
    Manages non-blocking auto-detection, live `PING`/`PONG` verification, and hardware USB HID command dispatch
    for Raspberry Pi Pico (`pico_usb_hid`, Method 21) and Arduino Leonardo / Pro Micro (`arduino_usb_hid`, Method 22).
    """

    def __init__(self) -> None:
        self._conn: Optional[_NativeSerialConnection] = None
        self._active_port: str = ""
        self._active_board: str = ""      # "pico", "arduino", or ""
        self._active_fw_banner: str = ""
        self._preferred_port: str = "auto"
        self._last_failed_probe_ts: Dict[str, float] = {}
        self._lock = threading.Lock()
        try:
            ensure_firmware_files_on_disk()
        except Exception:
            pass

    def set_preferred_port(self, port: str) -> None:
        clean = (port or "auto").strip()
        if clean != self._preferred_port:
            self._preferred_port = clean
            self._last_failed_probe_ts.clear()
            self.disconnect()

    def disconnect(self) -> None:
        with self._lock:
            if self._conn is not None:
                self._conn.close()
                self._conn = None
            self._active_port = ""
            self._active_board = ""
            self._active_fw_banner = ""

    def probe_and_connect(
        self,
        target_board: str = "any",
        preferred_port: str = "",
        force_scan: bool = False,
    ) -> Tuple[bool, str, str]:
        """
        Scan serial ports (or `preferred_port`), send `PING\n`, and verify `PONG:PICO...` or `PONG:ARDUINO...`.
        - Never opens non-MCU legacy/Bluetooth COM ports (`COM1`, etc.) when `preferred_port == 'auto'`.
        - Caches negative probe results for 4.0 seconds unless `force_scan=True` so UI & keystrokes NEVER block!
        """
        pref = (preferred_port or self._preferred_port or "auto").strip()
        if self._conn is not None and self._conn.is_open():
            if pref in ("auto", "", self._active_port):
                if target_board in ("any", "", self._active_board):
                    return True, self._active_port, self._active_fw_banner

        cache_key = f"{target_board}:{pref}"
        now = time.monotonic()
        if not force_scan and (now - self._last_failed_probe_ts.get(cache_key, -999.0)) < 4.0:
            return False, "", ""

        ports = list_serial_ports_with_mcu_info()
        if pref and pref.lower() != "auto":
            candidates = [p for p in ports if p["port"].upper() == pref.upper()]
            if not candidates:
                candidates = [{"port": pref, "description": pref, "vid": "", "pid": "", "board_type": "manual"}]
        else:
            # In Auto-Detect mode, ONLY open ports that are genuinely identified as USB MCU devices
            # (never open motherboard COM1 or unknown legacy ports that block CreateFileW/WriteFile!)
            candidates = [
                p for p in ports
                if p.get("board_type") in ("pico", "arduino", "usb_uart")
                and (target_board in ("any", "") or p.get("board_type") in (target_board, "usb_uart"))
            ]

        if not candidates:
            self._last_failed_probe_ts[cache_key] = now
            return False, "", ""

        for cand in candidates:
            p_name = cand["port"]
            bt_known = cand.get("board_type", "")
            if target_board in ("pico", "arduino") and bt_known in ("pico", "arduino") and bt_known != target_board:
                continue

            conn = _NativeSerialConnection(p_name, baudrate=115200, timeout_ms=25)
            if not conn.open():
                continue
            ok, reply = conn.send_command("PING", expect_reply=True)
            if ok and reply.startswith("PONG:"):
                b_type = "pico" if "PICO" in reply.upper() else ("arduino" if "ARDUINO" in reply.upper() else (bt_known if bt_known in ("pico", "arduino") else "pico"))
                if target_board not in ("any", "") and b_type != target_board:
                    conn.close()
                    continue
                with self._lock:
                    if self._conn is not None:
                        self._conn.close()
                    self._conn = conn
                    self._active_port = p_name
                    self._active_board = b_type
                    self._active_fw_banner = reply
                    self._last_failed_probe_ts.clear()
                return True, p_name, reply

            if bt_known in ("pico", "arduino") and (target_board in ("any", "", bt_known)):
                with self._lock:
                    if self._conn is not None:
                        self._conn.close()
                    self._conn = conn
                    self._active_port = p_name
                    self._active_board = bt_known
                    self._active_fw_banner = f"USB-VID:{cand.get('vid', 'MCU')}"
                    self._last_failed_probe_ts.clear()
                return True, p_name, self._active_fw_banner
            conn.close()

        self._last_failed_probe_ts[cache_key] = time.monotonic()
        return False, "", ""

    def is_board_ready(self, board_type: str = "any", allow_probe: bool = False) -> bool:
        if self._conn is not None and self._conn.is_open():
            if board_type in ("any", "", self._active_board):
                return True
        if not allow_probe:
            # Fast path: only probe if we haven't failed in the last 4 seconds
            cache_key = f"{board_type}:{self._preferred_port}"
            if (time.monotonic() - self._last_failed_probe_ts.get(cache_key, -999.0)) < 4.0:
                return False
        ok, _, _ = self.probe_and_connect(target_board=board_type, force_scan=allow_probe)
        return ok

    def send_key_event(self, usb_hid_usage: int, is_down: bool, mod_bit: int = 0, board_type: str = "any") -> bool:
        if not self.is_board_ready(board_type):
            return False
        cmd = f"{'KD' if is_down else 'KU'}:{int(usb_hid_usage)},{int(mod_bit)}"
        assert self._conn is not None
        ok, _ = self._conn.send_command(cmd, expect_reply=False)
        return ok

    def send_key_tap(self, usb_hid_usage: int, hold_ms: int = 25, mod_bit: int = 0, board_type: str = "any") -> bool:
        if not self.is_board_ready(board_type):
            return False
        cmd = f"KT:{int(usb_hid_usage)},{max(5, int(hold_ms))},{int(mod_bit)}"
        assert self._conn is not None
        ok, _ = self._conn.send_command(cmd, expect_reply=False)
        if ok and hold_ms > 0:
            time.sleep(max(0.005, min(2.0, hold_ms / 1000.0)))
        return ok

    def send_mouse_action(self, button: str = "left", action: str = "click", hold_ms: int = 25, board_type: str = "any") -> bool:
        if not self.is_board_ready(board_type):
            return False
        assert self._conn is not None
        b = (button or "left").strip().lower()
        if action == "down":
            ok, _ = self._conn.send_command(f"MD:{b}", expect_reply=False)
            return ok
        if action == "up":
            ok, _ = self._conn.send_command(f"MU:{b}", expect_reply=False)
            return ok
        ok, _ = self._conn.send_command(f"MC:{b},{max(5, int(hold_ms))}", expect_reply=False)
        if ok and hold_ms > 0:
            time.sleep(max(0.005, min(2.0, hold_ms / 1000.0)))
        return ok

    def send_mouse_move_rel(self, dx: int, dy: int, board_type: str = "any") -> bool:
        if not self.is_board_ready(board_type):
            return False
        assert self._conn is not None
        ok, _ = self._conn.send_command(f"MM:{int(dx)},{int(dy)}", expect_reply=False)
        return ok

    def send_mouse_wheel(self, wheel_delta: int, board_type: str = "any") -> bool:
        if not self.is_board_ready(board_type):
            return False
        assert self._conn is not None
        ok, _ = self._conn.send_command(f"MS:{int(wheel_delta)}", expect_reply=False)
        return ok


MCU_BRIDGE = MicrocontrollerBridge()


def find_circuitpy_drive() -> Optional[Path]:
    """Detect if a Raspberry Pi Pico running CircuitPython (`CIRCUITPY`) is mounted as a USB drive."""
    if IS_WINDOWS:
        kernel32 = ctypes.windll.kernel32
        for letter in string.ascii_uppercase:
            root = f"{letter}:\\"
            if not os.path.exists(root):
                continue
            vol_buf = ctypes.create_unicode_buffer(256)
            if kernel32.GetVolumeInformationW(root, vol_buf, 256, None, None, None, None, 0):
                if vol_buf.value.strip().upper() == "CIRCUITPY":
                    return Path(root)
    else:
        for base in ("/media", "/run/media", "/mnt", "/Volumes"):
            if os.path.exists(base):
                for p in Path(base).rglob("CIRCUITPY"):
                    if p.is_dir():
                        return p
    return None


def auto_install_circuitpython_to_pico_drive() -> Tuple[bool, str]:
    """Automatically copy `boot.py` and `code.py` onto a mounted `CIRCUITPY` drive if present."""
    from app.i18n import tl
    drive = find_circuitpy_drive()
    if drive is None:
        return False, tl(
            "لم يتم العثور على فلاشة CIRCUITPY متصلة — تأكد من تثبيت CircuitPython على الـ Pico وتوصيلها بالـ USB أولاً، أو استخدم زر «استخراج ملفات السوفتوير» لحفظها في أي مجلد.",
            "No CIRCUITPY USB drive detected — Make sure CircuitPython is flashed on your Pico and plugged in, or use 'Extract Firmware Files' to save them to any folder.",
        )
    try:
        (drive / "boot.py").write_text(PICO_CIRCUITPYTHON_BOOT_PY.lstrip(), encoding="utf-8")
        (drive / "code.py").write_text(PICO_CIRCUITPYTHON_CODE_PY.lstrip().replace("\n pressed_keys =", "\npressed_keys ="), encoding="utf-8")
        return True, tl(
            f"✓ تم نسخ boot.py و code.py تلقائياً إلى فلاشة الـ Pico ({drive})! افصل الـ Pico وأعد توصيلها لتفعيل الـ USB HID.",
            f"✓ Automatically installed boot.py & code.py onto Pico drive ({drive})! Unplug and replug your Pico once to activate USB HID.",
        )
    except Exception as exc:
        return False, f"Error writing to {drive}: {exc}"


def extract_all_firmware_files(target_dir: str) -> Tuple[bool, str]:
    """Extract all 3 firmware folders (Pico CircuitPython, Pico C++, and Arduino Leonardo/Pro Micro) to `target_dir`."""
    from app.i18n import tl
    try:
        out = ensure_firmware_files_on_disk(Path(target_dir) / "V-MacroPlus_MCU_Firmwares")
        return True, tl(
            f"✓ تم استخراج جميع ملفات السوفتوير الجاهزة (Pico CircuitPython + Pico C++ + Arduino) في: {out}",
            f"✓ Extracted all ready-to-flash firmware files (Pico CircuitPython + Pico C++ + Arduino) to: {out}",
        )
    except Exception as exc:
        return False, str(exc)


def diagnose_mcu_boards(preferred_port: str = "auto", run_live_ping: bool = True) -> Dict[str, Any]:
    """Scan all serial ports, optionally perform a live `PING`/`PONG` probe, and return a structured diagnostic dict."""
    MCU_BRIDGE.set_preferred_port(preferred_port)
    ports = list_serial_ports_with_mcu_info()
    circuitpy_drive = find_circuitpy_drive()
    connected = False
    active_port = ""
    banner = ""
    board_type = ""

    if run_live_ping:
        connected, active_port, banner = MCU_BRIDGE.probe_and_connect("any", preferred_port, force_scan=True)
        board_type = MCU_BRIDGE._active_board
    else:
        connected = MCU_BRIDGE._conn is not None and MCU_BRIDGE._conn.is_open()
        active_port = MCU_BRIDGE._active_port
        banner = MCU_BRIDGE._active_fw_banner
        board_type = MCU_BRIDGE._active_board

    return {
        "connected": connected,
        "active_port": active_port,
        "board_type": board_type,
        "firmware_banner": banner,
        "preferred_port": preferred_port,
        "ports": ports,
        "circuitpy_drive": str(circuitpy_drive) if circuitpy_drive else "",
        "firmware_dir": str(ensure_firmware_files_on_disk()),
    }


def format_mcu_diagnostic_report(diag: Dict[str, Any], is_arabic: bool = True) -> Tuple[str, str, str]:
    """Return `(headline, details_multiline, status_kind)` for the Settings Microcontroller Card."""
    conn = bool(diag.get("connected", False))
    port = str(diag.get("active_port", ""))
    b_type = str(diag.get("board_type", ""))
    banner = str(diag.get("firmware_banner", ""))
    ports = diag.get("ports", [])
    cpy_drive = str(diag.get("circuitpy_drive", ""))

    ports_summary = ", ".join(f"{p['port']}" + (f" [{p['board_type'].upper()}]" if p.get("board_type") else "") for p in ports)
    if not ports_summary:
        ports_summary = "لا توجد منافذ COM متصلة حالياً" if is_arabic else "No active COM ports detected"

    if conn:
        b_label = "Raspberry Pi Pico" if b_type == "pico" else ("Arduino Leonardo / Pro Micro" if b_type == "arduino" else "MCU USB HID")
        headline = (
            f"🟢 بوردة {b_label} متصلة ونشطة فعلياً على المنفذ [{port}]! ({banner})"
            if is_arabic
            else f"🟢 {b_label} Hardware USB HID Board ACTIVE & Verified on [{port}]! ({banner})"
        )
        kind = "ok"
    elif cpy_drive:
        headline = (
            f"🟡 تم اكتشاف فلاشة Raspberry Pi Pico ({cpy_drive}) — اضغط زر «تثبيت السوفتوير تلقائياً على الـ Pico» بالأسفل لتفعيلها فوراً!"
            if is_arabic
            else f"🟡 Detected Raspberry Pi Pico drive ({cpy_drive}) — Click 'Auto-Install Firmware to Pico Drive' below to activate!"
        )
        kind = "warn"
    else:
        headline = (
            "🔵 جاهز للكشف التلقائي عن Raspberry Pi Pico (الطريقة 21) و Arduino Leonardo/Pro Micro (الطريقة 22) بمجرد توصيل القطعة بالـ USB"
            if is_arabic
            else "🔵 Ready to Auto-Detect Raspberry Pi Pico (Method 21) & Arduino Leonardo/Pro Micro (Method 22) as soon as plugged into USB"
        )
        kind = "info"

    if is_arabic:
        l1 = f"• المنافذ المتاحة (COM Ports): {ports_summary}"
        l2 = f"• حالة الاتصال المباشر (PING/PONG @ 115200): {'✓ متصل ونشط على ' + port + ' (' + banner + ')' if conn else 'في انتظار توصيل القطعة (يعمل الكشف التلقائي Auto-Detect)'}"
        l3 = "• السوفتوير المدمج (3 نسخ جاهزة): 1) Pico CircuitPython (نسخ مباشر بدون برامج) | 2) Pico C++ (.ino) | 3) Arduino Leonardo/Pro Micro (.ino)"
    else:
        l1 = f"• Available Serial Ports (COM): {ports_summary}"
        l2 = f"• Live Hardware Bridge Status (115200 baud): {'✓ Connected on ' + port + ' (' + banner + ')' if conn else 'Waiting for board connection (Auto-Detect active)'}"
        l3 = "• Bundled Ready Firmwares (3 Editions): 1) Pico CircuitPython (Drag & Drop) | 2) Pico C++ (.ino) | 3) Arduino Leonardo/Pro Micro (.ino)"

    return headline, "\n".join([l1, l2, l3]), kind
