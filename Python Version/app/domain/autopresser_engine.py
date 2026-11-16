# -*- coding: utf-8 -*-
"""
Complete V-AutoKeyPresser v3.9.0 Multi-Tab Domain Engine (`AutoKeyPresserEngine`)
integrated natively into V-MacroPlus:
- Unlimited independent automation tabs (`AutoPresserTab`), each running on its own daemon thread.
- Per-tab Target Process / Window (`HWND` & `.exe` name + window title) with automatic re-binding if the target app restarts.
- 36 Built-in Key Tiles (`F1`–`F12`, `1`–`9`, `0`, `-`, `=`, `Num 1`–`Num 9`, `Num 0`, `Num /`, `Num *`)
  + Custom Keys (`+ Add Key` from physical press or 92-key catalog).
- 20 Unified Key-Pressing & Hardware Input Methods (10 Foreground Hardware/Driver + 10 Background HWND Anti-Protection):
  1. `sendinput_scancode`         11. `postmessage`
  2. `sendinput_hybrid`           12. `sendmessage`
  3. `keybd_mouse_event`          13. `post_child`
  4. `unicode`                    14. `post_children`
  5. `cpp_stealth_humanized`      15. `post_active`
  6. `cpp_vhid_ioctl_bridge`      16. `attach_state`
  7. `cpp_interception_driver`    17. `guithread_focus`
  8. `cpp_winring0_ps2_port`      18. `wm_char_triplet`
  9. `cpp_sendinput_batch_atomic` 19. `ime_pipeline`
  10. `linux_uinput_xtest`        20. `sendnotify_tree`
- Multi-Point Mouse Click Automation (`AutoPresserMousePoint`) with `F8` live coordinate capture,
  5 buttons (`left`, `right`, `middle`, `x1` Back, `x2` Forward), and 8 distinct Mouse Click Modes
  (`auto`, `real`, `post`, `post_top`, `post_hittest_chain`, `attach_mouse_state`, `guithread_capture`, `sendinput_abs`).
- Three 20-Method Sweep Engines for Test Lab:
  1. `execute_per_method_python_test`: Editable Python script executed strictly via `method_id`.
  2. `expand_and_execute_method_template`: Smart Template Expression (`key="Enter" {method_num}: ... {method_id} \\n`)
     with inline key directives (`key="..."`), wait tokens (`wait=150`), escape sequences (`\\n`, `\\t`), and 10 dynamic variables.
  3. `execute_per_method_macro_test`: Executes any user Smart Key (`ComboStep` list) or Saved Recording (`MacroEvent` list)
     strictly through each method `1..20` with optional template prefix.
"""

from __future__ import annotations
import ctypes
from ctypes import wintypes
from dataclasses import asdict, dataclass, field
from datetime import datetime
import json
import logging
import random
import re
import threading
import time
from pathlib import Path
from typing import Any, Callable, Dict, List, Optional, Tuple

import app.domain.win32_input as _w32
from app.domain.win32_input import (
    IS_WINDOWS,
    OUR_PID,
    STEALTH_TRACKER,
    _HW_BRIDGE,
    get_cursor_pos,
    is_hwnd_own_process,
    is_own_app_foreground,
    scancode_for_label,
    send_mouse_button,
    send_mouse_relative_move,
    send_scancode_key,
    send_unicode_text,
    user32,
    kernel32,
    vk_for_scancode,
)
from app.paths import get_appdata_dir

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
MOUSEEVENTF_VIRTUALDESK = 0x4000
MOUSEEVENTF_ABSOLUTE = 0x8000
XBUTTON1 = 0x0001
XBUTTON2 = 0x0002
INPUT = getattr(_w32, "INPUT", None)
KEYBDINPUT = getattr(_w32, "KEYBDINPUT", None)
MOUSEINPUT = getattr(_w32, "MOUSEINPUT", None)
_INPUT_UNION = getattr(_w32, "_INPUT_UNION", None)

logger = logging.getLogger(__name__)

# Built-in 36 Keys (3 rows x 12 columns — 100% identical to V-AutoKeyPresser)
BUILTIN_36_KEYS: List[str] = [
    # Row 1: F1 - F12
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
    # Row 2: Top number row
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",
    # Row 3: Numpad row
    "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Num0", "Num/", "Num*",
]

# Unified 20 Key-Pressing & Hardware Input Methods (Shared across Settings, Test Lab & Auto Presser)
AUTOPRESSER_KEY_METHODS: List[Tuple[str, str]] = [
    ("sendinput_scancode", "method_scancode"),
    ("sendinput_hybrid", "method_hybrid"),
    ("keybd_mouse_event", "method_keybd_event"),
    ("unicode", "akp_method_unicode"),
    ("cpp_stealth_humanized", "method_cpp_stealth"),
    ("cpp_vhid_ioctl_bridge", "method_cpp_vhid"),
    ("cpp_interception_driver", "method_cpp_interception"),
    ("cpp_winring0_ps2_port", "method_cpp_ps2_port"),
    ("cpp_sendinput_batch_atomic", "method_cpp_batch_atomic"),
    ("linux_uinput_xtest", "method_linux_uinput"),
    ("postmessage", "akp_method_postmessage"),
    ("sendmessage", "akp_method_sendmessage"),
    ("post_child", "akp_method_post_child"),
    ("post_children", "akp_method_post_children"),
    ("post_active", "akp_method_post_active"),
    ("attach_state", "akp_method_attach_state"),
    ("guithread_focus", "akp_method_guithread_focus"),
    ("wm_char_triplet", "akp_method_wm_char_triplet"),
    ("ime_pipeline", "akp_method_ime_pipeline"),
    ("sendnotify_tree", "akp_method_sendnotify_tree"),
    ("pico_usb_hid", "method_pico_hid"),
    ("arduino_usb_hid", "method_arduino_hid"),
]

# Foreground-only methods (Methods 1..10) vs Background HWND methods (Methods 11..20)
FOREGROUND_KEY_METHODS = {
    "sendinput",
    "scancode",
    "hybrid_hw",
    "ring0_interception",
    "keybd_event",
    "unicode",
    "sendinput_scancode",
    "sendinput_hybrid",
    "keybd_mouse_event",
    "cpp_stealth_humanized",
    "cpp_vhid_ioctl_bridge",
    "cpp_interception_driver",
    "cpp_winring0_ps2_port",
    "cpp_sendinput_batch_atomic",
    "linux_uinput_xtest",
    "pico_usb_hid",
    "arduino_usb_hid",
}

BACKGROUND_HWND_KEY_METHODS = {
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

# 8 Mouse Click Modes
AUTOPRESSER_MOUSE_MODES: List[Tuple[str, str]] = [
    ("auto", "akp_mouse_mode_auto"),
    ("real", "akp_mouse_mode_real"),
    ("post", "akp_mouse_mode_post"),
    ("post_top", "akp_mouse_mode_post_top"),
    ("post_hittest_chain", "akp_mouse_mode_hittest"),
    ("attach_mouse_state", "akp_mouse_mode_attach"),
    ("guithread_capture", "akp_mouse_mode_guithread"),
    ("sendinput_abs", "akp_mouse_mode_abs"),
]


@dataclass
class AutoPresserKeyItem:
    key: str = "F1"
    enabled: bool = False
    interval_ms: int = 1000
    is_custom: bool = False
    use_smart_combo: bool = False

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)

    @classmethod
    def from_dict(cls, d: Dict[str, Any]) -> "AutoPresserKeyItem":
        return cls(
            key=str(d.get("key", "F1")),
            enabled=bool(d.get("enabled", False)),
            interval_ms=max(10, min(3600000, int(d.get("interval_ms", d.get("delay_ms", 1000))))),
            is_custom=bool(d.get("is_custom", False)),
            use_smart_combo=bool(d.get("use_smart_combo", False)),
        )


@dataclass
class AutoPresserMousePoint:
    name: str = "Click 1"
    x: int = 500
    y: int = 500
    button: str = "left"        # "left", "right", "middle", "x1", "x2"
    interval_ms: int = 1000
    enabled: bool = True

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)

    @classmethod
    def from_dict(cls, d: Dict[str, Any]) -> "AutoPresserMousePoint":
        raw_btn = str(d.get("button", "left")).lower()
        return cls(
            name=str(d.get("name", "Click 1")),
            x=max(-32768, min(32768, int(d.get("x", 500)))),
            y=max(-32768, min(32768, int(d.get("y", 500)))),
            button=raw_btn if raw_btn in ("left", "right", "middle", "x1", "x2") else "left",
            interval_ms=max(10, min(3600000, int(d.get("interval_ms", 1000)))),
            enabled=bool(d.get("enabled", True)),
        )


@dataclass
class AutoPresserTab:
    tab_id: str = "tab_1"
    title: str = "Tab 1"
    target_process: str = ""
    target_hwnd: int = 0
    target_title: str = ""
    run_in_background: bool = True
    key_method: str = "attach_state"
    mouse_mode: str = "auto"
    hold_ms: int = 50
    stop_hotkey: str = "F6"
    mouse_enabled: bool = False
    keys: List[AutoPresserKeyItem] = field(default_factory=list)
    mouse_points: List[AutoPresserMousePoint] = field(default_factory=list)

    def ensure_builtin_keys(self) -> None:
        existing = {k.key: k for k in self.keys}
        ordered: List[AutoPresserKeyItem] = []
        for bk in BUILTIN_36_KEYS:
            if bk in existing:
                item = existing.pop(bk)
                item.is_custom = False
                ordered.append(item)
            else:
                ordered.append(AutoPresserKeyItem(key=bk, enabled=False, interval_ms=1000, is_custom=False))
        for ck in self.keys:
            if ck.key in existing:
                ck.is_custom = True
                ordered.append(ck)
        self.keys = ordered

    def to_dict(self) -> Dict[str, Any]:
        return {
            "tab_id": self.tab_id,
            "title": self.title,
            "target_process": self.target_process,
            "target_hwnd": self.target_hwnd,
            "target_title": self.target_title,
            "run_in_background": self.run_in_background,
            "key_method": self.key_method,
            "mouse_mode": self.mouse_mode,
            "hold_ms": self.hold_ms,
            "stop_hotkey": self.stop_hotkey,
            "mouse_enabled": self.mouse_enabled,
            "keys": [k.to_dict() for k in self.keys],
            "mouse_points": [m.to_dict() for m in self.mouse_points],
        }

    @classmethod
    def from_dict(cls, d: Dict[str, Any]) -> "AutoPresserTab":
        raw_keys = d.get("keys", [])
        keys = [AutoPresserKeyItem.from_dict(k) for k in raw_keys if isinstance(k, dict)]
        raw_mp = d.get("mouse_points", [])
        mps = [AutoPresserMousePoint.from_dict(m) for m in raw_mp if isinstance(m, dict)]
        tab = cls(
            tab_id=str(d.get("tab_id", f"tab_{int(time.time() * 1000)}")),
            title=str(d.get("title", "Tab 1")),
            target_process=str(d.get("target_process", "")),
            target_hwnd=int(d.get("target_hwnd", 0)),
            target_title=str(d.get("target_title", "")),
            run_in_background=bool(d.get("run_in_background", True)),
            key_method=str(d.get("key_method", "attach_state")),
            mouse_mode=str(d.get("mouse_mode", "auto")),
            hold_ms=max(10, min(2000, int(d.get("hold_ms", 50)))),
            stop_hotkey=str(d.get("stop_hotkey", "F6")),
            mouse_enabled=bool(d.get("mouse_enabled", False)),
            keys=keys,
            mouse_points=mps,
        )
        tab.ensure_builtin_keys()
        return tab


# ==============================================================================
# Low-Level Win32 Background & Foreground Dispatch Helpers (20 Key Methods + 8 Mouse Modes)
# ==============================================================================
WM_KEYDOWN = 0x0100
WM_KEYUP = 0x0101
WM_CHAR = 0x0102
WM_IME_KEYDOWN = 0x0290
WM_IME_KEYUP = 0x0291
WM_IME_CHAR = 0x0286
WM_ACTIVATE = 0x0006
WM_SETFOCUS = 0x0007
WM_NCHITTEST = 0x0084
WM_SETCURSOR = 0x0020
WM_MOUSEACTIVATE = 0x0021
WM_MOUSEMOVE = 0x0200
WM_LBUTTONDOWN = 0x0201
WM_LBUTTONUP = 0x0202
WM_RBUTTONDOWN = 0x0204
WM_RBUTTONUP = 0x0205
WM_MBUTTONDOWN = 0x0207
WM_MBUTTONUP = 0x0208
WM_XBUTTONDOWN = 0x020B
WM_XBUTTONUP = 0x020C
SMTO_ABORTIFHUNG = 0x0002
SMTO_NORMAL = 0x0000


