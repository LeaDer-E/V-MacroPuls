# -*- coding: utf-8 -*-
"""Unit tests for 10-language localization completeness and multi-size icon generation."""

import unittest
from pathlib import Path
from tests._qt_stub import install_qt_stubs_if_needed

install_qt_stubs_if_needed()

from app.i18n import SUPPORTED_LANGUAGES, TRANSLATIONS, fmt_num, set_language, t
from tools.make_icon import main as generate_icons


class TestI18nAndIcons(unittest.TestCase):
    def test_ten_supported_languages_have_non_empty_strings(self) -> None:
        self.assertEqual(len(SUPPORTED_LANGUAGES), 10)
        for code, _, _, _ in SUPPORTED_LANGUAGES:
            set_language(code)
            for key in TRANSLATIONS:
                val = t(key)
                self.assertTrue(bool(val and val.strip()), f"Empty translation for {key} in {code}")
        set_language("ar")

    def test_search_placeholder_matches_selected_language(self) -> None:
        set_language("en")
        self.assertEqual(t("search_placeholder"), "Search...")
        set_language("ar")
        self.assertEqual(t("search_placeholder"), "بحث...")

    def test_numeral_formatting_per_script(self) -> None:
        set_language("ar")
        self.assertEqual(fmt_num(123), "١٢٣")
        set_language("fa")
        self.assertEqual(fmt_num(123), "۱۲۳")
        set_language("en")
        self.assertEqual(fmt_num(123), "123")
        set_language("ar")

    def test_icon_generator_creates_ico_and_png(self) -> None:
        generate_icons()
        root = Path(__file__).resolve().parent.parent
        ico = root / "assets" / "icon.ico"
        png = root / "assets" / "icon.png"
        self.assertTrue(ico.exists() and ico.stat().st_size > 1000)
        self.assertTrue(png.exists() and png.stat().st_size > 1000)



    def test_every_t_call_in_codebase_exists_in_translations(self) -> None:
        import ast
        app_root = Path(__file__).resolve().parent.parent / "app"
        missing = []
        for py_file in app_root.rglob("*.py"):
            tree = ast.parse(py_file.read_text(encoding="utf-8"))
            for node in ast.walk(tree):
                if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and node.func.id in ("t", "tp"):
                    if node.args and isinstance(node.args[0], ast.Constant) and isinstance(node.args[0].value, str):
                        k = node.args[0].value
                        if k not in TRANSLATIONS:
                            missing.append((k, py_file.name))
        self.assertEqual(missing, [], f"Missing translation keys in app/: {missing}")


if __name__ == "__main__":
    unittest.main()
