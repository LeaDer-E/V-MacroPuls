# -*- coding: utf-8 -*-
"""Settings dataclass <-> JSON persistence in user AppData with schema migration."""

from __future__ import annotations
import json
import logging
from dataclasses import dataclass, asdict
from typing import Any, Dict
from app.paths import get_config_path

logger = logging.getLogger(__name__)

CURRENT_SCHEMA_VERSION = 8


@dataclass
class AppConfig:
    schema_version: int = CURRENT_SCHEMA_VERSION
    language: str = "ar"                        # 10 supported language codes
    theme_mode: str = "dark"                    # "dark" or "light"
    palette_id: str = "cyber_violet"
    custom_accent: str = "#8B5CF6"
    first_run_completed: bool = False

    # Input Simulation Method ("sendinput_scancode", "sendinput_hybrid", "keybd_mouse_event", "win32_message")
    input_method: str = "sendinput_scancode"
    test_hotkey: str = "F7"

    # Recorder capture settings
    record_mouse_moves: bool = False
    record_mouse_clicks: bool = True
    record_mouse_scroll: bool = True
    record_keyboard: bool = True
    mouse_sample_interval_ms: int = 25

    # Advanced Playback & Repeat settings
    default_playback_speed: float = 1.0         # Repeat Speed multiplier (0.25x .. 10.0x)
    repeat_mode: str = "once"                   # "once", "count", "duration"
    default_repeat_count: int = 5               # Used when repeat_mode == "count" (0 = infinite)
    repeat_duration_value: int = 10             # Used when repeat_mode == "duration"
    repeat_duration_unit: str = "minutes"       # "seconds", "minutes", "hours"

    # Delay between two repeats
    repeat_delay_enabled: bool = False          # Set delay time between two repeat
    repeat_delay_ms: int = 1000                 # Delay time between two repeat (ms)
    repeat_delay_random: bool = False           # Delay a random time (delay 0 to the above time)

    # Post-playback behavior
    do_not_activate_on_stop: bool = True        # Do not activate this window when repeating stopped
    shutdown_on_finish: bool = False            # Shutdown computer when repeating finished

    # Global Hotkeys (supports single key or multi-key chord like "Ctrl + F9")
    hotkey_record_toggle: str = "F9"
    hotkey_playback_toggle: str = "F10"
    hotkey_combo_master_toggle: str = "F11"
    hotkey_panic_stop: str = "Ctrl + Shift + Alt"
    hotkey_autopresser_stop_all: str = "F7"

    # Smart Keys & Windows Startup
    combos_enabled_globally: bool = True
    suppress_trigger_key: bool = True
    run_on_windows_startup: bool = False

    # Audio & Tray
    notifications_enabled: bool = False
    sound_effects_enabled: bool = False
    sound_volume: int = 60
    minimize_to_tray: bool = True
    always_on_top: bool = False

    @classmethod
    def get_defaults(cls) -> Dict[str, Any]:
        return asdict(cls())

    @classmethod
    def migrate_dict(cls, raw: Dict[str, Any]) -> Dict[str, Any]:
        defaults = cls.get_defaults()
        if not isinstance(raw, dict):
            return defaults

        version = int(raw.get("schema_version", 1))
        if version < 7:
            raw.setdefault("input_method", "sendinput_scancode")
            raw.setdefault("test_hotkey", "F7")
            raw.setdefault("repeat_mode", "once")
            raw.setdefault("repeat_duration_value", 10)
            raw.setdefault("repeat_duration_unit", "minutes")
            raw.setdefault("repeat_delay_enabled", False)
            raw.setdefault("repeat_delay_ms", 1000)
            raw.setdefault("repeat_delay_random", False)
            raw.setdefault("do_not_activate_on_stop", True)
            raw.setdefault("shutdown_on_finish", False)
        if version < 8:
            if not raw.get("hotkey_panic_stop") or raw.get("hotkey_panic_stop") == "Esc":
                raw["hotkey_panic_stop"] = "Ctrl + Shift + Alt"
            raw["schema_version"] = CURRENT_SCHEMA_VERSION

        for k, v in defaults.items():
            raw.setdefault(k, v)

        valid_keys = set(defaults.keys())
        return {k: raw[k] for k in valid_keys if k in raw}

    @classmethod
    def load(cls) -> "AppConfig":
        path = get_config_path()
        if not path.exists():
            cfg = cls()
            cfg.save()
            return cfg
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
            migrated = cls.migrate_dict(data)
            return cls(**migrated)
        except Exception as exc:
            logger.warning("Failed to load config (%s), resetting to defaults.", exc)
            cfg = cls()
            cfg.save()
            return cfg

    def save(self) -> None:
        try:
            get_config_path().write_text(
                json.dumps(asdict(self), ensure_ascii=False, indent=2),
                encoding="utf-8",
            )
        except Exception as exc:
            logger.error("Failed to save config: %s", exc)
