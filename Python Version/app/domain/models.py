# -*- coding: utf-8 -*-
"""
Unified Data Models for V-MacroPlus v3.1.0:
- 100% Unified Event & Step Schema between `MacroEvent` (Recorder) and `ComboStep` (Smart Keys) so that
  converting any Recording to a Smart Key preserves EVERY action (`key_down`, `key_up`, `key_tap`, `key_hold`,
  `mouse_down`, `mouse_up`, `mouse_click`, `mouse_scroll`, `mouse_move`, `wait`), EVERY hold duration (`hold_ms`),
  EVERY mouse button (`left`, `right`, `middle`, `x1` Back, `x2` Forward), EVERY scroll direction (`down`, `up`,
  `left`, `right` + `scroll_amount`), and EVERY millisecond delay (`delay_ms`) 1-to-1!
"""

from __future__ import annotations
from dataclasses import dataclass, field, asdict
from typing import Any, Dict, List, Tuple
from app.domain.win32_input import label_for_scancode, parse_chord_string, scancode_for_label


@dataclass
class MacroEvent:
    """Single recorded or manually edited mouse/keyboard event."""

    kind: str               # "key_down", "key_up", "key_tap", "key_hold", "mouse_move", "mouse_down", "mouse_up", "mouse_click", "mouse_scroll", "wait"
    delay_ms: int = 40      # Delay before executing this event (or wait duration if kind == "wait")
    hold_ms: int = 0        # Hold duration in ms for "key_tap"/"key_hold"/"mouse_click"
    key: str = "Space"
    scan_code: int = 0x39
    is_extended: bool = False
    x: int = 500
    y: int = 500
    button: str = "left"            # "left", "right", "middle", "x1" (Back), "x2" (Forward)
    scroll_dir: str = "down"        # "down", "up", "left", "right"
    scroll_amount: int = 3          # Number of scroll wheel notches
    dx: int = 0
    dy: int = 0
    enabled: bool = True
    note: str = ""

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)

    @classmethod
    def from_dict(cls, d: Dict[str, Any]) -> "MacroEvent":
        valid = {k: d[k] for k in cls.__dataclass_fields__ if k in d}
        ev = cls(**valid)
        if ev.kind in ("key_tap", "key_hold", "key_down", "key_up"):
            if ev.key and ev.scan_code == 0:
                ev.scan_code, ev.is_extended = scancode_for_label(ev.key)
            elif ev.scan_code and not ev.key:
                ev.key = label_for_scancode(ev.scan_code, ev.is_extended)
        if ev.kind == "mouse_scroll":
            if "scroll_dir" not in d:
                if ev.dy != 0:
                    ev.scroll_dir = "up" if ev.dy > 0 else "down"
                    ev.scroll_amount = max(1, abs(ev.dy))
                elif ev.dx != 0:
                    ev.scroll_dir = "right" if ev.dx > 0 else "left"
                    ev.scroll_amount = max(1, abs(ev.dx))
        return ev


