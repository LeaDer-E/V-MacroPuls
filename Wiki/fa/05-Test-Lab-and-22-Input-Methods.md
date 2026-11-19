[🌐 همه زبان‌ها](../README.md) | [🏠 خانه](01-Home-and-Getting-Started.md) | ← [ضبط‌کننده ماوس و کیبورد](04-Recorder-and-Playback.md) | [اسکریپت‌های کوتاه پایتون و شبیه‌سازی انسانی](06-Python-Mini-Scripting.md) →

# 🧪 آزمایشگاه تست و ۲۲ موتور ورودی

یک موتور را به‌تنهایی بسنجید یا بررسی جامع سخت‌گیرانه (`strict_no_fallback`) را روی هر **۲۲ روش ورودی** در حالت پیش‌زمینه یا پس‌زمینه اجرا کنید.

## ۶ حالت آزمایش
1. **اجرای ماکرو / تست سریع (`W -> Space`)**
2. **تایپ متن / قالب هوشمند**
3. **یک کلید تکی**
4. **بررسی جامع — اسکریپت پایتون قابل ویرایش**
5. **بررسی جامع — قالب متغیرها** (`key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n`)
6. **بررسی جامع — ماکرو ذخیره‌شده**

## خلاصه ۲۲ روش
- **۱ تا ۴:** `SendInput` (ScanCode, Hybrid, Unicode) و `keybd_event`.
- **۵ تا ۶:** `Stealth Humanized` و `8-Byte 6KRO USB HID`.
- **۷ تا ۸:** درایور کرنل Ring-0 `Interception` (`LLKHF_INJECTED=0`) و پورت PS/2 `0x60/0x64` (`InpOutx64`).
- **۹ تا ۱۰:** ارسال اتمی `INPUT[2]` و لینوکس `/dev/uinput` / `XTest`.
- **۱۱ تا ۲۰:** روش‌های پس‌زمینه `HWND` (`PostMessageW`, `SendMessageTimeoutW`, کنترل‌های داخلی `Scintilla`/`Edit`, `AttachThreadInput`, `WM_CHAR`, `WM_IME_KEYDOWN`).
- **۲۱ تا ۲۲:** سخت‌افزار خارجی USB HID (**Raspberry Pi Pico** و **Arduino Leonardo / Pro Micro**).

---

[🌐 همه زبان‌ها](../README.md) | [🏠 خانه](01-Home-and-Getting-Started.md) | ← [ضبط‌کننده ماوس و کیبورد](04-Recorder-and-Playback.md) | [اسکریپت‌های کوتاه پایتون و شبیه‌سازی انسانی](06-Python-Mini-Scripting.md) →
