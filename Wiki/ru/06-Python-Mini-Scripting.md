[🌐 Все Языки](../README.md) | [🏠 Главная](01-Home-and-Getting-Started.md) | ← [Тестовая Лаборатория и 22 Метода](05-Test-Lab-and-22-Input-Methods.md) | [Драйверы Ядра и USB HID Контроллеры](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 Мини-скрипты Python и Гуманизация

**Умные Клавиши** и **Тестовая Лаборатория (Режим 4)** используют единый встроенный интерпретатор, 6 готовых пресетов и общую библиотеку (`python_scripts.json`).

## Встроенные Функции
- `press(key, hold_ms=65, human=False)` / `human_press(key, min_hold, max_hold, min_wait, max_wait)`
- `hold_key(key, duration_ms=300)` — удержание клавиш движения `WASD` в 3D-играх.
- `rand(min_val, max_val)`, `wait(ms)`, `human_wait(min_ms, max_ms)`
- `print(text, enter=False, human=False)` / `type_text(...)`
- `click(button="left", x=None, y=None, hold_ms=68, human=False, in_game_3d=False)`
- `move_rel(dx, dy, steps=6, duration_ms=60)` — плавный относительный поворот 3D-камеры.

---

[🌐 Все Языки](../README.md) | [🏠 Главная](01-Home-and-Getting-Started.md) | ← [Тестовая Лаборатория и 22 Метода](05-Test-Lab-and-22-Input-Methods.md) | [Драйверы Ядра и USB HID Контроллеры](07-Hardware-Drivers-and-Microcontrollers.md) →
