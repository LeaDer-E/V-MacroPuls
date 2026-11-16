# -*- coding: utf-8 -*-
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
            while b"
" in buf:
                raw_line, buf = buf.split(b"
", 1)
                try:
                    resp = handle_line(raw_line.decode("utf-8", "ignore"))
                    if resp:
                        serial_port.write((resp + "
").encode("utf-8"))
                except Exception:
                    pass
    else:
        time.sleep(0.001)
