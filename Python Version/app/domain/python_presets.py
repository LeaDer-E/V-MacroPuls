# -*- coding: utf-8 -*-
"""
Shared Python Script Presets & User-Saved Scripts Library (`app/domain/python_presets.py`):
- Provides 6 built-in ready-to-use Python automation scripts (including In-Game Chat Sweep,
  Humanized 3D Game Movement & Keys `WASD/1/2/3/Space`, In-Match 3D Crosshair Mouse & Camera `move_rel`,
  Humanized Natural Typing, Randomized Skill Combo Loop, and Standard 20-Method Diagnostic Sweep).
- Persists user-created Python scripts in `AppData/python_scripts.json` so any script saved in
  `Test Lab` (`TestPage`) or `Smart Keys` (`CombosPage`) is immediately available in both places!
"""

from __future__ import annotations
import json
import logging
import time
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple
from app.paths import get_appdata_dir

logger = logging.getLogger(__name__)


BUILTIN_PYTHON_PRESETS: List[Tuple[str, str, str]] = [
    (
        "preset:chat_20_sweep",
        "py_preset_chat_sweep",
        """\
# 1. In-Game Chat Sweep (Opens Chat with Enter -> Types Method -> Sends with Enter)
# Solves odd/even chat toggling in games that require Enter before & after typing:
press("Enter", hold_ms=rand(60, 88), human=True)
human_wait(75, 115)
print(f"Method {method_num}: [{method_id}] - OK", human=True)
human_wait(65, 100)
press("Enter", hold_ms=rand(60, 88), human=True)
human_wait(150, 210)
""",
    ),
    (
        "preset:human_3d_keys",
        "py_preset_human_3d_keys",
        """\
# 2. Humanized 3D Game Keys (WASD Movement + 1/2/3 Weapon Switch + Space Jump)
# Uses multi-frame hardware hold durations (65..95ms taps, 250..400ms movement holds)
# and micro-jitter so 3D DirectInput / RawInput game engines register every action:
human_press("1", min_hold=65, max_hold=95, min_wait=80, max_wait=130)
hold_key("W", duration_ms=rand(260, 380))
human_wait(70, 120)
human_press("Space", min_hold=70, max_hold=105, min_wait=85, max_wait=140)
human_press("2", min_hold=65, max_hold=95, min_wait=80, max_wait=130)
hold_key("D", duration_ms=rand(200, 300))
""",
    ),
    (
        "preset:3d_mouse_combat",
        "py_preset_3d_mouse",
        """\
# 3. In-Match 3D Crosshair Click & Relative Camera Aim (in_game_3d=True + move_rel)
# Clicks at locked 3D crosshair without calling SetCursorPos, and moves 3D camera via relative deltas:
move_rel(120, 0, steps=8, duration_ms=80)
human_wait(55, 90)
click("left", hold_ms=rand(68, 98), human=True, in_game_3d=True)
human_wait(85, 135)
click("right", hold_ms=rand(65, 92), human=True, in_game_3d=True)
""",
    ),
    (
        "preset:human_typing",
        "py_preset_human_typing",
        """\
# 4. Humanized Natural Keyboard Typing Rhythm (human=True)
# Types each character with natural human finger hold (45..80ms) and variable inter-key rhythm:
print(f"Hello from V-MacroPlus [{method_id}]", enter=True, human=True)
human_wait(120, 180)
""",
    ),
    (
        "preset:skill_combo_loop",
        "py_preset_skill_loop",
        """\
# 5. Randomized Human Skill Combo Loop (1 -> 2 -> 3 -> Tab)
for key_name in ["1", "2", "3"]:
    press(key_name, hold_ms=rand(62, 95), human=True)
    human_wait(75, 135)
press("Tab", hold_ms=rand(70, 105), human=True)
human_wait(110, 160)
""",
    ),
    (
        "preset:default_sweep",
        "py_preset_default_sweep",
        """\
# 6. Standard Per-Method Diagnostic Script (Executed for each Method 1..20)
# Available variables: method_num, method_num_02, method_id, method_name, category, target_app, target_hwnd, mode
# Humanized helpers: rand(min, max), human_wait(min, max), human_press(key), hold_key(key, ms), move_rel(dx, dy)
print(f"Method {method_num}: [{method_id}] - OK")
press("Enter", hold_ms=65, human=True)
wait(120)
""",
    ),
]


