[🌐 همه زبان‌ها](../README.md) | [🏠 خانه](01-Home-and-Getting-Started.md) | ← [آزمایشگاه تست و ۲۲ موتور ورودی](05-Test-Lab-and-22-Input-Methods.md) | [درایورهای کرنل و میکروکنترلرهای USB HID](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 اسکریپت‌های کوتاه پایتون و شبیه‌سازی انسانی

**کلیدهای هوشمند** و **آزمایشگاه تست (حالت ۴)** از مفسر داخلی مشترک، ۶ قالب آماده و کتابخانه اسکریپت (`python_scripts.json`) بهره می‌برند.

## توابع داخلی
- `press(key, hold_ms=65, human=False)` / `human_press(key, min_hold, max_hold, min_wait, max_wait)`
- `hold_key(key, duration_ms=300)` — برای حرکت `WASD` در بازی‌های سه‌بعدی.
- `rand(min_val, max_val)`، `wait(ms)`، `human_wait(min_ms, max_ms)`
- `print(text, enter=False, human=False)` / `type_text(...)`
- `click(button="left", x=None, y=None, hold_ms=68, human=False, in_game_3d=False)`
- `move_rel(dx, dy, steps=6, duration_ms=60)` — چرخش نرم دوربین بازی‌های سه‌بعدی.

---

[🌐 همه زبان‌ها](../README.md) | [🏠 خانه](01-Home-and-Getting-Started.md) | ← [آزمایشگاه تست و ۲۲ موتور ورودی](05-Test-Lab-and-22-Input-Methods.md) | [درایورهای کرنل و میکروکنترلرهای USB HID](07-Hardware-Drivers-and-Microcontrollers.md) →
