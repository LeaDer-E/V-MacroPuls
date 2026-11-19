[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Home & Getting Started](01-Home-and-Getting-Started.md) | [Multi-Tab Auto Presser Studio](03-Auto-Presser-Studio.md) →

# ⚡ Smart Keys & Custom Macros

The **Smart Keys** studio binds any physical hardware key, multi-key chord (up to 3 keys, e.g., `Ctrl + Shift + K`), any of 5 mouse buttons, mouse scroll, media/launch keys, or a manual button to an ordered macro sequence.

## Zero Trigger Leakage & Layout Immunity
Physical keys are intercepted by hardware **ScanCode** (`WH_KEYBOARD_LL`), swallowing the original trigger key before it reaches the active window when suppression is enabled. Macros trigger reliably regardless of whether your OS keyboard layout is English, Arabic, Russian, or any other language.

## Execution Modes
| Mode | Behavior | How to Stop |
| :--- | :--- | :--- |
| **Run Once (`once`)** | Runs all steps once in sequence. | Finishes automatically or press `Ctrl + Shift + Alt`. |
| **Hold to Repeat (`repeat_hold`)** | Repeats continuously while the trigger chord is held. | Release trigger or press the mandatory **Stop Hotkey**. |
| **Toggle Loop (`toggle_loop`)** | Starts an infinite loop on first press; stops on second press. | Press trigger again, **Stop Hotkey**, or Emergency Stop. |
| **Manual Run (No Key)** | Click **Play ▶** on the sidebar card (uses `start_delay_ms`). | Finishes automatically or Emergency Stop. |

> **Safety Guardrail:** Repeating modes (`repeat_hold` and `toggle_loop`) require binding a dedicated **Stop Hotkey** before the macro can be armed.

## 9 Supported Step Types (Unlimited — 10,000+ Steps Supported)
1. **Key Press / Hold (`key_tap` / `key_hold`):** Tap or hold any key for `hold_ms` milliseconds.
2. **Wait (`wait`):** Pause from `1 ms` up to `10 minutes`.
3. **Mouse Click (`mouse_click`):** Click or hold Left, Right, Middle, Back (`X1`), or Forward (`X2`) at coordinates captured live via **`F8`**.
4. **Mouse Scroll (`mouse_scroll`):** Scroll Up, Down, Left, or Right by `1–100` notches.
5. **Type Text (`type_text`):** Inject arbitrary Unicode text.
6. **Python Mini-Script (`python_script`):** Execute loops, conditions, and humanized 3D game actions (see [Chapter 6](06-Python-Mini-Scripting.md)).
7. **Open Folder / App (`open_folder`):** Open a folder or launch an `.exe`/script with its parent folder as working directory.
8. **PowerShell (`powershell`):** Run a hidden background PowerShell command.
9. **CMD (`cmd`):** Run a hidden background Command Prompt command.

## Productivity Tools
- **Target & Excluded Apps:** Run in All Apps, restrict to a specific `.exe`, or exclude multiple processes via the horizontal chip bar.
- **Smooth Animated Reordering:** Drag steps or use Up/Down buttons with smooth `165 ms` animations.
- **Full Undo / Redo (`Ctrl+Z` / `Ctrl+Y`):** Revert or reapply step edits, additions, deletions, and reorders.
- **Step Pause & Instant Hover Notes:** Temporarily skip any step without deleting it, and attach notes with instant `0 ms` hover previews.

---

[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Home & Getting Started](01-Home-and-Getting-Started.md) | [Multi-Tab Auto Presser Studio](03-Auto-Presser-Studio.md) →