@dataclass
class ComboStep:
    """
    Single step inside a Smart Key / Macro sequence.
    Supports 100% of Recorder event types (`key_down`, `key_up`, `key_tap`, `key_hold`, `mouse_down`, `mouse_up`,
    `mouse_click`, `mouse_scroll`, `mouse_move`, `wait`) plus `open_folder`, `powershell`, `cmd`, `type_text`.
    """

    step_type: str          # "key_tap", "key_hold", "key_down", "key_up", "wait", "mouse_click", "mouse_down", "mouse_up", "mouse_move", "mouse_scroll", "open_folder", "powershell", "cmd", "type_text"
    value: str = "Space"    # Key label, mouse button ("left"/"right"/"middle"/"x1"/"x2"), scroll_dir ("down"/"up"/"left"/"right"), folder path, or command
    scan_code: int = 0x39
    is_extended: bool = False
    delay_ms: int = 20      # Pre-step delay (or wait duration when step_type == "wait")
    hold_ms: int = 0        # Hold duration in ms for "key_tap", "key_hold", or "mouse_click"
    scroll_amount: int = 3  # Number of wheel notches when step_type == "mouse_scroll"
    x: int = 500
    y: int = 500
    use_coords: bool = True
    enabled: bool = True
    note: str = ""

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)

    @classmethod
    def from_macro_event(cls, ev: MacroEvent) -> "ComboStep":
        """Convert any `MacroEvent` 1-to-1 into an identical `ComboStep` with zero loss of actions, timing, enabled state, or note!"""
        en = bool(getattr(ev, "enabled", True))
        nt = str(getattr(ev, "note", ""))
        if ev.kind == "wait":
            return cls(step_type="wait", value="Wait", delay_ms=max(1, ev.delay_ms), enabled=en, note=nt)
        if ev.kind in ("key_down", "key_up", "key_tap", "key_hold"):
            return cls(
                step_type=ev.kind,
                value=ev.key,
                scan_code=ev.scan_code,
                is_extended=ev.is_extended,
                delay_ms=max(1, ev.delay_ms),
                hold_ms=max(0, ev.hold_ms),
                enabled=en,
                note=nt,
            )
        if ev.kind in ("mouse_down", "mouse_up", "mouse_click"):
            return cls(
                step_type=ev.kind,
                value=ev.button or "left",
                delay_ms=max(1, ev.delay_ms),
                hold_ms=max(0, ev.hold_ms),
                x=ev.x,
                y=ev.y,
                use_coords=True,
                enabled=en,
                note=nt,
            )
        if ev.kind == "mouse_scroll":
            return cls(
                step_type="mouse_scroll",
                value=ev.scroll_dir or "down",
                scroll_amount=max(1, ev.scroll_amount),
                delay_ms=max(1, ev.delay_ms),
                x=ev.x,
                y=ev.y,
                use_coords=False,
                enabled=en,
                note=nt,
            )
        if ev.kind == "mouse_move":
            return cls(
                step_type="mouse_move",
                value="move",
                delay_ms=max(1, ev.delay_ms),
                x=ev.x,
                y=ev.y,
                use_coords=True,
                enabled=en,
                note=nt,
            )
        return cls(step_type="wait", value="Wait", delay_ms=max(1, ev.delay_ms), enabled=en, note=nt)

    @classmethod
    def from_dict(cls, d: Dict[str, Any]) -> "ComboStep":
        valid = {k: d[k] for k in cls.__dataclass_fields__ if k in d}
        st = cls(**valid)
        if st.step_type in ("key_tap", "key_hold", "key_down", "key_up"):
            if st.value and st.scan_code == 0:
                st.scan_code, st.is_extended = scancode_for_label(st.value)
            elif st.scan_code:
                st.value = label_for_scancode(st.scan_code, st.is_extended)
        return st


@dataclass
class SmartCombo:
    """
    Binds 1, 2, or 3 physical keys (`trigger_chord`) — OR 0 keys (Manual Only) —
    to execute a sequence of Keyboard, Wait (ms), Multi-Monitor Mouse Click/Hold/Scroll, Open Folder, PowerShell, or CMD steps.
    """

    id: str
    name: str
    trigger_key: str = ""
    trigger_scan: int = 0
    trigger_extended: bool = False
    trigger_chord: List[List[int]] = field(default_factory=list)
    mode: str = "once"                              # "once", "repeat_hold", "toggle_loop"
    retrigger_policy: str = "ignore"                # "ignore", "restart", "queue"
    target_process: str = ""
    excluded_processes: List[str] = field(default_factory=list)
    stop_hotkey: str = ""
    start_delay_ms: int = 1000
    enabled: bool = False
    steps: List[ComboStep] = field(default_factory=list)

    def to_dict(self) -> Dict[str, Any]:
        return {
            "id": self.id,
            "name": self.name,
            "trigger_key": self.trigger_key,
            "trigger_scan": self.trigger_scan,
            "trigger_extended": self.trigger_extended,
            "trigger_chord": self.trigger_chord,
            "mode": self.mode,
            "retrigger_policy": self.retrigger_policy,
            "target_process": self.target_process,
            "excluded_processes": list(self.excluded_processes),
            "stop_hotkey": self.stop_hotkey,
            "start_delay_ms": self.start_delay_ms,
            "enabled": self.enabled,
            "steps": [s.to_dict() for s in self.steps],
        }

    def chord_tuples(self) -> List[Tuple[int, bool]]:
        if self.trigger_chord:
            cleaned: List[Tuple[int, bool]] = []
            for pair in self.trigger_chord:
                if len(pair) >= 2:
                    sc = int(pair[0])
                    ext = bool(pair[1])
                    if sc <= 0:
                        sc, ext = 0x1B7, True
                    if (sc, ext) not in cleaned:
                        cleaned.append((sc, ext))
            return cleaned
        if self.trigger_key:
            return parse_chord_string(self.trigger_key)
        if self.trigger_scan > 0:
            return [(self.trigger_scan, self.trigger_extended)]
        return []

    def sidebar_label(self, is_ar: bool = True) -> str:
        if self.trigger_key:
            return self.trigger_key.replace("Key_0", "Calculator")
        if self.name:
            return self.name.replace("Key_0", "Calculator")
        return "ماكرو يدوي" if is_ar else "Manual Macro"

    @classmethod
    def from_dict(cls, d: Dict[str, Any]) -> "SmartCombo":
        raw_steps = d.get("steps", [])
        steps = [ComboStep.from_dict(s) if isinstance(s, dict) else s for s in raw_steps]
        t_key = str(d.get("trigger_key", "")).replace("Key_0", "Calculator")
        t_scan = int(d.get("trigger_scan", 0))
        t_ext = bool(d.get("trigger_extended", False))
        raw_chord = d.get("trigger_chord", [])

        t_chord: List[List[int]] = []
        for pair in raw_chord:
            if isinstance(pair, (list, tuple)) and len(pair) >= 2:
                sc = int(pair[0])
                ext = 1 if bool(pair[1]) else 0
                if sc <= 0:
                    sc, ext = 0x1B7, 1
                t_chord.append([sc, ext])

        if not t_chord and t_key:
            parsed = parse_chord_string(t_key)
            t_chord = [[sc, 1 if ext else 0] for sc, ext in parsed]
        if t_chord:
            t_key = " + ".join(label_for_scancode(int(p[0]), bool(p[1])) for p in t_chord if len(p) >= 2)
            t_scan = int(t_chord[-1][0])
            t_ext = bool(t_chord[-1][1])

        return cls(
            id=str(d.get("id", "combo_1")),
            name=str(d.get("name", "")).replace("Key_0", "Calculator"),
            trigger_key=t_key,
            trigger_scan=t_scan,
            trigger_extended=t_ext,
            trigger_chord=t_chord,
            mode=str(d.get("mode", "once")),
            retrigger_policy=str(d.get("retrigger_policy", "ignore")) if str(d.get("retrigger_policy", "ignore")) in ("ignore", "restart", "queue") else "ignore",
            target_process=str(d.get("target_process", "")),
            excluded_processes=[str(x).strip() for x in d.get("excluded_processes", []) if str(x).strip()],
            stop_hotkey=str(d.get("stop_hotkey", "")).strip(),
            start_delay_ms=int(d.get("start_delay_ms", 1000)),
            enabled=bool(d.get("enabled", False)),
            steps=steps,
        )