class GUITHREADINFO(ctypes.Structure):
    _fields_ = [
        ("cbSize", wintypes.DWORD),
        ("flags", wintypes.DWORD),
        ("hwndActive", wintypes.HWND),
        ("hwndFocus", wintypes.HWND),
        ("hwndCapture", wintypes.HWND),
        ("hwndMenuOwner", wintypes.HWND),
        ("hwndMoveSize", wintypes.HWND),
        ("hwndCaret", wintypes.HWND),
        ("rcCaret", wintypes.RECT),
    ]


def format_diagnostic_reason(entry: Dict[str, Any]) -> str:
    """
    Render a localized, BiDi-clean diagnostic explanation for any sweep or live test entry
    in the currently active UI language (`t(...)`).
    """
    from app.i18n import t as _t

    rc = str(entry.get("reason_code", ""))
    engine = str(entry.get("method_id", entry.get("method", entry.get("mode", ""))))
    child = str(entry.get("child_cls", "")) or "TopWindow"
    btn = str(entry.get("btn", "left")).upper()
    sx = str(entry.get("x", entry.get("screen_x", 0)))
    sy = str(entry.get("y", entry.get("screen_y", 0)))
    cx = str(entry.get("client_x", sx))
    cy = str(entry.get("client_y", sy))

    if rc == "delivered_fg":
        return _t("log_reason_delivered_fg").replace("{engine}", engine)
    if rc == "delivered_bg":
        return _t("log_reason_delivered_bg").replace("{engine}", engine).replace("{child}", child)
    if rc == "skipped_fg_in_bg":
        return _t("log_reason_skipped_fg_in_bg")
    if rc == "interception_missing":
        return _t("log_reason_interception_missing")
    if rc == "ps2_missing":
        return _t("log_reason_ps2_missing")
    if rc == "linux_on_win":
        return _t("log_reason_linux_on_win")
    if rc == "pico_missing":
        return _t("log_reason_pico_missing")
    if rc == "arduino_missing":
        return _t("log_reason_arduino_missing")
    if rc == "self_app_protected":
        return _t("log_reason_self_app_protected")
    if rc == "hwnd_rejected":
        return _t("log_reason_hwnd_rejected")
    if rc == "mouse_bg_ok":
        return (
            _t("log_reason_mouse_bg_ok")
            .replace("{btn}", btn)
            .replace("{mode}", engine)
            .replace("{sx}", sx)
            .replace("{sy}", sy)
            .replace("{cx}", cx)
            .replace("{cy}", cy)
            .replace("{child}", child)
        )
    if rc == "mouse_fg_ok":
        return (
            _t("log_reason_mouse_fg_ok")
            .replace("{btn}", btn)
            .replace("{mode}", engine)
            .replace("{sx}", sx)
            .replace("{sy}", sy)
        )
    if rc == "mouse_skipped_bg":
        return _t("log_reason_mouse_skipped_bg").replace("{mode}", engine)

    # Fallback for custom Python/Template/Macro sweep details
    raw_reason = str(entry.get("reason", ""))
    if "Self-Protection" in raw_reason:
        return _t("log_reason_self_app_protected")
    if "Foreground-only method skipped" in raw_reason:
        return _t("log_reason_skipped_fg_in_bg")
    if "Interception Ring-0" in raw_reason:
        return _t("log_reason_interception_missing")
    if "InpOutx64 PS/2" in raw_reason:
        return _t("log_reason_ps2_missing")
    if "Linux /dev/uinput" in raw_reason:
        return _t("log_reason_linux_on_win")
    if "Raspberry Pi Pico" in raw_reason:
        return _t("log_reason_pico_missing")
    if "Arduino" in raw_reason:
        return _t("log_reason_arduino_missing")
    if "rejected" in raw_reason.lower():
        return _t("log_reason_hwnd_rejected")
    if raw_reason.startswith("Delivered via "):
        if engine in BACKGROUND_HWND_KEY_METHODS:
            return _t("log_reason_delivered_bg").replace("{engine}", engine).replace("{child}", child)
        return _t("log_reason_delivered_fg").replace("{engine}", engine)
    return raw_reason


def list_detailed_windows_for_picker() -> List[Dict[str, Any]]:
    """
    Enumerate visible top-level windows with `(hwnd, proc_name, window_title, pid, exe_path)`
    for the ProcessPickerDialog in AutoKeyPresserPage and TestPage.
    """
    results: List[Dict[str, Any]] = []
    if not IS_WINDOWS:
        return results
    try:
        WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

        def _cb(hwnd: int, _: int) -> bool:
            if not user32.IsWindowVisible(hwnd):
                return True
            length = user32.GetWindowTextLengthW(hwnd)
            if length <= 0:
                return True
            buf = ctypes.create_unicode_buffer(length + 1)
            user32.GetWindowTextW(hwnd, buf, length + 1)
            title = buf.value.strip()
            if not title:
                return True
            pid = wintypes.DWORD(0)
            user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
            if not pid.value or int(pid.value) == int(OUR_PID):
                return True
            hproc = kernel32.OpenProcess(0x1000, False, pid.value)
            exe_path = ""
            proc_name = f"PID-{pid.value}"
            if hproc:
                try:
                    pbuf = ctypes.create_unicode_buffer(1024)
                    sz = wintypes.DWORD(1024)
                    if kernel32.QueryFullProcessImageNameW(hproc, 0, pbuf, ctypes.byref(sz)):
                        exe_path = pbuf.value
                        proc_name = Path(exe_path).name
                finally:
                    kernel32.CloseHandle(hproc)
            results.append(
                {
                    "hwnd": int(hwnd),
                    "pid": int(pid.value),
                    "proc_name": proc_name,
                    "title": title,
                    "exe_path": exe_path,
                }
            )
            return True

        user32.EnumWindows(WNDENUMPROC(_cb), 0)
    except Exception:
        pass
    return results


def focus_and_bring_hwnd_to_front(hwnd: int) -> bool:
    """
    Restore `hwnd` if minimized (`SW_RESTORE = 9`) and bring it to the foreground
    (`BringWindowToTop` + `SetForegroundWindow` + `SetActiveWindow`) when clicking `Switch`
    or running a foreground test. Never attaches to V-MacroPlus's own GUI thread.
    """
    if not IS_WINDOWS or not hwnd:
        return True
    try:
        if not user32.IsWindow(hwnd) or is_hwnd_own_process(hwnd):
            return False
        if user32.IsIconic(hwnd):
            user32.ShowWindow(hwnd, 9)  # SW_RESTORE
        cur_tid = kernel32.GetCurrentThreadId()
        fg_hwnd = user32.GetForegroundWindow()
        fg_tid = (
            user32.GetWindowThreadProcessId(fg_hwnd, None)
            if (fg_hwnd and not is_hwnd_own_process(fg_hwnd))
            else 0
        )
        target_tid = user32.GetWindowThreadProcessId(hwnd, None)
        attached_fg = False
        attached_tg = False
        if fg_tid and fg_tid != cur_tid:
            attached_fg = bool(user32.AttachThreadInput(cur_tid, fg_tid, True))
        if target_tid and target_tid != cur_tid and target_tid != fg_tid:
            attached_tg = bool(user32.AttachThreadInput(cur_tid, target_tid, True))
        try:
            user32.BringWindowToTop(hwnd)
            user32.SetForegroundWindow(hwnd)
            user32.SetActiveWindow(hwnd)
        finally:
            if attached_tg:
                user32.AttachThreadInput(cur_tid, target_tid, False)
            if attached_fg:
                user32.AttachThreadInput(cur_tid, fg_tid, False)
        time.sleep(0.06)
        return True
    except Exception:
        return False


def is_hwnd_or_child_foreground(hwnd: int) -> bool:
    """Return True if `hwnd` (or a child/owned window of the same process) is currently in the foreground."""
    if not IS_WINDOWS or not hwnd:
        return True
    try:
        fg = int(user32.GetForegroundWindow() or 0)
        if not fg:
            return False
        if fg == int(hwnd) or bool(user32.IsChild(hwnd, fg)):
            return True
        pid_fg = wintypes.DWORD(0)
        pid_tg = wintypes.DWORD(0)
        user32.GetWindowThreadProcessId(fg, ctypes.byref(pid_fg))
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid_tg))
        return bool(pid_fg.value and pid_fg.value == pid_tg.value)
    except Exception:
        return False


def resolve_target_hwnd(tab: AutoPresserTab) -> int:
    """Resolve or auto-rebind the target HWND for `tab` if the window handle changed (never returns V-MacroPlus's own HWND)."""
    if not IS_WINDOWS:
        return 0
    if tab.target_hwnd and user32.IsWindow(tab.target_hwnd):
        if is_hwnd_own_process(tab.target_hwnd):
            tab.target_hwnd = 0
            return 0
        return tab.target_hwnd
    if not tab.target_process:
        fg = int(user32.GetForegroundWindow() or 0)
        if fg and is_hwnd_own_process(fg):
            return 0
        return fg
    target_low = tab.target_process.lower().strip()
    for win in list_detailed_windows_for_picker():
        if win["proc_name"].lower() == target_low:
            tab.target_hwnd = win["hwnd"]
            tab.target_title = win["title"]
            return tab.target_hwnd
    return 0


def _enum_child_hwnds(parent_hwnd: int, limit: int = 64) -> List[int]:
    children: List[int] = []
    if not IS_WINDOWS or not parent_hwnd:
        return children
    try:
        WNDENUMPROC = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

        def _cb(ch: int, _: int) -> bool:
            if len(children) >= limit:
                return False
            children.append(int(ch))
            return True

        user32.EnumChildWindows(parent_hwnd, WNDENUMPROC(_cb), 0)
    except Exception:
        pass
    return children


def _get_window_class_name(hwnd: int) -> str:
    if not IS_WINDOWS or not hwnd:
        return ""
    try:
        buf = ctypes.create_unicode_buffer(256)
        if user32.GetClassNameW(hwnd, buf, 256) > 0:
            return buf.value
    except Exception:
        pass
    return ""


def _get_guithread_focus_hwnd(hwnd: int) -> int:
    """Query `GetGUIThreadInfo` for the target window's thread to discover its focused/active child control."""
    if not IS_WINDOWS or not hwnd:
        return hwnd
    try:
        tid = user32.GetWindowThreadProcessId(hwnd, None)
        if not tid:
            return hwnd
        gti = GUITHREADINFO()
        gti.cbSize = ctypes.sizeof(GUITHREADINFO)
        if user32.GetGUIThreadInfo(tid, ctypes.byref(gti)):
            if gti.hwndFocus and user32.IsWindow(gti.hwndFocus):
                return int(gti.hwndFocus)
            if gti.hwndCaret and user32.IsWindow(gti.hwndCaret):
                return int(gti.hwndCaret)
            if gti.hwndActive and user32.IsWindow(gti.hwndActive):
                return int(gti.hwndActive)
    except Exception:
        pass
    return hwnd


def _find_best_input_child_hwnd(parent_hwnd: int) -> int:
    """
    Find the best inner input/editor/render child control inside `parent_hwnd`
    (e.g., Windows 11 Notepad `RichEditD2DPT`, classic Notepad `Edit`, `Scintilla`,
    `Chrome_RenderWidgetHostHWND`, `Intermediate D3D Window`, `UnityWndClass`, `UnrealWindow`).
    Falls back to `_get_guithread_focus_hwnd(parent_hwnd)` or the largest visible child window.
    """
    if not IS_WINDOWS or not parent_hwnd:
        return parent_hwnd
    children = _enum_child_hwnds(parent_hwnd, 96)
    if not children:
        return parent_hwnd

    priority_tokens = (
        "richeditd2dpt",
        "richedit",
        "scintilla",
        "edit",
        "chrome_renderwidgethosthwnd",
        "intermediate d3d window",
        "cefbrowserwindow",
        "sdl_app",
        "unitywndclass",
        "unrealwindow",
        "canvas",
    )
    for tok in priority_tokens:
        for ch in children:
            if not user32.IsWindowVisible(ch):
                continue
            cls_low = _get_window_class_name(ch).lower()
            if tok in cls_low:
                return ch

    focus_h = _get_guithread_focus_hwnd(parent_hwnd)
    if focus_h and focus_h != parent_hwnd and user32.IsWindow(focus_h):
        return focus_h

    # Otherwise pick the largest visible child control by client area
    best_ch = parent_hwnd
    best_area = -1
    rect = wintypes.RECT()
    for ch in children:
        try:
            if not user32.IsWindowVisible(ch):
                continue
            if user32.GetClientRect(ch, ctypes.byref(rect)):
                w = max(0, rect.right - rect.left)
                h = max(0, rect.bottom - rect.top)
                area = w * h
                if area > best_area:
                    best_area = area
                    best_ch = ch
        except Exception:
            pass
    return best_ch


