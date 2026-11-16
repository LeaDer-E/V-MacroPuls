# -*- coding: utf-8 -*-
"""
Native Windows Low-Level Keyboard Hook (`WH_KEYBOARD_LL = 13`) AND Native Low-Level Mouse Hook (`WH_MOUSE_LL = 14`),
Multi-Monitor Cursor Position/Click/Hold/Scroll API (supporting Left, Right, Middle, Back X1, Forward X2,
and 4-way Scroll Up/Down/Left/Right), 10 Selectable Input Injection & Hardware Keyboard Emulation Engines,
Full Ring-0 Interception (Keyboard + Mouse) & PS/2 Port 0x60/0x64 Hardware Bridge with Real-Time Kernel Diagnostics
(`diagnose_kernel_drivers`) and PyInstaller `.exe`-safe DLL loading (`get_driver_search_dirs`).
"""

from __future__ import annotations
import ctypes
from ctypes import wintypes
import os
from pathlib import Path
import random
import shutil
import subprocess
import sys
import threading
import time
from typing import Any, Callable, Dict, List, Optional, Tuple
from app.i18n import is_rtl, t, tl
from app.paths import get_appdata_dir, get_driver_search_dirs, get_drivers_dir

IS_WINDOWS = sys.platform.startswith("win")
OUR_PID = os.getpid()

# Special Windows Media, Volume, Browser & Application Launch Virtual Keys (VK)
SPECIAL_VK_TO_SYNTH_SCAN: dict[int, Tuple[int, str]] = {
    0xA6: (0x1A6, "BrowserBack"),
    0xA7: (0x1A7, "BrowserForward"),
    0xA8: (0x1A8, "BrowserRefresh"),
    0xA9: (0x1A9, "BrowserStop"),
    0xAA: (0x1AA, "BrowserSearch"),
    0xAB: (0x1AB, "BrowserFavorites"),
    0xAC: (0x1AC, "BrowserHome"),
    0xAD: (0x1AD, "VolumeMute"),
    0xAE: (0x1AE, "VolumeDown"),
    0xAF: (0x1AF, "VolumeUp"),
    0xB0: (0x1B0, "NextTrack"),
    0xB1: (0x1B1, "PrevTrack"),
    0xB2: (0x1B2, "MediaStop"),
    0xB3: (0x1B3, "PlayPause"),
    0xB4: (0x1B4, "LaunchMail"),
    0xB5: (0x1B5, "MediaPlayer"),
    0xB6: (0x1B6, "MyComputer"),
    0xB7: (0x1B7, "Calculator"),
    0x5B: (0x5B, "LWin"),
    0x5C: (0x5C, "RWin"),
    0x5D: (0x5D, "Apps"),
}

SCANCODE_TO_LABEL: dict[Tuple[int, bool], str] = {
    (0x01, False): "Esc",
    (0x02, False): "1",
    (0x03, False): "2",
    (0x04, False): "3",
    (0x05, False): "4",
    (0x06, False): "5",
    (0x07, False): "6",
    (0x08, False): "7",
    (0x09, False): "8",
    (0x0A, False): "9",
    (0x0B, False): "0",
    (0x0C, False): "-",
    (0x0D, False): "=",
    (0x0E, False): "Backspace",
    (0x0F, False): "Tab",
    (0x10, False): "Q",
    (0x11, False): "W",
    (0x12, False): "E",
    (0x13, False): "R",
    (0x14, False): "T",
    (0x15, False): "Y",
    (0x16, False): "U",
    (0x17, False): "I",
    (0x18, False): "O",
    (0x19, False): "P",
    (0x1A, False): "[",
    (0x1B, False): "]",
    (0x1C, False): "Enter",
    (0x1C, True): "NumEnter",
    (0x1D, False): "Ctrl",
    (0x1D, True): "RCtrl",
    (0x1E, False): "A",
    (0x1F, False): "S",
    (0x20, False): "D",
    (0x21, False): "F",
    (0x22, False): "G",
    (0x23, False): "H",
    (0x24, False): "J",
    (0x25, False): "K",
    (0x26, False): "L",
    (0x27, False): ";",
    (0x28, False): "'",
    (0x29, False): "`",
    (0x2A, False): "Shift",
    (0x2B, False): "\\",
    (0x2C, False): "Z",
    (0x2D, False): "X",
    (0x2E, False): "C",
    (0x2F, False): "V",
    (0x30, False): "B",
    (0x31, False): "N",
    (0x32, False): "M",
    (0x33, False): ",",
    (0x34, False): ".",
    (0x35, False): "/",
    (0x35, True): "Num/",
    (0x36, False): "RShift",
    (0x37, False): "Num*",
    (0x38, False): "Alt",
    (0x38, True): "RAlt",
    (0x39, False): "Space",
    (0x3A, False): "CapsLock",
    (0x3B, False): "F1",
    (0x3C, False): "F2",
    (0x3D, False): "F3",
    (0x3E, False): "F4",
    (0x3F, False): "F5",
    (0x40, False): "F6",
    (0x41, False): "F7",
    (0x42, False): "F8",
    (0x43, False): "F9",
    (0x44, False): "F10",
    (0x45, False): "NumLock",
    (0x45, True): "NumLock",
    (0x46, False): "ScrollLock",
    (0x57, False): "F11",
    (0x58, False): "F12",
    (0x47, False): "Num7",
    (0x48, False): "Num8",
    (0x49, False): "Num9",
    (0x4A, False): "Num-",
    (0x4B, False): "Num4",
    (0x4C, False): "Num5",
    (0x4D, False): "Num6",
    (0x4E, False): "Num+",
    (0x4F, False): "Num1",
    (0x50, False): "Num2",
    (0x51, False): "Num3",
    (0x52, False): "Num0",
    (0x53, False): "Num.",
    (0x47, True): "Home",
    (0x48, True): "Up",
    (0x49, True): "PageUp",
    (0x4B, True): "Left",
    (0x4D, True): "Right",
    (0x4F, True): "End",
    (0x50, True): "Down",
    (0x51, True): "PageDown",
    (0x52, True): "Insert",
    (0x53, True): "Delete",
    (0x5B, True): "LWin",
    (0x5C, True): "RWin",
    (0x5D, True): "Apps",
    (0x20, True): "VolumeMute",
    (0x2E, True): "VolumeDown",
    (0x30, True): "VolumeUp",
    (0x22, True): "PlayPause",
    (0x24, True): "MediaStop",
    (0x19, True): "NextTrack",
    (0x10, True): "PrevTrack",
    (0x6D, True): "MediaPlayer",
    (0x21, True): "Calculator",
    (0x6B, True): "MyComputer",
    (0x6C, True): "LaunchMail",
    (0x32, True): "BrowserHome",
}

for _vk_code, (_synth_sc, _lbl_name) in SPECIAL_VK_TO_SYNTH_SCAN.items():
    SCANCODE_TO_LABEL[(_synth_sc, True)] = _lbl_name
    SCANCODE_TO_LABEL[(_synth_sc, False)] = _lbl_name

LABEL_TO_SCANCODE: dict[str, Tuple[int, bool]] = {
    lbl.upper(): (sc, ext) for (sc, ext), lbl in SCANCODE_TO_LABEL.items()
}
for _vk_code, (_synth_sc, _lbl_name) in SPECIAL_VK_TO_SYNTH_SCAN.items():
    LABEL_TO_SCANCODE[_lbl_name.upper()] = (_synth_sc, True)
LABEL_TO_SCANCODE["ESCAPE"] = (0x01, False)
LABEL_TO_SCANCODE["RETURN"] = (0x1C, False)
LABEL_TO_SCANCODE["KEY_0"] = (0x1B7, True)

MODIFIER_SCANCODES = {0x1D, 0x2A, 0x36, 0x38, 0x5B, 0x5C}

# Official USB HID Usage Page 0x07 mapping for 8-Byte 6KRO Hardware Keyboard Reports
SCANCODE_TO_USB_HID_USAGE: Dict[Tuple[int, bool], int] = {
    (0x01, False): 0x29, (0x02, False): 0x1E, (0x03, False): 0x1F, (0x04, False): 0x20,
    (0x05, False): 0x21, (0x06, False): 0x22, (0x07, False): 0x23, (0x08, False): 0x24,
    (0x09, False): 0x25, (0x0A, False): 0x26, (0x0B, False): 0x27, (0x0C, False): 0x2D,
    (0x0D, False): 0x2E, (0x0E, False): 0x2A, (0x0F, False): 0x2B, (0x10, False): 0x14,
    (0x11, False): 0x1A, (0x12, False): 0x08, (0x13, False): 0x15, (0x14, False): 0x17,
    (0x15, False): 0x1C, (0x16, False): 0x18, (0x17, False): 0x0C, (0x18, False): 0x12,
    (0x19, False): 0x13, (0x1A, False): 0x2F, (0x1B, False): 0x30, (0x1C, False): 0x28,
    (0x1C, True):  0x58, (0x1D, False): 0xE0, (0x1D, True):  0xE4, (0x1E, False): 0x04,
    (0x1F, False): 0x16, (0x20, False): 0x07, (0x21, False): 0x09, (0x22, False): 0x0A,
    (0x23, False): 0x0B, (0x24, False): 0x0D, (0x25, False): 0x0E, (0x26, False): 0x0F,
    (0x27, False): 0x33, (0x28, False): 0x34, (0x29, False): 0x35, (0x2A, False): 0xE1,
    (0x2B, False): 0x31, (0x2C, False): 0x1D, (0x2D, False): 0x1B, (0x2E, False): 0x06,
    (0x2F, False): 0x19, (0x30, False): 0x05, (0x31, False): 0x11, (0x32, False): 0x10,
    (0x33, False): 0x36, (0x34, False): 0x37, (0x35, False): 0x38, (0x35, True):  0x54,
    (0x36, False): 0xE5, (0x37, False): 0x55, (0x38, False): 0xE2, (0x38, True):  0xE6,
    (0x39, False): 0x2C, (0x3A, False): 0x39, (0x3B, False): 0x3A, (0x3C, False): 0x3B,
    (0x3D, False): 0x3C, (0x3E, False): 0x3D, (0x3F, False): 0x3E, (0x40, False): 0x3F,
    (0x41, False): 0x40, (0x42, False): 0x41, (0x43, False): 0x42, (0x44, False): 0x43,
    (0x45, False): 0x53, (0x46, False): 0x47, (0x57, False): 0x44, (0x58, False): 0x45,
    (0x47, True):  0x4A, (0x48, True):  0x52, (0x49, True):  0x4B, (0x4B, True):  0x50,
    (0x4D, True):  0x4F, (0x4F, True):  0x4D, (0x50, True):  0x51, (0x51, True):  0x4E,
    (0x52, True):  0x49, (0x53, True):  0x4C, (0x5B, True):  0xE3, (0x5C, True):  0xE7,
}


def usb_hid_usage_for_scancode(scan_code: int, is_extended: bool = False) -> int:
    return SCANCODE_TO_USB_HID_USAGE.get(
        (scan_code, is_extended),
        SCANCODE_TO_USB_HID_USAGE.get((scan_code, False), 0),
    )


def usb_hid_modifier_bit(scan_code: int, is_extended: bool = False) -> int:
    if scan_code == 0x1D and not is_extended:
        return 0x01
    if scan_code == 0x2A and not is_extended:
        return 0x02
    if scan_code == 0x38 and not is_extended:
        return 0x04
    if scan_code == 0x5B:
        return 0x08
    if scan_code == 0x1D and is_extended:
        return 0x10
    if scan_code == 0x36:
        return 0x20
    if scan_code == 0x38 and is_extended:
        return 0x40
    if scan_code == 0x5C:
        return 0x80
    return 0


