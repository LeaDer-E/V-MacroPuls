[🌐 Alle Sprachen](../README.md) | [🏠 Startseite](01-Home-and-Getting-Started.md) | ← [Testlabor & 22 Eingabemethoden](05-Test-Lab-and-22-Input-Methods.md) | [Kernel-Treiber & USB-HID-Mikrocontroller](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 Python Mini-Skripte & Humanisierung

**Smart Keys** und das **Testlabor (Modus 4)** teilen sich einen eingebauten Interpreter, 6 fertige Vorlagen und eine gemeinsame Skriptbibliothek (`python_scripts.json`).

## Hilfsfunktionen
| Funktion | Beschreibung |
| :--- | :--- |
| `press(key, hold_ms=65, human=False)` | Taste drücken/halten (`human=True` für menschliche Varianz). |
| `human_press(key, min_hold, max_hold, min_wait, max_wait)` | Zufällige Halte- und Wartezeit. |
| `hold_key(key, duration_ms=300)` | Hält Bewegungstasten (`W`, `A`, `S`, `D`) in 3D-Spielen. |
| `rand(min_val, max_val)` | Zufällige Ganzzahl im Bereich. |
| `wait(ms)` / `human_wait(min_ms, max_ms)` | Feste oder natürliche Pause. |
| `print(text, enter=False, human=False)` | Tippt Text über die aktive Methode. |
| `click(button, x, y, hold_ms, human, in_game_3d)` | 5-Tasten-Klick; `in_game_3d=True` klickt im Fadenkreuz ohne `SetCursorPos`. |
| `move_rel(dx, dy, steps, duration_ms)` | Sanfte relative 3D-Kamerabewegung (`MOUSEEVENTF_MOVE`). |

```python
for i in range(3):
    human_press("1", 65, 95, 80, 130)
    hold_key("W", rand(260, 380))
    move_rel(120, 0, steps=8, duration_ms=80)
    click("left", hold_ms=rand(68, 98), human=True, in_game_3d=True)
```

---

[🌐 Alle Sprachen](../README.md) | [🏠 Startseite](01-Home-and-Getting-Started.md) | ← [Testlabor & 22 Eingabemethoden](05-Test-Lab-and-22-Input-Methods.md) | [Kernel-Treiber & USB-HID-Mikrocontroller](07-Hardware-Drivers-and-Microcontrollers.md) →