def _find_child_at_screen_pt(parent_hwnd: int, screen_x: int, screen_y: int) -> Tuple[int, int, int]:
    """
    Resolve the deepest child control inside `parent_hwnd` at screen coordinates `(screen_x, screen_y)`
    and return `(target_child_hwnd, client_x, client_y)`.
    """
    if not IS_WINDOWS or not parent_hwnd:
        return parent_hwnd, screen_x, screen_y
    try:
        pt = wintypes.POINT(int(screen_x), int(screen_y))
        user32.ScreenToClient(parent_hwnd, ctypes.byref(pt))
        # CWP_SKIPINVISIBLE (0x0001) | CWP_SKIPDISABLED (0x0002) = 0x0003
        ch = user32.ChildWindowFromPointEx(parent_hwnd, pt, 0x0003)
        target = int(ch) if (ch and user32.IsWindow(ch)) else int(parent_hwnd)
        best_in = _find_best_input_child_hwnd(parent_hwnd)
        if target == parent_hwnd and best_in != parent_hwnd:
            target = best_in
        pt_child = wintypes.POINT(int(screen_x), int(screen_y))
        user32.ScreenToClient(target, ctypes.byref(pt_child))
        return target, int(pt_child.x), int(pt_child.y)
    except Exception:
        return parent_hwnd, screen_x, screen_y


def _post_msg_checked(hwnd: int, msg: int, wparam: int, lparam: int) -> bool:
    """Call `PostMessageW` and verify it was accepted by the OS message queue (detects UIPI / invalid HWND)."""
    if not IS_WINDOWS or not hwnd or not user32.IsWindow(hwnd):
        return False
    try:
        kernel32.SetLastError(0)
        res = user32.PostMessageW(hwnd, msg, wparam, lparam)
        return bool(res != 0)
    except Exception:
        return False


def _send_batch_atomic_scancode_tap(scan_code: int, is_ext: bool) -> bool:
    """
    Method 9 (`cpp_sendinput_batch_atomic`): Build a contiguous 2-element `INPUT[2]` array
    (KeyDown + KeyUp) and dispatch both atomically in a single `SendInput(2, ...)` syscall.
    """
    if not IS_WINDOWS:
        return True
    try:
        flags_dn = KEYEVENTF_SCANCODE | (KEYEVENTF_EXTENDEDKEY if is_ext else 0)
        flags_up = flags_dn | KEYEVENTF_KEYUP
        vk = vk_for_scancode(scan_code, is_ext)
        extra = user32.GetMessageExtraInfo()
        arr = (INPUT * 2)()
        arr[0].type = INPUT_KEYBOARD
        arr[0].union.ki = KEYBDINPUT(wVk=vk, wScan=scan_code, dwFlags=flags_dn, time=0, dwExtraInfo=extra)
        arr[1].type = INPUT_KEYBOARD
        arr[1].union.ki = KEYBDINPUT(wVk=vk, wScan=scan_code, dwFlags=flags_up, time=0, dwExtraInfo=extra)
        sent = user32.SendInput(2, ctypes.byref(arr), ctypes.sizeof(INPUT))
        return bool(sent == 2)
    except Exception:
        return False


def _char_for_key_label(key_label: str) -> Optional[int]:
    """Return the `WM_CHAR` UTF-16 code point if `key_label` corresponds to a character/Enter/Space/Tab key."""
    kl = key_label.strip()
    if len(kl) == 1:
        return ord(kl)
    kl_low = kl.lower()
    if kl_low in ("enter", "return", "numenter"):
        return 0x0D
    if kl_low == "space":
        return 0x20
    if kl_low == "tab":
        return 0x09
    return None


def dispatch_autopresser_key(
    key_label: str,
    method: str,
    hold_ms: int = 50,
    target_hwnd: int = 0,
    run_in_background: bool = True,
    strict_no_fallback: bool = False,
    allow_self_test: bool = False,
) -> Tuple[bool, str]:
    """
    Execute a single key press + hold + release using one of V-AutoKeyPresser's 20 methods.
    - Every method `1..20` executes its own genuine protocol (Methods 11..20 ALWAYS execute their
      background/HWND message pipeline even if `run_in_background=False` is passed).
    - When `strict_no_fallback=True` (used during Test / Sweep / Per-Method Python Test), hardware
      drivers that are not installed return `(False, reason)` instead of silently falling back to
      `sendinput_scancode`, and foreground-only methods (`1..10`) refuse to fire into V-MacroPlus
      when `run_in_background=True` and the target window is not focused.
    """
    kl_clean = key_label.strip().lower()
    if kl_clean in ("mouseback", "xbutton1", "mouse_x1", "mouseforward", "xbutton2", "mouse_x2"):
        btn_code = "x1" if ("back" in kl_clean or "1" in kl_clean) else "x2"
        cx, cy = get_cursor_pos()
        mp_temp = AutoPresserMousePoint(name=key_label, x=cx, y=cy, button=btn_code, interval_ms=1000, enabled=True)
        m_mode = "post" if (run_in_background and target_hwnd) else "real"
        ok_m = dispatch_autopresser_mouse(
            mp_temp,
            mouse_mode=m_mode,
            target_hwnd=target_hwnd,
            run_in_background=run_in_background,
            strict_no_fallback=strict_no_fallback,
        )
        return ok_m, f"mouse_{btn_code}_{m_mode}"

    sc_label = key_label.replace("Num/", "NumDivide").replace("Num*", "NumMultiply")
    scan_code, is_ext = scancode_for_label(sc_label)
    vk = vk_for_scancode(scan_code, is_ext)
    hold_s = max(0.010, min(2.0, hold_ms / 1000.0))

    if not IS_WINDOWS:
        if method == "linux_uinput_xtest":
            time.sleep(hold_s)
            return True, "linux_uinput_xtest"
        time.sleep(hold_s)
        return True, method

    # Self-Protection Guard: NEVER inject into V-MacroPlus unless explicitly testing in Test Lab Sandbox
    if not allow_self_test and target_hwnd and is_hwnd_own_process(target_hwnd):
        return False, "Self-Protection — Blocked injecting into V-MacroPlus window"

    STEALTH_TRACKER.record_key(scan_code, True)

    # 1. Check if `method` is one of the 10 Foreground Hardware / OS Queue methods (Methods 1..10)
    if method in FOREGROUND_KEY_METHODS:
        mapped = {
            "sendinput": "sendinput_hybrid",
            "scancode": "sendinput_scancode",
            "hybrid_hw": "cpp_stealth_humanized",
            "ring0_interception": "cpp_interception_driver",
            "keybd_event": "keybd_mouse_event",
        }.get(method, method)

        if strict_no_fallback:
            if mapped == "cpp_interception_driver" and not _HW_BRIDGE.is_interception_ring0_ready():
                return False, "Interception Ring-0 Driver not installed"
            if mapped == "cpp_winring0_ps2_port" and not _HW_BRIDGE.is_ps2_port60_ready():
                return False, "InpOutx64 PS/2 Port 0x60 Driver not loaded"
            if mapped == "linux_uinput_xtest" and IS_WINDOWS:
                return False, "Linux /dev/uinput not available on Windows"
            if mapped == "pico_usb_hid":
                from app.domain.microcontroller_bridge import MCU_BRIDGE
                if not MCU_BRIDGE.is_board_ready("pico"):
                    return False, "Raspberry Pi Pico USB HID board not connected"
            if mapped == "arduino_usb_hid":
                from app.domain.microcontroller_bridge import MCU_BRIDGE
                if not MCU_BRIDGE.is_board_ready("arduino"):
                    return False, "Arduino Leonardo/Pro Micro USB HID board not connected"

        if run_in_background and target_hwnd:
            if not is_hwnd_or_child_foreground(target_hwnd):
                if strict_no_fallback:
                    return False, "Foreground-only method skipped in Background Mode (Target window not focused)"
                return False, "Target window not focused (Background Mode active)"
        elif target_hwnd and not run_in_background:
            if not is_hwnd_or_child_foreground(target_hwnd):
                focus_and_bring_hwnd_to_front(target_hwnd)

        if not allow_self_test and is_own_app_foreground():
            return False, "Self-Protection — V-MacroPlus window is in the foreground"

        # Method 4: Unicode packet injection (`KEYEVENTF_UNICODE`)
        if mapped == "unicode":
            ch_code = _char_for_key_label(key_label)
            if ch_code is not None and len(key_label.strip()) == 1:
                send_unicode_text(key_label.strip())
                return True, "unicode"
            ok_dn = send_scancode_key(scan_code, is_ext, key_up=False, method="sendinput_scancode", strict_no_fallback=strict_no_fallback)
            time.sleep(hold_s)
            STEALTH_TRACKER.record_key(scan_code, False)
            ok_up = send_scancode_key(scan_code, is_ext, key_up=True, method="sendinput_scancode", strict_no_fallback=strict_no_fallback)
            return bool(ok_dn and ok_up), "unicode"

        # Method 9: Atomic Batch SendInput (`INPUT[2]` in a single syscall)
        if mapped == "cpp_sendinput_batch_atomic":
            ok_batch = _send_batch_atomic_scancode_tap(scan_code, is_ext)
            STEALTH_TRACKER.record_key(scan_code, False)
            return ok_batch, "cpp_sendinput_batch_atomic"

        ok_dn = send_scancode_key(scan_code, is_ext, key_up=False, method=mapped, strict_no_fallback=strict_no_fallback)
        time.sleep(hold_s)
        STEALTH_TRACKER.record_key(scan_code, False)
        ok_up = send_scancode_key(scan_code, is_ext, key_up=True, method=mapped, strict_no_fallback=strict_no_fallback)
        return bool(ok_dn and ok_up), mapped

    # 2. Background Window Message & Anti-Protection Methods (Methods 11..20)
    hwnd = int(target_hwnd or (user32.GetForegroundWindow() or 0))
    if not allow_self_test and hwnd and is_hwnd_own_process(hwnd):
        return False, "Self-Protection — Blocked injecting into V-MacroPlus window"
    if not hwnd or not user32.IsWindow(hwnd):
        if strict_no_fallback:
            return False, "No valid target HWND available"
        ok_dn = send_scancode_key(scan_code, is_ext, key_up=False, method="sendinput_scancode")
        time.sleep(hold_s)
        ok_up = send_scancode_key(scan_code, is_ext, key_up=True, method="sendinput_scancode")
        return bool(ok_dn and ok_up), "sendinput_scancode"

    best_child = _find_best_input_child_hwnd(hwnd)
    lparam_dn = 1 | ((scan_code & 0xFF) << 16) | ((1 << 24) if is_ext else 0)
    lparam_up = lparam_dn | (1 << 30) | (1 << 31)
    ch_ord = _char_for_key_label(key_label)

    try:
        if method == "postmessage":
            dest = best_child if best_child else hwnd
            ok1 = _post_msg_checked(dest, WM_KEYDOWN, vk, lparam_dn)
            if ch_ord is not None and len(key_label.strip()) == 1:
                _post_msg_checked(dest, WM_CHAR, ch_ord, lparam_dn)
            time.sleep(hold_s)
            ok2 = _post_msg_checked(dest, WM_KEYUP, vk, lparam_up)
            return bool(ok1 and ok2), "postmessage"

        if method == "sendmessage":
            dest = best_child if best_child else hwnd
            res = wintypes.DWORD(0)
            kernel32.SetLastError(0)
            r1 = user32.SendMessageTimeoutW(dest, WM_KEYDOWN, vk, lparam_dn, SMTO_ABORTIFHUNG, 80, ctypes.byref(res))
            if ch_ord is not None and len(key_label.strip()) == 1:
                user32.SendMessageTimeoutW(dest, WM_CHAR, ch_ord, lparam_dn, SMTO_ABORTIFHUNG, 80, ctypes.byref(res))
            time.sleep(hold_s)
            r2 = user32.SendMessageTimeoutW(dest, WM_KEYUP, vk, lparam_up, SMTO_ABORTIFHUNG, 80, ctypes.byref(res))
            return bool(r1 != 0 and r2 != 0), "sendmessage"

        if method == "post_child":
            children = _enum_child_hwnds(hwnd, 64)
            target = best_child if best_child != hwnd else (children[0] if children else hwnd)
            ok1 = _post_msg_checked(target, WM_KEYDOWN, vk, lparam_dn)
            if ch_ord is not None and len(key_label.strip()) == 1:
                _post_msg_checked(target, WM_CHAR, ch_ord, lparam_dn)
            time.sleep(hold_s)
            ok2 = _post_msg_checked(target, WM_KEYUP, vk, lparam_up)
            return bool(ok1 and ok2), "post_child"

        if method in ("post_children", "sendnotify_tree"):
            children = _enum_child_hwnds(hwnd, 48)
            if ch_ord is not None and best_child != hwnd:
                targets = [best_child] if best_child else [hwnd]
            else:
                targets = [hwnd] + [c for c in children if c != hwnd]
            any_ok = False
            for t_h in targets:
                if method == "sendnotify_tree":
                    r = user32.SendNotifyMessageW(t_h, WM_KEYDOWN, vk, lparam_dn)
                    if ch_ord is not None and len(key_label.strip()) == 1:
                        user32.SendNotifyMessageW(t_h, WM_CHAR, ch_ord, lparam_dn)
                    any_ok = any_ok or bool(r != 0)
                else:
                    r = _post_msg_checked(t_h, WM_KEYDOWN, vk, lparam_dn)
                    if ch_ord is not None and len(key_label.strip()) == 1:
                        _post_msg_checked(t_h, WM_CHAR, ch_ord, lparam_dn)
                    any_ok = any_ok or r
            time.sleep(hold_s)
            for t_h in targets:
                if method == "sendnotify_tree":
                    user32.SendNotifyMessageW(t_h, WM_KEYUP, vk, lparam_up)
                else:
                    _post_msg_checked(t_h, WM_KEYUP, vk, lparam_up)
            return bool(any_ok), method

        if method == "post_active":
            dest = best_child if best_child else hwnd
            _post_msg_checked(hwnd, WM_ACTIVATE, 1, 0)
            _post_msg_checked(dest, WM_SETFOCUS, 0, 0)
            ok1 = _post_msg_checked(dest, WM_KEYDOWN, vk, lparam_dn)
            if ch_ord is not None and len(key_label.strip()) == 1:
                _post_msg_checked(dest, WM_CHAR, ch_ord, lparam_dn)
            time.sleep(hold_s)
            ok2 = _post_msg_checked(dest, WM_KEYUP, vk, lparam_up)
            return bool(ok1 and ok2), "post_active"

        if method == "attach_state":
            dest = best_child if best_child else hwnd
            cur_tid = kernel32.GetCurrentThreadId()
            target_tid = user32.GetWindowThreadProcessId(dest, None)
            can_attach = bool(target_tid and target_tid != cur_tid and not is_hwnd_own_process(dest))
            kbd_state = (ctypes.c_ubyte * 256)()
            attached = False
            if can_attach:
                attached = bool(user32.AttachThreadInput(cur_tid, target_tid, True))
            try:
                if attached:
                    user32.GetKeyboardState(kbd_state)
                    kbd_state[vk & 0xFF] |= 0x80
                    user32.SetKeyboardState(kbd_state)
                ok1 = _post_msg_checked(dest, WM_KEYDOWN, vk, lparam_dn)
                if ch_ord is not None and len(key_label.strip()) == 1:
                    _post_msg_checked(dest, WM_CHAR, ch_ord, lparam_dn)
            finally:
                if attached:
                    user32.AttachThreadInput(cur_tid, target_tid, False)
                    attached = False

            time.sleep(hold_s)

            if can_attach:
                attached = bool(user32.AttachThreadInput(cur_tid, target_tid, True))
            try:
                if attached:
                    user32.GetKeyboardState(kbd_state)
                    kbd_state[vk & 0xFF] &= ~0x80
                    user32.SetKeyboardState(kbd_state)
                ok2 = _post_msg_checked(dest, WM_KEYUP, vk, lparam_up)
            finally:
                if attached:
                    user32.AttachThreadInput(cur_tid, target_tid, False)
            return bool(ok1 and ok2), "attach_state"

        if method == "guithread_focus":
            gti_target = _get_guithread_focus_hwnd(hwnd)
            if gti_target == hwnd and best_child != hwnd:
                gti_target = best_child
            ok1 = _post_msg_checked(gti_target, WM_KEYDOWN, vk, lparam_dn)
            if ch_ord is not None and len(key_label.strip()) == 1:
                _post_msg_checked(gti_target, WM_CHAR, ch_ord, lparam_dn)
            time.sleep(hold_s)
            ok2 = _post_msg_checked(gti_target, WM_KEYUP, vk, lparam_up)
            return bool(ok1 and ok2), "guithread_focus"

        if method == "wm_char_triplet":
            dest = best_child if best_child else hwnd
            ok1 = _post_msg_checked(dest, WM_KEYDOWN, vk, lparam_dn)
            if ch_ord is not None:
                _post_msg_checked(dest, WM_CHAR, ch_ord, lparam_dn)
            time.sleep(hold_s)
            ok2 = _post_msg_checked(dest, WM_KEYUP, vk, lparam_up)
            return bool(ok1 and ok2), "wm_char_triplet"

        if method == "ime_pipeline":
            dest = best_child if best_child else hwnd
            ok1 = _post_msg_checked(dest, WM_IME_KEYDOWN, vk, lparam_dn)
            if ch_ord is not None and len(key_label.strip()) == 1:
                _post_msg_checked(dest, WM_IME_CHAR, ch_ord, lparam_dn)
            time.sleep(hold_s)
            ok2 = _post_msg_checked(dest, WM_IME_KEYUP, vk, lparam_up)
            return bool(ok1 and ok2), "ime_pipeline"

        dest = best_child if best_child else hwnd
        ok1 = _post_msg_checked(dest, WM_KEYDOWN, vk, lparam_dn)
        time.sleep(hold_s)
        ok2 = _post_msg_checked(dest, WM_KEYUP, vk, lparam_up)
        return bool(ok1 and ok2), method
    except Exception as exc:
        return False, str(exc)


