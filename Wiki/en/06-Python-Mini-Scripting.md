[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Test Lab & 22 Input Methods](05-Test-Lab-and-22-Input-Methods.md) | [Kernel Drivers & USB HID Microcontrollers](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 Python Mini-Scripting & Humanization

Both **Smart Keys** (`python_script` step) and **Test Lab** (Mode 4 sweep) share a built-in script interpreter and a persistent user script library (`python_scripts.json`).

## 6 Built-In Presets & Shared Script Library
1. **In-Game Chat Sweep:** Opens chat (`Enter`), types method number & ID, and sends (`Enter`).
2. **Humanized 3D Game Keys:** `WASD` movement + `1/2/3` weapon switch + `Space` jump with multi-frame holds.
3. **3D Crosshair Click & Relative Aim:** Rotates camera via `move_rel` and clicks with `in_game_3d=True`.
4. **Humanized Typing Rhythm:** Types text character-by-character at natural human speed (`human=True`).
5. **Randomized Skill Combo Loop:** Cycles `1 -> 2 -> 3 -> Tab` with randomized delays.
6. **Standard Per-Method Diagnostic Script.**

Enter a name and click **`+ Save Script`** in either page to save your custom scripts for instant reuse across the entire app.

## Built-In Helper Functions Reference
| Function | Description |
| :--- | :--- |
| `press(key, hold_ms=65, human=False)` | Presses and holds `key` (65 ms default spans 4+ frames at 60 FPS). `human=True` adds finger-hold jitter. |
| `human_press(key, min_hold=58, max_hold=95, min_wait=65, max_wait=130)` | Randomized human key hold followed by randomized human pause. |
| `hold_key(key, duration_ms=300)` | Sustains movement/action keys (`W`, `A`, `S`, `D`, `Shift`). |
| `rand(min_val=60, max_val=110)` | Returns a random integer in `[min_val, max_val]`. |
| `wait(ms)` / `human_wait(min_ms, max_ms)` | Fixed or Gaussian-jittered human pause. |
| `print(text, enter=False, human=False)` | Types text through the active engine; `human=True` simulates natural typing cadence. |
| `type_text(text, enter=False, human=False)` | Same as `print()` with `{method_num}` and `{method_id}` expansion. |
| `click(button="left", x=None, y=None, hold_ms=68, human=False, in_game_3d=False)` | Clicks any of 5 mouse buttons; `in_game_3d=True` clicks at the locked crosshair without `SetCursorPos`. |
| `move_rel(dx, dy, steps=6, duration_ms=60)` | Sends smooth relative `MOUSEEVENTF_MOVE` deltas for 3D camera rotation. |

```python
# Example: Humanized 3D Game Action & Camera Turn
for i in range(3):
    human_press("1", min_hold=65, max_hold=95, min_wait=80, max_wait=130)
    hold_key("W", duration_ms=rand(260, 380))
    move_rel(120, 0, steps=8, duration_ms=80)
    click("left", hold_ms=rand(68, 98), human=True, in_game_3d=True)
```

---

[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Test Lab & 22 Input Methods](05-Test-Lab-and-22-Input-Methods.md) | [Kernel Drivers & USB HID Microcontrollers](07-Hardware-Drivers-and-Microcontrollers.md) →
