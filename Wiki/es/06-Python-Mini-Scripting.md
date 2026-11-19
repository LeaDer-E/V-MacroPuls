[🌐 Todos los Idiomas](../README.md) | [🏠 Inicio](01-Home-and-Getting-Started.md) | ← [Laboratorio de Pruebas y 22 Métodos](05-Test-Lab-and-22-Input-Methods.md) | [Drivers Kernel y Microcontroladores USB HID](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 Mini-Scripts de Python y Humanización

Tanto **Teclas Inteligentes** como el **Laboratorio de Pruebas (Modo 4)** comparten un intérprete integrado, 6 plantillas listas para usar y biblioteca personal (`python_scripts.json`).

## Funciones Integradas
| Función | Uso |
| :--- | :--- |
| `press(key, hold_ms=65, human=False)` | Pulsa y mantiene una tecla (con variación humana si `human=True`). |
| `human_press(key, min_hold, max_hold, min_wait, max_wait)` | Pulsación y pausa con tiempos humanos aleatorios. |
| `hold_key(key, duration_ms=300)` | Mantiene teclas de movimiento (`W`, `A`, `S`, `D`) en juegos 3D. |
| `rand(min_val, max_val)` | Genera un entero aleatorio. |
| `wait(ms)` / `human_wait(min_ms, max_ms)` | Espera fija o con jitter gaussiano natural. |
| `print(text, enter=False, human=False)` | Escribe texto con el motor activo. |
| `click(button, x, y, hold_ms, human, in_game_3d)` | Clic en 5 botones; `in_game_3d=True` hace clic en la mira 3D sin mover el cursor absoluto. |
| `move_rel(dx, dy, steps, duration_ms)` | Rotación relativa suave de cámara 3D (`MOUSEEVENTF_MOVE`). |

```python
for i in range(3):
    human_press("1", 65, 95, 80, 130)
    hold_key("W", rand(260, 380))
    move_rel(120, 0, steps=8, duration_ms=80)
    click("left", hold_ms=rand(68, 98), human=True, in_game_3d=True)
```

---

[🌐 Todos los Idiomas](../README.md) | [🏠 Inicio](01-Home-and-Getting-Started.md) | ← [Laboratorio de Pruebas y 22 Métodos](05-Test-Lab-and-22-Input-Methods.md) | [Drivers Kernel y Microcontroladores USB HID](07-Hardware-Drivers-and-Microcontrollers.md) →