def dispatch_text_via_method(
    text: str,
    method: str,
    target_hwnd: int = 0,
    run_in_background: bool = False,
    strict_no_fallback: bool = True,
    human: bool = False,
    char_delay_ms: int = 0,
    allow_self_test: bool = False,
) -> Tuple[bool, str]:
    """
    Type `text` strictly using the specified `method` (`1..20`) rather than always calling
    `send_unicode_text`. This guarantees that during a 20-method sweep or custom Python/Template
    test, ONLY the methods that genuinely work on the target window will write their text into it!
    """
    if not text:
        return True, method
    if not IS_WINDOWS:
        return True, method

    mapped = {
        "sendinput": "sendinput_hybrid",
        "scancode": "sendinput_scancode",
        "hybrid_hw": "cpp_stealth_humanized",
        "ring0_interception": "cpp_interception_driver",
        "keybd_event": "keybd_mouse_event",
    }.get(method, method)

    if strict_no_fallback:
        if mapped == "cpp_interception_driver" and not _HW_BRIDGE.is_interception_ring0_ready():
            return False, "Interception Ring-0 Driver not installed"
        if mapped == "cpp_winring0_ps2_port" and not _HW_BRIDGE.is_ps2_port60_ready():
            return False, "InpOutx64 PS/2 Port 0x60 Driver not loaded"
        if mapped == "linux_uinput_xtest" and IS_WINDOWS:
            return False, "Linux /dev/uinput not available on Windows"
        if mapped == "pico_usb_hid":
            from app.domain.microcontroller_bridge import MCU_BRIDGE
            if not MCU_BRIDGE.is_board_ready("pico"):
                return False, "Raspberry Pi Pico USB HID board not connected"
        if mapped == "arduino_usb_hid":
            from app.domain.microcontroller_bridge import MCU_BRIDGE
            if not MCU_BRIDGE.is_board_ready("arduino"):
                return False, "Arduino Leonardo/Pro Micro USB HID board not connected"

    if not allow_self_test and target_hwnd and is_hwnd_own_process(target_hwnd):
        return False, "Self-Protection — Blocked injecting into V-MacroPlus window"

    # Foreground methods (1..10)
    if mapped in FOREGROUND_KEY_METHODS:
        if run_in_background and target_hwnd and not is_hwnd_or_child_foreground(target_hwnd):
            return False, "Foreground-only method skipped in Background Mode (Target window not focused)"
        if target_hwnd and not run_in_background and not is_hwnd_or_child_foreground(target_hwnd):
            focus_and_bring_hwnd_to_front(target_hwnd)
        if not allow_self_test and is_own_app_foreground():
            return False, "Self-Protection — V-MacroPlus window is in the foreground"

        if mapped == "unicode":
            send_unicode_text(text)
            return True, "unicode"

        shift_sc, shift_ext = scancode_for_label("Shift")
        shift_symbols = {
            "_": "-", "+": "=", ":": ";", '"': "'", "<": ",", ">": ".", "?": "/",
            "!": "1", "@": "2", "#": "3", "$": "4", "%": "5", "^": "6", "&": "7", "*": "8", "(": "9", ")": "0",
            "{": "[", "}": "]", "|": "\\",
        }
        any_ok = False
        for ch in text:
            if ch == "\n":
                ok_k, _ = dispatch_autopresser_key(
                    "Enter",
                    method=mapped,
                    hold_ms=10,
                    target_hwnd=target_hwnd,
                    run_in_background=run_in_background,
                    strict_no_fallback=strict_no_fallback,
                    allow_self_test=allow_self_test,
                )
                any_ok = any_ok or ok_k
                continue
            if ch == "\t":
                ok_k, _ = dispatch_autopresser_key(
                    "Tab",
                    method=mapped,
                    hold_ms=10,
                    target_hwnd=target_hwnd,
                    run_in_background=run_in_background,
                    strict_no_fallback=strict_no_fallback,
                    allow_self_test=allow_self_test,
                )
                any_ok = any_ok or ok_k
                continue
            if ch == " ":
                ok_k, _ = dispatch_autopresser_key(
                    "Space",
                    method=mapped,
                    hold_ms=10,
                    target_hwnd=target_hwnd,
                    run_in_background=run_in_background,
                    strict_no_fallback=strict_no_fallback,
                    allow_self_test=allow_self_test,
                )
                any_ok = any_ok or ok_k
                continue

            needs_shift = ch.isupper() or (ch in shift_symbols)
            base_lbl = shift_symbols.get(ch, ch.upper())
            sc, ext = scancode_for_label(base_lbl)
            if sc == 0:
                continue
            hold_s = (random.uniform(0.042, 0.078) if human else 0.006)
            gap_s = (random.uniform(0.045, 0.092) if human else (max(0.004, char_delay_ms / 1000.0) if char_delay_ms > 0 else 0.004))
            if needs_shift:
                send_scancode_key(shift_sc, shift_ext, key_up=False, method=mapped, strict_no_fallback=strict_no_fallback)
                if human:
                    time.sleep(random.uniform(0.012, 0.025))
            ok_dn = send_scancode_key(sc, ext, key_up=False, method=mapped, strict_no_fallback=strict_no_fallback)
            time.sleep(hold_s)
            ok_up = send_scancode_key(sc, ext, key_up=True, method=mapped, strict_no_fallback=strict_no_fallback)
            if needs_shift:
                if human:
                    time.sleep(random.uniform(0.008, 0.018))
                send_scancode_key(shift_sc, shift_ext, key_up=True, method=mapped, strict_no_fallback=strict_no_fallback)
            if not (ok_dn and ok_up) and strict_no_fallback:
                return False, f"{mapped} hardware dispatch rejected"
            any_ok = any_ok or bool(ok_dn and ok_up)
            time.sleep(gap_s)
        return bool(any_ok), mapped

    # Background HWND methods (11..20)
    hwnd = int(target_hwnd or (user32.GetForegroundWindow() or 0))
    if not allow_self_test and hwnd and is_hwnd_own_process(hwnd):
        return False, "Self-Protection — Blocked injecting into V-MacroPlus window"
    if not hwnd or not user32.IsWindow(hwnd):
        return False, "No valid target HWND available"

    best_child = _find_best_input_child_hwnd(hwnd)
    if mapped == "guithread_focus":
        dest = _get_guithread_focus_hwnd(hwnd)
        if dest == hwnd and best_child != hwnd:
            dest = best_child
    elif mapped == "post_child":
        children = _enum_child_hwnds(hwnd, 64)
        dest = best_child if best_child != hwnd else (children[0] if children else hwnd)
    else:
        dest = best_child if best_child else hwnd

    if mapped == "post_active":
        _post_msg_checked(hwnd, WM_ACTIVATE, 1, 0)
        _post_msg_checked(dest, WM_SETFOCUS, 0, 0)

    cur_tid = kernel32.GetCurrentThreadId()
    target_tid = user32.GetWindowThreadProcessId(dest, None)
    attached = False
    if mapped == "attach_state" and target_tid and target_tid != cur_tid and not is_hwnd_own_process(dest):
        attached = bool(user32.AttachThreadInput(cur_tid, target_tid, True))

    try:
        any_ok = False
        for ch in text:
            if ch == "\n":
                ok_e, _ = dispatch_autopresser_key(
                    "Enter",
                    method=mapped,
                    hold_ms=10,
                    target_hwnd=hwnd,
                    run_in_background=True,
                    strict_no_fallback=strict_no_fallback,
                    allow_self_test=allow_self_test,
                )
                any_ok = any_ok or ok_e
                continue
            if ch == "\t":
                ok_e, _ = dispatch_autopresser_key(
                    "Tab",
                    method=mapped,
                    hold_ms=10,
                    target_hwnd=hwnd,
                    run_in_background=True,
                    strict_no_fallback=strict_no_fallback,
                    allow_self_test=allow_self_test,
                )
                any_ok = any_ok or ok_e
                continue
            cp = ord(ch)
            if mapped == "sendmessage":
                res = wintypes.DWORD(0)
                r = user32.SendMessageTimeoutW(dest, WM_CHAR, cp, 1, SMTO_ABORTIFHUNG, 60, ctypes.byref(res))
                any_ok = any_ok or bool(r != 0)
            elif mapped == "sendnotify_tree":
                r = user32.SendNotifyMessageW(dest, WM_CHAR, cp, 1)
                any_ok = any_ok or bool(r != 0)
            elif mapped == "ime_pipeline":
                r = _post_msg_checked(dest, WM_IME_CHAR, cp, 1)
                any_ok = any_ok or r
            else:
                r = _post_msg_checked(dest, WM_CHAR, cp, 1)
                any_ok = any_ok or r
            gap_bg = random.uniform(0.038, 0.075) if human else (max(0.002, char_delay_ms / 1000.0) if char_delay_ms > 0 else 0.002)
            time.sleep(gap_bg)
        return bool(any_ok), mapped
    finally:
        if attached:
            user32.AttachThreadInput(cur_tid, target_tid, False)


