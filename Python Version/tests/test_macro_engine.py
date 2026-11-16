# -*- coding: utf-8 -*-
"""Unit tests for MacroEngine stop-recording exclusion (hotkey, chord, and mouse click)."""

import unittest
from tests._qt_stub import install_qt_stubs_if_needed

install_qt_stubs_if_needed()

from app.config import AppConfig
from app.domain.macro_engine import MacroEngine
from app.state import AppState


class TestMacroEngineRecording(unittest.TestCase):
    def setUp(self) -> None:
        self.state = AppState(AppConfig())
        self.engine = MacroEngine(self.state)

    def test_single_key_stop_hotkey_excluded(self) -> None:
        self.state.config.hotkey_record_toggle = "F9"
        self.engine.start_recording()
        self.engine._on_win32_key(0x11, False, True, False)   # W down
        self.engine._on_win32_key(0x11, False, False, False)  # W up
        self.assertEqual(len(self.state.recorded_events), 2)

        self.assertTrue(self.engine._on_win32_key(0x43, False, True, False))   # F9 down
        self.assertTrue(self.engine._on_win32_key(0x43, False, False, False))  # F9 up
        self.assertFalse(self.state.is_recording)
        self.assertEqual(len(self.state.recorded_events), 2)
        self.assertTrue(all(e.get("key") != "F9" for e in self.state.recorded_events))

    def test_multi_key_chord_stop_hotkey_stripped(self) -> None:
        self.state.config.hotkey_record_toggle = "Ctrl + F9"
        self.engine.start_recording()
        self.engine._on_win32_key(0x11, False, True, False)   # W down
        self.engine._on_win32_key(0x11, False, False, False)  # W up
        self.engine._on_win32_key(0x1D, False, True, False)   # Ctrl down
        self.assertTrue(self.engine._on_win32_key(0x43, False, True, False))   # F9 down -> stops & strips Ctrl
        self.assertFalse(self.state.is_recording)
        self.assertTrue(self.engine._on_win32_key(0x43, False, False, False))
        self.assertTrue(self.engine._on_win32_key(0x1D, False, False, False))
        self.assertEqual(len(self.state.recorded_events), 2)

    def test_stop_button_mouse_click_ignored_and_stripped(self) -> None:
        self.state.register_stop_button_rect_provider(lambda: (100, 100, 250, 150))
        self.engine.start_recording()
        self.engine._on_win32_mouse("mouse_down", 500, 500, "left", 0, 0, True)
        self.engine._on_win32_mouse("mouse_up", 500, 500, "left", 0, 0, False)
        self.assertEqual(len(self.state.recorded_events), 2)

        # Click inside Stop button rect (150, 120)
        self.engine._on_win32_mouse("mouse_down", 150, 120, "left", 0, 0, True)
        self.engine._on_win32_mouse("mouse_up", 150, 120, "left", 0, 0, False)
        self.engine.toggle_recording(via_mouse=True)
        self.assertFalse(self.state.is_recording)
        self.assertEqual(len(self.state.recorded_events), 2)

    def test_convert_recording_to_smart_combo_1_to_1(self) -> None:
        self.state.smart_combos.clear()
        events = [
            {"kind": "key_down", "delay_ms": 40, "key": "W", "scan_code": 0x11, "is_extended": False},
            {"kind": "mouse_scroll", "delay_ms": 75, "scroll_dir": "down", "scroll_amount": 4, "x": 300, "y": 400},
            {"kind": "key_up", "delay_ms": 120, "key": "W", "scan_code": 0x11, "is_extended": False},
        ]
        self.engine.convert_recording_to_smart_combo("Test 1-to-1", events)
        self.assertEqual(len(self.state.smart_combos), 1)
        steps = self.state.smart_combos[0]["steps"]
        self.assertEqual(len(steps), 3)
        self.assertEqual(steps[0]["step_type"], "key_down")
        self.assertEqual(steps[1]["step_type"], "mouse_scroll")
        self.assertEqual(steps[1]["scroll_amount"], 4)
        self.assertEqual(steps[2]["step_type"], "key_up")



    def test_stop_playback_via_hotkey_and_panic_key_mid_loop(self) -> None:
        self.state.config.hotkey_playback_toggle = "F10"
        self.state.config.hotkey_panic_stop = "Esc"
        self.state.is_playing = True
        # Pressing F10 (0x44) while playing must immediately stop playback
        self.assertTrue(self.engine._on_win32_key(0x44, False, True, False))
        self.assertFalse(self.state.is_playing)
        self.assertTrue(self.engine._on_win32_key(0x44, False, False, False))

        # Pressing Esc (0x01) while playing must immediately stop playback even if a modifier (Shift 0x2A) is held
        self.state.is_playing = True
        self.engine._down_modifiers.add(0x2A)
        self.assertTrue(self.engine._on_win32_key(0x01, False, True, False))
        self.assertFalse(self.state.is_playing)
        self.engine._down_modifiers.clear()


if __name__ == "__main__":
    unittest.main()

    def test_retrigger_policy_and_global_panic_stop(self) -> None:
        from app.domain.models import SmartCombo
        import time

        # Default panic stop must be Ctrl + Shift + Alt
        self.assertEqual(self.state.config.hotkey_panic_stop, "Ctrl + Shift + Alt")

        # SmartCombo preserves retrigger_policy in serialization
        sc = SmartCombo.from_dict({
            "id": "c_retrig",
            "name": "Retrig Test",
            "mode": "once",
            "retrigger_policy": "queue",
            "steps": [{"step_type": "wait", "value": "Wait", "delay_ms": 120}],
        })
        self.assertEqual(sc.retrigger_policy, "queue")
        self.assertEqual(sc.to_dict()["retrigger_policy"], "queue")

        # Queue policy increments _combo_queued_counts when already running
        self.engine._running_combos.add("c_retrig")
        self.engine.execute_smart_combo(sc.to_dict())
        self.assertEqual(self.engine._combo_queued_counts.get("c_retrig"), 1)

        # Global panic stop clears running combos, queued counts, and active playback
        self.engine.stop_all()
        self.assertEqual(len(self.engine._running_combos), 0)
        self.assertEqual(len(self.engine._combo_queued_counts), 0)
        time.sleep(0.01)

    def test_appstate_has_test_lab_logs_and_multilang_render(self) -> None:
        from app.i18n import SUPPORTED_LANGUAGES, set_language
        from app.ui.pages.test_page import TestPage

        self.assertTrue(hasattr(self.state, "test_lab_logs"))
        self.assertIsInstance(self.state.test_lab_logs, list)

        tp = TestPage.__new__(TestPage)
        tp.state = self.state
        tp.engine = self.engine

        sample_entries = [
            {"kind": "method_switch", "requested_method": "linux_uinput_xtest"},
            {"kind": "run_quick", "delay_ms": 1000, "requested_method": "linux_uinput_xtest"},
            {"kind": "key", "lbl": "W", "hw_sc": 0x11, "hid_usage": 0x1A, "requested_method": "sendinput_scancode"},
            {"kind": "mouse", "btn_name": "Left Click", "x": 400, "y": 300, "requested_method": "sendinput_scancode"},
        ]
        for code, _, _, _ in SUPPORTED_LANGUAGES:
            set_language(code)
            for ent in sample_entries:
                html = tp._render_log_entry_html(ent)
                self.assertTrue(len(html) > 10)
        set_language("ar")