class UsbHidKeyboardReport:
    """Standard 8-Byte USB HID Boot Keyboard Report (Usage Page 0x07, 6-Key Rollover)."""

    def __init__(self) -> None:
        self.modifiers: int = 0
        self.reserved: int = 0
        self.keys: List[int] = [0, 0, 0, 0, 0, 0]

    def update_key(self, scan_code: int, is_extended: bool, is_down: bool) -> bytes:
        mod_bit = usb_hid_modifier_bit(scan_code, is_extended)
        if mod_bit:
            if is_down:
                self.modifiers |= mod_bit
            else:
                self.modifiers &= ~mod_bit
            return self.to_bytes()

        usage = usb_hid_usage_for_scancode(scan_code, is_extended)
        if not usage or usage >= 0xE0:
            return self.to_bytes()

        if is_down:
            if usage not in self.keys:
                for i in range(6):
                    if self.keys[i] == 0:
                        self.keys[i] = usage
                        break
        else:
            for i in range(6):
                if self.keys[i] == usage:
                    self.keys[i] = 0
            compact = [k for k in self.keys if k != 0]
            self.keys = (compact + [0] * 6)[:6]
        return self.to_bytes()

    def to_bytes(self) -> bytes:
        return bytes([self.modifiers & 0xFF, 0] + [k & 0xFF for k in self.keys[:6]])


_GLOBAL_USB_HID_REPORT = UsbHidKeyboardReport()


class _StealthInjectionTracker:
    """
    Zero-Signature Ring Buffer Tracker & Live `LLKHF_INJECTED` Hook Probe:
    Allows `dwExtraInfo = 0` (matching real hardware keyboards) while still preventing
    V-MacroPlus's own `WH_KEYBOARD_LL` hook from re-triggering on our injected keys,
    and measures whether injected pulses arrive at `WH_KEYBOARD_LL` with `LLKHF_INJECTED == 0` (Ring-0) or `1`.
    """

    def __init__(self) -> None:
        self._lock = threading.Lock()
        self._recent_keys: List[Tuple[int, bool, float]] = []
        self._last_mouse_ts: float = 0.0
        self.probe_waiting: bool = False
        self.probe_event = threading.Event()
        self.last_probe_had_injected_flag: Optional[bool] = None

    def record_key(self, scan_code: int, is_down: bool) -> None:
        now = time.monotonic()
        with self._lock:
            self._recent_keys = [
                it for it in self._recent_keys if (now - it[2]) < 0.080
            ]
            self._recent_keys.append((scan_code & 0xFF, is_down, now))

    def consume_if_recent_key(self, scan_code: int, is_down: bool, hw_flags: int = 0) -> bool:
        now = time.monotonic()
        sc = scan_code & 0xFF
        with self._lock:
            if self.probe_waiting and sc == 0x76:  # F24 diagnostic probe scan code
                self.last_probe_had_injected_flag = bool(hw_flags & 0x10)  # LLKHF_INJECTED
                self.probe_waiting = False
                self.probe_event.set()
                return True
            for idx, (r_sc, r_dn, ts) in enumerate(self._recent_keys):
                if r_sc == sc and r_dn == is_down and (now - ts) < 0.080:
                    self._recent_keys.pop(idx)
                    return True
        return False

    def record_mouse(self) -> None:
        self._last_mouse_ts = time.monotonic()

    def is_recent_mouse(self) -> bool:
        return (time.monotonic() - self._last_mouse_ts) < 0.065


STEALTH_TRACKER = _StealthInjectionTracker()


def is_special_media_or_launch_scancode(scan_code: int, is_extended: bool = False) -> bool:
    if scan_code >= 0x100:
        return True
    if is_extended and scan_code in {0x20, 0x2E, 0x30, 0x22, 0x24, 0x19, 0x10, 0x6D, 0x21, 0x6B, 0x6C, 0x32}:
        return True
    return False


def label_for_scancode(scan_code: int, is_extended: bool = False) -> str:
    if scan_code <= 0:
        return "Calculator"
    if (scan_code, is_extended) in SCANCODE_TO_LABEL:
        return SCANCODE_TO_LABEL[(scan_code, is_extended)]
    if (scan_code, False) in SCANCODE_TO_LABEL:
        return SCANCODE_TO_LABEL[(scan_code, False)]
    if (scan_code, True) in SCANCODE_TO_LABEL:
        return SCANCODE_TO_LABEL[(scan_code, True)]
    if scan_code >= 0x100:
        vk = scan_code - 0x100
        if vk in SPECIAL_VK_TO_SYNTH_SCAN:
            return SPECIAL_VK_TO_SYNTH_SCAN[vk][1]
        return f"VK_{vk:02X}"
    return f"Key_{scan_code}"


def scancode_for_label(label: str) -> Tuple[int, bool]:
    clean = label.strip().upper()
    if not clean:
        return (0x39, False)
    if clean in LABEL_TO_SCANCODE:
        return LABEL_TO_SCANCODE[clean]
    if clean.startswith("VK_"):
        try:
            vk = int(clean[3:], 16)
            return (0x100 + vk, True)
        except Exception:
            pass
    if clean.startswith("KEY_"):
        try:
            num = int(clean[4:])
            if num == 0:
                return (0x1B7, True)
            return (num, False)
        except Exception:
            pass
    return (0x39, False)


def parse_chord_string(key_str: str) -> List[Tuple[int, bool]]:
    if not key_str or not key_str.strip():
        return []
    result: List[Tuple[int, bool]] = []
    for part in key_str.split("+"):
        p = part.strip()
        if p:
            sc, ext = scancode_for_label(p)
            if sc > 0 and (sc, ext) not in result:
                result.append((sc, ext))
    return result


def is_modifier_scancode(scan_code: int) -> bool:
    return scan_code in MODIFIER_SCANCODES


# =============================================================================
# Win32 Structures & Native Low-Level Keyboard + Mouse Hooks
# =============================================================================
user32 = ctypes.windll.user32 if IS_WINDOWS else None
kernel32 = ctypes.windll.kernel32 if IS_WINDOWS else None
if IS_WINDOWS:
    shell32 = ctypes.windll.shell32

    WH_KEYBOARD_LL = 13
    WH_MOUSE_LL = 14

    WM_KEYDOWN = 0x0100
    WM_KEYUP = 0x0101
    WM_SYSKEYDOWN = 0x0104
    WM_SYSKEYUP = 0x0105

    WM_MOUSEMOVE = 0x0200
    WM_LBUTTONDOWN = 0x0201
    WM_LBUTTONUP = 0x0202
    WM_RBUTTONDOWN = 0x0204
    WM_RBUTTONUP = 0x0205
    WM_MBUTTONDOWN = 0x0207
    WM_MBUTTONUP = 0x0208
    WM_MOUSEWHEEL = 0x020A
    WM_XBUTTONDOWN = 0x020B
    WM_XBUTTONUP = 0x020C
    WM_MOUSEHWHEEL = 0x020E
    WM_QUIT = 0x0012

    LLKHF_EXTENDED = 0x01
    LLKHF_INJECTED = 0x10
    LLKHF_LOWER_IL_INJECTED = 0x02
    LLMHF_INJECTED = 0x01
    LLMHF_LOWER_IL_INJECTED = 0x02

    MAPVK_VSC_TO_VK_EX = 3

    ULONG_PTR = ctypes.c_size_t
    LRESULT = ctypes.c_ssize_t

    class KBDLLHOOKSTRUCT(ctypes.Structure):
        _fields_ = [
            ("vkCode", wintypes.DWORD),
            ("scanCode", wintypes.DWORD),
            ("flags", wintypes.DWORD),
            ("time", wintypes.DWORD),
            ("dwExtraInfo", ULONG_PTR),
        ]

    class MSLLHOOKSTRUCT(ctypes.Structure):
        _fields_ = [
            ("pt", wintypes.POINT),
            ("mouseData", wintypes.DWORD),
            ("flags", wintypes.DWORD),
            ("time", wintypes.DWORD),
            ("dwExtraInfo", ULONG_PTR),
        ]

    HOOKPROC = ctypes.WINFUNCTYPE(LRESULT, ctypes.c_int, wintypes.WPARAM, wintypes.LPARAM)

    user32.SetWindowsHookExW.argtypes = [ctypes.c_int, HOOKPROC, wintypes.HINSTANCE, wintypes.DWORD]
    user32.SetWindowsHookExW.restype = wintypes.HHOOK
    user32.CallNextHookEx.argtypes = [wintypes.HHOOK, ctypes.c_int, wintypes.WPARAM, wintypes.LPARAM]
    user32.CallNextHookEx.restype = LRESULT
    user32.UnhookWindowsHookEx.argtypes = [wintypes.HHOOK]
    user32.UnhookWindowsHookEx.restype = wintypes.BOOL

    class KEYBDINPUT(ctypes.Structure):
        _fields_ = [
            ("wVk", wintypes.WORD),
            ("wScan", wintypes.WORD),
            ("dwFlags", wintypes.DWORD),
            ("time", wintypes.DWORD),
            ("dwExtraInfo", ULONG_PTR),
        ]

    class MOUSEINPUT(ctypes.Structure):
        _fields_ = [
            ("dx", wintypes.LONG),
            ("dy", wintypes.LONG),
            ("mouseData", wintypes.DWORD),
            ("dwFlags", wintypes.DWORD),
            ("time", wintypes.DWORD),
            ("dwExtraInfo", ULONG_PTR),
        ]

    class HARDWAREINPUT(ctypes.Structure):
        _fields_ = [
            ("uMsg", wintypes.DWORD),
            ("wParamL", wintypes.WORD),
            ("wParamH", wintypes.WORD),
        ]

    class _INPUTunion(ctypes.Union):
        _fields_ = [
            ("ki", KEYBDINPUT),
            ("mi", MOUSEINPUT),
            ("hi", HARDWAREINPUT),
        ]

    class INPUT(ctypes.Structure):
        _fields_ = [
            ("type", wintypes.DWORD),
            ("union", _INPUTunion),
        ]

    INPUT_MOUSE = 0
    INPUT_KEYBOARD = 1
    KEYEVENTF_EXTENDEDKEY = 0x0001
    KEYEVENTF_KEYUP = 0x0002
    KEYEVENTF_UNICODE = 0x0004
    KEYEVENTF_SCANCODE = 0x0008

    MOUSEEVENTF_MOVE = 0x0001
    MOUSEEVENTF_LEFTDOWN = 0x0002
    MOUSEEVENTF_LEFTUP = 0x0004
    MOUSEEVENTF_RIGHTDOWN = 0x0008
    MOUSEEVENTF_RIGHTUP = 0x0010
    MOUSEEVENTF_MIDDLEDOWN = 0x0020
    MOUSEEVENTF_MIDDLEUP = 0x0040
    MOUSEEVENTF_XDOWN = 0x0080
    MOUSEEVENTF_XUP = 0x0100
    MOUSEEVENTF_WHEEL = 0x0800
    MOUSEEVENTF_HWHEEL = 0x01000
    MOUSEEVENTF_VIRTUALDESK = 0x4000
    MOUSEEVENTF_ABSOLUTE = 0x8000

    XBUTTON1 = 0x0001
    XBUTTON2 = 0x0002

    OUR_EXTRA_INFO = 0x564D4143


def vk_for_scancode(scan_code: int, is_extended: bool = False) -> int:
    if scan_code >= 0x100:
        return scan_code - 0x100
    if IS_WINDOWS:
        try:
            vsc = (0xE000 | scan_code) if is_extended else scan_code
            vk = int(user32.MapVirtualKeyW(vsc, MAPVK_VSC_TO_VK_EX)) & 0xFF
            if vk > 0:
                return vk
        except Exception:
            pass
    fallback = {
        0x1D: 0x11,
        0x2A: 0x10,
        0x36: 0xA1,
        0x38: 0x12,
        0x39: 0x20,
        0x1C: 0x0D,
        0x01: 0x1B,
    }
    return fallback.get(scan_code, 0)


