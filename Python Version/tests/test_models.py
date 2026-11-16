# -*- coding: utf-8 -*-
"""Unit tests for MacroEvent, ComboStep, SmartCombo, and 1-to-1 conversion fidelity."""

import unittest
from tests._qt_stub import install_qt_stubs_if_needed

install_qt_stubs_if_needed()

from app.domain.models import ComboStep, MacroEvent, SmartCombo


class TestDomainModels(unittest.TestCase):
    def test_macro_event_to_combo_step_key_events(self) -> None:
        for kind in ("key_down", "key_up", "key_tap", "key_hold"):
            ev = MacroEvent(kind=kind, delay_ms=125, hold_ms=450, key="W", scan_code=0x11, is_extended=False)
            step = ComboStep.from_macro_event(ev)
            self.assertEqual(step.step_type, kind)
            self.assertEqual(step.value, "W")
            self.assertEqual(step.scan_code, 0x11)
            self.assertEqual(step.delay_ms, 125)
            self.assertEqual(step.hold_ms, 450)

    def test_macro_event_to_combo_step_mouse_buttons(self) -> None:
        for btn in ("left", "right", "middle", "x1", "x2"):
            ev = MacroEvent(kind="mouse_click", delay_ms=80, hold_ms=300, x=640, y=480, button=btn)
            step = ComboStep.from_macro_event(ev)
            self.assertEqual(step.step_type, "mouse_click")
            self.assertEqual(step.value, btn)
            self.assertEqual(step.x, 640)
            self.assertEqual(step.y, 480)
            self.assertEqual(step.hold_ms, 300)

    def test_macro_event_to_combo_step_scroll_directions(self) -> None:
        for s_dir in ("down", "up", "left", "right"):
            ev = MacroEvent(kind="mouse_scroll", delay_ms=60, scroll_dir=s_dir, scroll_amount=5)
            step = ComboStep.from_macro_event(ev)
            self.assertEqual(step.step_type, "mouse_scroll")
            self.assertEqual(step.value, s_dir)
            self.assertEqual(step.scroll_amount, 5)
            self.assertEqual(step.delay_ms, 60)

    def test_smart_combo_chord_parsing(self) -> None:
        combo = SmartCombo.from_dict({
            "id": "c_test",
            "name": "Chord Test",
            "trigger_key": "Ctrl + Shift + N",
            "mode": "once",
            "steps": [],
        })
        tuples = combo.chord_tuples()
        self.assertEqual(len(tuples), 3)
        self.assertEqual(tuples[0], (0x1D, False))  # Ctrl
        self.assertEqual(tuples[1], (0x2A, False))  # Shift
        self.assertEqual(tuples[2], (0x31, False))  # N


if __name__ == "__main__":
    unittest.main()