DEFAULT_PYTHON_SWEEP_SCRIPT = """\
# Python Per-Method Test Script (Executed for each Method 1..20)
# Available variables: method_num (1..20), method_id, method_name, category, target_app, target_hwnd, mode
# Available functions (all use ONLY the current method being tested):
#   print(text, enter=False)  -> Types text via current method
#   type_text(text)           -> Types text via current method
#   press(key, hold_ms=20)    -> Presses a key (e.g. "Enter", "Space", "F1")
#   wait(ms)                  -> Waits in milliseconds

print(f"Method {method_num}: [{method_id}] - OK")
press("Enter")
wait(120)
"""

DEFAULT_TEMPLATE_SWEEP_STRING = 'key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \\n'

# Supported Smart Template Variables (for UI chip insertion & documentation)
SWEEP_TEMPLATE_VARIABLES: List[Tuple[str, str]] = [
    ('key="Enter"', 'key="Enter"'),
    ("{method_num}", "{method_num}"),
    ("{method_num_02}", "{method_num_02}"),
    ("{method_id}", "{method_id}"),
    ("{method_name}", "{method_name}"),
    ("{category}", "{category}"),
    ("{target_app}", "{target_app}"),
    ("{target_hwnd}", "{target_hwnd}"),
    ("{mode}", "{mode}"),
    ("{time}", "{time}"),
    ("\\n", "\\n"),
    ("wait=120", "wait=120"),
]


def _build_template_context(
    method_num: int,
    method_id: str,
    method_name: str,
    target_hwnd: int = 0,
    target_app: str = "",
    run_in_background: bool = False,
) -> Dict[str, str]:
    now = datetime.now()
    cat = "Background" if method_id in BACKGROUND_HWND_KEY_METHODS else "Foreground"
    mode_str = "Background" if run_in_background else "Foreground"
    return {
        "method_num": str(method_num),
        "method_num_02": f"{method_num:02d}",
        "method_id": str(method_id),
        "method_name": str(method_name),
        "category": cat,
        "target_app": str(target_app or "Sandbox"),
        "target_hwnd": f"0x{int(target_hwnd):X}" if target_hwnd else "0x0",
        "mode": mode_str,
        "time": now.strftime("%H:%M:%S"),
        "date": now.strftime("%Y-%m-%d"),
    }


def _substitute_template_vars(raw_text: str, ctx: Dict[str, str]) -> str:
    out = raw_text
    for k, v in ctx.items():
        out = out.replace("{" + k + "}", v)
    return out


_DIRECTIVE_PATTERN = re.compile(
    r"""(?:\bkey\s*=\s*(?:"([^"]+)"|'([^']+)'|([^\s,;]+)))"""
    r"""|(?:\[\s*key\s*[:=]\s*([^\]]+)\s*\])"""
    r"""|(?:\{\s*key\s*[:=]\s*([^\}]+)\s*\})"""
    r"""|(?:\bwait\s*=\s*(?:"(\d+)"|'(\d+)'|(\d+)))"""
    r"""|(?:\{\s*wait\s*[:=]\s*(\d+)\s*\})"""
    r"""|(\\n|\\t)""",
    re.IGNORECASE,
)


def expand_and_execute_method_template(
    template_str: str,
    method_num: int,
    method_id: str,
    method_name: str,
    target_hwnd: int = 0,
    target_app: str = "",
    run_in_background: bool = False,
    extra_enter: bool = False,
    allow_self_test: bool = False,
) -> Tuple[bool, str, str]:
    """
    Parse and execute a Smart Template string for `(method_num, method_id, method_name)` strictly via `method_id`.
    Supports:
    - Inline key directives anywhere: `key="Enter"`, `key='Space'`, `key=F1`, `[key:Enter]`, `{key:Enter}`
    - Inline delays: `wait=150`, `{wait:150}`
    - Escape sequences: `\\n` (presses `Enter`), `\\t` (presses `Tab`)
    - Variables: `{method_num}`, `{method_num_02}`, `{method_id}`, `{method_name}`, `{category}`,
      `{target_app}`, `{target_hwnd}`, `{mode}`, `{time}`, `{date}`
    Returns `(ok, reason, rendered_preview)`.
    """
    ctx = _build_template_context(
        method_num=method_num,
        method_id=method_id,
        method_name=method_name,
        target_hwnd=target_hwnd,
        target_app=target_app,
        run_in_background=run_in_background,
    )
    tpl = (template_str or "").strip() or DEFAULT_TEMPLATE_SWEEP_STRING

    ops_ok = False
    last_reason = f"Delivered via {method_id}"
    preview_parts: List[str] = []
    ended_with_enter = False

    pos = 0
    for m in _DIRECTIVE_PATTERN.finditer(tpl):
        start, end = m.span()
        if start > pos:
            raw_seg = tpl[pos:start]
            # Strip single space immediately following a directive if the user wrote `key="Enter" {method_num}...`
            if pos > 0 and raw_seg.startswith(" ") and len(raw_seg) > 1:
                raw_seg = raw_seg[1:]
            seg = _substitute_template_vars(raw_seg, ctx)
            if seg:
                ok_t, r_t = dispatch_text_via_method(
                    seg,
                    method=method_id,
                    target_hwnd=target_hwnd,
                    run_in_background=run_in_background,
                    strict_no_fallback=True,
                    allow_self_test=allow_self_test,
                )
                if not ok_t:
                    return False, r_t, "".join(preview_parts)
                ops_ok = True
                ended_with_enter = False
                preview_parts.append(seg)

        key_val = m.group(1) or m.group(2) or m.group(3) or m.group(4) or m.group(5)
        wait_val = m.group(6) or m.group(7) or m.group(8) or m.group(9)
        esc_val = m.group(10)

        if key_val:
            k_clean = key_val.strip()
            ok_k, r_k = dispatch_autopresser_key(
                k_clean,
                method=method_id,
                hold_ms=16,
                target_hwnd=target_hwnd,
                run_in_background=run_in_background,
                strict_no_fallback=True,
                allow_self_test=allow_self_test,
            )
            if not ok_k:
                return False, r_k, "".join(preview_parts)
            ops_ok = True
            ended_with_enter = k_clean.lower() in ("enter", "return")
            preview_parts.append(f"[{k_clean}]")
        elif wait_val:
            w_ms = max(0, min(10000, int(wait_val)))
            time.sleep(w_ms / 1000.0)
            preview_parts.append(f"({w_ms}ms)")
        elif esc_val:
            esc_low = esc_val.lower()
            k_esc = "Enter" if esc_low == "\\n" else "Tab"
            ok_e, r_e = dispatch_autopresser_key(
                k_esc,
                method=method_id,
                hold_ms=15,
                target_hwnd=target_hwnd,
                run_in_background=run_in_background,
                strict_no_fallback=True,
                allow_self_test=allow_self_test,
            )
            if not ok_e:
                return False, r_e, "".join(preview_parts)
            ops_ok = True
            ended_with_enter = (k_esc == "Enter")
            preview_parts.append(f"[{k_esc}]")

        pos = end

    if pos < len(tpl):
        raw_tail = tpl[pos:]
        if pos > 0 and raw_tail.startswith(" ") and len(raw_tail) > 1:
            raw_tail = raw_tail[1:]
        tail = _substitute_template_vars(raw_tail, ctx)
        if tail:
            ok_t, r_t = dispatch_text_via_method(
                tail,
                method=method_id,
                target_hwnd=target_hwnd,
                run_in_background=run_in_background,
                strict_no_fallback=True,
                allow_self_test=allow_self_test,
            )
            if not ok_t:
                return False, r_t, "".join(preview_parts)
            ops_ok = True
            ended_with_enter = False
            preview_parts.append(tail)

    if extra_enter and not ended_with_enter:
        ok_e, r_e = dispatch_autopresser_key(
            "Enter",
            method=method_id,
            hold_ms=15,
            target_hwnd=target_hwnd,
            run_in_background=run_in_background,
            strict_no_fallback=True,
        )
        if not ok_e:
            return False, r_e, "".join(preview_parts)
        ops_ok = True
        preview_parts.append("[Enter]")

    return bool(ops_ok), last_reason, "".join(preview_parts)


def execute_per_method_python_test(
    script_code: str,
    method_num: int,
    method_id: str,
    method_name: str,
    target_hwnd: int = 0,
    run_in_background: bool = False,
    target_app: str = "",
    allow_self_test: bool = False,
) -> Tuple[bool, str]:
    """
    Execute user-customized Python test code for a single method `(method_num, method_id, method_name)`.
    Every `print()`, `type_text()`, `press()`, `human_press()`, `hold_key()`, `click()`, and `move_rel()`
    call inside the script is routed strictly through `method_id` with `strict_no_fallback=True`.
    """
    ops_count = 0
    ops_ok = False
    last_reason = f"Delivered via {method_id}"
    ctx = _build_template_context(
        method_num=method_num,
        method_id=method_id,
        method_name=method_name,
        target_hwnd=target_hwnd,
        target_app=target_app,
        run_in_background=run_in_background,
    )

    def _py_rand(min_val: int = 60, max_val: int = 110) -> int:
        lo, hi = int(min(min_val, max_val)), int(max(min_val, max_val))
        return random.randint(lo, hi)

    def _py_wait(ms: float = 100) -> None:
        time.sleep(max(0.0, min(15.0, float(ms) / 1000.0)))

    def _py_human_wait(min_ms: float = 65, max_ms: float = 130) -> None:
        lo, hi = float(min(min_ms, max_ms)), float(max(min_ms, max_ms))
        jitter = random.gauss(0.0, 2.5)
        ms = max(5.0, min(15000.0, random.uniform(lo, hi) + jitter))
        time.sleep(ms / 1000.0)

    def _py_type_text(txt: Any, enter: bool = False, human: bool = False, delay_ms: int = 0) -> bool:
        nonlocal ops_count, ops_ok, last_reason
        ops_count += 1
        s = _substitute_template_vars(str(txt), ctx)
        ok_t, r_t = dispatch_text_via_method(
            s,
            method=method_id,
            target_hwnd=target_hwnd,
            run_in_background=run_in_background,
            strict_no_fallback=True,
            human=bool(human),
            char_delay_ms=int(delay_ms),
            allow_self_test=allow_self_test,
        )
        if not ok_t:
            last_reason = r_t
            return False
        ops_ok = True
        if enter:
            if human:
                _py_human_wait(45, 85)
            ok_e, r_e = dispatch_autopresser_key(
                "Enter",
                method=method_id,
                hold_ms=random.randint(58, 92) if human else 60,
                target_hwnd=target_hwnd,
                run_in_background=run_in_background,
                strict_no_fallback=True,
                allow_self_test=allow_self_test,
            )
            if not ok_e:
                last_reason = r_e
                return False
        return True

    def _py_print(*args: Any, sep: str = " ", end: str = "", enter: bool = False, human: bool = False, delay_ms: int = 0) -> bool:
        msg = sep.join(str(a) for a in args) + end
        return _py_type_text(msg, enter=enter, human=human, delay_ms=delay_ms)

    def _py_press(key_label: str = "Enter", hold_ms: Any = 65, human: bool = False) -> bool:
        nonlocal ops_count, ops_ok, last_reason
        ops_count += 1
        if isinstance(hold_ms, str) and hold_ms.lower() == "human":
            human = True
            eff_hold = random.randint(58, 95)
        elif human:
            base_h = int(hold_ms) if isinstance(hold_ms, (int, float)) and int(hold_ms) > 25 else 72
            eff_hold = max(45, int(base_h + random.randint(-12, 16)))
        else:
            eff_hold = max(15, int(hold_ms) if isinstance(hold_ms, (int, float)) else 65)

        ok_k, r_k = dispatch_autopresser_key(
            str(key_label),
            method=method_id,
            hold_ms=eff_hold,
            target_hwnd=target_hwnd,
            run_in_background=run_in_background,
            strict_no_fallback=True,
        )
        if not ok_k:
            last_reason = r_k
            return False
        ops_ok = True
        return True

    def _py_human_press(
        key_label: str = "Enter",
        min_hold: int = 58,
        max_hold: int = 95,
        min_wait: int = 65,
        max_wait: int = 130,
    ) -> bool:
        ok = _py_press(key_label, hold_ms=_py_rand(min_hold, max_hold), human=False)
        if ok and max_wait > 0:
            _py_human_wait(min_wait, max_wait)
        return ok

    def _py_hold_key(key_label: str = "W", duration_ms: int = 300) -> bool:
        return _py_press(key_label, hold_ms=max(40, int(duration_ms)), human=False)

    def _py_click(
        arg1: Any = "left",
        arg2: Any = None,
        button: str = "left",
        hold_ms: int = 68,
        human: bool = False,
        in_game_3d: bool = False,
    ) -> bool:
        nonlocal ops_count, ops_ok, last_reason
        ops_count += 1
        cx, cy = get_cursor_pos()
        if isinstance(arg1, (int, float)) and isinstance(arg2, (int, float)):
            tx, ty = int(arg1), int(arg2)
            btn = str(button).lower()
            move_cur = not in_game_3d
        else:
            btn = str(arg1 or button or "left").lower()
            tx = int(cx if arg2 is None else arg2)
            ty = int(cy)
            move_cur = (arg2 is not None) and (not in_game_3d)

        eff_hold = random.randint(58, 95) if human else max(25, int(hold_ms))
        if in_game_3d or not move_cur:
            if run_in_background and target_hwnd and not is_hwnd_or_child_foreground(target_hwnd):
                last_reason = "Foreground-only 3D mouse click skipped in Background Mode"
                return False
            send_mouse_button(tx, ty, button=btn, action="click", move_cursor=False, hold_ms=eff_hold, method=method_id)
            ops_ok = True
            return True

        mp = AutoPresserMousePoint(
            name="PyClick",
            x=tx,
            y=ty,
            button=btn,
            interval_ms=100,
            enabled=True,
        )
        m_mode = "post" if (run_in_background and target_hwnd) else "real"
        ok_m = dispatch_autopresser_mouse(
            mp,
            mouse_mode=m_mode,
            target_hwnd=target_hwnd,
            run_in_background=run_in_background,
            strict_no_fallback=True,
        )
        if not ok_m:
            last_reason = "Mouse click rejected"
            return False
        ops_ok = True
        return True

    def _py_move_rel(dx: int, dy: int, steps: int = 6, duration_ms: int = 60) -> bool:
        nonlocal ops_count, ops_ok, last_reason
        ops_count += 1
        if run_in_background and target_hwnd and not is_hwnd_or_child_foreground(target_hwnd):
            last_reason = "Foreground-only 3D camera move skipped in Background Mode"
            return False
        ok_mv = send_mouse_relative_move(int(dx), int(dy), steps=int(steps), duration_ms=int(duration_ms), method=method_id)
        if ok_mv:
            ops_ok = True
        return ok_mv

    safe_globals: Dict[str, Any] = {
        "__builtins__": {
            "len": len,
            "range": range,
            "str": str,
            "int": int,
            "float": float,
            "bool": bool,
            "min": min,
            "max": max,
            "abs": abs,
            "round": round,
            "enumerate": enumerate,
        },
        "method_num": method_num,
        "method_num_02": ctx["method_num_02"],
        "method_id": method_id,
        "method_name": method_name,
        "category": ctx["category"],
        "target_app": ctx["target_app"],
        "target_hwnd": ctx["target_hwnd"],
        "mode": ctx["mode"],
        "time_str": ctx["time"],
        "date_str": ctx["date"],
        "rand": _py_rand,
        "randint": _py_rand,
        "print": _py_print,
        "type_text": _py_type_text,
        "press": _py_press,
        "send_key": _py_press,
        "human_press": _py_human_press,
        "hold_key": _py_hold_key,
        "wait": _py_wait,
        "sleep": _py_wait,
        "sleep_ms": _py_wait,
        "human_wait": _py_human_wait,
        "click": _py_click,
        "move_rel": _py_move_rel,
    }
    try:
        code = (script_code or "").strip() or DEFAULT_PYTHON_SWEEP_SCRIPT
        exec(compile(code, "<test_lab_python_sweep>", "exec"), safe_globals, {})
        if ops_count == 0:
            _py_press("Enter", 15)
        return bool(ops_ok), last_reason
    except Exception as exc:
        return False, f"Python Script Error: {exc}"