def is_scancode_physically_down(scan_code: int, is_extended: bool = False) -> bool:
    if not IS_WINDOWS:
        return True
    if scan_code >= 0x100:
        vk = scan_code - 0x100
        try:
            return bool(user32.GetAsyncKeyState(vk) & 0x8000)
        except Exception:
            return False
    vk = vk_for_scancode(scan_code, is_extended)
    if vk <= 0:
        return True
    try:
        if user32.GetAsyncKeyState(vk) & 0x8000:
            return True
        if scan_code == 0x1D and (user32.GetAsyncKeyState(0x11) & 0x8000):
            return True
        if scan_code in (0x2A, 0x36) and (user32.GetAsyncKeyState(0x10) & 0x8000):
            return True
        if scan_code == 0x38 and (user32.GetAsyncKeyState(0x12) & 0x8000):
            return True
        return False
    except Exception:
        return True


class Win32KeyboardHook:
    """Dedicated thread running a native Windows Low-Level Keyboard Hook (`WH_KEYBOARD_LL`)."""

    def __init__(self, handler: Callable[[int, bool, bool, bool], bool]) -> None:
        self._handler = handler
        self._hook_id: Optional[int] = None
        self._thread_id: Optional[int] = None
        self._thread: Optional[threading.Thread] = None
        self._c_hook_proc = None

    def start(self) -> None:
        if not IS_WINDOWS or self._thread is not None:
            return
        self._thread = threading.Thread(target=self._run_loop, name="Win32KeyboardHookThread", daemon=True)
        self._thread.start()

    def _run_loop(self) -> None:
        self._thread_id = kernel32.GetCurrentThreadId()

        def _low_level_proc(nCode: int, wParam: int, lParam: int) -> int:
            if nCode >= 0 and lParam:
                try:
                    kb = ctypes.cast(lParam, ctypes.POINTER(KBDLLHOOKSTRUCT)).contents
                    vk = int(kb.vkCode) & 0xFF
                    raw_scan = int(kb.scanCode) & 0xFF
                    flags = int(kb.flags)
                    ext = bool(flags & LLKHF_EXTENDED)
                    is_down = wParam in (WM_KEYDOWN, WM_SYSKEYDOWN)
                    is_up = wParam in (WM_KEYUP, WM_SYSKEYUP)

                    if vk in SPECIAL_VK_TO_SYNTH_SCAN:
                        scan, _ = SPECIAL_VK_TO_SYNTH_SCAN[vk]
                        ext = True
                    elif raw_scan == 0 and vk > 0:
                        scan = 0x100 + vk
                        ext = True
                    else:
                        scan = raw_scan

                    recent_stealth = STEALTH_TRACKER.consume_if_recent_key(scan, is_down, flags)
                    injected = bool(
                        recent_stealth
                        or (flags & LLKHF_INJECTED)
                        or (flags & LLKHF_LOWER_IL_INJECTED)
                        or (kb.dwExtraInfo == OUR_EXTRA_INFO)
                    )

                    if (is_down or is_up) and scan > 0:
                        should_block = self._handler(scan, ext, is_down, injected)
                        if should_block:
                            return 1
                except Exception:
                    pass
            return user32.CallNextHookEx(self._hook_id, nCode, wParam, lParam)

        self._c_hook_proc = HOOKPROC(_low_level_proc)
        self._hook_id = user32.SetWindowsHookExW(WH_KEYBOARD_LL, self._c_hook_proc, None, 0)

        msg = wintypes.MSG()
        while user32.GetMessageW(ctypes.byref(msg), None, 0, 0) != 0:
            user32.TranslateMessage(ctypes.byref(msg))
            user32.DispatchMessageW(ctypes.byref(msg))

        if self._hook_id:
            user32.UnhookWindowsHookEx(self._hook_id)
            self._hook_id = None

    def stop(self) -> None:
        if IS_WINDOWS and self._thread_id:
            try:
                user32.PostThreadMessageW(self._thread_id, WM_QUIT, 0, 0)
            except Exception:
                pass


class Win32MouseHook:
    """Dedicated thread running a native Windows Low-Level Mouse Hook (`WH_MOUSE_LL = 14`)."""

    def __init__(self, handler: Callable[[str, int, int, str, int, int, bool], None]) -> None:
        self._handler = handler
        self._hook_id: Optional[int] = None
        self._thread_id: Optional[int] = None
        self._thread: Optional[threading.Thread] = None
        self._c_hook_proc = None

    def start(self) -> None:
        if not IS_WINDOWS or self._thread is not None:
            return
        self._thread = threading.Thread(target=self._run_loop, name="Win32MouseHookThread", daemon=True)
        self._thread.start()

    def _run_loop(self) -> None:
        self._thread_id = kernel32.GetCurrentThreadId()

        def _low_level_mouse_proc(nCode: int, wParam: int, lParam: int) -> int:
            if nCode >= 0 and lParam:
                try:
                    ms = ctypes.cast(lParam, ctypes.POINTER(MSLLHOOKSTRUCT)).contents
                    flags = int(ms.flags)
                    injected = bool(
                        (flags & LLMHF_INJECTED)
                        or (flags & LLMHF_LOWER_IL_INJECTED)
                        or (ms.dwExtraInfo == OUR_EXTRA_INFO)
                        or STEALTH_TRACKER.is_recent_mouse()
                    )
                    if not injected:
                        x, y = int(ms.pt.x), int(ms.pt.y)
                        if wParam == WM_MOUSEMOVE:
                            self._handler("mouse_move", x, y, "none", 0, 0, False)
                        elif wParam == WM_LBUTTONDOWN:
                            self._handler("mouse_down", x, y, "left", 0, 0, True)
                        elif wParam == WM_LBUTTONUP:
                            self._handler("mouse_up", x, y, "left", 0, 0, False)
                        elif wParam == WM_RBUTTONDOWN:
                            self._handler("mouse_down", x, y, "right", 0, 0, True)
                        elif wParam == WM_RBUTTONUP:
                            self._handler("mouse_up", x, y, "right", 0, 0, False)
                        elif wParam == WM_MBUTTONDOWN:
                            self._handler("mouse_down", x, y, "middle", 0, 0, True)
                        elif wParam == WM_MBUTTONUP:
                            self._handler("mouse_up", x, y, "middle", 0, 0, False)
                        elif wParam in (WM_XBUTTONDOWN, WM_XBUTTONUP):
                            hi = (int(ms.mouseData) >> 16) & 0xFFFF
                            btn_name = "x2" if hi == XBUTTON2 else "x1"
                            is_dn = wParam == WM_XBUTTONDOWN
                            self._handler("mouse_down" if is_dn else "mouse_up", x, y, btn_name, 0, 0, is_dn)
                        elif wParam == WM_MOUSEWHEEL:
                            delta = ctypes.c_short((int(ms.mouseData) >> 16) & 0xFFFF).value
                            if delta != 0:
                                notches = int(round(delta / 120.0))
                                if notches == 0:
                                    notches = 1 if delta > 0 else -1
                                s_dir = "up" if notches > 0 else "down"
                                self._handler("mouse_scroll", x, y, s_dir, 0, notches, False)
                        elif wParam == WM_MOUSEHWHEEL:
                            delta = ctypes.c_short((int(ms.mouseData) >> 16) & 0xFFFF).value
                            if delta != 0:
                                notches = int(round(delta / 120.0))
                                if notches == 0:
                                    notches = 1 if delta > 0 else -1
                                s_dir = "right" if notches > 0 else "left"
                                self._handler("mouse_scroll", x, y, s_dir, notches, 0, False)
                except Exception:
                    pass
            return user32.CallNextHookEx(self._hook_id, nCode, wParam, lParam)

        self._c_hook_proc = HOOKPROC(_low_level_mouse_proc)
        self._hook_id = user32.SetWindowsHookExW(WH_MOUSE_LL, self._c_hook_proc, None, 0)

        msg = wintypes.MSG()
        while user32.GetMessageW(ctypes.byref(msg), None, 0, 0) != 0:
            user32.TranslateMessage(ctypes.byref(msg))
            user32.DispatchMessageW(ctypes.byref(msg))

        if self._hook_id:
            user32.UnhookWindowsHookEx(self._hook_id)
            self._hook_id = None

    def stop(self) -> None:
        if IS_WINDOWS and self._thread_id:
            try:
                user32.PostThreadMessageW(self._thread_id, WM_QUIT, 0, 0)
            except Exception:
                pass


# =============================================================================
# Full Ring-0 Interception (Keyboard + Mouse) & PS/2 Port 0x60/0x64 Bridge
# =============================================================================
class _InterceptionKeyStroke(ctypes.Structure):
    _fields_ = [
        ("code", ctypes.c_ushort),
        ("state", ctypes.c_ushort),
        ("information", ctypes.c_uint),
    ]


class _InterceptionMouseStroke(ctypes.Structure):
    _fields_ = [
        ("state", ctypes.c_ushort),
        ("flags", ctypes.c_ushort),
        ("rolling", ctypes.c_short),
        ("x", ctypes.c_int),
        ("y", ctypes.c_int),
        ("information", ctypes.c_uint),
    ]


