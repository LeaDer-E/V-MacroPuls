# -*- coding: utf-8 -*-
"""
System-wide Hardware ScanCode, Native Win32 Mouse Hook (`WH_MOUSE_LL`), Media/App-Launch Keys & Multi-Key Chord Macro Engine:
1. Instant Stop Playback via Hotkey (`hotkey_playback_toggle` e.g. `F10`, `hotkey_panic_stop` e.g. `Esc`, or the
   Saved Recording's own shortcut) at ANY moment during a 10-minute duration repeat, count repeat, or long wait/hold,
   with automatic release of any held keys or mouse buttons (`_release_all_playback_held_inputs`).
2. Zero Stop-Action Pollution in Recordings:
   - Clicking EITHER the main `Stop Recording` button (`btn_rec`) OR the floating `Stop` pill (`rec_floating_pill`)
     is intercepted by `is_point_on_stop_button(x, y)` in `_on_win32_mouse` AND stripped by `_strip_stop_mouse_click_from_recording()`
     in `stop_recording(stopped_via_mouse=True)`, so the final mouse click that stops recording is NEVER recorded!
   - Pressing the Stop Recording Hotkey (`F9` or multi-key chord like `Ctrl + F9`) is intercepted on key-down,
     blocks auto-repeat while held (`kid in self._suppressed_down_keys`), and `_strip_stop_hotkey_from_recording(rec_chord)`
     strips all events belonging to `rec_chord` from the recording so the stop hotkey is NEVER recorded!
3. 100% 1-to-1 Faithful `convert_recording_to_smart_combo`.
"""

from __future__ import annotations
import ctypes
import json
import logging
import random
import subprocess
import threading
import time
from datetime import datetime
from typing import Any, Dict, List, Optional, Tuple
from PySide6.QtCore import QObject
from app.domain.models import (
    ComboStep,
    MacroEvent,
    SmartCombo,
    get_default_smart_combos,
    get_starter_recorded_macro,
)
from app.domain.win32_input import (
    IS_WINDOWS,
    MODIFIER_SCANCODES,
    Win32KeyboardHook,
    Win32MouseHook,
    get_cursor_pos,
    get_foreground_process_name,
    is_own_app_foreground,
    is_scancode_physically_down,
    is_special_media_or_launch_scancode,
    label_for_scancode,
    open_folder_or_path,
    parse_chord_string,
    run_shell_command,
    scancode_for_label,
    send_mouse_button,
    send_mouse_relative_move,
    send_mouse_scroll,
    send_scancode_key,
    send_unicode_text,
)
from app.i18n import t, tl
from app.paths import get_combos_path, get_macros_dir
from app.state import AppState
from app.workers import run_async

logger = logging.getLogger(__name__)

_PYNPUT_AVAILABLE = False
try:
    from pynput import keyboard as pynput_kb
    from pynput import mouse as pynput_mouse

    _PYNPUT_AVAILABLE = True
except Exception as exc:
    logger.info("pynput unavailable (%s); using native Win32 hooks.", exc)