def execute_per_method_macro_test(
    steps_or_events: List[Dict[str, Any]],
    is_recording: bool,
    method_num: int,
    method_id: str,
    method_name: str,
    target_hwnd: int = 0,
    target_app: str = "",
    run_in_background: bool = False,
    prefix_template: str = "",
    allow_self_test: bool = False,
) -> Tuple[bool, str, int]:
    """
    Execute a user's Smart Key (`steps`) or Saved Recording (`events`) strictly through `method_id`
    (`strict_no_fallback=True`). Optionally types `prefix_template` first via `method_id`.
    Returns `(ok, reason, executed_steps_count)`.
    """
    executed_count = 0
    any_ok = False
    last_reason = f"Delivered via {method_id}"

    if (prefix_template or "").strip():
        ok_p, r_p, _ = expand_and_execute_method_template(
            prefix_template,
            method_num=method_num,
            method_id=method_id,
            method_name=method_name,
            target_hwnd=target_hwnd,
            target_app=target_app,
            run_in_background=run_in_background,
            extra_enter=False,
            allow_self_test=allow_self_test,
        )
        if not ok_p:
            return False, r_p, 0
        any_ok = True
        executed_count += 1

    if not steps_or_events:
        ok_d, r_d = dispatch_autopresser_key(
            "Space",
            method=method_id,
            hold_ms=15,
            target_hwnd=target_hwnd,
            run_in_background=run_in_background,
            strict_no_fallback=True,
        )
        return bool(ok_d), r_d, (executed_count + 1 if ok_d else 0)

    for raw_st in steps_or_events:
        if not isinstance(raw_st, dict):
            continue
        if not bool(raw_st.get("enabled", True)):
            continue
        if is_recording:
            from app.domain.models import ComboStep, MacroEvent
            ev = MacroEvent.from_dict(raw_st)
            st_obj = ComboStep.from_macro_event(ev)
            st = st_obj.to_dict()
        else:
            st = raw_st

        if not bool(st.get("enabled", True)):
            continue
        stype = str(st.get("step_type", "key_tap")).lower()
        val = str(st.get("value", "Space"))
        delay_ms = max(0, min(1500, int(st.get("delay_ms", 20))))
        hold_ms = max(10, min(1500, int(st.get("hold_ms", 20) or 20)))

        if stype == "wait":
            time.sleep(delay_ms / 1000.0)
            executed_count += 1
            continue

        if stype in ("key_tap", "key_hold", "key_down", "key_up"):
            ok_k, r_k = dispatch_autopresser_key(
                val or "Space",
                method=method_id,
                hold_ms=hold_ms if stype != "key_up" else 10,
                target_hwnd=target_hwnd,
                run_in_background=run_in_background,
                strict_no_fallback=True,
                allow_self_test=allow_self_test,
            )
            if not ok_k:
                return False, r_k, executed_count
            any_ok = True
            executed_count += 1
        elif stype == "type_text":
            ok_t, r_t, _ = expand_and_execute_method_template(
                val,
                method_num=method_num,
                method_id=method_id,
                method_name=method_name,
                target_hwnd=target_hwnd,
                target_app=target_app,
                run_in_background=run_in_background,
                extra_enter=False,
                allow_self_test=allow_self_test,
            )
            if not ok_t:
                return False, r_t, executed_count
            any_ok = True
            executed_count += 1
        elif stype == "python_script":
            ok_py, r_py = execute_per_method_python_test(
                val,
                method_num=method_num,
                method_id=method_id,
                method_name=method_name,
                target_hwnd=target_hwnd,
                run_in_background=run_in_background,
                target_app=target_app,
                allow_self_test=allow_self_test,
            )
            if not ok_py:
                return False, r_py, executed_count
            any_ok = True
            executed_count += 1
        elif stype in ("mouse_click", "mouse_down", "mouse_up"):
            cx, cy = get_cursor_pos()
            mx = int(st.get("x", cx) if st.get("use_coords", True) else cx)
            my = int(st.get("y", cy) if st.get("use_coords", True) else cy)
            mp = AutoPresserMousePoint(name="MacroClick", x=mx, y=my, button=val or "left", interval_ms=100, enabled=True)
            m_mode = "post" if (run_in_background or method_id in BACKGROUND_HWND_KEY_METHODS) and target_hwnd else "real"
            ok_m = dispatch_autopresser_mouse(
                mp,
                mouse_mode=m_mode,
                target_hwnd=target_hwnd,
                run_in_background=run_in_background,
                strict_no_fallback=True,
                allow_self_test=allow_self_test,
            )
            if not ok_m:
                return False, "Mouse click step rejected", executed_count
            any_ok = True
            executed_count += 1

        if delay_ms > 0:
            time.sleep(min(0.25, delay_ms / 1000.0))

    return bool(any_ok), last_reason, executed_count


def _send_mouse_abs_65535(x: int, y: int, button: str = "left", hold_ms: int = 30) -> bool:
    """
    Mouse Mode 8 (`sendinput_abs`): Hardware-normalized 0..65535 `MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK`
    via `user32.SendInput` without calling `SetCursorPos`.
    """
    if not IS_WINDOWS:
        return True
    try:
        vx = user32.GetSystemMetrics(76)
        vy = user32.GetSystemMetrics(77)
        vw = max(1, user32.GetSystemMetrics(78) or user32.GetSystemMetrics(0) or 1920)
        vh = max(1, user32.GetSystemMetrics(79) or user32.GetSystemMetrics(1) or 1080)
        norm_x = int(round(((x - vx) * 65535) / max(1, vw - 1)))
        norm_y = int(round(((y - vy) * 65535) / max(1, vh - 1)))
        norm_x = max(0, min(65535, norm_x))
        norm_y = max(0, min(65535, norm_y))

        b_low = (button or "left").lower()
        mouse_data = 0
        if b_low == "right":
            dn_flag, up_flag = MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP
        elif b_low == "middle":
            dn_flag, up_flag = MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP
        elif b_low in ("x1", "back"):
            dn_flag, up_flag, mouse_data = MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP, XBUTTON1
        elif b_low in ("x2", "forward"):
            dn_flag, up_flag, mouse_data = MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP, XBUTTON2
        else:
            dn_flag, up_flag = MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP

        abs_flags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK
        extra = user32.GetMessageExtraInfo()

        inp_dn = INPUT(
            type=INPUT_MOUSE,
            union=_INPUT_UNION(
                mi=MOUSEINPUT(
                    dx=norm_x,
                    dy=norm_y,
                    mouseData=mouse_data,
                    dwFlags=abs_flags | dn_flag,
                    time=0,
                    dwExtraInfo=extra,
                )
            ),
        )
        s1 = user32.SendInput(1, ctypes.byref(inp_dn), ctypes.sizeof(INPUT))
        time.sleep(max(0.010, hold_ms / 1000.0))
        inp_up = INPUT(
            type=INPUT_MOUSE,
            union=_INPUT_UNION(
                mi=MOUSEINPUT(
                    dx=norm_x,
                    dy=norm_y,
                    mouseData=mouse_data,
                    dwFlags=abs_flags | up_flag,
                    time=0,
                    dwExtraInfo=extra,
                )
            ),
        )
        s2 = user32.SendInput(1, ctypes.byref(inp_up), ctypes.sizeof(INPUT))
        return bool(s1 > 0 and s2 > 0)
    except Exception:
        return False


