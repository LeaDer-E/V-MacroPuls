# -*- coding: utf-8 -*-
"""Unit tests for AppConfig schema defaults, migration, and JSON serialization."""

import unittest
from tests._qt_stub import install_qt_stubs_if_needed

install_qt_stubs_if_needed()

from app.config import AppConfig, CURRENT_SCHEMA_VERSION


class TestAppConfig(unittest.TestCase):
    def test_default_values(self) -> None:
        cfg = AppConfig()
        self.assertEqual(cfg.schema_version, CURRENT_SCHEMA_VERSION)
        self.assertEqual(cfg.language, "ar")
        self.assertEqual(cfg.theme_mode, "dark")
        self.assertEqual(cfg.palette_id, "cyber_violet")
        self.assertEqual(cfg.input_method, "sendinput_scancode")
        self.assertEqual(cfg.hotkey_record_toggle, "F9")
        self.assertEqual(cfg.hotkey_playback_toggle, "F10")
        self.assertEqual(cfg.hotkey_combo_master_toggle, "F11")
        self.assertEqual(cfg.hotkey_panic_stop, "Ctrl + Shift + Alt")
        self.assertTrue(cfg.suppress_trigger_key)
        self.assertFalse(cfg.shutdown_on_finish)

    def test_migrate_legacy_schema(self) -> None:
        raw_v1 = {"schema_version": 1, "language": "en", "unknown_obsolete_key": 123}
        migrated = AppConfig.migrate_dict(raw_v1)
        self.assertEqual(migrated["schema_version"], CURRENT_SCHEMA_VERSION)
        self.assertEqual(migrated["language"], "en")
        self.assertEqual(migrated["input_method"], "sendinput_scancode")
        self.assertNotIn("unknown_obsolete_key", migrated)

    def test_migrate_non_dict_returns_defaults(self) -> None:
        migrated = AppConfig.migrate_dict(None)  # type: ignore[arg-type]
        self.assertEqual(migrated["schema_version"], CURRENT_SCHEMA_VERSION)
        self.assertEqual(migrated["palette_id"], "cyber_violet")


if __name__ == "__main__":
    unittest.main()