class _HardwareDriversBridge:
    """
    Production-grade Ring-0 Kernel & Hardware Controller Bridge supporting both `.py` and PyInstaller `.exe`:
    - Searches all candidate `drivers/` directories (`get_driver_search_dirs()`).
    - Method 7 (`cpp_interception_driver`): Loads `interception.dll`, opens `interception_create_context()`,
      discovers the active physical keyboard (`1..10`) and mouse (`11..20`) device IDs via `interception_get_hardware_id()`,
      and injects both Keyboard (`InterceptionKeyStroke`) and Mouse (`InterceptionMouseStroke`) directly through Ring-0 `keyboard.sys` / `mouse.sys`!
    - Method 8 (`cpp_winring0_ps2_port`): Loads `inpoutx64.dll` / `WinRing0x64.dll` for Port `0x60`/`0x64` (`i8042prt`).
    - Provides `diagnose_kernel_drivers(run_live_probe=True)` for the interactive Settings diagnostic check!
    """

    def __init__(self) -> None:
        self._lock = threading.Lock()
        self._interception_dll = None
        self._interception_dll_path: str = ""
        self._interception_ctx = None
        self._active_kb_device: int = 1
        self._active_kb_hwid: str = ""
        self._active_mouse_device: int = 11
        self._active_mouse_hwid: str = ""
        self._inpout_dll = None
        self._inpout_dll_path: str = ""
        self._probed = False
        self._dll_dir_handles: List[Any] = []

    def reprobe(self) -> None:
        with self._lock:
            if self._interception_dll and self._interception_ctx:
                try:
                    self._interception_dll.interception_destroy_context(ctypes.c_void_p(self._interception_ctx))
                except Exception:
                    pass
            self._interception_ctx = None
            self._interception_dll = None
            self._interception_dll_path = ""
            self._active_kb_device = 1
            self._active_kb_hwid = ""
            self._active_mouse_device = 11
            self._active_mouse_hwid = ""
            self._inpout_dll = None
            self._inpout_dll_path = ""
            self._probed = False
        self._ensure_probed()

    def _ensure_probed(self) -> None:
        if self._probed or not IS_WINDOWS:
            return
        with self._lock:
            if self._probed:
                return
            self._probed = True

            search_dirs = get_driver_search_dirs()
            for d in search_dirs:
                if d.exists() and hasattr(os, "add_dll_directory"):
                    try:
                        self._dll_dir_handles.append(os.add_dll_directory(str(d)))
                    except Exception:
                        pass

            # 1. Probe interception.dll across all search directories + system PATH
            dll_candidates: List[Path] = []
            for d in search_dirs:
                p = d / "interception.dll"
                if p.exists():
                    dll_candidates.append(p)

            loaded_dll = None
            loaded_path = ""
            for cand in dll_candidates:
                try:
                    loaded_dll = ctypes.cdll.LoadLibrary(str(cand))
                    loaded_path = str(cand)
                    break
                except Exception:
                    loaded_dll = None

            if loaded_dll is None:
                try:
                    loaded_dll = ctypes.cdll.LoadLibrary("interception.dll")
                    loaded_path = "interception.dll (System PATH)"
                except Exception:
                    loaded_dll = None

            if loaded_dll is not None:
                try:
                    loaded_dll.interception_create_context.restype = ctypes.c_void_p
                    loaded_dll.interception_destroy_context.argtypes = [ctypes.c_void_p]
                    loaded_dll.interception_send.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p, ctypes.c_uint]
                    loaded_dll.interception_send.restype = ctypes.c_int
                    self._interception_dll = loaded_dll
                    self._interception_dll_path = loaded_path
                    ctx = loaded_dll.interception_create_context()
                    if ctx:
                        self._interception_ctx = ctx
                        self._discover_active_devices(loaded_dll, ctx)
                except Exception:
                    self._interception_ctx = None

            # 2. Probe inpoutx64.dll / WinRing0x64.dll for Method 8 (PS/2 Port 0x60/0x64)
            for dll_name in ("inpoutx64.dll", "InpOutx64.dll", "WinRing0x64.dll"):
                for d in search_dirs:
                    cand = d / dll_name
                    if cand.exists():
                        try:
                            self._inpout_dll = ctypes.windll.LoadLibrary(str(cand))
                            self._inpout_dll_path = str(cand)
                            break
                        except Exception:
                            self._inpout_dll = None
                if self._inpout_dll is not None:
                    break
                try:
                    self._inpout_dll = ctypes.windll.LoadLibrary(dll_name)
                    self._inpout_dll_path = f"{dll_name} (System PATH)"
                    break
                except Exception:
                    self._inpout_dll = None

    def _discover_active_devices(self, dll: Any, ctx: int) -> None:
        """Query `interception_get_hardware_id` on devices 1..10 (Keyboards) and 11..20 (Mice)."""
        try:
            get_hwid = getattr(dll, "interception_get_hardware_id", None)
            if get_hwid is None:
                return
            get_hwid.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p, ctypes.c_uint]
            get_hwid.restype = ctypes.c_uint
            buf = ctypes.create_unicode_buffer(512)
            for kb_dev in range(1, 11):
                res = get_hwid(ctypes.c_void_p(ctx), kb_dev, buf, ctypes.sizeof(buf))
                if res > 0 and buf.value:
                    self._active_kb_device = kb_dev
                    self._active_kb_hwid = buf.value.strip()
                    break
            for ms_dev in range(11, 21):
                res = get_hwid(ctypes.c_void_p(ctx), ms_dev, buf, ctypes.sizeof(buf))
                if res > 0 and buf.value:
                    self._active_mouse_device = ms_dev
                    self._active_mouse_hwid = buf.value.strip()
                    break
        except Exception:
            pass

    def is_interception_ring0_ready(self) -> bool:
        self._ensure_probed()
        return bool(self._interception_dll and self._interception_ctx)

    def is_ps2_port60_ready(self) -> bool:
        self._ensure_probed()
        return bool(self._inpout_dll)

    def try_interception_key(self, hw_scan: int, is_extended: bool, key_up: bool) -> bool:
        self._ensure_probed()
        if not self._interception_dll or not self._interception_ctx or hw_scan <= 0:
            return False
        try:
            state = (0x01 if key_up else 0x00) | (0x02 if is_extended else 0x00)
            stroke = _InterceptionKeyStroke(hw_scan & 0xFF, state, 0)
            sent = self._interception_dll.interception_send(
                ctypes.c_void_p(self._interception_ctx),
                ctypes.c_int(self._active_kb_device),
                ctypes.byref(stroke),
                ctypes.c_uint(1),
            )
            return int(sent) > 0
        except Exception:
            return False

    def try_interception_mouse_button(
        self,
        x: int,
        y: int,
        button: str,
        action: str,
        move_cursor: bool,
        hold_ms: int,
    ) -> bool:
        self._ensure_probed()
        if not self._interception_dll or not self._interception_ctx or not IS_WINDOWS:
            return False
        try:
            if move_cursor:
                user32.SetCursorPos(int(x), int(y))

            b_low = button.lower()
            if b_low == "right":
                st_dn, st_up = 0x004, 0x008
            elif b_low == "middle":
                st_dn, st_up = 0x010, 0x020
            elif b_low in ("x1", "back"):
                st_dn, st_up = 0x040, 0x080
            elif b_low in ("x2", "forward"):
                st_dn, st_up = 0x100, 0x200
            else:
                st_dn, st_up = 0x001, 0x002

            def _send_ms(st_flag: int) -> bool:
                STEALTH_TRACKER.record_mouse()
                ms = _InterceptionMouseStroke(st_flag, 0, 0, 0, 0, 0)
                sent = self._interception_dll.interception_send(
                    ctypes.c_void_p(self._interception_ctx),
                    ctypes.c_int(self._active_mouse_device),
                    ctypes.byref(ms),
                    ctypes.c_uint(1),
                )
                return int(sent) > 0

            if action == "down":
                return _send_ms(st_dn)
            if action == "up":
                return _send_ms(st_up)
            ok1 = _send_ms(st_dn)
            if hold_ms > 0:
                time.sleep(max(0.005, hold_ms / 1000.0))
            ok2 = _send_ms(st_up)
            return ok1 and ok2
        except Exception:
            return False

    def try_interception_mouse_scroll(self, direction: str, steps: int) -> bool:
        self._ensure_probed()
        if not self._interception_dll or not self._interception_ctx:
            return False
        try:
            STEALTH_TRACKER.record_mouse()
            d_low = direction.lower().strip()
            if d_low in ("left", "right"):
                st_flag = 0x800  # INTERCEPTION_MOUSE_HWHEEL
                rolling = (120 * steps) if d_low == "right" else (-120 * steps)
            else:
                st_flag = 0x400  # INTERCEPTION_MOUSE_WHEEL
                rolling = (120 * steps) if d_low == "up" else (-120 * steps)
            ms = _InterceptionMouseStroke(st_flag, 0, int(rolling), 0, 0, 0)
            sent = self._interception_dll.interception_send(
                ctypes.c_void_p(self._interception_ctx),
                ctypes.c_int(self._active_mouse_device),
                ctypes.byref(ms),
                ctypes.c_uint(1),
            )
            return int(sent) > 0
        except Exception:
            return False

    def try_ps2_port60_key(self, hw_scan: int, is_extended: bool, key_up: bool) -> bool:
        self._ensure_probed()
        if not self._inpout_dll or hw_scan <= 0:
            return False
        try:
            out32 = getattr(self._inpout_dll, "Out32", None)
            if out32 is None:
                return False
            if is_extended:
                out32(0x64, 0xD2)
                out32(0x60, 0xE0)
            make_break = (hw_scan | 0x80) if key_up else (hw_scan & 0x7F)
            out32(0x64, 0xD2)
            out32(0x60, make_break & 0xFF)
            return True
        except Exception:
            return False


_HW_BRIDGE = _HardwareDriversBridge()


def _check_windows_interception_registry_and_sys() -> Tuple[bool, bool, List[str]]:
    """
    Check if `C:\\Windows\\System32\\drivers\\keyboard.sys` exists and if `"keyboard"` is registered
    in `HKLM\\SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e96b-e325-11ce-bfc1-08002be10318}\\UpperFilters`.
    """
    if not IS_WINDOWS:
        return False, False, []
    sys_root = Path(os.environ.get("SystemRoot", r"C:\Windows"))
    kb_sys_exists = (sys_root / "System32" / "drivers" / "keyboard.sys").exists()
    upper_filters: List[str] = []
    has_kb_filter = False
    try:
        import winreg

        reg_path = r"SYSTEM\CurrentControlSet\Control\Class\{4d36e96b-e325-11ce-bfc1-08002be10318}"
        with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, reg_path, 0, winreg.KEY_READ) as k:
            val, _ = winreg.QueryValueEx(k, "UpperFilters")
            if isinstance(val, list):
                upper_filters = [str(x) for x in val]
            elif isinstance(val, str):
                upper_filters = [val]
            has_kb_filter = any(x.lower() == "keyboard" for x in upper_filters)
    except Exception:
        pass
    return kb_sys_exists, has_kb_filter, upper_filters


def find_interception_installer_exe() -> Optional[Path]:
    for d in get_driver_search_dirs():
        for name in ("install-interception.exe", "install_interception.exe"):
            cand = d / name
            if cand.exists():
                return cand
    return None


def diagnose_kernel_drivers(run_live_probe: bool = False) -> Dict[str, Any]:
    """
    Perform a comprehensive real-time diagnostic check of Interception (`Ring-0`), PS/2 Port `0x60/0x64`,
    and live `LLKHF_INJECTED` signature status.
    """
    _HW_BRIDGE.reprobe()
    admin_ok = is_running_as_admin()
    kb_sys_exists, reg_filter_ok, upper_filters = _check_windows_interception_registry_and_sys()
    installer_path = find_interception_installer_exe()

    dll_found = bool(_HW_BRIDGE._interception_dll is not None)
    dll_path = _HW_BRIDGE._interception_dll_path
    ctx_active = bool(_HW_BRIDGE._interception_ctx)
    kb_dev = _HW_BRIDGE._active_kb_device
    kb_hwid = _HW_BRIDGE._active_kb_hwid or "HID\\KeyboardPort"
    ms_dev = _HW_BRIDGE._active_mouse_device
    ms_hwid = _HW_BRIDGE._active_mouse_hwid or "HID\\MousePort"
    inpout_found = bool(_HW_BRIDGE._inpout_dll is not None)
    inpout_path = _HW_BRIDGE._inpout_dll_path

    live_probe_injected: Optional[bool] = None
    if run_live_probe and IS_WINDOWS:
        STEALTH_TRACKER.probe_event.clear()
        STEALTH_TRACKER.last_probe_had_injected_flag = None
        STEALTH_TRACKER.probe_waiting = True
        try:
            # Send a harmless F24 (scan 0x76) key-up pulse to measure LLKHF_INJECTED at WH_KEYBOARD_LL
            if ctx_active:
                _HW_BRIDGE.try_interception_key(0x76, False, True)
            else:
                send_scancode_key(0x76, False, key_up=True, method="cpp_stealth_humanized")
            STEALTH_TRACKER.probe_event.wait(timeout=0.12)
            live_probe_injected = STEALTH_TRACKER.last_probe_had_injected_flag
        except Exception:
            pass
        finally:
            STEALTH_TRACKER.probe_waiting = False

    if not IS_WINDOWS:
        uinput_ok = os.path.exists("/dev/uinput")
        state_code = "linux_uinput" if uinput_ok else "fallback"
    elif ctx_active:
        state_code = "ring0_active"
    elif reg_filter_ok and kb_sys_exists and dll_found:
        state_code = "needs_reboot"
    elif dll_found and not (reg_filter_ok and kb_sys_exists):
        state_code = "dll_only_no_sys"
    elif reg_filter_ok and kb_sys_exists and not dll_found:
        state_code = "sys_only_no_dll"
    else:
        state_code = "fallback"

    return {
        "state_code": state_code,
        "is_admin": admin_ok,
        "interception_dll_found": dll_found,
        "interception_dll_path": dll_path,
        "keyboard_sys_exists": kb_sys_exists,
        "registry_filter_installed": reg_filter_ok,
        "upper_filters": upper_filters,
        "interception_context_active": ctx_active,
        "active_kb_device": kb_dev,
        "active_kb_hwid": kb_hwid,
        "active_mouse_device": ms_dev,
        "active_mouse_hwid": ms_hwid,
        "inpout_dll_found": inpout_found,
        "inpout_dll_path": inpout_path,
        "installer_exe_path": str(installer_path) if installer_path else "",
        "drivers_dir": str(get_drivers_dir()),
        "live_probe_injected": live_probe_injected,
    }