def dispatch_autopresser_mouse_detailed(
    point: AutoPresserMousePoint,
    mouse_mode: str,
    target_hwnd: int = 0,
    run_in_background: bool = True,
    strict_no_fallback: bool = False,
    allow_self_test: bool = False,
) -> Tuple[bool, Dict[str, Any]]:
    """
    Execute a mouse click at `point` using one of the 8 distinct Mouse Click Modes and return
    `(ok, diagnostic_meta_dict)` with resolved child `HWND`, class name, and client `(cx, cy)` coordinates.
    """
    eff_mode = mouse_mode
    if eff_mode == "auto":
        eff_mode = "post" if (run_in_background and target_hwnd) else "real"

    meta: Dict[str, Any] = {
        "mode": mouse_mode,
        "effective_mode": eff_mode,
        "btn": (point.button or "left").lower(),
        "screen_x": int(point.x),
        "screen_y": int(point.y),
        "client_x": int(point.x),
        "client_y": int(point.y),
        "dest_hwnd": f"0x{int(target_hwnd):X}" if target_hwnd else "Foreground",
        "child_cls": "Screen" if eff_mode in ("real", "sendinput_abs") else "TopWindow",
        "reason_code": "mouse_fg_ok" if eff_mode in ("real", "sendinput_abs") else "mouse_bg_ok",
    }

    if not IS_WINDOWS:
        return True, meta

    if not allow_self_test and target_hwnd and is_hwnd_own_process(target_hwnd):
        meta["reason_code"] = "self_app_protected"
        return False, meta

    if eff_mode in ("real", "sendinput_abs") or not target_hwnd:
        if run_in_background and target_hwnd and not is_hwnd_or_child_foreground(target_hwnd):
            meta["reason_code"] = "mouse_skipped_bg"
            return False, meta
        if not allow_self_test and is_own_app_foreground():
            meta["reason_code"] = "self_app_protected"
            return False, meta
        if eff_mode == "sendinput_abs":
            ok_abs = _send_mouse_abs_65535(point.x, point.y, button=point.button, hold_ms=30)
            meta["reason_code"] = "mouse_fg_ok" if ok_abs else "hwnd_rejected"
            return ok_abs, meta
        send_mouse_button(point.x, point.y, button=point.button, action="click", move_cursor=True, hold_ms=30)
        meta["reason_code"] = "mouse_fg_ok"
        return True, meta

    if not user32.IsWindow(target_hwnd):
        meta["reason_code"] = "hwnd_rejected"
        return False, meta

    try:
        if eff_mode == "post_top":
            dest_hwnd = int(target_hwnd)
            pt = wintypes.POINT(int(point.x), int(point.y))
            user32.ScreenToClient(dest_hwnd, ctypes.byref(pt))
            cx, cy = int(pt.x), int(pt.y)
        elif eff_mode == "guithread_capture":
            tid = user32.GetWindowThreadProcessId(target_hwnd, None)
            gti = GUITHREADINFO()
            gti.cbSize = ctypes.sizeof(GUITHREADINFO)
            cap_hwnd = 0
            if tid and user32.GetGUIThreadInfo(tid, ctypes.byref(gti)):
                cap_hwnd = int(gti.hwndCapture or gti.hwndFocus or gti.hwndActive or 0)
            if cap_hwnd and user32.IsWindow(cap_hwnd):
                dest_hwnd = cap_hwnd
                pt = wintypes.POINT(int(point.x), int(point.y))
                user32.ScreenToClient(dest_hwnd, ctypes.byref(pt))
                cx, cy = int(pt.x), int(pt.y)
            else:
                dest_hwnd, cx, cy = _find_child_at_screen_pt(target_hwnd, point.x, point.y)
        else:
            dest_hwnd, cx, cy = _find_child_at_screen_pt(target_hwnd, point.x, point.y)

        cls_name = _get_window_class_name(dest_hwnd) or "Window"
        meta["client_x"] = cx
        meta["client_y"] = cy
        meta["dest_hwnd"] = f"0x{dest_hwnd:X}"
        meta["child_cls"] = cls_name

        lparam = ((cy & 0xFFFF) << 16) | (cx & 0xFFFF)
        b_low = (point.button or "left").lower()
        up_wparam = 0
        vk_btn = 0x01
        if b_low == "right":
            dn, up, mk, vk_btn = WM_RBUTTONDOWN, WM_RBUTTONUP, 0x0002, 0x02
        elif b_low == "middle":
            dn, up, mk, vk_btn = WM_MBUTTONDOWN, WM_MBUTTONUP, 0x0010, 0x04
        elif b_low in ("x1", "back"):
            dn, up = WM_XBUTTONDOWN, WM_XBUTTONUP
            mk = (0x0001 << 16) | 0x0020
            up_wparam = 0x0001 << 16
            vk_btn = 0x05
        elif b_low in ("x2", "forward"):
            dn, up = WM_XBUTTONDOWN, WM_XBUTTONUP
            mk = (0x0002 << 16) | 0x0040
            up_wparam = 0x0002 << 16
            vk_btn = 0x06
        else:
            dn, up, mk, vk_btn = WM_LBUTTONDOWN, WM_LBUTTONUP, 0x0001, 0x01

        if eff_mode == "post_hittest_chain":
            screen_lparam = ((int(point.y) & 0xFFFF) << 16) | (int(point.x) & 0xFFFF)
            _post_msg_checked(dest_hwnd, WM_NCHITTEST, 0, screen_lparam)
            _post_msg_checked(dest_hwnd, WM_SETCURSOR, dest_hwnd, (1 | (dn << 16)))
            _post_msg_checked(target_hwnd, WM_MOUSEACTIVATE, target_hwnd, (1 | (dn << 16)))
            _post_msg_checked(dest_hwnd, WM_MOUSEMOVE, 0, lparam)

        cur_tid = kernel32.GetCurrentThreadId()
        target_tid = user32.GetWindowThreadProcessId(dest_hwnd, None)
        can_attach = bool(
            eff_mode == "attach_mouse_state"
            and target_tid
            and target_tid != cur_tid
            and not is_hwnd_own_process(dest_hwnd)
        )
        kbd_state = (ctypes.c_ubyte * 256)()
        attached = False
        if can_attach:
            attached = bool(user32.AttachThreadInput(cur_tid, target_tid, True))
        try:
            if attached:
                user32.GetKeyboardState(kbd_state)
                kbd_state[vk_btn] |= 0x80
                user32.SetKeyboardState(kbd_state)
            ok1 = _post_msg_checked(dest_hwnd, dn, mk, lparam)
        finally:
            if attached:
                user32.AttachThreadInput(cur_tid, target_tid, False)
                attached = False

        time.sleep(0.025)

        if can_attach:
            attached = bool(user32.AttachThreadInput(cur_tid, target_tid, True))
        try:
            if attached:
                user32.GetKeyboardState(kbd_state)
                kbd_state[vk_btn] &= ~0x80
                user32.SetKeyboardState(kbd_state)
            ok2 = _post_msg_checked(dest_hwnd, up, up_wparam, lparam)
            ok_all = bool(ok1 and ok2)
            meta["reason_code"] = "mouse_bg_ok" if ok_all else "hwnd_rejected"
            return ok_all, meta
        finally:
            if attached:
                user32.AttachThreadInput(cur_tid, target_tid, False)
    except Exception:
        meta["reason_code"] = "hwnd_rejected"
        return False, meta


def dispatch_autopresser_mouse(
    point: AutoPresserMousePoint,
    mouse_mode: str,
    target_hwnd: int = 0,
    run_in_background: bool = True,
    strict_no_fallback: bool = False,
    allow_self_test: bool = False,
) -> bool:
    ok, _ = dispatch_autopresser_mouse_detailed(
        point=point,
        mouse_mode=mouse_mode,
        target_hwnd=target_hwnd,
        run_in_background=run_in_background,
        strict_no_fallback=strict_no_fallback,
        allow_self_test=allow_self_test,
    )
    return ok