class MacroEngine(QObject):
    """Core hardware-level ScanCode, Native Mouse Hook & Multi-Key Chord engine."""

    SPECIAL_KEY_CHORD_WINDOW_SEC = 0.75

    def __init__(self, state: AppState) -> None:
        super().__init__()
        self.state = state

        self._playback_stop = threading.Event()
        self._was_aborted_by_user: bool = False
        self._combo_stops: Dict[str, threading.Event] = {}
        self._combo_done_events: Dict[str, threading.Event] = {}
        self._combo_queued_counts: Dict[str, int] = {}
        self._combo_locks: Dict[str, threading.Lock] = {}
        self._running_combos: set[str] = set()
        self._active_toggle_loops: set[str] = set()

        self._playback_held_keys: set[Tuple[int, bool]] = set()
        self._playback_held_mouse_buttons: set[str] = set()

        self._down_keys: set[Tuple[int, bool]] = set()
        self._down_key_ts: Dict[Tuple[int, bool], float] = {}
        self._recent_special_keys: Dict[Tuple[int, bool], float] = {}
        self._down_modifiers: set[int] = set()
        self._suppressed_down_keys: set[Tuple[int, bool]] = set()

        self._chord_triggers: List[Tuple[Tuple[Tuple[int, bool], ...], Dict[str, Any]]] = []
        self._special_keys_in_chords: set[Tuple[int, bool]] = set()
        self._recording_hotkeys: List[Tuple[Tuple[Tuple[int, bool], ...], Dict[str, Any]]] = []

        self._last_record_ts: float = 0.0
        self._last_mouse_sample_ts: float = 0.0
        self._last_ui_count_emit_ts: float = 0.0
        self._recording_down_keys: set[Tuple[int, bool]] = set()
        self._stop_button_mouse_ignored: bool = False
        self._simulating_fallback_flag: bool = False

        self._win32_kb_hook: Optional[Win32KeyboardHook] = None
        self._win32_mouse_hook: Optional[Win32MouseHook] = None
        self._kb_controller: Optional[Any] = None
        self._mouse_controller: Optional[Any] = None
        self._kb_listener_fallback: Optional[Any] = None
        self._mouse_listener_fallback: Optional[Any] = None

        self.state.combos_list_updated.connect(self.rebuild_trigger_map)
        self.state.macros_library_updated.connect(self.rebuild_trigger_map)
        self._init_controllers()
        self.load_all_data_async()

    def _init_controllers(self) -> None:
        if IS_WINDOWS:
            try:
                self._win32_kb_hook = Win32KeyboardHook(self._on_win32_key)
                self._win32_kb_hook.start()
            except Exception as exc:
                logger.error("Failed to start Win32KeyboardHook: %s", exc)
            try:
                self._win32_mouse_hook = Win32MouseHook(self._on_win32_mouse)
                self._win32_mouse_hook.start()
            except Exception as exc:
                logger.error("Failed to start Win32MouseHook: %s", exc)

        if not _PYNPUT_AVAILABLE:
            return
        try:
            self._kb_controller = pynput_kb.Controller()
            self._mouse_controller = pynput_mouse.Controller()

            if not IS_WINDOWS:
                self._kb_listener_fallback = pynput_kb.Listener(
                    on_press=self._on_fallback_key_press,
                    on_release=self._on_fallback_key_release,
                )
                self._kb_listener_fallback.daemon = True
                self._kb_listener_fallback.start()

                self._mouse_listener_fallback = pynput_mouse.Listener(
                    on_move=self._on_global_mouse_move,
                    on_click=self._on_global_mouse_click,
                    on_scroll=self._on_global_mouse_scroll,
                )
                self._mouse_listener_fallback.daemon = True
                self._mouse_listener_fallback.start()
        except Exception as exc:
            logger.info("Fallback listener skipped (%s).", exc)

    def _on_win32_mouse(
        self,
        event_type: str,
        x: int,
        y: int,
        button_or_dir: str,
        dx: int,
        dy: int,
        is_pressed: bool,
    ) -> None:
        """
        Handle native Windows `WH_MOUSE_LL` events during live recording.
        Automatically ignores any mouse click/move landing on the Stop Recording button or Floating Stop Pill!
        """
        if not self.state.is_recording:
            return

        if is_own_app_foreground():
            return

        if event_type in ("mouse_down", "mouse_up", "mouse_move") and self.state.is_point_on_stop_button(x, y):
            if event_type in ("mouse_down", "mouse_up"):
                self._stop_button_mouse_ignored = True
            return

        now = time.perf_counter()

        if event_type == "mouse_move":
            if not self.state.config.record_mouse_moves:
                return
            if (now - self._last_mouse_sample_ts) * 1000.0 < self.state.config.mouse_sample_interval_ms:
                return
            delay_ms = max(5, int((now - self._last_record_ts) * 1000))
            self._last_record_ts = now
            self._last_mouse_sample_ts = now
            ev = MacroEvent(kind="mouse_move", delay_ms=delay_ms, x=x, y=y)
            self.state.recorded_events.append(ev.to_dict())
            self._emit_throttled_recording_count(now)

        elif event_type in ("mouse_down", "mouse_up"):
            if not self.state.config.record_mouse_clicks:
                return
            delay_ms = max(5, int((now - self._last_record_ts) * 1000))
            self._last_record_ts = now
            ev = MacroEvent(
                kind=event_type,
                delay_ms=delay_ms,
                x=x,
                y=y,
                button=button_or_dir,
            )
            self.state.recorded_events.append(ev.to_dict())
            self._emit_throttled_recording_count(now)

        elif event_type == "mouse_scroll":
            if not self.state.config.record_mouse_scroll:
                return
            delay_ms = max(5, int((now - self._last_record_ts) * 1000))
            self._last_record_ts = now
            amount = max(1, abs(dy) if dy != 0 else abs(dx))
            ev = MacroEvent(
                kind="mouse_scroll",
                delay_ms=delay_ms,
                x=x,
                y=y,
                scroll_dir=button_or_dir,
                scroll_amount=amount,
                dx=dx,
                dy=dy,
            )
            self.state.recorded_events.append(ev.to_dict())
            self._emit_throttled_recording_count(now)

    def rebuild_trigger_map(self) -> None:
        combos_list: List[Tuple[Tuple[Tuple[int, bool], ...], Dict[str, Any]]] = []
        special_in_chords: set[Tuple[int, bool]] = set()
        for c in self.state.smart_combos:
            if not c.get("enabled", False):
                continue
            sc_obj = SmartCombo.from_dict(c)
            chord = tuple(sc_obj.chord_tuples())
            if chord:
                combos_list.append((chord, c))
                for k_sc, k_ext in chord:
                    if is_special_media_or_launch_scancode(k_sc, k_ext):
                        special_in_chords.add((k_sc, k_ext))

        combos_list.sort(key=lambda item: len(item[0]), reverse=True)
        self._chord_triggers = combos_list
        self._special_keys_in_chords = special_in_chords

        rec_list: List[Tuple[Tuple[Tuple[int, bool], ...], Dict[str, Any]]] = []
        for m in self.state.saved_macros:
            hk_str = str(m.get("hotkey", "")).strip()
            if hk_str:
                chord = tuple(parse_chord_string(hk_str))
                if chord:
                    rec_list.append((chord, m))
        rec_list.sort(key=lambda item: len(item[0]), reverse=True)
        self._recording_hotkeys = rec_list

    def _prune_stale_down_keys(self, now: float) -> None:
        stale: List[Tuple[int, bool]] = []
        for k_sc, k_ext in list(self._down_keys):
            ts = self._down_key_ts.get((k_sc, k_ext), 0.0)
            if is_special_media_or_launch_scancode(k_sc, k_ext):
                if (now - ts) > self.SPECIAL_KEY_CHORD_WINDOW_SEC:
                    stale.append((k_sc, k_ext))
            elif (now - ts) > 0.35 and not is_scancode_physically_down(k_sc, k_ext):
                stale.append((k_sc, k_ext))
        for k in stale:
            self._down_keys.discard(k)
            self._down_key_ts.pop(k, None)
            if k[0] in MODIFIER_SCANCODES:
                self._down_modifiers.discard(k[0])

    def _is_key_active_for_chord(self, k: Tuple[int, bool], now: float) -> bool:
        if k in self._down_keys:
            return True
        if is_special_media_or_launch_scancode(k[0], k[1]):
            last_ts = self._recent_special_keys.get(k, 0.0)
            if (now - last_ts) <= self.SPECIAL_KEY_CHORD_WINDOW_SEC:
                return True
        return False

    def _chord_matches_down_event(
        self,
        chord: Tuple[Tuple[int, bool], ...],
        current_kid: Tuple[int, bool],
        now: float,
        ignore_extra_modifiers: bool = False,
    ) -> bool:
        if not chord:
            return False
        matched_current = (current_kid in chord) or any(sc == current_kid[0] for sc, _ in chord)
        if not matched_current:
            return False
        for k in chord:
            if k[0] == current_kid[0]:
                continue
            if not self._is_key_active_for_chord(k, now) and not any(
                dk[0] == k[0] for dk in self._down_keys
            ):
                return False
        if not ignore_extra_modifiers and len(chord) == 1 and chord[0][0] not in MODIFIER_SCANCODES:
            if self._down_modifiers & {0x1D, 0x38, 0x5B, 0x5C}:
                return False
        return True

    def _on_win32_key(self, scan: int, ext: bool, is_down: bool, is_injected: bool) -> bool:
        if is_injected:
            return False

        now = time.perf_counter()
        kid = (scan, ext)
        is_up = not is_down
        is_special = is_special_media_or_launch_scancode(scan, ext)

        if is_down:
            self._prune_stale_down_keys(now)
            if is_special:
                self._recent_special_keys[kid] = now

        if scan in MODIFIER_SCANCODES:
            if is_down:
                self._down_modifiers.add(scan)
            else:
                self._down_modifiers.discard(scan)

        label = label_for_scancode(scan, ext)

        # 1. Key Capture Mode in UI
        if self.state.is_capturing_key:
            if is_down:
                self._down_keys.add(kid)
                self._down_key_ts[kid] = now
            else:
                self._down_keys.discard(kid)
                self._down_key_ts.pop(kid, None)
            self.state.hook_key_event.emit(scan, ext, is_down, label)
            return True

        # 2a. Global Emergency Panic Stop (Default: Ctrl + Shift + Alt) — ALWAYS active regardless of state!
        if is_down:
            akp_eng = getattr(self.state, "autopresser_engine", None)
            akp_running = bool(akp_eng is not None and akp_eng.any_tab_running())
            panic_chord = tuple(parse_chord_string(self.state.config.hotkey_panic_stop or "Ctrl + Shift + Alt"))
            if panic_chord and self._chord_matches_down_event(
                panic_chord,
                kid,
                now,
                ignore_extra_modifiers=len(panic_chord) == 1,
            ):
                if self.state.is_playing or self.state.is_recording or self._running_combos or self._active_toggle_loops or any(self._combo_queued_counts.values()) or akp_running:
                    self._suppressed_down_keys.add(kid)
                    self.stop_all()
                    return True

            # Global Stop-All AutoKeyPresser Tabs Hotkey (Synchronized between Settings & Auto Presser)
            akp_stop_all_str = getattr(self.state.config, "hotkey_autopresser_stop_all", "F7") or "F7"
            akp_stop_all_chord = tuple(parse_chord_string(akp_stop_all_str))
            if akp_running and akp_stop_all_chord and self._chord_matches_down_event(
                akp_stop_all_chord,
                kid,
                now,
                ignore_extra_modifiers=len(akp_stop_all_chord) == 1,
            ):
                self._suppressed_down_keys.add(kid)
                akp_eng.stop_all_tabs()
                self.state.toast_requested.emit(
                    tl("تم إيقاف جميع تبويبات الضغط التلقائي", "Stopped all Auto-Presser tabs"),
                    "info",
                )
                return True

            # Per-Tab Start/Stop Toggle Hotkey in AutoKeyPresser (Starts tab if stopped, stops tab if running)
            if akp_eng is not None and not self.state.is_recording and kid not in self._down_keys:
                for tab_obj in list(akp_eng.tabs):
                    thk = (tab_obj.stop_hotkey or "").strip()
                    if not thk:
                        continue
                    t_chord = tuple(parse_chord_string(thk))
                    if t_chord and self._chord_matches_down_event(t_chord, kid, now):
                        self._down_keys.add(kid)
                        self._down_key_ts[kid] = now
                        self._suppressed_down_keys.add(kid)
                        now_running = akp_eng.toggle_tab(tab_obj)
                        if now_running:
                            self.state.toast_requested.emit(
                                tl(f"▶ بدأ تشغيل التبويب: {tab_obj.title}", f"▶ Started tab: {tab_obj.title}"),
                                "ok",
                            )
                        else:
                            self.state.toast_requested.emit(
                                tl(f"⏹ تم إيقاف التبويب: {tab_obj.title}", f"⏹ Stopped tab: {tab_obj.title}"),
                                "info",
                            )
                        return True

        # 2b. Priority Stop Check during Active Playback or Active Loops!
        if is_down and (self.state.is_playing or self._running_combos or self._active_toggle_loops):
            if self._running_combos or self._active_toggle_loops:
                active_cids = set(self._running_combos) | set(self._active_toggle_loops)
                for c_dict in self.state.smart_combos:
                    cid = str(c_dict.get("id", ""))
                    if cid not in active_cids:
                        continue
                    shk = str(c_dict.get("stop_hotkey", "")).strip()
                    if not shk:
                        continue
                    shk_chord = tuple(parse_chord_string(shk))
                    if shk_chord and self._chord_matches_down_event(
                        shk_chord,
                        kid,
                        now,
                        ignore_extra_modifiers=True,
                    ):
                        self._suppressed_down_keys.add(kid)
                        self.stop_combo(cid)
                        self.state.toast_requested.emit(t("toast_combo_stopped_by_hk"), "info")
                        return True

            if self.state.is_playing:
                play_chord = tuple(parse_chord_string(self.state.config.hotkey_playback_toggle))
                if self._chord_matches_down_event(play_chord, kid, now, ignore_extra_modifiers=True):
                    self._suppressed_down_keys.add(kid)
                    self.stop_all()
                    return True

                for r_chord, _ in self._recording_hotkeys:
                    if self._chord_matches_down_event(r_chord, kid, now, ignore_extra_modifiers=True):
                        self._suppressed_down_keys.add(kid)
                        self.stop_all()
                        return True

        # 3. If this key is currently suppressed (e.g. Start/Stop Recording hotkey held down or releasing),
        # swallow BOTH auto-repeat key_down AND the final key_up so it NEVER leaks into recorded_events!
        if kid in self._suppressed_down_keys or any(sk[0] == scan for sk in self._suppressed_down_keys):
            if is_up:
                for sk in [k for k in self._suppressed_down_keys if k[0] == scan]:
                    self._suppressed_down_keys.discard(sk)
                for dk in [k for k in self._down_keys if k[0] == scan]:
                    self._down_keys.discard(dk)
                    self._down_key_ts.pop(dk, None)
            return True

        # 4. F8 Live Cursor Coordinate Grabber
        f8_sc, _ = scancode_for_label("F8")
        if is_down and scan == f8_sc and kid not in self._down_keys and not self.state.is_recording:
            x, y = get_cursor_pos()
            self.state.cursor_pick_hotkey_pressed.emit(x, y)

        # 5. Global Hotkeys (Record Start/Stop, Play Start, Toggle Smart Keys)
        rec_chord = tuple(parse_chord_string(self.state.config.hotkey_record_toggle))
        if is_down and self._chord_matches_down_event(rec_chord, kid, now, ignore_extra_modifiers=self.state.is_recording):
            for rk in rec_chord:
                self._down_keys.add(rk)
                self._down_key_ts[rk] = now
                self._suppressed_down_keys.add(rk)
                self._recording_down_keys.discard(rk)
            self._down_keys.add(kid)
            self._down_key_ts[kid] = now
            self._suppressed_down_keys.add(kid)
            self._recording_down_keys.discard(kid)

            if self.state.is_recording:
                self._strip_stop_hotkey_from_recording(rec_chord)
                self.stop_recording(stopped_via_mouse=False)
            else:
                self.start_recording()
            return True

        if is_down and kid not in self._down_keys and not self.state.is_recording:
            play_chord = tuple(parse_chord_string(self.state.config.hotkey_playback_toggle))
            cmb_chord = tuple(parse_chord_string(self.state.config.hotkey_combo_master_toggle))

            if self._chord_matches_down_event(play_chord, kid, now):
                self._down_keys.add(kid)
                self._down_key_ts[kid] = now
                self._suppressed_down_keys.add(kid)
                self.toggle_playback()
                return True
            if self._chord_matches_down_event(cmb_chord, kid, now):
                self._down_keys.add(kid)
                self._down_key_ts[kid] = now
                self._suppressed_down_keys.add(kid)
                self.toggle_combos_master()
                return True

        # 6. Live Recording Mode
        if self.state.is_recording and self.state.config.record_keyboard:
            if is_own_app_foreground():
                return False
            if is_down and kid not in self._recording_down_keys:
                self._recording_down_keys.add(kid)
                delay_ms = max(5, int((now - self._last_record_ts) * 1000))
                self._last_record_ts = now
                ev = MacroEvent(kind="key_down", delay_ms=delay_ms, key=label, scan_code=scan, is_extended=ext)
                self.state.recorded_events.append(ev.to_dict())
                self._emit_throttled_recording_count(now)
            elif is_up and kid in self._recording_down_keys:
                self._recording_down_keys.discard(kid)
                delay_ms = max(5, int((now - self._last_record_ts) * 1000))
                self._last_record_ts = now
                ev = MacroEvent(kind="key_up", delay_ms=delay_ms, key=label, scan_code=scan, is_extended=ext)
                self.state.recorded_events.append(ev.to_dict())
                self._emit_throttled_recording_count(now)

            if is_down:
                self._down_keys.add(kid)
                self._down_key_ts[kid] = now
            else:
                self._down_keys.discard(kid)
                self._down_key_ts.pop(kid, None)
            return False

        # Self-Protection: NEVER trigger Saved Recordings or Smart Keys when V-MacroPlus's own window is in the foreground
        if is_own_app_foreground():
            if is_down:
                self._down_keys.add(kid)
                self._down_key_ts[kid] = now
            else:
                self._down_keys.discard(kid)
                self._down_key_ts.pop(kid, None)
            return False

        # 7. Saved Recording Hotkeys
        if is_down and kid not in self._down_keys and not self.state.is_recording:
            for r_chord, m_dict in self._recording_hotkeys:
                if self._chord_matches_down_event(r_chord, kid, now):
                    self._down_keys.add(kid)
                    self._down_key_ts[kid] = now
                    self._suppressed_down_keys.add(kid)
                    if self.state.is_playing:
                        self.stop_all()
                    else:
                        self.start_playback(events=m_dict.get("events", []))
                    return True

        # 8. Smart Key Chords
        if self.state.combos_active and not self.state.is_recording:
            if is_down and (kid not in self._down_keys or is_special):
                active_proc: Optional[str] = None
                for chord, c_dict in self._chord_triggers:
                    if not self._chord_matches_down_event(chord, kid, now):
                        continue
                    excluded_list = [
                        str(x).strip().lower()
                        for x in c_dict.get("excluded_processes", [])
                        if str(x).strip()
                    ]
                    if excluded_list:
                        if active_proc is None:
                            active_proc = get_foreground_process_name()
                        if active_proc and active_proc.lower() in excluded_list:
                            continue

                    target_proc = str(c_dict.get("target_process", "")).strip().lower()
                    if target_proc:
                        if active_proc is None:
                            active_proc = get_foreground_process_name()
                        if active_proc != target_proc:
                            continue

                    self._down_keys.add(kid)
                    self._down_key_ts[kid] = now
                    for ck in chord:
                        self._recent_special_keys.pop(ck, None)
                    self.execute_smart_combo(c_dict, release_chord=chord)
                    if self.state.config.suppress_trigger_key or is_special:
                        self._suppressed_down_keys.add(kid)
                        return True
                    return False

                if is_special and kid in self._special_keys_in_chords:
                    self._down_keys.add(kid)
                    self._down_key_ts[kid] = now
                    self._suppressed_down_keys.add(kid)
                    return True

        if is_down:
            self._down_keys.add(kid)
            self._down_key_ts[kid] = now
        else:
            self._down_keys.discard(kid)
            self._down_key_ts.pop(kid, None)
        return False

    def _strip_stop_hotkey_from_recording(self, rec_chord: Tuple[Tuple[int, bool], ...]) -> None:
        if not self.state.recorded_events or not rec_chord:
            return
        chord_scans = {sc for sc, _ in rec_chord}
        chord_labels = {label_for_scancode(sc, ext).upper() for sc, ext in rec_chord}
        while self.state.recorded_events:
            last = self.state.recorded_events[-1]
            kind = str(last.get("kind", ""))
            sc = int(last.get("scan_code", 0))
            lbl = str(last.get("key", "")).upper()
            if kind in ("key_down", "key_up", "key_tap", "key_hold") and (
                sc in chord_scans or lbl in chord_labels
            ):
                self.state.recorded_events.pop()
            else:
                break

    def _strip_stop_mouse_click_from_recording(self) -> None:
        if not self.state.recorded_events:
            return
        while self.state.recorded_events:
            last = self.state.recorded_events[-1]
            kind = str(last.get("kind", ""))
            if "mouse" in kind and self.state.is_point_on_stop_button(int(last.get("x", -99999)), int(last.get("y", -99999))):
                self.state.recorded_events.pop()
            else:
                break

        if self._stop_button_mouse_ignored:
            return

        removed_down = False
        while self.state.recorded_events:
            last = self.state.recorded_events[-1]
            kind = str(last.get("kind", ""))
            if kind == "mouse_move" and int(last.get("delay_ms", 0)) < 350:
                self.state.recorded_events.pop()
            elif kind in ("mouse_up", "mouse_click") and not removed_down:
                self.state.recorded_events.pop()
            elif kind == "mouse_down" and not removed_down:
                self.state.recorded_events.pop()
                removed_down = True
                while (
                    self.state.recorded_events
                    and self.state.recorded_events[-1].get("kind") == "mouse_move"
                    and int(self.state.recorded_events[-1].get("delay_ms", 0)) < 120
                ):
                    self.state.recorded_events.pop()
                break
            else:
                break

    def _emit_throttled_recording_count(self, now: float) -> None:
        if (now - self._last_ui_count_emit_ts) >= 0.10:
            self._last_ui_count_emit_ts = now
            self.state.recording_count_changed.emit(len(self.state.recorded_events))

    def _on_fallback_key_press(self, key: Any) -> None:
        if IS_WINDOWS or self._simulating_fallback_flag:
            return
        raw = getattr(key, "name", None) or getattr(key, "char", None) or str(key)
        lbl = str(raw).replace("'", "").capitalize()
        sc, ext = scancode_for_label(lbl)
        self._on_win32_key(sc, ext, True, False)

    def _on_fallback_key_release(self, key: Any) -> None:
        if IS_WINDOWS or self._simulating_fallback_flag:
            return
        raw = getattr(key, "name", None) or getattr(key, "char", None) or str(key)
        lbl = str(raw).replace("'", "").capitalize()
        sc, ext = scancode_for_label(lbl)
        self._on_win32_key(sc, ext, False, False)

    # =========================================================================
    # Persistence & 100% 1-to-1 Conversion from Recording to Smart Key
    # =========================================================================
    def load_all_data_async(self) -> None:
        def _io_load() -> tuple[List[Dict[str, Any]], List[Dict[str, Any]]]:
            c_path = get_combos_path()
            if c_path.exists():
                try:
                    raw_combos = json.loads(c_path.read_text(encoding="utf-8"))
                    combos = [SmartCombo.from_dict(c).to_dict() for c in raw_combos if isinstance(c, dict)]
                except Exception:
                    combos = get_default_smart_combos()
            else:
                combos = get_default_smart_combos()
                c_path.write_text(json.dumps(combos, ensure_ascii=False, indent=2), encoding="utf-8")

            m_dir = get_macros_dir()
            macros: List[Dict[str, Any]] = []
            for fp in sorted(m_dir.glob("*.json")):
                try:
                    macros.append(json.loads(fp.read_text(encoding="utf-8")))
                except Exception:
                    pass

            if not macros:
                sample = {
                    "id": "macro_sample_01",
                    "name": "W -> Wait 1s -> Space",
                    "hotkey": "",
                    "created_at": datetime.now().strftime("%Y-%m-%d %H:%M"),
                    "events": get_starter_recorded_macro(),
                }
                (m_dir / "macro_sample_01.json").write_text(
                    json.dumps(sample, ensure_ascii=False, indent=2), encoding="utf-8"
                )
                macros.append(sample)

            return combos, macros

        def _on_loaded(res: tuple[List[Dict[str, Any]], List[Dict[str, Any]]]) -> None:
            combos, macros = res
            self.state.smart_combos = combos
            self.state.saved_macros = macros
            if not self.state.recorded_events:
                self.state.recorded_events = get_starter_recorded_macro()
            self.rebuild_trigger_map()
            self.state.combos_list_updated.emit()
            self.state.macros_library_updated.emit()
            self.state.events_updated.emit()

        run_async(_io_load, on_done=_on_loaded)

    def save_combos_async(self) -> None:
        self.rebuild_trigger_map()
        combos_copy = list(self.state.smart_combos)

        def _io_write() -> None:
            get_combos_path().write_text(
                json.dumps(combos_copy, ensure_ascii=False, indent=2), encoding="utf-8"
            )

        run_async(_io_write)

    def update_saved_macro(self, macro_id: str, name: str, events: List[Dict[str, Any]]) -> bool:
        clean_name = name.strip() or "Macro"
        for entry in self.state.saved_macros:
            if str(entry.get("id", "")) == str(macro_id):
                entry["name"] = clean_name
                entry["events"] = [dict(e) for e in events]
                self.rebuild_trigger_map()

                def _io_upd() -> None:
                    fp = get_macros_dir() / f"{macro_id}.json"
                    fp.write_text(json.dumps(entry, ensure_ascii=False, indent=2), encoding="utf-8")

                def _done(_) -> None:
                    self.state.macros_library_updated.emit()
                    self.state.toast_requested.emit(
                        tl(f"✓ تم حفظ التعديلات على «{clean_name}»", f"✓ Saved changes to '{clean_name}'"),
                        "ok",
                    )

                run_async(_io_upd, on_done=_done)
                return True
        return False

    def save_current_macro_to_library(self, name: str, hotkey: str = "") -> None:
        clean_name = name.strip() or f"Macro {len(self.state.saved_macros) + 1}"
        macro_id = f"macro_{int(time.time() * 1000)}"
        events_copy = list(self.state.recorded_events)
        entry = {
            "id": macro_id,
            "name": clean_name,
            "hotkey": hotkey,
            "created_at": datetime.now().strftime("%Y-%m-%d %H:%M"),
            "events": events_copy,
        }
        self.state.saved_macros.insert(0, entry)
        self.rebuild_trigger_map()

        def _io_save() -> None:
            fp = get_macros_dir() / f"{macro_id}.json"
            fp.write_text(json.dumps(entry, ensure_ascii=False, indent=2), encoding="utf-8")

        def _done(_) -> None:
            self.state.macros_library_updated.emit()
            self.state.toast_requested.emit(
                tl(f"تم حفظ التسجيل «{clean_name}»", f"Saved '{clean_name}'"),
                "ok",
            )

        run_async(_io_save, on_done=_done)

    def update_saved_macro_hotkey(self, macro_id: str, new_hotkey: str) -> None:
        target: Optional[Dict[str, Any]] = None
        for m in self.state.saved_macros:
            if m.get("id") == macro_id:
                m["hotkey"] = new_hotkey
                target = dict(m)
                break
        self.rebuild_trigger_map()
        if target is not None:
            def _io_upd() -> None:
                fp = get_macros_dir() / f"{macro_id}.json"
                fp.write_text(json.dumps(target, ensure_ascii=False, indent=2), encoding="utf-8")
            run_async(_io_upd)

    def delete_saved_macro(self, macro_id: str) -> None:
        self.state.saved_macros = [m for m in self.state.saved_macros if m.get("id") != macro_id]
        self.rebuild_trigger_map()

        def _io_del() -> None:
            fp = get_macros_dir() / f"{macro_id}.json"
            if fp.exists():
                fp.unlink()

        run_async(_io_del, on_done=lambda _: self.state.macros_library_updated.emit())

    def convert_recording_to_smart_combo(self, name: str, events: List[Dict[str, Any]]) -> None:
        steps: List[ComboStep] = []
        for raw in events:
            ev = MacroEvent.from_dict(raw)
            steps.append(ComboStep.from_macro_event(ev))

        if not steps:
            steps.append(ComboStep(step_type="key_tap", value="Space", scan_code=0x39, delay_ms=20))

        new_combo = SmartCombo(
            id=f"combo_{int(time.time() * 1000)}",
            name=name,
            trigger_key="",
            trigger_scan=0,
            trigger_chord=[],
            mode="once",
            enabled=False,
            steps=steps,
        ).to_dict()
        self.state.smart_combos.insert(0, new_combo)
        self.save_combos_async()
        self.state.combos_list_updated.emit()
        self.state.navigation_requested.emit("combos")
        self.state.toast_requested.emit(
            tl(f"تم تحويل التسجيل بالكامل ({len(steps)} خطوة متطابقة ١٠٠٪) إلى الأزرار الذكية!", f"Converted all {len(steps)} steps 1-to-1 to Smart Keys!"),
            "ok",
        )

    # =========================================================================
    # Recording & Advanced Playback Controls
    # =========================================================================
    def export_all_backup(self, file_path: str) -> bool:
        try:
            from pathlib import Path
            bundle = {
                "app": "V-MacroPlus",
                "backup_version": 3,
                "exported_at": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                "smart_combos": list(self.state.smart_combos),
                "saved_macros": list(self.state.saved_macros),
                "current_recorded_events": list(self.state.recorded_events),
            }
            Path(file_path).write_text(json.dumps(bundle, ensure_ascii=False, indent=2), encoding="utf-8")
            self.state.toast_requested.emit(
                tl("تم تصدير النسخة الاحتياطية الشاملة بنجاح!", "Full backup exported successfully!"),
                "ok",
            )
            return True
        except Exception as exc:
            self.state.toast_requested.emit(f"Export Error: {exc}", "danger")
            return False

    def import_all_backup(self, file_path: str) -> bool:
        try:
            from pathlib import Path
            data = json.loads(Path(file_path).read_text(encoding="utf-8"))
            if isinstance(data, dict) and ("smart_combos" in data or "saved_macros" in data):
                if "smart_combos" in data and isinstance(data["smart_combos"], list):
                    self.state.smart_combos = [SmartCombo.from_dict(c).to_dict() for c in data["smart_combos"] if isinstance(c, dict)]
                    self.save_combos_async()
                if "saved_macros" in data and isinstance(data["saved_macros"], list):
                    self.state.saved_macros = data["saved_macros"]
                    m_dir = get_macros_dir()
                    for old_fp in m_dir.glob("*.json"):
                        try:
                            old_fp.unlink()
                        except Exception:
                            pass
                    for m_entry in self.state.saved_macros:
                        mid = m_entry.get("id", f"macro_{int(time.time() * 1000)}")
                        (m_dir / f"{mid}.json").write_text(
                            json.dumps(m_entry, ensure_ascii=False, indent=2), encoding="utf-8"
                        )
                if "current_recorded_events" in data and isinstance(data["current_recorded_events"], list):
                    self.state.recorded_events = data["current_recorded_events"]
                self.rebuild_trigger_map()
                self.state.combos_list_updated.emit()
                self.state.macros_library_updated.emit()
                self.state.events_updated.emit()
                self.state.toast_requested.emit(
                    tl("تم استيراد جميع الأزرار الذكية والتسجيلات بنجاح!", "All Smart Keys & Recordings imported!"),
                    "ok",
                )
                return True
            elif isinstance(data, dict) and "events" in data:
                prev = list(self.state.recorded_events)
                self.state.recorded_events = data["events"]
                self.save_current_macro_to_library(data.get("name", Path(file_path).stem), data.get("hotkey", ""))
                self.state.recorded_events = prev
                return True
        except Exception as exc:
            self.state.toast_requested.emit(f"Import Error: {exc}", "danger")
        return False

    def toggle_recording(self, via_mouse: bool = False) -> None:
        if self.state.is_recording:
            self.stop_recording(stopped_via_mouse=via_mouse)
        else:
            self.start_recording()

    def start_recording(self) -> None:
        if self.state.is_playing:
            self.stop_all()
        self.state.recorded_events.clear()
        self._recording_down_keys.clear()
        self._stop_button_mouse_ignored = False
        for k in list(self._down_keys):
            self._suppressed_down_keys.add(k)
        self._last_record_ts = time.perf_counter()
        self._last_mouse_sample_ts = self._last_record_ts
        self._last_ui_count_emit_ts = 0.0
        self.state.is_recording = True
        self.state.sound_requested.emit("start")
        self.state.recording_state_changed.emit(True)
        self.state.recording_count_changed.emit(0)

    def stop_recording(self, stopped_via_mouse: bool = False) -> None:
        if not self.state.is_recording:
            return
        self.state.is_recording = False
        self._recording_down_keys.clear()

        rec_chord = tuple(parse_chord_string(self.state.config.hotkey_record_toggle))
        self._strip_stop_hotkey_from_recording(rec_chord)
        if stopped_via_mouse:
            self._strip_stop_mouse_click_from_recording()

        self.state.sound_requested.emit("stop")
        self.state.recording_state_changed.emit(False)
        self.state.events_updated.emit()
        count = len(self.state.recorded_events)
        self.state.toast_requested.emit(
            tl(f"تم إيقاف التسجيل ({count} خطوة)", f"Recording stopped ({count} steps)"),
            "ok",
        )

    def _release_all_playback_held_inputs(self) -> None:
        """
        Safely release any keys or mouse buttons that were held down (`key_down` / `mouse_down`)
        when playback was stopped mid-loop by the user's Stop Hotkey.
        """
        method = self.state.config.input_method
        for sc, ext in list(self._playback_held_keys):
            if IS_WINDOWS and sc > 0:
                send_scancode_key(sc, ext, key_up=True, method=method)
        self._playback_held_keys.clear()

        cx, cy = get_cursor_pos()
        for btn in list(self._playback_held_mouse_buttons):
            if IS_WINDOWS:
                send_mouse_button(x=cx, y=cy, button=btn, action="up", move_cursor=False, method=method)
        self._playback_held_mouse_buttons.clear()

    def toggle_playback(self) -> None:
        if self.state.is_playing:
            self.stop_all()
        else:
            self.start_playback()

    def start_playback(
        self,
        events: Optional[List[Dict[str, Any]]] = None,
        speed: Optional[float] = None,
        repeat_count: Optional[int] = None,
        allow_self_test: bool = False,
    ) -> None:
        if self.state.is_recording:
            self.stop_recording()
        if self.state.is_playing:
            self.stop_all()
            return

        ev_list = list(events if events is not None else self.state.recorded_events)
        if not ev_list:
            self.state.toast_requested.emit(tl("لا توجد خطوات لتشغيلها", "No steps to play"), "danger")
            return

        cfg = self.state.config
        spd = max(0.1, float(speed if speed is not None else cfg.default_playback_speed))
        rep_mode = cfg.repeat_mode if repeat_count is None else "count"
        reps = int(repeat_count if repeat_count is not None else cfg.default_repeat_count)

        unit_mult = 3600.0 if cfg.repeat_duration_unit == "hours" else 60.0 if cfg.repeat_duration_unit == "minutes" else 1.0
        max_duration_sec = max(1.0, float(cfg.repeat_duration_value) * unit_mult)

        delay_between_enabled = bool(cfg.repeat_delay_enabled)
        delay_between_ms = max(0, int(cfg.repeat_delay_ms))
        delay_random = bool(cfg.repeat_delay_random)

        self._playback_stop.clear()
        self._was_aborted_by_user = False
        self._playback_held_keys.clear()
        self._playback_held_mouse_buttons.clear()
        self.state.is_playing = True
        self.state.playback_state_changed.emit(True)

        def _worker_play() -> int:
            executed = 0
            loop_idx = 0
            start_ts = time.perf_counter()

            try:
                start_delay_ms = max(0, int(getattr(cfg, "playback_start_delay_ms", 0)))
                if start_delay_ms > 0:
                    self._sleep_with_event(start_delay_ms / 1000.0, self._playback_stop)
                while not self._playback_stop.is_set():
                    loop_idx += 1
                    for raw_ev in ev_list:
                        if self._playback_stop.is_set():
                            break
                        if rep_mode == "duration" and (time.perf_counter() - start_ts) >= max_duration_sec:
                            break
                        ev = MacroEvent.from_dict(raw_ev)
                        if not getattr(ev, "enabled", True):
                            continue
                        wait_s = max(0.002, (ev.delay_ms / 1000.0) / spd)
                        self._sleep_with_event(wait_s, self._playback_stop)
                        if self._playback_stop.is_set():
                            break
                        if rep_mode == "duration" and (time.perf_counter() - start_ts) >= max_duration_sec:
                            break
                        self._execute_macro_event(ev, stop_ev=self._playback_stop, allow_self_test=allow_self_test)
                        executed += 1

                    if self._playback_stop.is_set():
                        break
                    if rep_mode == "once":
                        break
                    elif rep_mode == "count":
                        if reps > 0 and loop_idx >= reps:
                            break
                    elif rep_mode == "duration":
                        if (time.perf_counter() - start_ts) >= max_duration_sec:
                            break

                    if delay_between_enabled and delay_between_ms > 0:
                        actual_ms = random.uniform(0.0, float(delay_between_ms)) if delay_random else float(delay_between_ms)
                        self._sleep_with_event(actual_ms / 1000.0, self._playback_stop)
                    else:
                        self._sleep_with_event(0.04, self._playback_stop)
            finally:
                self._release_all_playback_held_inputs()

            return executed

        def _on_done(_: int) -> None:
            if self.state.is_playing:
                self.state.is_playing = False
                self.state.playback_state_changed.emit(False)

            if not self.state.config.do_not_activate_on_stop:
                self.state.activate_window_requested.emit()

            if self.state.config.shutdown_on_finish and not self._was_aborted_by_user:
                self._trigger_system_shutdown()

        run_async(_worker_play, on_done=_on_done)

    def _trigger_system_shutdown(self) -> None:
        self.state.toast_requested.emit(
            tl("اكتمل التكرار — سيتم إيقاف تشغيل الكمبيوتر خلال 5 ثوانٍ...", "Repeat finished — shutting down PC in 5s..."),
            "danger",
        )
        if IS_WINDOWS:
            try:
                subprocess.Popen(
                    ["shutdown", "/s", "/t", "5"],
                    creationflags=0x08000000,
                )
            except Exception:
                pass

    # =========================================================================
    # Smart Key & Single-Step Execution (100% Unified with Recorder Events!)
    # =========================================================================
    def toggle_combos_master(self) -> None:
        new_state = not self.state.combos_active
        self.state.combos_active = new_state
        self.state.update_setting("combos_enabled_globally", new_state)
        self.state.combos_master_changed.emit(new_state)

    def execute_single_step_after(self, step_dict: Dict[str, Any], delay_ms: int = 1000) -> None:
        st = ComboStep.from_dict(step_dict)
        stop_ev = threading.Event()

        def _worker() -> None:
            if delay_ms > 0:
                self._sleep_with_event(delay_ms / 1000.0, stop_ev)
            if not stop_ev.is_set():
                self._execute_combo_step(st, stop_ev=stop_ev)

        run_async(_worker)

    def execute_smart_combo(
        self,
        combo_dict: Dict[str, Any],
        release_chord: Optional[Tuple[Tuple[int, bool], ...]] = None,
        initial_delay_ms: int = 0,
        allow_self_test: bool = False,
    ) -> None:
        combo = SmartCombo.from_dict(combo_dict)
        if not combo.steps:
            return

        cid = combo.id
        chord = release_chord or tuple(combo.chord_tuples())
        method = self.state.config.input_method

        if combo.mode == "toggle_loop":
            if cid in self._active_toggle_loops:
                self._active_toggle_loops.discard(cid)
                ev = self._combo_stops.get(cid)
                if ev is not None:
                    ev.set()
                return
            self._active_toggle_loops.add(cid)

        policy = getattr(combo, "retrigger_policy", "ignore")
        if policy not in ("ignore", "restart", "queue"):
            policy = "ignore"

        prev_done_ev: Optional[threading.Event] = None
        if cid in self._running_combos:
            if policy == "ignore":
                return
            elif policy == "queue":
                self._combo_queued_counts[cid] = min(10, self._combo_queued_counts.get(cid, 0) + 1)
                return
            elif policy == "restart":
                old_stop = self._combo_stops.get(cid)
                if old_stop is not None:
                    old_stop.set()
                prev_done_ev = self._combo_done_events.get(cid)
                self._combo_queued_counts[cid] = 0

        stop_ev = threading.Event()
        done_ev = threading.Event()
        self._combo_stops[cid] = stop_ev
        self._combo_done_events[cid] = done_ev
        self._running_combos.add(cid)

        def _worker_combo() -> None:
            try:
                if prev_done_ev is not None:
                    prev_done_ev.wait(timeout=0.35)

                if initial_delay_ms > 0:
                    self._sleep_with_event(initial_delay_ms / 1000.0, stop_ev)

                if IS_WINDOWS and chord and len(chord) > 1:
                    for sc_m, ext_m in chord:
                        if sc_m in MODIFIER_SCANCODES:
                            send_scancode_key(sc_m, ext_m, key_up=True, method=method)

                while not stop_ev.is_set():
                    for step in combo.steps:
                        if stop_ev.is_set():
                            break
                        if not getattr(step, "enabled", True):
                            continue
                        if step.step_type == "wait":
                            self._sleep_with_event(max(0.001, step.delay_ms / 1000.0), stop_ev)
                        else:
                            if step.delay_ms > 0:
                                self._sleep_with_event(max(0.001, step.delay_ms / 1000.0), stop_ev)
                            if stop_ev.is_set():
                                break
                            self._execute_combo_step(step, stop_ev=stop_ev, allow_self_test=allow_self_test)

                    if stop_ev.is_set():
                        break
                    if combo.mode == "once" or initial_delay_ms > 0:
                        queued = self._combo_queued_counts.get(cid, 0)
                        if queued > 0:
                            self._combo_queued_counts[cid] = queued - 1
                            continue
                        break
                    elif combo.mode == "repeat_hold":
                        if not chord or not all(k in self._down_keys for k in chord):
                            break
                        self._sleep_with_event(0.02, stop_ev)
                    elif combo.mode == "toggle_loop":
                        if cid not in self._active_toggle_loops:
                            break
                        self._sleep_with_event(0.03, stop_ev)
                    else:
                        break
            finally:
                self._release_all_playback_held_inputs()
                if self._combo_done_events.get(cid) is done_ev:
                    self._running_combos.discard(cid)
                    self._active_toggle_loops.discard(cid)
                    self._combo_queued_counts.pop(cid, None)
                done_ev.set()

        run_async(_worker_combo)

    def stop_combo(self, cid: str) -> None:
        """Immediately stop a specific running/repeating Smart Key combo by its `cid`."""
        self._active_toggle_loops.discard(cid)
        self._combo_queued_counts.pop(cid, None)
        ev = self._combo_stops.get(cid)
        if ev is not None:
            ev.set()
        self._running_combos.discard(cid)
        self._release_all_playback_held_inputs()

    def stop_all(self) -> None:
        akp_eng = getattr(self.state, "autopresser_engine", None)
        if akp_eng is not None:
            try:
                akp_eng.stop_all_tabs()
            except Exception:
                pass
        self._was_aborted_by_user = True
        self._playback_stop.set()
        self._active_toggle_loops.clear()
        self._combo_queued_counts.clear()
        for ev in list(self._combo_stops.values()):
            ev.set()
        self._running_combos.clear()
        self._release_all_playback_held_inputs()
        if self.state.is_recording:
            self.stop_recording()
        if self.state.is_playing:
            self.state.is_playing = False
            self.state.playback_state_changed.emit(False)
        hk_play = self.state.config.hotkey_playback_toggle or "F10"
        hk_stop = self.state.config.hotkey_panic_stop or "Ctrl + Shift + Alt"
        self.state.sound_requested.emit("stop")
        self.state.toast_requested.emit(
            t("toast_panic_stopped").format(hk_stop=hk_stop),
            "info",
        )

    def _sleep_with_event(self, seconds: float, stop_ev: threading.Event) -> None:
        end_t = time.perf_counter() + seconds
        while time.perf_counter() < end_t:
            if stop_ev.is_set():
                return
            rem = end_t - time.perf_counter()
            time.sleep(min(0.010, max(0.001, rem)))

    def _tap_or_hold_scancode(
        self,
        scan_code: int,
        is_extended: bool,
        hold_ms: int = 0,
        fallback_label: str = "",
        stop_ev: Optional[threading.Event] = None,
    ) -> None:
        method = self.state.config.input_method
        hold_s = max(0.015, hold_ms / 1000.0) if hold_ms > 0 else 0.015
        if IS_WINDOWS and scan_code > 0:
            self._playback_held_keys.add((scan_code, is_extended))
            send_scancode_key(scan_code, is_extended, key_up=False, method=method)
            if stop_ev is not None and hold_s > 0.02:
                self._sleep_with_event(hold_s, stop_ev)
            else:
                time.sleep(hold_s)
            send_scancode_key(scan_code, is_extended, key_up=True, method=method)
            self._playback_held_keys.discard((scan_code, is_extended))
            return
        if _PYNPUT_AVAILABLE and self._kb_controller and fallback_label:
            self._simulating_fallback_flag = True
            try:
                k = fallback_label.lower() if len(fallback_label) == 1 else getattr(pynput_kb.Key, fallback_label.lower(), fallback_label.lower())
                self._kb_controller.press(k)
                if stop_ev is not None and hold_s > 0.02:
                    self._sleep_with_event(hold_s, stop_ev)
                else:
                    time.sleep(hold_s)
                self._kb_controller.release(k)
            except Exception:
                pass
            finally:
                self._simulating_fallback_flag = False

    def _execute_python_mini_script(
        self,
        code: str,
        method: str = "sendinput_scancode",
        stop_ev: Optional[threading.Event] = None,
        allow_self_test: bool = False,
    ) -> None:
        """
        Execute a user-written Mini-Python Automation Script (`for`, `if`, `while`, `print`, `press`, `wait`, `click`)
        inside a safe, restricted namespace with cooperative stop support (`stop_ev`) and self-app immunity.
        """
        class _ScriptAborted(Exception):
            pass

        def _check_stop() -> None:
            if stop_ev is not None and stop_ev.is_set():
                raise _ScriptAborted()
            if not allow_self_test and is_own_app_foreground():
                raise _ScriptAborted()

        def _fn_rand(min_val: int = 60, max_val: int = 110) -> int:
            lo, hi = int(min(min_val, max_val)), int(max(min_val, max_val))
            return random.randint(lo, hi)

        def _fn_wait(ms: float = 100) -> None:
            _check_stop()
            sec = max(0.001, float(ms) / 1000.0)
            if stop_ev is not None:
                self._sleep_with_event(sec, stop_ev)
            else:
                time.sleep(sec)
            _check_stop()

        def _fn_human_wait(min_ms: float = 65, max_ms: float = 130) -> None:
            lo, hi = float(min(min_ms, max_ms)), float(max(min_ms, max_ms))
            ms = max(5.0, random.uniform(lo, hi) + random.gauss(0.0, 2.5))
            _fn_wait(ms)

        def _fn_press(key_label: str = "Enter", hold_ms: Any = 65, human: bool = False) -> None:
            _check_stop()
            if isinstance(hold_ms, str) and hold_ms.lower() == "human":
                eff_hold = random.randint(58, 95)
            elif human:
                base_h = int(hold_ms) if isinstance(hold_ms, (int, float)) and int(hold_ms) > 25 else 72
                eff_hold = max(45, int(base_h + random.randint(-12, 16)))
            else:
                eff_hold = int(hold_ms) if isinstance(hold_ms, (int, float)) and int(hold_ms) > 0 else 65
            sc, ext = scancode_for_label(str(key_label))
            if sc > 0:
                self._tap_or_hold_scancode(sc, ext, hold_ms=eff_hold, fallback_label=str(key_label), stop_ev=stop_ev)

        def _fn_human_press(
            key_label: str = "Enter",
            min_hold: int = 58,
            max_hold: int = 95,
            min_wait: int = 65,
            max_wait: int = 130,
        ) -> None:
            _fn_press(key_label, hold_ms=_fn_rand(min_hold, max_hold), human=False)
            if max_wait > 0:
                _fn_human_wait(min_wait, max_wait)

        def _fn_hold_key(key_label: str = "W", duration_ms: int = 300) -> None:
            _fn_press(key_label, hold_ms=max(40, int(duration_ms)), human=False)

        def _fn_type_text(text: Any, enter: bool = False, human: bool = False, delay_ms: int = 0) -> None:
            _check_stop()
            txt = str(text)
            if txt:
                if IS_WINDOWS and (human or delay_ms > 0):
                    from app.domain.autopresser_engine import dispatch_text_via_method
                    dispatch_text_via_method(txt, method=method, target_hwnd=0, run_in_background=False, strict_no_fallback=False, human=bool(human), char_delay_ms=int(delay_ms))
                elif IS_WINDOWS:
                    send_unicode_text(txt)
                elif _PYNPUT_AVAILABLE and self._kb_controller:
                    self._kb_controller.type(txt)
            if enter:
                if human:
                    _fn_human_wait(45, 85)
                _fn_press("Enter", hold_ms=65, human=human)

        def _fn_print(*args: Any, sep: str = " ", enter: bool = False, human: bool = False, delay_ms: int = 0) -> None:
            _check_stop()
            txt = sep.join(str(a) for a in args)
            _fn_type_text(txt, enter=enter, human=human, delay_ms=delay_ms)

        def _fn_click(
            arg1: Any = "left",
            arg2: Any = None,
            button: str = "left",
            hold_ms: int = 68,
            human: bool = False,
            in_game_3d: bool = False,
        ) -> None:
            _check_stop()
            if not IS_WINDOWS:
                return
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
            eff_hold = random.randint(58, 95) if human else max(20, int(hold_ms) if hold_ms > 0 else 65)
            send_mouse_button(x=tx, y=ty, button=btn, action="click", move_cursor=move_cur, hold_ms=eff_hold, method=method)

        def _fn_move_rel(dx: int, dy: int, steps: int = 6, duration_ms: int = 60) -> None:
            _check_stop()
            if IS_WINDOWS:
                send_mouse_relative_move(int(dx), int(dy), steps=int(steps), duration_ms=int(duration_ms), method=method)

        safe_builtins = {
            "range": range,
            "len": len,
            "int": int,
            "float": float,
            "str": str,
            "bool": bool,
            "list": list,
            "dict": dict,
            "tuple": tuple,
            "set": set,
            "min": min,
            "max": max,
            "abs": abs,
            "round": round,
            "enumerate": enumerate,
            "zip": zip,
            "True": True,
            "False": False,
            "None": None,
            "print": _fn_print,
        }
        env: Dict[str, Any] = {
            "__builtins__": safe_builtins,
            "method_num": 1,
            "method_num_02": "01",
            "method_id": method,
            "method_name": method,
            "rand": _fn_rand,
            "randint": _fn_rand,
            "print": _fn_print,
            "type_text": _fn_type_text,
            "press": _fn_press,
            "send_key": _fn_press,
            "human_press": _fn_human_press,
            "hold_key": _fn_hold_key,
            "wait": _fn_wait,
            "sleep": _fn_wait,
            "sleep_ms": _fn_wait,
            "human_wait": _fn_human_wait,
            "click": _fn_click,
            "move_rel": _fn_move_rel,
        }
        try:
            exec(compile(code, "<v_macroplus_mini_script>", "exec"), env, env)
        except _ScriptAborted:
            pass
        except Exception as exc:
            self.state.toast_requested.emit(f"Python Script Error: {exc}", "danger")

    def _execute_combo_step(
        self,
        step: ComboStep,
        stop_ev: Optional[threading.Event] = None,
        allow_self_test: bool = False,
    ) -> None:
        if not getattr(step, "enabled", True):
            return
        method = self.state.config.input_method
        st = step.step_type
        if (
            not allow_self_test
            and st not in ("wait", "open_folder", "powershell", "cmd")
            and is_own_app_foreground()
        ):
            return

        if st in ("key_tap", "key_hold"):
            chord = parse_chord_string(step.value) if "+" in step.value else []
            hold_s = max(0.018, step.hold_ms / 1000.0) if step.hold_ms > 0 else 0.018
            if len(chord) > 1 and IS_WINDOWS:
                for sc, ext in chord:
                    self._playback_held_keys.add((sc, ext))
                    send_scancode_key(sc, ext, key_up=False, method=method)
                if stop_ev is not None and hold_s > 0.02:
                    self._sleep_with_event(hold_s, stop_ev)
                else:
                    time.sleep(hold_s)
                for sc, ext in reversed(chord):
                    send_scancode_key(sc, ext, key_up=True, method=method)
                    self._playback_held_keys.discard((sc, ext))
            else:
                sc, ext = step.scan_code, step.is_extended
                if sc <= 0 and step.value:
                    sc, ext = scancode_for_label(step.value)
                self._tap_or_hold_scancode(sc, ext, hold_ms=step.hold_ms, fallback_label=step.value, stop_ev=stop_ev)

        elif st == "key_down":
            sc, ext = step.scan_code, step.is_extended
            if sc <= 0 and step.value:
                sc, ext = scancode_for_label(step.value)
            if IS_WINDOWS and sc > 0:
                self._playback_held_keys.add((sc, ext))
                send_scancode_key(sc, ext, key_up=False, method=method)

        elif st == "key_up":
            sc, ext = step.scan_code, step.is_extended
            if sc <= 0 and step.value:
                sc, ext = scancode_for_label(step.value)
            if IS_WINDOWS and sc > 0:
                send_scancode_key(sc, ext, key_up=True, method=method)
                self._playback_held_keys.discard((sc, ext))

        elif st == "open_folder":
            open_folder_or_path(step.value)

        elif st in ("powershell", "cmd"):
            run_shell_command(st, step.value)

        elif st == "type_text" and step.value:
            if IS_WINDOWS:
                send_unicode_text(step.value)
            elif _PYNPUT_AVAILABLE and self._kb_controller:
                self._kb_controller.type(step.value)

        elif st == "python_script" and step.value:
            self._execute_python_mini_script(step.value, method=method, stop_ev=stop_ev, allow_self_test=allow_self_test)

        elif st in ("mouse_click", "mouse_down", "mouse_up"):
            btn = step.value or "left"
            if IS_WINDOWS:
                if st == "mouse_down":
                    self._playback_held_mouse_buttons.add(btn)
                    send_mouse_button(x=step.x, y=step.y, button=btn, action="down", move_cursor=step.use_coords, method=method)
                elif st == "mouse_up":
                    send_mouse_button(x=step.x, y=step.y, button=btn, action="up", move_cursor=step.use_coords, method=method)
                    self._playback_held_mouse_buttons.discard(btn)
                else:
                    if step.hold_ms > 0 and stop_ev is not None:
                        self._playback_held_mouse_buttons.add(btn)
                        send_mouse_button(x=step.x, y=step.y, button=btn, action="down", move_cursor=step.use_coords, method=method)
                        self._sleep_with_event(max(0.005, step.hold_ms / 1000.0), stop_ev)
                        send_mouse_button(x=step.x, y=step.y, button=btn, action="up", move_cursor=False, method=method)
                        self._playback_held_mouse_buttons.discard(btn)
                    else:
                        send_mouse_button(x=step.x, y=step.y, button=btn, action="click", move_cursor=step.use_coords, hold_ms=step.hold_ms, method=method)

        elif st == "mouse_move":
            if IS_WINDOWS:
                ctypes.windll.user32.SetCursorPos(int(step.x), int(step.y))

        elif st == "mouse_scroll":
            if IS_WINDOWS:
                send_mouse_scroll(
                    direction=step.value or "down",
                    amount=max(1, step.scroll_amount),
                )

    def _execute_macro_event(
        self,
        ev: MacroEvent,
        stop_ev: Optional[threading.Event] = None,
        allow_self_test: bool = False,
    ) -> None:
        if not getattr(ev, "enabled", True):
            return
        if ev.kind == "wait":
            return
        if not allow_self_test and is_own_app_foreground():
            return
        method = self.state.config.input_method
        sc, ext = ev.scan_code, ev.is_extended
        if sc <= 0 and ev.key:
            sc, ext = scancode_for_label(ev.key)

        if ev.kind in ("key_tap", "key_hold"):
            self._tap_or_hold_scancode(sc, ext, hold_ms=ev.hold_ms, fallback_label=ev.key, stop_ev=stop_ev)
        elif ev.kind == "key_down":
            if IS_WINDOWS and sc > 0:
                self._playback_held_keys.add((sc, ext))
                send_scancode_key(sc, ext, key_up=False, method=method)
        elif ev.kind == "key_up":
            if IS_WINDOWS and sc > 0:
                send_scancode_key(sc, ext, key_up=True, method=method)
                self._playback_held_keys.discard((sc, ext))
        elif ev.kind == "mouse_move":
            if IS_WINDOWS:
                ctypes.windll.user32.SetCursorPos(int(ev.x), int(ev.y))
        elif ev.kind in ("mouse_click", "mouse_down", "mouse_up"):
            btn = ev.button or "left"
            if IS_WINDOWS:
                if ev.kind == "mouse_down":
                    self._playback_held_mouse_buttons.add(btn)
                    send_mouse_button(x=ev.x, y=ev.y, button=btn, action="down", move_cursor=True, method=method)
                elif ev.kind == "mouse_up":
                    send_mouse_button(x=ev.x, y=ev.y, button=btn, action="up", move_cursor=True, method=method)
                    self._playback_held_mouse_buttons.discard(btn)
                else:
                    if ev.hold_ms > 0 and stop_ev is not None:
                        self._playback_held_mouse_buttons.add(btn)
                        send_mouse_button(x=ev.x, y=ev.y, button=btn, action="down", move_cursor=True, method=method)
                        self._sleep_with_event(max(0.005, ev.hold_ms / 1000.0), stop_ev)
                        send_mouse_button(x=ev.x, y=ev.y, button=btn, action="up", move_cursor=False, method=method)
                        self._playback_held_mouse_buttons.discard(btn)
                    else:
                        send_mouse_button(x=ev.x, y=ev.y, button=btn, action="click", move_cursor=True, hold_ms=ev.hold_ms, method=method)
        elif ev.kind == "mouse_scroll":
            if IS_WINDOWS:
                send_mouse_scroll(
                    direction=ev.scroll_dir or "down",
                    amount=max(1, ev.scroll_amount),
                    dx=ev.dx,
                    dy=ev.dy,
                )

    def _on_global_mouse_move(self, x: int, y: int) -> None:
        if not IS_WINDOWS:
            self._on_win32_mouse("mouse_move", int(x), int(y), "none", 0, 0, False)

    def _on_global_mouse_click(self, x: int, y: int, button: Any, pressed: bool) -> None:
        if not IS_WINDOWS:
            b_str = str(button).lower()
            b_name = "right" if "right" in b_str else "middle" if "middle" in b_str else "x2" if "x2" in b_str else "x1" if "x1" in b_str else "left"
            self._on_win32_mouse("mouse_down" if pressed else "mouse_up", int(x), int(y), b_name, 0, 0, pressed)

    def _on_global_mouse_scroll(self, x: int, y: int, dx: int, dy: int) -> None:
        if not IS_WINDOWS:
            s_dir = "up" if dy > 0 else "down" if dy < 0 else "right" if dx > 0 else "left"
            self._on_win32_mouse("mouse_scroll", int(x), int(y), s_dir, int(dx), int(dy), False)

    def shutdown(self) -> None:
        self.stop_all()
        if self._win32_kb_hook is not None:
            self._win32_kb_hook.stop()
        if self._win32_mouse_hook is not None:
            self._win32_mouse_hook.stop()
        try:
            if self._kb_listener_fallback is not None:
                self._kb_listener_fallback.stop()
            if self._mouse_listener_fallback is not None:
                self._mouse_listener_fallback.stop()
        except Exception:
            pass