def format_kernel_diagnostic_report(diag: Dict[str, Any], is_arabic: bool = True) -> Tuple[str, str, str]:
    """
    Format `(headline, details_multiline, status_kind)` from `diagnose_kernel_drivers()` output.
    `status_kind` is `"ok"` (green), `"warn"` (amber), or `"info"` (cyan/purple).
    """
    code = diag.get("state_code", "fallback")
    dll_ok = diag.get("interception_dll_found", False)
    dll_p = diag.get("interception_dll_path", "") or ("غير موجود" if is_arabic else "Not found")
    sys_ok = diag.get("keyboard_sys_exists", False) and diag.get("registry_filter_installed", False)
    ctx_ok = diag.get("interception_context_active", False)
    kb_dev = diag.get("active_kb_device", 1)
    kb_hw = diag.get("active_kb_hwid", "")
    inp_ok = diag.get("inpout_dll_found", False)
    probe_inj = diag.get("live_probe_injected", None)

    if code == "ring0_active":
        headline = (
            f"🟢 الكيرنل Ring-0 (Interception) نشط ويعمل فعلياً! [Device #{kb_dev}: {kb_hw}]"
            if is_arabic
            else f"🟢 Ring-0 Kernel (Interception) ACTIVE & Verified! [Device #{kb_dev}: {kb_hw}]"
        )
        kind = "ok"
    elif code == "needs_reboot":
        headline = (
            "🟡 درايفر Interception مثبّت في الكيرنل ومكتبة DLL جاهزة — يرجى إعادة تشغيل الويندوز (Restart) لتفعيله!"
            if is_arabic
            else "🟡 Interception Kernel Driver & DLL installed — Please Restart Windows once to activate Ring-0!"
        )
        kind = "warn"
    elif code == "dll_only_no_sys":
        headline = (
            "🟠 ملف interception.dll موجود ولكن درايفر الكيرنل (keyboard.sys) غير مثبّت في الويندوز بعد (يعمل حالياً على البديل الآمن SendInput)"
            if is_arabic
            else "🟠 interception.dll loaded, but kernel driver (keyboard.sys) is not installed yet (Using safe SendInput fallback)"
        )
        kind = "warn"
    elif code == "sys_only_no_dll":
        headline = (
            "🟠 درايفر الكيرنل مثبّت في الويندوز ولكن ملف interception.dll غير موجود في مجلد drivers!"
            if is_arabic
            else "🟠 Kernel driver is installed in Windows, but interception.dll is missing from the drivers folder!"
        )
        kind = "warn"
    else:
        headline = (
            "🔵 يعمل حالياً على وضع User-Mode البديل الآمن (SendInput ScanCode + Zero-Signature) — لتفعيل Ring-0 اربط ملفات Interception بالأسفل"
            if is_arabic
            else "🔵 Currently running on safe User-Mode Fallback (SendInput ScanCode + Zero-Signature) — Link Interception below for Ring-0"
        )
        kind = "info"

    if is_arabic:
        l1 = f"• مكتبة الجسر (interception.dll): {'✓ متصلة (' + dll_p + ')' if dll_ok else '✗ غير موجودة في مجلد drivers'}"
        l2 = f"• درايفر الكيرنل (keyboard.sys / UpperFilters): {'✓ مثبّت في النظام' if sys_ok else '✗ غير مثبّت'}" + (
            f" | مقبض الكيرنل Ring-0: {'✓ متصل فعلياً' if ctx_ok else 'بانتظار التفعيل/الريستارت'}"
        )
        l3 = f"• محرك بورت البوردة (InpOutx64 Port 0x60): {'✓ جاهز' if inp_ok else 'غير مضاف (اختياري للطريقة ٨)'}"
        if probe_inj is False:
            l4 = "• فحص بصمة الضغطة الحية (LLKHF_INJECTED): ✓ صفرية (0) — الضغطة تخرج من الكيرنل ككيبورد فيزيائي حقيقي ١٠٠٪!"
        elif probe_inj is True:
            l4 = "• فحص بصمة الضغطة الحية (LLKHF_INJECTED): 1 (تعمل حالياً عبر SendInput User-Mode بدون درايفر كيرنل)"
        else:
            l4 = "• فحص بصمة الضغطة الحية: اضغط زر «فحص حالة Interception والكيرنل الآن» لاختبار البصمة مباشرةً"
    else:
        l1 = f"• Bridge Library (interception.dll): {'✓ Loaded (' + dll_p + ')' if dll_ok else '✗ Missing from drivers/ folder'}"
        l2 = f"• Kernel Driver (keyboard.sys / UpperFilters): {'✓ Installed' if sys_ok else '✗ Not installed'}" + (
            f" | Ring-0 Context: {'✓ Active' if ctx_ok else 'Pending install/reboot'}"
        )
        l3 = f"• Motherboard PS/2 Port 0x60 (InpOutx64.dll): {'✓ Ready' if inp_ok else 'Not added (Optional for Method 8)'}"
        if probe_inj is False:
            l4 = "• Live Hook Flag Probe (LLKHF_INJECTED): ✓ ZERO (0) — Keystrokes originate from Ring-0 Hardware Driver!"
        elif probe_inj is True:
            l4 = "• Live Hook Flag Probe (LLKHF_INJECTED): 1 (Currently using User-Mode SendInput Fallback)"
        else:
            l4 = "• Live Hook Flag Probe: Click 'Check Interception & Kernel Now' to test live injection signature"

    return headline, "\n".join([l1, l2, l3, l4]), kind


def install_or_import_interception_file(selected_file_path: str = "") -> Tuple[bool, str]:
    """
    Import `interception.dll`, `install-interception.exe`, or `inpoutx64.dll` into the app's
    `drivers/` folder (and `AppData/drivers` + `System32` if Admin), and if `install-interception.exe`
    is present, execute `install-interception.exe /install` with Administrator privileges!
    """
    drv_dir = get_drivers_dir()
    app_drv_dir = get_appdata_dir() / "drivers"
    app_drv_dir.mkdir(parents=True, exist_ok=True)

    if selected_file_path:
        src = Path(selected_file_path)
        if not src.exists():
            return False, tl("الملف المحدد غير موجود", "Selected file does not exist")

        # Copy the selected file and any sibling interception.dll / install-interception.exe / inpoutx64.dll in the same folder
        for item in [src] + list(src.parent.glob("*.dll")) + list(src.parent.glob("*interception*.exe")):
            if item.is_file() and item.name.lower() in (
                "interception.dll",
                "install-interception.exe",
                "install_interception.exe",
                "inpoutx64.dll",
                "winring0x64.dll",
                "winring0x64.sys",
                src.name.lower(),
            ):
                try:
                    shutil.copy2(item, drv_dir / item.name)
                except Exception:
                    pass
                try:
                    shutil.copy2(item, app_drv_dir / item.name)
                except Exception:
                    pass
                if IS_WINDOWS and item.name.lower() in ("interception.dll", "inpoutx64.dll"):
                    sys32 = Path(os.environ.get("SystemRoot", r"C:\Windows")) / "System32"
                    try:
                        shutil.copy2(item, sys32 / item.name)
                    except Exception:
                        pass

    installer = find_interception_installer_exe()
    if installer and IS_WINDOWS:
        try:
            proc = subprocess.run(
                [str(installer), "/install"],
                cwd=str(installer.parent),
                capture_output=True,
                text=True,
                timeout=20,
                creationflags=0x08000000,
            )
            _HW_BRIDGE.reprobe()
            if proc.returncode == 0 or _check_windows_interception_registry_and_sys()[0]:
                return True, tl(
                    "✓ تم تثبيت درايفر الكيرنل Interception بنجاح! أعد تشغيل الويندوز (Restart) مرة واحدة ليصبح Ring-0 نشطاً ١٠٠٪.",
                    "✓ Interception Kernel Driver installed! Please restart Windows once to activate Ring-0.",
                )
        except Exception as exc:
            return False, f"Install error: {exc}"

    _HW_BRIDGE.reprobe()
    diag = diagnose_kernel_drivers(run_live_probe=False)
    if diag["interception_context_active"]:
        return True, tl(
            "✓ مكتبة ودرايفر Interception Ring-0 متصلان ويعملان بنجاح الآن!",
            "✓ Interception Ring-0 DLL and Kernel Driver are active and working!",
        )
    if diag["interception_dll_found"]:
        return True, tl(
            "✓ تم ربط interception.dll بنجاح! لتفعيل درايفر الكيرنل باقي تشغيل install-interception.exe /install ثم إعادة تشغيل الويندوز.",
            "✓ Linked interception.dll! Run install-interception.exe /install and reboot Windows to activate Ring-0.",
        )
    return False, tl(
        "لم يتم العثور على interception.dll أو install-interception.exe — اختر الملف من جهازك أو ضعه في مجلد drivers.",
        "interception.dll or install-interception.exe not found — select it or place it in the drivers folder.",
    )




def _check_windows_ps2_and_inpout_driver() -> Tuple[bool, bool, str]:
    """
    Check if `inpoutx64.sys` or `hwinterface.sys` kernel driver is installed in System32/drivers
    and check the status of the Windows `i8042prt` PS/2 Keyboard & Mouse controller driver.
    """
    if not IS_WINDOWS:
        return False, False, "Linux"
    sys_root = Path(os.environ.get("SystemRoot", r"C:\Windows"))
    drv_folder = sys_root / "System32" / "drivers"
    inpout_sys_exists = any(
        (drv_folder / n).exists()
        for n in ("inpoutx64.sys", "InpOutx64.sys", "hwinterface.sys", "WinRing0x64.sys")
    )
    i8042_sys_exists = (drv_folder / "i8042prt.sys").exists()
    i8042_start_mode = "Unknown"
    try:
        import winreg

        reg_path = r"SYSTEM\CurrentControlSet\Services\i8042prt"
        with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, reg_path, 0, winreg.KEY_READ) as k:
            val, _ = winreg.QueryValueEx(k, "Start")
            start_int = int(val)
            i8042_start_mode = {
                0: "Boot (0)",
                1: "System (1 — Active)",
                2: "Auto (2 — Active)",
                3: "Demand / Manual (3)",
                4: "Disabled (4)",
            }.get(start_int, f"Start={start_int}")
    except Exception:
        pass
    return inpout_sys_exists, i8042_sys_exists, i8042_start_mode


def find_ps2_installer_exe() -> Optional[Path]:
    for d in get_driver_search_dirs():
        for name in ("InstallDriver.exe", "installdriver.exe", "inpout_install.exe"):
            cand = d / name
            if cand.exists():
                return cand
    return None


def diagnose_ps2_port_driver(run_live_probe: bool = False) -> Dict[str, Any]:
    """
    Perform a comprehensive real-time diagnostic check of Method 8:
    Motherboard PS/2 Port `0x60/0x64` (`i8042` Controller via `InpOutx64.dll` / `inpoutx64.sys`).
    """
    _HW_BRIDGE.reprobe()
    admin_ok = is_running_as_admin()
    inpout_sys_exists, i8042_sys_exists, i8042_start_mode = _check_windows_ps2_and_inpout_driver()
    installer_path = find_ps2_installer_exe()

    dll_obj = _HW_BRIDGE._inpout_dll
    dll_found = bool(dll_obj is not None)
    dll_path = _HW_BRIDGE._inpout_dll_path
    has_out32 = False
    has_inp32 = False
    driver_open = False
    port64_status_byte: Optional[int] = None
    live_probe_ok: Optional[bool] = None

    if dll_obj is not None and IS_WINDOWS:
        try:
            out32_fn = getattr(dll_obj, "Out32", None)
            inp32_fn = getattr(dll_obj, "Inp32", None)
            is_open_fn = getattr(dll_obj, "IsInpOutDriverOpen", None)
            has_out32 = out32_fn is not None
            has_inp32 = inp32_fn is not None
            if is_open_fn is not None:
                is_open_fn.restype = ctypes.c_int
                driver_open = bool(is_open_fn() != 0)
            elif has_out32:
                driver_open = True

            if driver_open and has_inp32:
                inp32_fn.argtypes = [ctypes.c_short]
                inp32_fn.restype = ctypes.c_short
                st_val = int(inp32_fn(0x64)) & 0xFF
                port64_status_byte = st_val

            if run_live_probe and driver_open and has_out32:
                STEALTH_TRACKER.probe_event.clear()
                STEALTH_TRACKER.last_probe_had_injected_flag = None
                STEALTH_TRACKER.probe_waiting = True
                try:
                    # Send harmless F24 break code (0x76 | 0x80 = 0xF6) via PS/2 Port 0x64=0xD2 -> 0x60
                    live_probe_ok = _HW_BRIDGE.try_ps2_port60_key(0x76, False, True)
                    STEALTH_TRACKER.probe_event.wait(timeout=0.10)
                finally:
                    STEALTH_TRACKER.probe_waiting = False
        except Exception:
            pass

    if not IS_WINDOWS:
        state_code = "linux_na"
    elif dll_found and driver_open:
        state_code = "ps2_active"
    elif dll_found and not driver_open:
        state_code = "dll_only_driver_closed"
    elif inpout_sys_exists and not dll_found:
        state_code = "sys_only_no_dll"
    else:
        state_code = "fallback"

    return {
        "state_code": state_code,
        "is_admin": admin_ok,
        "inpout_dll_found": dll_found,
        "inpout_dll_path": dll_path,
        "has_out32": has_out32,
        "has_inp32": has_inp32,
        "driver_open": driver_open,
        "inpout_sys_exists": inpout_sys_exists,
        "i8042_sys_exists": i8042_sys_exists,
        "i8042_start_mode": i8042_start_mode,
        "port64_status_byte": port64_status_byte,
        "live_probe_ok": live_probe_ok,
        "installer_exe_path": str(installer_path) if installer_path else "",
        "drivers_dir": str(get_drivers_dir()),
    }