def get_python_scripts_file_path() -> Path:
    return get_appdata_dir() / "python_scripts.json"


def load_saved_python_scripts() -> List[Dict[str, str]]:
    p = get_python_scripts_file_path()
    if not p.exists():
        return []
    try:
        raw = json.loads(p.read_text(encoding="utf-8"))
        if isinstance(raw, list):
            out: List[Dict[str, str]] = []
            for item in raw:
                if isinstance(item, dict) and item.get("name") and item.get("code") is not None:
                    out.append(
                        {
                            "id": str(item.get("id") or f"user:{int(time.time() * 1000)}"),
                            "name": str(item["name"]).strip(),
                            "code": str(item["code"]),
                        }
                    )
            return out
    except Exception as exc:
        logger.warning("Failed to load python_scripts.json: %s", exc)
    return []


def save_saved_python_scripts(scripts: List[Dict[str, str]]) -> bool:
    p = get_python_scripts_file_path()
    try:
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(json.dumps(scripts, ensure_ascii=False, indent=2), encoding="utf-8")
        return True
    except Exception as exc:
        logger.error("Failed to save python_scripts.json: %s", exc)
        return False


def get_python_script_dropdown_items() -> List[Tuple[str, str]]:
    """
    Return `[(key, localized_label), ...]` combining all user-saved scripts first (marked with `★`)
    followed by the 6 built-in ready-to-use presets.
    """
    from app.i18n import t as _t

    items: List[Tuple[str, str]] = []
    for usr in load_saved_python_scripts():
        uid = usr["id"]
        uname = usr["name"]
        items.append((uid, f"★ {_t('py_saved_prefix')} {uname}"))
    for pid, tr_k, _ in BUILTIN_PYTHON_PRESETS:
        items.append((pid, _t(tr_k)))
    return items


def get_python_script_code_by_key(script_key: str) -> Optional[str]:
    if not script_key:
        return None
    for pid, _, code in BUILTIN_PYTHON_PRESETS:
        if pid == script_key:
            return code
    for usr in load_saved_python_scripts():
        if usr["id"] == script_key:
            return usr["code"]
    return None


def get_python_script_name_by_key(script_key: str) -> str:
    for usr in load_saved_python_scripts():
        if usr["id"] == script_key:
            return usr["name"]
    return ""


def upsert_user_python_script(name: str, code: str, existing_key: str = "") -> Tuple[str, str]:
    """
    Save a new user Python script or update an existing user script if `name` or `existing_key` matches.
    Returns `(saved_id, saved_name)`.
    """
    clean_name = (name or "").strip()
    scripts = load_saved_python_scripts()
    if not clean_name:
        clean_name = f"Script {len(scripts) + 1}"

    # Check if updating by existing user key or matching name
    for item in scripts:
        if (existing_key.startswith("user:") and item["id"] == existing_key) or item["name"].lower() == clean_name.lower():
            item["name"] = clean_name
            item["code"] = code
            save_saved_python_scripts(scripts)
            return item["id"], clean_name

    new_id = f"user:{int(time.time() * 1000)}_{len(scripts) + 1}"
    scripts.insert(0, {"id": new_id, "name": clean_name, "code": code})
    save_saved_python_scripts(scripts)
    return new_id, clean_name


def delete_user_python_script(script_key: str) -> bool:
    if not script_key or not script_key.startswith("user:"):
        return False
    scripts = load_saved_python_scripts()
    filtered = [s for s in scripts if s["id"] != script_key]
    if len(filtered) == len(scripts):
        return False
    save_saved_python_scripts(filtered)
    return True
