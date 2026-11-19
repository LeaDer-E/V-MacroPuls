[🌐 تمام زبانیں](../README.md) | [🏠 ہوم](01-Home-and-Getting-Started.md) | ← [پائتھن منی اسکرپٹس اور انسانی رفتار](06-Python-Mini-Scripting.md) | [ترتیبات، تھیمز اور شارٹ کٹس](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 کرنل ڈرائیورز اور USB HID مائیکرو کنٹرولرز

سافٹ ویئر ان پٹ (`LLKHF_INJECTED = 1`) بلاک کرنے والی گیمز کے لیے حقیقی ہارڈویئر ان پٹ (`LLKHF_INJECTED = 0`):

## 1. Raspberry Pi Pico (طریقہ 21 — ون کلک سیٹ اپ)
1. **USB ڈیٹا کیبل** لگاتے وقت سفید **BOOTSEL** بٹن دبائے رکھیں (`RPI-RP2`)۔
2. آفیشل **CircuitPython `.uf2`** کو `RPI-RP2` میں کاپی کریں (یہ **`CIRCUITPY`** بن کر دوبارہ آن ہوگا)۔
3. **V-MacroPlus -> ترتیبات** میں **Pico پر ون کلک انسٹال (`CIRCUITPY`)** دبائیں۔
4. کیبل نکال کر دوبارہ لگائیں، بورڈ ٹیسٹ کریں اور **طریقہ 21** منتخب کریں۔

## 2. Arduino Leonardo / Pro Micro (طریقہ 22 — ATmega32U4)
1. `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` کو **Arduino IDE** میں کھولیں۔
2. **Board: `Arduino Leonardo`** اور COM پورٹ منتخب کر کے **Upload** کریں۔
3. ترتیبات میں بورڈ ٹیسٹ کریں اور **طریقہ 22** چنیں۔

## 3. Interception (طریقہ 7) اور InpOutx64 PS/2 (طریقہ 8)
ترتیبات کے ذریعے `interception.dll` (64-bit) یا `inpoutx64.dll` کو `drivers/` فولڈر میں لنک کریں اور `install-interception.exe` کے بعد ونڈوز ری اسٹارٹ کریں۔

---

[🌐 تمام زبانیں](../README.md) | [🏠 ہوم](01-Home-and-Getting-Started.md) | ← [پائتھن منی اسکرپٹس اور انسانی رفتار](06-Python-Mini-Scripting.md) | [ترتیبات، تھیمز اور شارٹ کٹس](08-Settings-Themes-and-Hotkeys.md) →