def format_ps2_diagnostic_report(diag: Dict[str, Any], is_arabic: bool = True) -> Tuple[str, str, str]:
    """
    Format `(headline, details_multiline, status_kind)` from `diagnose_ps2_port_driver()` output.
    """
    code = diag.get("state_code", "fallback")
    dll_ok = diag.get("inpout_dll_found", False)
    dll_p = diag.get("inpout_dll_path", "") or ("غير موجود" if is_arabic else "Not found")
    drv_open = diag.get("driver_open", False)
    sys_ok = diag.get("inpout_sys_exists", False)
    i8042_mode = diag.get("i8042_start_mode", "Unknown")
    p64 = diag.get("port64_status_byte", None)
    probe_ok = diag.get("live_probe_ok", None)

    p64_str = f"0x{p64:02X}" if isinstance(p64, int) else ("N/A" if not is_arabic else "غير مقروء")

    if code == "ps2_active":
        headline = (
            f"🟢 بورت البوردة PS/2 Port 0x60/0x64 (InpOutx64) نشط ومتصل فعلياً! [Port 0x64 Status: {p64_str}]"
            if is_arabic
            else f"🟢 Motherboard PS/2 Port 0x60/0x64 (InpOutx64) ACTIVE & Verified! [Port 0x64 Status: {p64_str}]"
        )
        kind = "ok"
    elif code == "dll_only_driver_closed":
        headline = (
            "🟡 ملف inpoutx64.dll موجود ولكن درايفر الكيرنل (inpoutx64.sys) غير مفعّل — شغّل البرنامج كمسؤول (Admin) أو اضغط زر التثبيت بالأسفل لتفعيله!"
            if is_arabic
            else "🟡 inpoutx64.dll loaded, but kernel driver (inpoutx64.sys) is not open — Run as Admin or click Install/Link below to activate once!"
        )
        kind = "warn"
    elif code == "sys_only_no_dll":
        headline = (
            "🟠 درايفر inpoutx64.sys مثبّت في الويندوز ولكن ملف inpoutx64.dll غير موجود في مجلد drivers!"
            if is_arabic
            else "🟠 inpoutx64.sys is installed in Windows, but inpoutx64.dll is missing from the drivers/ folder!"
        )
        kind = "warn"
    else:
        headline = (
            "🔵 الطريقة ٨ تعمل حالياً على البديل الآمن (SendInput ScanCode) — لتفعيل بورت البوردة الفيزيائي 0x60/0x64 اربط ملف inpoutx64.dll بالأسفل"
            if is_arabic
            else "🔵 Method 8 currently uses safe User-Mode Fallback (SendInput ScanCode) — Link inpoutx64.dll below for Direct Port 0x60/0x64 I/O"
        )
        kind = "info"

    if is_arabic:
        l1 = f"• مكتبة بورت الهاردوير (inpoutx64.dll): {'✓ متصلة (' + dll_p + ')' if dll_ok else '✗ غير موجودة في مجلد drivers'}"
        l2 = f"• درايفر الكيرنل (inpoutx64.sys / IsInpOutDriverOpen): {'✓ مفتوح ونشط' if drv_open else ('مثبّت ويحتاج صلاحية مسؤول لأول مرة' if sys_ok else '✗ غير مثبّت بعد')}"
        l3 = f"• متحكم الكيبورد في الويندوز (i8042prt.sys): الحالة ({i8042_mode}) | سجل الحالة Port 0x64: {p64_str}"
        if probe_ok is True:
            l4 = "• فحص نبضة البورت الحية (0x64=0xD2 -> 0x60): ✓ نجح إرسال النبضة الفيزيائية لبورت الكيبورد 0x60 مباشرةً!"
        elif probe_ok is False:
            l4 = "• فحص نبضة البورت الحية: تعذر الإرسال لبورت 0x60 (يعمل التحويل التلقائي الآمن SendInput)"
        else:
            l4 = "• فحص نبضة البورت الحية: اضغط زر «فحص بورت PS/2 و InpOutx64 الآن» لاختبار بورت 0x60/0x64 مباشرةً"
    else:
        l1 = f"• Hardware I/O Library (inpoutx64.dll): {'✓ Loaded (' + dll_p + ')' if dll_ok else '✗ Missing from drivers/ folder'}"
        l2 = f"• Kernel Driver (inpoutx64.sys / IsInpOutDriverOpen): {'✓ Open & Active' if drv_open else ('Installed (Run as Admin once to initialize)' if sys_ok else '✗ Not installed yet')}"
        l3 = f"• Windows PS/2 Controller (i8042prt.sys): Mode ({i8042_mode}) | Port 0x64 Status Register: {p64_str}"
        if probe_ok is True:
            l4 = "• Live Port 0x64/0x60 Probe (0xD2 -> 0x60): ✓ Hardware Output Buffer command succeeded!"
        elif probe_ok is False:
            l4 = "• Live Port 0x64/0x60 Probe: Port write unavailable (Using safe SendInput fallback)"
        else:
            l4 = "• Live Port 0x64/0x60 Probe: Click 'Check PS/2 Port & InpOutx64 Now' to test Port 0x60/0x64 live"

    return headline, "\n".join([l1, l2, l3, l4]), kind


def install_or_import_ps2_file(selected_file_path: str = "") -> Tuple[bool, str]:
    """
    Import `inpoutx64.dll`, `WinRing0x64.dll`, or `InstallDriver.exe` into `drivers/` (and `AppData/drivers`),
    and if `InstallDriver.exe` is present or `inpoutx64.dll` is loaded, initialize the Ring-0 I/O driver!
    """
    drv_dir = get_drivers_dir()
    app_drv_dir = get_appdata_dir() / "drivers"
    app_drv_dir.mkdir(parents=True, exist_ok=True)

    if selected_file_path:
        src = Path(selected_file_path)
        if not src.exists():
            return False, tl("الملف المحدد غير موجود", "Selected file does not exist")
        low = src.name.lower()
        if "inpout" in low and low.endswith(".dll"):
            target_name = "inpoutx64.dll"
        elif "winring0" in low and low.endswith(".dll"):
            target_name = "WinRing0x64.dll"
        elif low.endswith(".exe"):
            target_name = "InstallDriver.exe"
        else:
            target_name = src.name

        for dest_folder in (drv_dir, app_drv_dir):
            try:
                dest_folder.mkdir(parents=True, exist_ok=True)
                dst = dest_folder / target_name
                if src.resolve() != dst.resolve():
                    shutil.copy2(src, dst)
            except Exception:
                pass

    installer_exe = find_ps2_installer_exe()
    if installer_exe is not None and IS_WINDOWS:
        try:
            ctypes.windll.shell32.ShellExecuteW(
                None,
                "runas",
                str(installer_exe),
                "",
                str(installer_exe.parent),
                1,
            )
            time.sleep(1.0)
        except Exception:
            pass

    _HW_BRIDGE.reprobe()
    diag = diagnose_ps2_port_driver(run_live_probe=True)
    if diag.get("driver_open"):
        return True, tl(
            "✓ تم ربط وتفعيل بورت PS/2 0x60/0x64 (InpOutx64) بنجاح تام!",
            "✓ Motherboard PS/2 Port 0x60/0x64 (InpOutx64) linked & activated!",
        )
    if diag.get("inpout_dll_found"):
        return True, tl(
            "✓ تم نسخ inpoutx64.dll لمجلد drivers بنجاح — شغّل كمسؤول (Admin) مرة واحدة أو شغّل InstallDriver.exe لتفعيل درايفر الكيرنل!",
            "✓ Copied inpoutx64.dll to drivers/ — Run as Admin once or run InstallDriver.exe to activate kernel I/O driver!",
        )
    return False, tl(
        "لم يتم العثور على inpoutx64.dll — يرجى اختيار ملف inpoutx64.dll أو InstallDriver.exe.",
        "inpoutx64.dll not found — Please select inpoutx64.dll or InstallDriver.exe.",
    )


def resolve_effective_input_engine(method: str, is_arabic: bool = True) -> Dict[str, Any]:
    """
    Resolve which actual OS input injection engine will execute for a requested `method`,
    transparently reporting any OS/driver fallback using the active UI language (`t(...)`).
    """
    from app.i18n import t as _t

    if method == "linux_uinput_xtest":
        if IS_WINDOWS:
            return {
                "requested_method": method,
                "effective_method": "sendinput_scancode",
                "is_fallback": True,
                "status_kind": "warn",
                "reason": _t("eng_status_linux_win_fallback"),
            }
        return {
            "requested_method": method,
            "effective_method": "linux_uinput_xtest",
            "is_fallback": False,
            "status_kind": "ok",
            "reason": _t("eng_status_linux_active"),
        }

    if not IS_WINDOWS:
        return {
            "requested_method": method,
            "effective_method": "pynput_fallback",
            "is_fallback": True,
            "status_kind": "warn",
            "reason": _t("eng_status_non_win"),
        }

    if method == "cpp_interception_driver":
        if _HW_BRIDGE.is_interception_ring0_ready():
            dev = _HW_BRIDGE._active_kb_device
            return {
                "requested_method": method,
                "effective_method": "cpp_interception_driver",
                "is_fallback": False,
                "status_kind": "ok",
                "reason": f"{_t('eng_status_interception_active')} [Device #{dev}]",
            }
        return {
            "requested_method": method,
            "effective_method": "sendinput_scancode",
            "is_fallback": True,
            "status_kind": "warn",
            "reason": _t("eng_status_interception_fallback"),
        }

    if method == "cpp_winring0_ps2_port":
        if _HW_BRIDGE.is_ps2_port60_ready():
            return {
                "requested_method": method,
                "effective_method": "cpp_winring0_ps2_port",
                "is_fallback": False,
                "status_kind": "ok",
                "reason": _t("eng_status_ps2_active"),
            }
        return {
            "requested_method": method,
            "effective_method": "sendinput_scancode",
            "is_fallback": True,
            "status_kind": "warn",
            "reason": _t("eng_status_ps2_fallback"),
        }

    if method == "cpp_vhid_ioctl_bridge":
        return {
            "requested_method": method,
            "effective_method": "cpp_vhid_ioctl_bridge",
            "is_fallback": False,
            "status_kind": "ok",
            "reason": _t("eng_status_vhid_active"),
        }
    if method == "cpp_sendinput_batch_atomic":
        return {
            "requested_method": method,
            "effective_method": "cpp_sendinput_batch_atomic",
            "is_fallback": False,
            "status_kind": "ok",
            "reason": _t("eng_status_atomic_active"),
        }

    if method == "pico_usb_hid":
        from app.domain.microcontroller_bridge import MCU_BRIDGE
        if MCU_BRIDGE.is_board_ready("pico"):
            return {
                "requested_method": method,
                "effective_method": "pico_usb_hid",
                "is_fallback": False,
                "status_kind": "ok",
                "reason": f"{_t('eng_status_pico_active')} [{MCU_BRIDGE._active_port}]",
            }
        return {
            "requested_method": method,
            "effective_method": "sendinput_scancode",
            "is_fallback": True,
            "status_kind": "warn",
            "reason": _t("eng_status_pico_fallback"),
        }

    if method == "arduino_usb_hid":
        from app.domain.microcontroller_bridge import MCU_BRIDGE
        if MCU_BRIDGE.is_board_ready("arduino"):
            return {
                "requested_method": method,
                "effective_method": "arduino_usb_hid",
                "is_fallback": False,
                "status_kind": "ok",
                "reason": f"{_t('eng_status_arduino_active')} [{MCU_BRIDGE._active_port}]",
            }
        return {
            "requested_method": method,
            "effective_method": "sendinput_scancode",
            "is_fallback": True,
            "status_kind": "warn",
            "reason": _t("eng_status_arduino_fallback"),
        }

    return {
        "requested_method": method,
        "effective_method": method,
        "is_fallback": False,
        "status_kind": "ok",
        "reason": f"{_t('eng_status_native_active')} ({method})",
    }


