# -*- coding: utf-8 -*-
"""Lazy QMediaPlayer + QAudioOutput sound effect synthesizer with safe try/except fallback."""

from __future__ import annotations
import io
import math
import struct
import wave
import logging
from typing import Dict, Optional
from PySide6.QtCore import QBuffer, QByteArray, QIODevice, QObject

logger = logging.getLogger(__name__)


def _generate_chime_wav(freqs: list[float], duration_ms: int = 110, volume: float = 0.35) -> bytes:
    """Synthesize a clean soft sine-wave chime into an in-memory WAV buffer."""
    sample_rate = 22050
    total_samples = int(sample_rate * (duration_ms / 1000.0))
    buf = io.BytesIO()
    with wave.open(buf, "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(sample_rate)
        frames = bytearray()
        for i in range(total_samples):
            t = i / sample_rate
            env = math.sin(math.pi * (i / max(1, total_samples))) ** 0.7
            idx = min(len(freqs) - 1, int((i / max(1, total_samples)) * len(freqs)))
            f = freqs[idx]
            val = math.sin(2.0 * math.pi * f * t) * volume * env
            sample_int = max(-32767, min(32767, int(val * 32767)))
            frames.extend(struct.pack("<h", sample_int))
        wf.writeframes(bytes(frames))
    return buf.getvalue()


class SoundEngine(QObject):
    """Lazy-initialized audio feedback engine safe on headless or no-audio machines."""

    def __init__(self) -> None:
        super().__init__()
        self._initialized: bool = False
        self._failed: bool = False
        self._player: Optional[object] = None
        self._audio_out: Optional[object] = None
        self._buffers: Dict[str, QByteArray] = {}
        self._active_qbuf: Optional[QBuffer] = None
        self.enabled: bool = True
        self.volume: int = 75

    def _ensure_init(self) -> bool:
        if self._failed:
            return False
        if self._initialized:
            return True
        try:
            from PySide6.QtMultimedia import QAudioOutput, QMediaPlayer

            self._audio_out = QAudioOutput()
            self._player = QMediaPlayer()
            self._player.setAudioOutput(self._audio_out)

            self._buffers = {
                "start": QByteArray(_generate_chime_wav([523.25, 659.25, 783.99], 140)),
                "stop": QByteArray(_generate_chime_wav([783.99, 523.25], 130)),
                "combo": QByteArray(_generate_chime_wav([587.33, 880.0], 95)),
                "click": QByteArray(_generate_chime_wav([660.0], 55)),
                "error": QByteArray(_generate_chime_wav([311.13, 261.63], 170)),
            }
            self._initialized = True
            return True
        except Exception as exc:
            logger.debug("Audio subsystem unavailable (headless/no audio): %s", exc)
            self._failed = True
            return False

    def play(self, sound_name: str = "click") -> None:
        if not self.enabled or self.volume <= 0:
            return
        if not self._ensure_init():
            return
        try:
            data = self._buffers.get(sound_name, self._buffers.get("click"))
            if not data or self._player is None or self._audio_out is None:
                return
            self._audio_out.setVolume(max(0.0, min(1.0, self.volume / 100.0)))
            self._player.stop()
            self._active_qbuf = QBuffer()
            self._active_qbuf.setData(data)
            self._active_qbuf.open(QIODevice.OpenModeFlag.ReadOnly)
            self._player.setSourceDevice(self._active_qbuf)
            self._player.play()
        except Exception as exc:
            logger.debug("Sound playback skipped: %s", exc)


SOUND = SoundEngine()
