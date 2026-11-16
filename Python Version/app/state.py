# -*- coding: utf-8 -*-
"""Central AppState(QObject): application state, stop-button hit-testing & Qt Signals."""

from __future__ import annotations
from typing import Any, Callable, List, Optional, Tuple
from PySide6.QtCore import QObject, QPoint, Signal
from PySide6.QtGui import QCursor
from app.config import AppConfig
from app.i18n import set_language
from app.theme import T
from app.notifier import NOTIFIER
from app.sound import SOUND
from app.workers import run_async


class AppState(QObject):
    """Single source of truth for application state, configuration, and thread-safe Qt Signals."""

    config_changed = Signal()
    settings_changed = Signal(str, object)
    language_changed = Signal(str)
    navigation_requested = Signal(str)

    # Engine state signals
    recording_state_changed = Signal(bool)
    playback_state_changed = Signal(bool)
    combos_master_changed = Signal(bool)
    events_updated = Signal()
    recording_count_changed = Signal(int)
    macros_library_updated = Signal()
    combos_list_updated = Signal()
    python_scripts_updated = Signal()
    activate_window_requested = Signal()

    # Hook -> GUI Key Capture signals
    hook_key_captured = Signal(int, bool, str)
    hook_key_event = Signal(int, bool, bool, str)
    cursor_pick_hotkey_pressed = Signal(int, int)

    # Thread-safe sound & toast signals
    sound_requested = Signal(str)
    toast_requested = Signal(str, str)

    def __init__(self, config: AppConfig) -> None:
        super().__init__()
        self.config: AppConfig = config
        self.current_page_id: str = "combos"

        # Runtime flags
        self.is_recording: bool = False
        self.is_playing: bool = False
        self.combos_active: bool = config.combos_enabled_globally
        self.is_capturing_key: bool = False

        # Providers returning global screen rect `(left, top, right, bottom)` of active Stop Recording buttons
        self._stop_btn_rect_providers: List[Callable[[], Optional[Tuple[int, int, int, int]]]] = []

        # Active data in memory
        self.current_macro_name: str = "Macro 1"
        self.recorded_events: List[dict[str, Any]] = []
        self.saved_macros: List[dict[str, Any]] = []
        self.smart_combos: List[dict[str, Any]] = []
        self.test_lab_logs: List[dict[str, Any]] = []

        # Sync initial settings
        set_language(self.config.language)
        T.configure(self.config.palette_id, self.config.theme_mode, self.config.custom_accent)
        NOTIFIER.enabled = self.config.notifications_enabled
        SOUND.enabled = self.config.sound_effects_enabled
        SOUND.volume = self.config.sound_volume

        self.sound_requested.connect(SOUND.play)
        self.toast_requested.connect(NOTIFIER.toast)

    def register_stop_button_rect_provider(
        self, provider: Callable[[], Optional[Tuple[int, int, int, int]]]
    ) -> None:
        if provider not in self._stop_btn_rect_providers:
            self._stop_btn_rect_providers.append(provider)

    def is_point_on_stop_button(self, raw_x: int, raw_y: int) -> bool:
        """
        Return True if either the raw Win32 hook coordinates `(raw_x, raw_y)` OR Qt's logical
        `QCursor.pos()` falls inside any visible Stop Recording button (main Stop button or floating Stop pill).
        Checking both raw and logical coordinates makes this 100% immune to Windows High-DPI scaling (125%/150%)!
        """
        pts = [(raw_x, raw_y)]
        try:
            qpt: QPoint = QCursor.pos()
            pts.append((int(qpt.x()), int(qpt.y())))
        except Exception:
            pass

        for provider in list(self._stop_btn_rect_providers):
            try:
                rect = provider()
                if rect is not None:
                    l_x, t_y, r_x, b_y = rect
                    for px, py in pts:
                        if (l_x - 8) <= px <= (r_x + 8) and (t_y - 8) <= py <= (b_y + 8):
                            return True
            except Exception:
                pass
        return False

    def save_config_async(self) -> None:
        NOTIFIER.enabled = self.config.notifications_enabled
        SOUND.enabled = self.config.sound_effects_enabled
        SOUND.volume = self.config.sound_volume
        run_async(self.config.save)
        self.config_changed.emit()

    def update_setting(self, key: str, value: Any) -> None:
        if hasattr(self.config, key):
            setattr(self.config, key, value)
            self.save_config_async()
            self.settings_changed.emit(key, value)

    def set_lang(self, lang: str) -> None:
        self.config.language = lang
        set_language(lang)
        self.save_config_async()
        self.language_changed.emit(lang)

    def set_theme(self, palette_id: str | None = None, mode: str | None = None, custom_accent: str | None = None) -> None:
        if palette_id is not None:
            self.config.palette_id = palette_id
        if mode is not None:
            self.config.theme_mode = mode
        if custom_accent is not None:
            self.config.custom_accent = custom_accent
        T.configure(self.config.palette_id, self.config.theme_mode, self.config.custom_accent)
        self.save_config_async()