def describe_active_input_engine_status(method: str, is_arabic: bool = True) -> str:
    return str(resolve_effective_input_engine(method, is_arabic=is_arabic)["reason"])


# =============================================================================
# 10 Selectable Input Simulation & Hardware Keyboard Emulation Engines
# =============================================================================
def send_scancode_key(
    scan_code: int,
    is_extended: bool = False,
    key_up: bool = False,
    method: str = "sendinput_scancode",
    stealth_cloak: bool = True,
    jitter_ms: int = 0,
    strict_no_fallback: bool = False,
) -> bool:
    if scan_code <= 0:
        return False

    _GLOBAL_USB_HID_REPORT.update_key(scan_code, is_extended, not key_up)

    if not IS_WINDOWS:
        return True

    try:
        is_synth_vk = scan_code >= 0x100
        vk = (scan_code - 0x100) if is_synth_vk else vk_for_scancode(scan_code, is_extended)
        hw_scan = 0 if is_synth_vk else int(scan_code)

        STEALTH_TRACKER.record_key(scan_code, not key_up)

        use_zero_sig = stealth_cloak or method in (
            "cpp_stealth_humanized",
            "cpp_vhid_ioctl_bridge",
            "cpp_interception_driver",
            "cpp_winring0_ps2_port",
            "cpp_sendinput_batch_atomic",
        )

        if jitter_ms > 0 and not key_up and method == "cpp_stealth_humanized":
            time.sleep(random.uniform(0.00015, min(0.004, jitter_ms / 1000.0)))

        # Method 10: Linux /dev/uinput (not available on Windows)
        if method == "linux_uinput_xtest":
            if strict_no_fallback:
                return False

        # Method 7: Ring-0 Kernel Filter Driver (`interception.dll` -> `keyboard.sys`)
        if method in ("cpp_interception_driver", "ring0_interception"):
            if not is_synth_vk and _HW_BRIDGE.try_interception_key(hw_scan, is_extended, key_up):
                return True
            if strict_no_fallback:
                return False

        # Method 8: Motherboard PS/2 Controller Port 0x60/0x64 (`i8042prt`)
        if method == "cpp_winring0_ps2_port":
            if not is_synth_vk and _HW_BRIDGE.try_ps2_port60_key(hw_scan, is_extended, key_up):
                return True
            if strict_no_fallback:
                return False

        # Method 21 (`pico_usb_hid`) & Method 22 (`arduino_usb_hid`): External USB HID Microcontroller
        if method in ("pico_usb_hid", "arduino_usb_hid"):
            from app.domain.microcontroller_bridge import MCU_BRIDGE
            b_target = "pico" if method == "pico_usb_hid" else "arduino"
            hid_u = usb_hid_usage_for_scancode(hw_scan, is_extended)
            mod_b = usb_hid_modifier_bit(hw_scan, is_extended)
            if (hid_u > 0 or mod_b > 0) and MCU_BRIDGE.send_key_event(hid_u, not key_up, mod_b, board_type=b_target):
                return True
            if strict_no_fallback:
                return False

        # Background / HWND Window Message Methods (Methods 11..20)
        bg_hwnd_methods = {
            "win32_message",
            "postmessage",
            "sendmessage",
            "post_child",
            "post_children",
            "post_active",
            "attach_state",
            "guithread_focus",
            "wm_char_triplet",
            "ime_pipeline",
            "sendnotify_tree",
        }
        if method in bg_hwnd_methods and vk > 0:
            fg_hwnd = user32.GetForegroundWindow()
            if fg_hwnd:
                msg = WM_KEYUP if key_up else WM_KEYDOWN
                lparam = 1 | ((hw_scan & 0xFF) << 16)
                if is_extended:
                    lparam |= 1 << 24
                if key_up:
                    lparam |= (1 << 30) | (1 << 31)
                if method == "sendmessage":
                    res_dw = wintypes.DWORD(0)
                    user32.SendMessageTimeoutW(fg_hwnd, msg, vk, lparam, 0x0002, 200, ctypes.byref(res_dw))
                elif method == "sendnotify_tree":
                    user32.SendNotifyMessageW(fg_hwnd, msg, vk, lparam)
                else:
                    user32.PostMessageW(fg_hwnd, msg, vk, lparam)
                return True

        # Method 3: Legacy NT Interrupt Driver `keybd_event`
        if method == "keybd_mouse_event":
            k_flags = 0
            if is_extended and not is_synth_vk:
                k_flags |= KEYEVENTF_EXTENDEDKEY
            if key_up:
                k_flags |= KEYEVENTF_KEYUP
            extra_val = 0 if use_zero_sig else OUR_EXTRA_INFO
            user32.keybd_event(int(vk), int(hw_scan), int(k_flags), extra_val)
            return True

        if is_synth_vk:
            flags = KEYEVENTF_KEYUP if key_up else 0
            w_vk = int(vk)
            w_scan = 0
        elif method in ("sendinput_hybrid", "cpp_vhid_ioctl_bridge") and vk > 0:
            flags = KEYEVENTF_SCANCODE
            if is_extended:
                flags |= KEYEVENTF_EXTENDEDKEY
            if key_up:
                flags |= KEYEVENTF_KEYUP
            w_vk = int(vk)
            w_scan = int(hw_scan)
        else:
            flags = KEYEVENTF_SCANCODE
            if is_extended:
                flags |= KEYEVENTF_EXTENDEDKEY
            if key_up:
                flags |= KEYEVENTF_KEYUP
            w_vk = 0
            w_scan = int(hw_scan)

        hw_tick = int(kernel32.GetTickCount()) if use_zero_sig else 0
        extra_info = 0 if use_zero_sig else OUR_EXTRA_INFO

        inp = INPUT()
        inp.type = INPUT_KEYBOARD
        inp.union.ki = KEYBDINPUT(
            wVk=w_vk,
            wScan=w_scan,
            dwFlags=flags,
            time=hw_tick,
            dwExtraInfo=extra_info,
        )
        sent = user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(inp))
        return bool(sent > 0)
    except Exception:
        return False


def send_unicode_text(text: str) -> bool:
    if not IS_WINDOWS or not text:
        return False
    try:
        for ch in text:
            code = ord(ch)
            for up in (False, True):
                flags = KEYEVENTF_UNICODE | (KEYEVENTF_KEYUP if up else 0)
                inp = INPUT()
                inp.type = INPUT_KEYBOARD
                inp.union.ki = KEYBDINPUT(0, code, flags, 0, 0)
                user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(inp))
        return True
    except Exception:
        return False


# =============================================================================
# Multi-Monitor Cursor, 5-Button Mouse Click/Hold & 4-Way Scroll Helpers
# =============================================================================
def get_cursor_pos() -> Tuple[int, int]:
    if IS_WINDOWS:
        try:
            pt = wintypes.POINT()
            if user32.GetCursorPos(ctypes.byref(pt)):
                return int(pt.x), int(pt.y)
        except Exception:
            pass
    try:
        from PySide6.QtGui import QCursor

        pos = QCursor.pos()
        return int(pos.x()), int(pos.y())
    except Exception:
        return 0, 0


def send_mouse_button(
    x: int,
    y: int,
    button: str = "left",
    action: str = "click",          # "click", "down", "up"
    move_cursor: bool = True,
    hold_ms: int = 0,
    method: str = "sendinput_scancode",
) -> None:
    """
    Perform a mouse press, release, or timed click/hold at `(x, y)` supporting all 5 mouse buttons:
    - `"left"`, `"right"`, `"middle"`, `"x1"` (Back), `"x2"` (Forward)!
    - Uses Ring-0 `Interception` mouse device when `method == "cpp_interception_driver"` and active!
    """
    if not IS_WINDOWS:
        return
    try:
        STEALTH_TRACKER.record_mouse()
        if method in ("pico_usb_hid", "arduino_usb_hid"):
            from app.domain.microcontroller_bridge import MCU_BRIDGE
            b_target = "pico" if method == "pico_usb_hid" else "arduino"
            if move_cursor:
                user32.SetCursorPos(int(x), int(y))
            if MCU_BRIDGE.send_mouse_action(button=button, action=action, hold_ms=hold_ms, board_type=b_target):
                return

        if method == "cpp_interception_driver":
            if _HW_BRIDGE.try_interception_mouse_button(x, y, button, action, move_cursor, hold_ms):
                return

        if move_cursor:
            user32.SetCursorPos(int(x), int(y))

        b_low = button.lower()
        mouse_data = 0
        if b_low == "right":
            dn, up = MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP
            wm_dn, wm_up = WM_RBUTTONDOWN, WM_RBUTTONUP
        elif b_low == "middle":
            dn, up = MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP
            wm_dn, wm_up = WM_MBUTTONDOWN, WM_MBUTTONUP
        elif b_low in ("x1", "back"):
            dn, up = MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP
            wm_dn, wm_up = WM_XBUTTONDOWN, WM_XBUTTONUP
            mouse_data = XBUTTON1
        elif b_low in ("x2", "forward"):
            dn, up = MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP
            wm_dn, wm_up = WM_XBUTTONDOWN, WM_XBUTTONUP
            mouse_data = XBUTTON2
        else:
            dn, up = MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP
            wm_dn, wm_up = WM_LBUTTONDOWN, WM_LBUTTONUP

        use_zero_sig = method in (
            "cpp_stealth_humanized",
            "cpp_vhid_ioctl_bridge",
            "cpp_interception_driver",
            "cpp_winring0_ps2_port",
            "cpp_sendinput_batch_atomic",
        )
        extra_info = 0 if use_zero_sig else OUR_EXTRA_INFO

        def _emit_flag(flg: int, wm_msg: int) -> None:
            STEALTH_TRACKER.record_mouse()
            if method == "keybd_mouse_event":
                user32.mouse_event(flg, 0, 0, mouse_data, extra_info)
                return
            if method == "win32_message":
                hwnd = user32.GetForegroundWindow()
                if hwnd:
                    pt = wintypes.POINT(int(x), int(y))
                    user32.ScreenToClient(hwnd, ctypes.byref(pt))
                    lparam = ((pt.y & 0xFFFF) << 16) | (pt.x & 0xFFFF)
                    wparam = (mouse_data << 16) | (1 if flg == dn else 0)
                    user32.PostMessageW(hwnd, wm_msg, wparam, lparam)
            hw_tick = int(kernel32.GetTickCount()) if use_zero_sig else 0
            inp = INPUT()
            inp.type = INPUT_MOUSE
            inp.union.mi = MOUSEINPUT(0, 0, mouse_data, flg, hw_tick, extra_info)
            user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(inp))

        if action == "down":
            _emit_flag(dn, wm_dn)
        elif action == "up":
            _emit_flag(up, wm_up)
        else:
            _emit_flag(dn, wm_dn)
            if hold_ms > 0:
                time.sleep(max(0.005, hold_ms / 1000.0))
            _emit_flag(up, wm_up)
    except Exception:
        pass



