# -*- coding: utf-8 -*-
"""Unit tests for hardware scan-code tables, chord parsing, and media/launch key mapping."""

import unittest
from tests._qt_stub import install_qt_stubs_if_needed

install_qt_stubs_if_needed()

from app.domain.win32_input import (
    is_special_media_or_launch_scancode,
    label_for_scancode,
    parse_chord_string,
    scancode_for_label,
)


class TestWin32InputTables(unittest.TestCase):
    def test_scancode_roundtrip_standard_keys(self) -> None:
        for lbl in ("Esc", "W", "A", "S", "D", "Space", "F8", "F9", "F10", "F11", "Ctrl", "Shift", "Alt"):
            sc, ext = scancode_for_label(lbl)
            self.assertGreater(sc, 0)
            resolved = label_for_scancode(sc, ext)
            self.assertEqual(resolved.lower(), lbl.lower())

    def test_media_and_calculator_keys(self) -> None:
        calc_sc, calc_ext = scancode_for_label("Calculator")
        self.assertTrue(is_special_media_or_launch_scancode(calc_sc, calc_ext))
        self.assertEqual(label_for_scancode(0x1B7, True), "Calculator")
        self.assertEqual(label_for_scancode(0x21, True), "Calculator")

    def test_parse_chord_string_deduplicates_and_parses(self) -> None:
        chord = parse_chord_string("Ctrl + Shift + N + Ctrl")
        self.assertEqual(len(chord), 3)
        self.assertEqual(chord, [(0x1D, False), (0x2A, False), (0x31, False)])



    def test_kernel_driver_diagnostics_and_report_formatting(self) -> None:
        from app.domain.win32_input import diagnose_kernel_drivers, format_kernel_diagnostic_report
        diag = diagnose_kernel_drivers(run_live_probe=False)
        self.assertIn("state_code", diag)
        self.assertIn("drivers_dir", diag)
        h_ar, d_ar, k_ar = format_kernel_diagnostic_report(diag, is_arabic=True)
        h_en, d_en, k_en = format_kernel_diagnostic_report(diag, is_arabic=False)
        self.assertTrue(bool(h_ar and d_ar))
        self.assertTrue(bool(h_en and d_en))
        self.assertIn(k_ar, ("ok", "warn", "info"))


if __name__ == "__main__":
    unittest.main()

    def test_resolve_effective_input_engine(self) -> None:
        from app.domain.win32_input import resolve_effective_input_engine

        res = resolve_effective_input_engine("linux_uinput_xtest", is_arabic=True)
        self.assertIn("effective_method", res)
        self.assertIn("is_fallback", res)
        self.assertIn("reason", res)
        self.assertTrue(len(res["reason"]) > 0)