class AutoKeyPresserEngine:
    """
    Manages multiple independent Auto Key Presser tabs, their worker threads,
    structured multi-language logs, and JSON persistence.
    """

    def __init__(self, state: Any = None) -> None:
        self.state = state
        self.tabs: List[AutoPresserTab] = []
        self.active_tab_index: int = 0
        self._running_tabs: Dict[str, threading.Event] = {}
        self._tab_logs: Dict[str, List[Dict[str, Any]]] = {}
        self._on_ui_update: Optional[Callable[[], None]] = None
        self.load_workspace()

    def find_matching_smart_combo(self, key_label: str) -> Optional[Dict[str, Any]]:
        """Find a Smart Key / Macro in `state.smart_combos` whose trigger_key matches `key_label`."""
        if self.state is None or not hasattr(self.state, "smart_combos"):
            return None
        k_low = key_label.strip().lower()
        for c_dict in self.state.smart_combos:
            if not isinstance(c_dict, dict):
                continue
            trig = str(c_dict.get("trigger_key", "")).strip().lower()
            if trig == k_low and c_dict.get("steps"):
                return c_dict
        return None

    def set_macro_engine(self, macro_engine: Any) -> None:
        self._macro_engine = macro_engine

    def set_ui_callback(self, cb: Optional[Callable[[], None]]) -> None:
        self._on_ui_update = cb

    def workspace_path(self) -> Path:
        return get_appdata_dir() / "autopresser_workspace.json"

    def load_workspace(self, file_path: Optional[str] = None) -> bool:
        p = Path(file_path) if file_path else self.workspace_path()
        if p.exists():
            try:
                raw = json.loads(p.read_text(encoding="utf-8"))
                raw_tabs = raw.get("tabs", []) if isinstance(raw, dict) else []
                loaded = [AutoPresserTab.from_dict(t) for t in raw_tabs if isinstance(t, dict)]
                if loaded:
                    self.tabs = loaded
                    self.active_tab_index = max(0, min(len(self.tabs) - 1, int(raw.get("active_tab_index", 0))))
                    return True
            except Exception as exc:
                logger.warning("Failed to load autopresser workspace: %s", exc)

        if not self.tabs:
            t1 = AutoPresserTab(tab_id="tab_1", title="Tab 1")
            t1.ensure_builtin_keys()
            self.tabs = [t1]
            self.active_tab_index = 0
            self.save_workspace()
        return True

    def save_workspace(self, file_path: Optional[str] = None) -> bool:
        p = Path(file_path) if file_path else self.workspace_path()
        try:
            payload = {
                "schema_version": 6,
                "active_tab_index": self.active_tab_index,
                "tabs": [t.to_dict() for t in self.tabs],
            }
            p.write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")
            return True
        except Exception as exc:
            logger.error("Failed to save autopresser workspace: %s", exc)
            return False

    def save_tab_profile(self, tab: AutoPresserTab, file_path: str) -> bool:
        try:
            Path(file_path).write_text(json.dumps(tab.to_dict(), ensure_ascii=False, indent=2), encoding="utf-8")
            return True
        except Exception:
            return False

    def load_tab_profile(self, tab_index: int, file_path: str) -> bool:
        try:
            raw = json.loads(Path(file_path).read_text(encoding="utf-8"))
            if isinstance(raw, dict) and 0 <= tab_index < len(self.tabs):
                old_id = self.tabs[tab_index].tab_id
                loaded = AutoPresserTab.from_dict(raw)
                loaded.tab_id = old_id
                self.tabs[tab_index] = loaded
                self.save_workspace()
                return True
        except Exception:
            pass
        return False

    def add_new_tab(self, duplicate_from: Optional[int] = None, save_immediately: bool = True) -> AutoPresserTab:
        new_id = f"tab_{int(time.time() * 1000)}_{len(self.tabs) + 1}"
        if duplicate_from is not None and 0 <= duplicate_from < len(self.tabs):
            src = self.tabs[duplicate_from].to_dict()
            src["tab_id"] = new_id
            src["title"] = f"{self.tabs[duplicate_from].title} (Copy)"
            nt = AutoPresserTab.from_dict(src)
        else:
            nt = AutoPresserTab(tab_id=new_id, title=f"Tab {len(self.tabs) + 1}")
            nt.ensure_builtin_keys()
        self.tabs.append(nt)
        self.active_tab_index = len(self.tabs) - 1
        if save_immediately:
            self.save_workspace()
        return nt

    def close_tab(self, index: int, save_immediately: bool = True) -> None:
        if len(self.tabs) <= 1 or not (0 <= index < len(self.tabs)):
            return
        tab = self.tabs[index]
        self.stop_tab(tab.tab_id)
        self.tabs.pop(index)
        self.active_tab_index = max(0, min(len(self.tabs) - 1, self.active_tab_index))
        if save_immediately:
            self.save_workspace()

    def is_tab_running(self, tab_id: str) -> bool:
        ev = self._running_tabs.get(tab_id)
        return ev is not None and not ev.is_set()

    def any_tab_running(self) -> bool:
        return any(self.is_tab_running(tid) for tid in list(self._running_tabs.keys()))

    def get_tab_logs(self, tab_id: str) -> List[Dict[str, Any]]:
        return self._tab_logs.setdefault(tab_id, [])

    def clear_tab_logs(self, tab_id: str) -> None:
        self._tab_logs[tab_id] = []

    def append_tab_log(self, tab_id: str, entry: Dict[str, Any], accent_hex: str = "#8B5CF6") -> None:
        logs = self._tab_logs.setdefault(tab_id, [])
        item = dict(entry)
        item["ts"] = datetime.now().strftime("%H:%M:%S")
        item["accent"] = accent_hex
        logs.insert(0, item)
        if len(logs) > 200:
            logs.pop()

    def toggle_tab(self, tab: AutoPresserTab) -> bool:
        if self.is_tab_running(tab.tab_id):
            self.stop_tab(tab.tab_id)
            return False
        return self.start_tab(tab)

    def start_tab(self, tab: AutoPresserTab) -> bool:
        if self.is_tab_running(tab.tab_id):
            return True

        armed_keys = [k for k in tab.keys if k.enabled]
        armed_mouse = [m for m in tab.mouse_points if m.enabled] if tab.mouse_enabled else []
        if not armed_keys and not armed_mouse:
            self.append_tab_log(tab.tab_id, {"kind": "no_keys_armed"}, "#F59E0B")
            return False

        stop_ev = threading.Event()
        self._running_tabs[tab.tab_id] = stop_ev
        self.append_tab_log(
            tab.tab_id,
            {
                "kind": "started",
                "keys_count": len(armed_keys),
                "mouse_count": len(armed_mouse),
                "method": tab.key_method,
                "target": tab.target_process or "All",
            },
            "#10B981",
        )

        def _worker() -> None:
            next_key_ts: Dict[str, float] = {k.key: 0.0 for k in armed_keys}
            next_ms_ts: Dict[int, float] = {i: 0.0 for i in range(len(armed_mouse))}

            try:
                while not stop_ev.is_set():
                    now = time.perf_counter()
                    hwnd = resolve_target_hwnd(tab)

                    # Fire due keys
                    for k_item in [k for k in tab.keys if k.enabled]:
                        if stop_ev.is_set():
                            break
                        due = next_key_ts.get(k_item.key, 0.0)
                        if now >= due:
                            t0 = time.perf_counter()
                            matched_combo = self.find_matching_smart_combo(k_item.key) if getattr(k_item, "use_smart_combo", False) else None
                            m_eng = getattr(self, "_macro_engine", None)
                            if matched_combo is not None and m_eng is not None:
                                m_eng.execute_smart_combo(matched_combo)
                                dt_ms = int((time.perf_counter() - t0) * 1000)
                                next_key_ts[k_item.key] = time.perf_counter() + max(0.010, k_item.interval_ms / 1000.0)
                                self.append_tab_log(
                                    tab.tab_id,
                                    {
                                        "kind": "triggered_smart_macro",
                                        "key": k_item.key,
                                        "combo_name": str(matched_combo.get("name") or k_item.key),
                                        "interval_ms": k_item.interval_ms,
                                    },
                                    "#F59E0B",
                                )
                            else:
                                ok, actual_m = dispatch_autopresser_key(
                                    k_item.key,
                                    method=tab.key_method,
                                    hold_ms=tab.hold_ms,
                                    target_hwnd=hwnd,
                                    run_in_background=tab.run_in_background,
                                    strict_no_fallback=False,
                                )
                                dt_ms = int((time.perf_counter() - t0) * 1000)
                                next_key_ts[k_item.key] = time.perf_counter() + max(0.010, k_item.interval_ms / 1000.0)
                                self.append_tab_log(
                                    tab.tab_id,
                                    {
                                        "kind": "sent_key",
                                        "key": k_item.key,
                                        "interval_ms": k_item.interval_ms,
                                        "method": actual_m,
                                        "dt_ms": dt_ms,
                                        "ok": ok,
                                        "target_app": tab.target_process or "All",
                                        "hwnd": f"0x{hwnd:X}" if hwnd else "Foreground",
                                        "is_bg": tab.run_in_background,
                                    },
                                    "#8B5CF6" if ok else "#EF4444",
                                )

                    # Fire due mouse points
                    if tab.mouse_enabled:
                        for idx, mp in enumerate(tab.mouse_points):
                            if stop_ev.is_set() or not mp.enabled:
                                continue
                            due_m = next_ms_ts.get(idx, 0.0)
                            if now >= due_m:
                                t0_m = time.perf_counter()
                                ok_m, meta_m = dispatch_autopresser_mouse_detailed(
                                    mp,
                                    mouse_mode=tab.mouse_mode,
                                    target_hwnd=hwnd,
                                    run_in_background=tab.run_in_background,
                                    strict_no_fallback=False,
                                )
                                dt_m = max(1, int((time.perf_counter() - t0_m) * 1000))
                                next_ms_ts[idx] = time.perf_counter() + max(0.010, mp.interval_ms / 1000.0)
                                self.append_tab_log(
                                    tab.tab_id,
                                    {
                                        "kind": "sent_mouse",
                                        "btn": mp.button,
                                        "x": mp.x,
                                        "y": mp.y,
                                        "client_x": meta_m.get("client_x", mp.x),
                                        "client_y": meta_m.get("client_y", mp.y),
                                        "child_cls": meta_m.get("child_cls", ""),
                                        "dest_hwnd": meta_m.get("dest_hwnd", ""),
                                        "reason_code": meta_m.get("reason_code", ""),
                                        "mode": tab.mouse_mode,
                                        "dt_ms": dt_m,
                                        "ok": ok_m,
                                        "target_app": tab.target_process or "All",
                                        "is_bg": tab.run_in_background,
                                    },
                                    "#06B6D4" if ok_m else "#EF4444",
                                )

                    stop_ev.wait(timeout=0.015)
            finally:
                self._running_tabs.pop(tab.tab_id, None)
                self.append_tab_log(tab.tab_id, {"kind": "stopped"}, "#F59E0B")

        t = threading.Thread(target=_worker, daemon=True)
        t.start()
        return True

    def stop_tab(self, tab_id: str) -> None:
        ev = self._running_tabs.pop(tab_id, None)
        if ev is not None:
            ev.set()

    def stop_all_tabs(self) -> None:
        for tid in list(self._running_tabs.keys()):
            self.stop_tab(tid)

    def sweep_all_key_methods(
        self,
        tab: AutoPresserTab,
        test_key: str = "F1",
        allow_self_test: bool = False,
    ) -> Tuple[int, int, List[Dict[str, Any]]]:
        """
        Test all 20 Key-Pressing Methods sequentially against `tab`'s target process/HWND
        using `strict_no_fallback=True` so each method is genuinely evaluated on its own merits.
        Respects `tab.run_in_background`: when `run_in_background=True`, it NEVER brings the
        target window to the foreground or steals focus!
        """
        hwnd = resolve_target_hwnd(tab)
        if hwnd and not tab.run_in_background:
            focus_and_bring_hwnd_to_front(hwnd)

        best_child = _find_best_input_child_hwnd(hwnd) if hwnd else 0
        child_cls = _get_window_class_name(best_child) if best_child else "TopWindow"
        target_app = tab.target_process or "Active Window"
        hwnd_label = f"0x{hwnd:X}" + (f" -> 0x{best_child:X} [{child_cls}]" if (best_child and best_child != hwnd) else "") if hwnd else "Foreground"

        self.append_tab_log(
            tab.tab_id,
            {
                "kind": "sweep_header",
                "target": target_app,
                "is_bg": tab.run_in_background,
                "hwnd": hwnd_label,
                "test_key": test_key,
                "total_count": len(AUTOPRESSER_KEY_METHODS),
            },
            "#06B6D4",
        )

        ok_count = 0
        fail_count = 0
        results: List[Dict[str, Any]] = []

        for idx, (m_id, tr_key) in enumerate(AUTOPRESSER_KEY_METHODS, start=1):
            t0 = time.perf_counter()
            dispatched_ok, detail = dispatch_autopresser_key(
                test_key,
                method=m_id,
                hold_ms=15,
                target_hwnd=hwnd,
                run_in_background=tab.run_in_background,
                strict_no_fallback=True,
                allow_self_test=allow_self_test,
            )
            ok = bool(dispatched_ok)
            if ok:
                reason_code = "delivered_bg" if m_id in BACKGROUND_HWND_KEY_METHODS else "delivered_fg"
                reason = f"Delivered via {m_id}"
            else:
                if "Self-Protection" in str(detail):
                    reason_code = "self_app_protected"
                elif "Foreground-only method skipped" in str(detail):
                    reason_code = "skipped_fg_in_bg"
                elif "Interception" in str(detail):
                    reason_code = "interception_missing"
                elif "InpOutx64" in str(detail):
                    reason_code = "ps2_missing"
                elif "Linux" in str(detail):
                    reason_code = "linux_on_win"
                elif "Raspberry Pi Pico" in str(detail):
                    reason_code = "pico_missing"
                elif "Arduino" in str(detail):
                    reason_code = "arduino_missing"
                else:
                    reason_code = "hwnd_rejected"
                reason = detail or "Target HWND rejected Win32 message"

            dt_ms = max(1, int((time.perf_counter() - t0) * 1000))
            if ok:
                ok_count += 1
            else:
                fail_count += 1

            res_entry = {
                "kind": "sweep_item",
                "sweep_type": "key",
                "idx": idx,
                "total": len(AUTOPRESSER_KEY_METHODS),
                "method_id": m_id,
                "method_tr_key": tr_key,
                "key": test_key,
                "target_app": target_app,
                "hwnd": f"0x{hwnd:X}" if hwnd else "Foreground",
                "child_cls": child_cls,
                "is_bg": tab.run_in_background,
                "ok": ok,
                "dt_ms": dt_ms,
                "reason_code": reason_code,
                "reason": reason,
            }
            results.append(res_entry)
            self.append_tab_log(
                tab.tab_id,
                res_entry,
                "#10B981" if ok else "#EF4444",
            )
            time.sleep(0.035)

        self.append_tab_log(
            tab.tab_id,
            {
                "kind": "sweep_summary",
                "ok_count": ok_count,
                "fail_count": fail_count,
                "target": target_app,
                "is_bg": tab.run_in_background,
            },
            "#10B981" if ok_count > 0 else "#F59E0B",
        )
        return ok_count, fail_count, results

    def sweep_all_mouse_modes(
        self,
        tab: AutoPresserTab,
        allow_self_test: bool = False,
    ) -> Tuple[int, int, List[Dict[str, Any]]]:
        """
        Test all 8 Mouse Click Modes against `tab`'s target process/HWND (using first mouse point or cursor pos)
        with `strict_no_fallback=True`, respecting `tab.run_in_background` and logging full client/screen coordinate diagnostics.
        """
        hwnd = resolve_target_hwnd(tab)
        if hwnd and not tab.run_in_background:
            focus_and_bring_hwnd_to_front(hwnd)
        target_app = tab.target_process or "Active Window"
        if tab.mouse_points:
            test_pt = tab.mouse_points[0]
        else:
            cx, cy = get_cursor_pos()
            test_pt = AutoPresserMousePoint(name="Test Click", x=cx, y=cy, button="left", interval_ms=500, enabled=True)

        self.append_tab_log(
            tab.tab_id,
            {
                "kind": "sweep_header",
                "target": f"{target_app} @ ({test_pt.x}, {test_pt.y})",
                "is_bg": tab.run_in_background,
                "hwnd": f"0x{hwnd:X}" if hwnd else "Foreground",
                "test_key": f"Mouse [{test_pt.button.upper()}]",
                "total_count": 8,
            },
            "#06B6D4",
        )

        ok_count = 0
        fail_count = 0
        results: List[Dict[str, Any]] = []

        for idx, (mode_id, tr_key) in enumerate(AUTOPRESSER_MOUSE_MODES, start=1):
            t0 = time.perf_counter()
            ok, meta_m = dispatch_autopresser_mouse_detailed(
                test_pt,
                mouse_mode=mode_id,
                target_hwnd=hwnd,
                run_in_background=tab.run_in_background,
                strict_no_fallback=True,
                allow_self_test=allow_self_test,
            )
            dt_ms = max(1, int((time.perf_counter() - t0) * 1000))
            if ok:
                ok_count += 1
            else:
                fail_count += 1

            res_entry = {
                "kind": "sweep_item",
                "sweep_type": "mouse",
                "idx": idx,
                "total": 8,
                "method_id": mode_id,
                "method_tr_key": tr_key,
                "btn": test_pt.button,
                "screen_x": test_pt.x,
                "screen_y": test_pt.y,
                "client_x": meta_m.get("client_x", test_pt.x),
                "client_y": meta_m.get("client_y", test_pt.y),
                "dest_hwnd": meta_m.get("dest_hwnd", f"0x{hwnd:X}" if hwnd else "Foreground"),
                "child_cls": meta_m.get("child_cls", "TopWindow"),
                "target_app": target_app,
                "hwnd": f"0x{hwnd:X}" if hwnd else "Foreground",
                "is_bg": tab.run_in_background,
                "key": f"Mouse {test_pt.button.upper()} ({test_pt.x}, {test_pt.y})",
                "ok": ok,
                "dt_ms": dt_ms,
                "reason_code": meta_m.get("reason_code", ""),
                "reason": format_diagnostic_reason(
                    {
                        "reason_code": meta_m.get("reason_code", ""),
                        "method_id": mode_id,
                        "btn": test_pt.button,
                        "screen_x": test_pt.x,
                        "screen_y": test_pt.y,
                        "client_x": meta_m.get("client_x", test_pt.x),
                        "client_y": meta_m.get("client_y", test_pt.y),
                        "child_cls": meta_m.get("child_cls", "TopWindow"),
                    }
                ),
            }
            results.append(res_entry)
            self.append_tab_log(
                tab.tab_id,
                res_entry,
                "#10B981" if ok else "#EF4444",
            )
            time.sleep(0.035)

        self.append_tab_log(
            tab.tab_id,
            {
                "kind": "sweep_summary",
                "ok_count": ok_count,
                "fail_count": fail_count,
                "target": target_app,
                "is_bg": tab.run_in_background,
            },
            "#10B981" if ok_count > 0 else "#F59E0B",
        )
        return ok_count, fail_count, results


def get_input_method_options() -> List[Tuple[str, str]]:
    """Return the unified 20 Input & Key-Pressing Methods localized in the active UI language."""
    from app.i18n import t as _t
    return [(k, _t(tr_key)) for k, tr_key in AUTOPRESSER_KEY_METHODS]
