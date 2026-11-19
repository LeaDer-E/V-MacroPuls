[🌐 تمام زبانیں](../README.md) | [🏠 ہوم](01-Home-and-Getting-Started.md) | ← [ٹیسٹ لیب اور 22 ان پٹ طریقے](05-Test-Lab-and-22-Input-Methods.md) | [کرنل ڈرائیورز اور USB HID ہارڈویئر](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 پائتھن منی اسکرپٹس اور انسانی رفتار

**اسمارٹ کیز** اور **ٹیسٹ لیب (موڈ 4)** ایک مشترکہ انٹرپریٹر، 6 تیار ٹیمپلیٹس اور صارف لائبریری (`python_scripts.json`) استعمال کرتے ہیں۔

## بلٹ ان فنکشنز
- `press(key, hold_ms=65, human=False)` / `human_press(key, min_hold, max_hold, min_wait, max_wait)`
- `hold_key(key, duration_ms=300)` — تھری ڈی گیمز میں `WASD` حرکت کے لیے۔
- `rand(min_val, max_val)`، `wait(ms)`، `human_wait(min_ms, max_ms)`
- `print(text, enter=False, human=False)` / `type_text(...)`
- `click(button="left", x=None, y=None, hold_ms=68, human=False, in_game_3d=False)`
- `move_rel(dx, dy, steps=6, duration_ms=60)` — تھری ڈی کیمرہ گھمانے کے لیے۔

---

[🌐 تمام زبانیں](../README.md) | [🏠 ہوم](01-Home-and-Getting-Started.md) | ← [ٹیسٹ لیب اور 22 ان پٹ طریقے](05-Test-Lab-and-22-Input-Methods.md) | [کرنل ڈرائیورز اور USB HID ہارڈویئر](07-Hardware-Drivers-and-Microcontrollers.md) →
