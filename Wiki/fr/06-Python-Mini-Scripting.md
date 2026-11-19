[🌐 Toutes les Langues](../README.md) | [🏠 Accueil](01-Home-and-Getting-Started.md) | ← [Laboratoire de Test et 22 Méthodes](05-Test-Lab-and-22-Input-Methods.md) | [Pilotes Kernel et Microcontrôleurs USB HID](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 Mini-Scripts Python et Humanisation

Les **Touches Intelligentes** et le **Laboratoire de Test (Mode 4)** partagent un interpréteur intégré, 6 préréglages et une bibliothèque partagée (`python_scripts.json`).

## Fonctions Intégrées
| Fonction | Rôle |
| :--- | :--- |
| `press(key, hold_ms=65, human=False)` | Maintient la touche (variation humaine avec `human=True`). |
| `human_press(key, min_hold, max_hold, min_wait, max_wait)` | Pression et pause à durées aléatoires naturelles. |
| `hold_key(key, duration_ms=300)` | Maintien continu pour les déplacements 3D (`W`, `A`, `S`, `D`). |
| `rand(min_val, max_val)` | Entier aléatoire dans l'intervalle. |
| `wait(ms)` / `human_wait(min_ms, max_ms)` | Pause fixe ou naturelle. |
| `print(text, enter=False, human=False)` | Saisie de texte via le moteur actif. |
| `click(button, x, y, hold_ms, human, in_game_3d)` | Clic 5 boutons ; `in_game_3d=True` clique sur le réticule 3D sans `SetCursorPos`. |
| `move_rel(dx, dy, steps, duration_ms)` | Rotation caméra 3D fluide (`MOUSEEVENTF_MOVE`). |

```python
for i in range(3):
    human_press("1", 65, 95, 80, 130)
    hold_key("W", rand(260, 380))
    move_rel(120, 0, steps=8, duration_ms=80)
    click("left", hold_ms=rand(68, 98), human=True, in_game_3d=True)
```

---

[🌐 Toutes les Langues](../README.md) | [🏠 Accueil](01-Home-and-Getting-Started.md) | ← [Laboratoire de Test et 22 Méthodes](05-Test-Lab-and-22-Input-Methods.md) | [Pilotes Kernel et Microcontrôleurs USB HID](07-Hardware-Drivers-and-Microcontrollers.md) →