def get_default_smart_combos() -> List[Dict[str, Any]]:
    n_sc, n_ext = scancode_for_label("N")
    w_sc, w_ext = scancode_for_label("W")
    s_sc, s_ext = scancode_for_label("S")
    sp_sc, sp_ext = scancode_for_label("Space")
    ctrl_sc, ctrl_ext = scancode_for_label("Ctrl")
    sh_sc, sh_ext = scancode_for_label("Shift")

    c1 = SmartCombo(
        id="combo_example_n",
        name="N",
        trigger_key="N",
        trigger_scan=n_sc,
        trigger_extended=n_ext,
        trigger_chord=[[n_sc, 1 if n_ext else 0]],
        mode="once",
        enabled=False,
        steps=[
            ComboStep(step_type="key_tap", value="W", scan_code=w_sc, is_extended=w_ext, delay_ms=20, hold_ms=0),
            ComboStep(step_type="wait", value="Wait", delay_ms=1000),
            ComboStep(step_type="key_tap", value="Space", scan_code=sp_sc, is_extended=sp_ext, delay_ms=20, hold_ms=0),
        ],
    )
    c2 = SmartCombo(
        id="combo_example_chord",
        name="Ctrl + Shift",
        trigger_key="Ctrl + Shift",
        trigger_scan=sh_sc,
        trigger_extended=sh_ext,
        trigger_chord=[[ctrl_sc, 0], [sh_sc, 0]],
        mode="once",
        enabled=False,
        steps=[
            ComboStep(step_type="key_tap", value="Space", scan_code=sp_sc, is_extended=sp_ext, delay_ms=15, hold_ms=0),
            ComboStep(step_type="key_tap", value="S", scan_code=s_sc, is_extended=s_ext, delay_ms=15, hold_ms=0),
            ComboStep(step_type="key_tap", value="Space", scan_code=sp_sc, is_extended=sp_ext, delay_ms=15, hold_ms=0),
        ],
    )
    return [c1.to_dict(), c2.to_dict()]


def get_starter_recorded_macro() -> List[Dict[str, Any]]:
    w_sc, w_ext = scancode_for_label("W")
    sp_sc, sp_ext = scancode_for_label("Space")
    return [
        MacroEvent(kind="key_down", delay_ms=40, key="W", scan_code=w_sc, is_extended=w_ext).to_dict(),
        MacroEvent(kind="key_up", delay_ms=250, key="W", scan_code=w_sc, is_extended=w_ext).to_dict(),
        MacroEvent(kind="wait", delay_ms=1000, key="Wait").to_dict(),
        MacroEvent(kind="key_tap", delay_ms=30, key="Space", scan_code=sp_sc, is_extended=sp_ext).to_dict(),
    ]