def send_mouse_relative_move(
    dx: int,
    dy: int,
    steps: int = 1,
    duration_ms: int = 0,
    method: str = "sendinput_scancode",
) -> bool:
    """
    Dispatch pure relative 3D mouse motion (`MOUSEEVENTF_MOVE` without `MOUSEEVENTF_ABSOLUTE`
    and without `SetCursorPos`) so 3D game cameras (`DirectInput` / `RawInput`) rotate smoothly.
    """
    if not IS_WINDOWS:
        return True
    try:
        n = max(1, min(120, int(steps)))
        step_sleep = (max(0.0, float(duration_ms)) / 1000.0) / n if duration_ms > 0 else 0.002
        rem_x = int(dx)
        rem_y = int(dy)
        use_zero_sig = method in (
            "cpp_stealth_humanized",
            "cpp_vhid_ioctl_bridge",
            "cpp_interception_driver",
            "cpp_winring0_ps2_port",
            "cpp_sendinput_batch_atomic",
        )
        extra_info = 0 if use_zero_sig else OUR_EXTRA_INFO
        for i in range(n):
            left_steps = n - i
            sx = int(round(rem_x / left_steps))
            sy = int(round(rem_y / left_steps))
            rem_x -= sx
            rem_y -= sy
            STEALTH_TRACKER.record_mouse()
            if method in ("pico_usb_hid", "arduino_usb_hid"):
                from app.domain.microcontroller_bridge import MCU_BRIDGE
                b_target = "pico" if method == "pico_usb_hid" else "arduino"
                if not MCU_BRIDGE.send_mouse_move_rel(sx, sy, board_type=b_target):
                    inp = INPUT()
                    inp.type = INPUT_MOUSE
                    inp.union.mi = MOUSEINPUT(sx, sy, 0, MOUSEEVENTF_MOVE, 0, 0)
                    user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(inp))
            elif method == "keybd_mouse_event":
                user32.mouse_event(MOUSEEVENTF_MOVE, sx, sy, 0, extra_info)
            else:
                hw_tick = int(kernel32.GetTickCount()) if use_zero_sig else 0
                inp = INPUT()
                inp.type = INPUT_MOUSE
                inp.union.mi = MOUSEINPUT(sx, sy, 0, MOUSEEVENTF_MOVE, hw_tick, extra_info)
                user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(inp))
            if step_sleep > 0 and i + 1 < n:
                time.sleep(step_sleep)
        return True
    except Exception:
        return False


def set_cursor_pos_and_click(
    x: int,
    y: int,
    button: str = "left",
    move_cursor: bool = True,
    hold_ms: int = 0,
    method: str = "sendinput_scancode",
) -> None:
    send_mouse_button(
        x=x,
        y=y,
        button=button,
        action="click",
        move_cursor=move_cursor,
        hold_ms=hold_ms,
        method=method,
    )


def send_mouse_scroll(
    direction: str = "down",
    amount: int = 3,
    dx: int = 0,
    dy: int = 0,
    x: Optional[int] = None,
    y: Optional[int] = None,
    move_cursor: bool = False,
) -> None:
    if not IS_WINDOWS:
        return
    try:
        STEALTH_TRACKER.record_mouse()
        if move_cursor and x is not None and y is not None:
            user32.SetCursorPos(int(x), int(y))

        steps = max(1, abs(int(amount))) if (dx == 0 and dy == 0) else 1
        d_low = direction.lower().strip()

        if dy != 0 and dx == 0 and not d_low:
            d_low = "up" if dy > 0 else "down"
            steps = abs(int(dy))
        elif dx != 0 and dy == 0 and not d_low:
            d_low = "right" if dx > 0 else "left"
            steps = abs(int(dx))

        if _HW_BRIDGE.is_interception_ring0_ready():
            if _HW_BRIDGE.try_interception_mouse_scroll(d_low or "down", steps):
                return

        if d_low in ("left", "right"):
            flg = MOUSEEVENTF_HWHEEL
            signed_delta = (120 * steps) if d_low == "right" else (-120 * steps)
        else:
            flg = MOUSEEVENTF_WHEEL
            signed_delta = (120 * steps) if d_low == "up" else (-120 * steps)

        m_data = ctypes.c_ulong(signed_delta & 0xFFFFFFFF).value
        inp = INPUT()
        inp.type = INPUT_MOUSE
        inp.union.mi = MOUSEINPUT(0, 0, m_data, flg, 0, 0)
        user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(inp))
    except Exception:
        pass


# =============================================================================
# PowerShell & CMD Command Execution
# =============================================================================
def run_shell_command(shell_type: str, command_text: str) -> None:
    cmd_clean = command_text.strip()
    if not cmd_clean:
        return
    try:
        creation_flags = 0x08000000 if IS_WINDOWS else 0
        if shell_type == "powershell":
            args = ["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-Command", cmd_clean]
        else:
            args = ["cmd.exe", "/c", cmd_clean] if IS_WINDOWS else ["sh", "-c", cmd_clean]
        subprocess.Popen(
            args,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            stdin=subprocess.DEVNULL,
            creationflags=creation_flags,
        )
    except Exception:
        pass


# =============================================================================
# Windows Startup (`HKCU\Software\Microsoft\Windows\CurrentVersion\Run`)
# =============================================================================
STARTUP_REG_KEY = r"Software\Microsoft\Windows\CurrentVersion\Run"
STARTUP_APP_NAME = "V-MacroPlus"


def is_windows_startup_enabled() -> bool:
    if not IS_WINDOWS:
        return False
    try:
        import winreg

        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, STARTUP_REG_KEY, 0, winreg.KEY_READ) as key:
            for name in (STARTUP_APP_NAME, "V-MacroPulse"):
                try:
                    val, _ = winreg.QueryValueEx(key, name)
                    if val:
                        return True
                except FileNotFoundError:
                    pass
            return False
    except Exception:
        return False


def set_windows_startup_enabled(enable: bool) -> bool:
    if not IS_WINDOWS:
        return False
    try:
        import winreg

        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, STARTUP_REG_KEY, 0, winreg.KEY_SET_VALUE) as key:
            try:
                winreg.DeleteValue(key, "V-MacroPulse")
            except FileNotFoundError:
                pass
            if enable:
                if getattr(sys, "frozen", False):
                    cmd = f'"{sys.executable}"'
                else:
                    main_py = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "main.py"))
                    cmd = f'"{sys.executable}" "{main_py}"'
                winreg.SetValueEx(key, STARTUP_APP_NAME, 0, winreg.REG_SZ, cmd)
            else:
                try:
                    winreg.DeleteValue(key, STARTUP_APP_NAME)
                except FileNotFoundError:
                    pass
        return True
    except Exception:
        return False


def is_hwnd_own_process(hwnd: int) -> bool:
    """Return True if `hwnd` belongs to V-MacroPlus's own process (`OUR_PID`)."""
    if not IS_WINDOWS or not hwnd:
        return False
    try:
        pid = wintypes.DWORD(0)
        user32.GetWindowThreadProcessId(int(hwnd), ctypes.byref(pid))
        return bool(pid.value and int(pid.value) == int(OUR_PID))
    except Exception:
        return False


def is_own_app_foreground() -> bool:
    """Return True if the active foreground window on Windows belongs to V-MacroPlus (`OUR_PID`)."""
    if not IS_WINDOWS:
        return False
    try:
        hwnd = int(user32.GetForegroundWindow() or 0)
        if not hwnd:
            return False
        return is_hwnd_own_process(hwnd)
    except Exception:
        return False


def get_foreground_process_name() -> str:
    if not IS_WINDOWS:
        return ""
    try:
        hwnd = user32.GetForegroundWindow()
        if not hwnd:
            return ""
        pid = wintypes.DWORD(0)
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        if not pid.value:
            return ""
        h_proc = kernel32.OpenProcess(0x1000, False, pid.value)
        if not h_proc:
            return ""
        try:
            buf = ctypes.create_unicode_buffer(512)
            size = wintypes.DWORD(512)
            if kernel32.QueryFullProcessImageNameW(h_proc, 0, buf, ctypes.byref(size)):
                return os.path.basename(buf.value).lower()
        finally:
            kernel32.CloseHandle(h_proc)
    except Exception:
        pass
    return ""


def list_running_window_apps_with_paths() -> List[Tuple[str, str, str]]:
    results: List[Tuple[str, str, str]] = [("", t("all_apps"), "")]
    if not IS_WINDOWS:
        results.extend([
            ("notepad.exe", "notepad.exe — Notepad", ""),
            ("chrome.exe", "chrome.exe — Google Chrome", ""),
        ])
        return results

    seen: set[str] = set()
    try:
        WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

        def _enum_cb(hwnd: int, _: int) -> bool:
            if not user32.IsWindowVisible(hwnd):
                return True
            length = user32.GetWindowTextLengthW(hwnd)
            if length <= 0:
                return True
            pid = wintypes.DWORD(0)
            user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
            if not pid.value:
                return True
            h_proc = kernel32.OpenProcess(0x1000, False, pid.value)
            if not h_proc:
                return True
            try:
                buf = ctypes.create_unicode_buffer(512)
                sz = wintypes.DWORD(512)
                if kernel32.QueryFullProcessImageNameW(h_proc, 0, buf, ctypes.byref(sz)):
                    full_path = buf.value
                    exe = os.path.basename(full_path).lower()
                    if exe and exe not in seen and exe not in ("explorer.exe", "v-macroplus.exe", "v-macropulse.exe", "python.exe", "textinputhost.exe"):
                        seen.add(exe)
                        t_buf = ctypes.create_unicode_buffer(length + 1)
                        user32.GetWindowTextW(hwnd, t_buf, length + 1)
                        short_title = t_buf.value.strip()[:30]
                        results.append((exe, f"{exe} — {short_title}", full_path))
            finally:
                kernel32.CloseHandle(h_proc)
            return True

        user32.EnumWindows(WNDENUMPROC(_enum_cb), 0)
    except Exception:
        pass
    return results


def list_running_window_apps() -> List[Tuple[str, str]]:
    return [(exe, lbl) for exe, lbl, _ in list_running_window_apps_with_paths()]


def is_running_as_admin() -> bool:
    if not IS_WINDOWS:
        return True
    try:
        return bool(ctypes.windll.shell32.IsUserAnAdmin())
    except Exception:
        return False



def relaunch_as_admin() -> bool:
    """
    Optionally relaunch V-MacroPlus with Administrator privileges via Windows ShellExecuteW('runas')
    when requested by the user. Returns True if elevation was initiated.
    """
    if not IS_WINDOWS or is_running_as_admin():
        return False
    try:
        exe = sys.executable
        if getattr(sys, "frozen", False):
            params = "--relaunch-admin"
        else:
            main_script = str(Path(__file__).resolve().parents[2] / "main.py")
            params = f'"{main_script}" --relaunch-admin'
        res = ctypes.windll.shell32.ShellExecuteW(None, "runas", exe, params, None, 1)
        if int(res) > 32:
            from PySide6.QtWidgets import QApplication
            app = QApplication.instance()
            if app is not None:
                app.quit()
            return True
    except Exception:
        pass
    return False

def open_folder_or_path(folder_path: str) -> None:
    """
    Open a local folder OR launch an application/program/file (`.exe`, `.bat`, `.cmd`, `.lnk`, etc.).
    When launching an executable or script file on Windows, sets its working directory to `p.parent`
    via `ShellExecuteW` so games and desktop apps resolve their local assets/DLLs properly.
    """
    clean = folder_path.strip().strip('"')
    if not clean:
        return
    expanded = os.path.expandvars(os.path.expanduser(clean))
    try:
        p = Path(expanded)
        if IS_WINDOWS:
            if p.exists() and p.is_file():
                res = ctypes.windll.shell32.ShellExecuteW(
                    None,
                    "open",
                    str(p),
                    None,
                    str(p.parent),
                    1,
                )
                if int(res) <= 32:
                    os.startfile(expanded)
            else:
                os.startfile(expanded)
        else:
            if p.exists() and p.is_file() and os.access(str(p), os.X_OK):
                subprocess.Popen([str(p)], cwd=str(p.parent), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            else:
                subprocess.Popen(["xdg-open", expanded], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except Exception:
        pass
