[🌐 همه زبان‌ها](../README.md) | [🏠 خانه](01-Home-and-Getting-Started.md) | ← [اسکریپت‌های کوتاه پایتون و شبیه‌سازی انسانی](06-Python-Mini-Scripting.md) | [تنظیمات، تم‌ها و میانبرها](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 درایورهای کرنل و میکروکنترلرهای USB HID

برای بازی‌های سه‌بعدی که ورودی‌های نرم‌افزاری (`LLKHF_INJECTED = 1`) را مسدود می‌کنند، از سخت‌افزار واقعی (`LLKHF_INJECTED = 0`) استفاده کنید:

## ۱. Raspberry Pi Pico (روش ۲۱ — نصب یک‌کلیکی)
1. دکمه **BOOTSEL** را هنگام اتصال Pico با **کابل دیتای USB** نگه دارید (`RPI-RP2`).
2. فایل رسمی `.uf2` مربوط به **CircuitPython** را در `RPI-RP2` کپی کنید تا به‌عنوان **`CIRCUITPY`** بالا بیاید.
3. در **V-MacroPlus -> تنظیمات** روی **نصب یک‌کلیکی روی Pico (`CIRCUITPY`)** کلیک کنید.
4. کابل را یک‌بار جدا و وصل کنید، برد را تست کرده و **روش ۲۱** را انتخاب کنید.

## ۲. Arduino Leonardo / Pro Micro (روش ۲۲ — ATmega32U4)
1. فایل `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` را در **Arduino IDE** باز کنید.
2. برد **`Arduino Leonardo`** و پورت COM را انتخاب و **Upload** کنید.
3. در تنظیمات برد را تست کرده و **روش ۲۲** را انتخاب نمایید.

## ۳. Interception (روش ۷) و InpOutx64 PS/2 (روش ۸)
فایل `interception.dll` (نسخه 64-bit) یا `inpoutx64.dll` را در پوشه `drivers/` متصل کنید و پس از اجرای `install-interception.exe` یک‌بار ویندوز را ریستارت کنید.

---

[🌐 همه زبان‌ها](../README.md) | [🏠 خانه](01-Home-and-Getting-Started.md) | ← [اسکریپت‌های کوتاه پایتون و شبیه‌سازی انسانی](06-Python-Mini-Scripting.md) | [تنظیمات، تم‌ها و میانبرها](08-Settings-Themes-and-Hotkeys.md) →
