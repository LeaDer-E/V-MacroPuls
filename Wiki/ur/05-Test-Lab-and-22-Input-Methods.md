[🌐 تمام زبانیں](../README.md) | [🏠 ہوم](01-Home-and-Getting-Started.md) | ← [ماؤس اور کی بورڈ ریکارڈر](04-Recorder-and-Playback.md) | [پائتھن منی اسکرپٹس اور انسانی رفتار](06-Python-Mini-Scripting.md) →

# 🧪 ٹیسٹ لیب اور 22 ان پٹ طریقے

کسی ایک طریقے کو جانچیں یا تمام **22 ان پٹ طریقوں** پر سخت جانچ (`strict_no_fallback`) چلائیں۔

## 6 ٹیسٹ موڈز
1. **میکرو / فوری ڈیمو (`W -> Space`)**
2. **ٹیکسٹ / اسمارٹ ٹیمپلیٹ**
3. **ایک کلید**
4. **مکمل جانچ — قابلِ ترمیم پائتھن اسکرپٹ**
5. **مکمل جانچ — متغیر ٹیمپلیٹ** (`key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n`)
6. **مکمل جانچ — محفوظ میکرو**

## 22 طریقوں کا خلاصہ
- **1 تا 4:** `SendInput` (ScanCode, Hybrid, Unicode) اور `keybd_event`۔
- **5 تا 6:** `Stealth Humanized` اور `8-Byte 6KRO USB HID`۔
- **7 تا 8:** Ring-0 کرنل `Interception` (`LLKHF_INJECTED=0`) اور PS/2 پورٹ `0x60/0x64` (`InpOutx64`)۔
- **9 تا 10:** ایٹمک `INPUT[2]` اور لینکس `/dev/uinput` / `XTest`۔
- **11 تا 20:** بیک گراؤنڈ `HWND` پائپ لائنز (`PostMessageW`, `SendMessageTimeoutW`, `Scintilla`/`Edit`, `AttachThreadInput`, `WM_CHAR`, `WM_IME_KEYDOWN`)۔
- **21 تا 22:** بیرونی USB HID ہارڈویئر (**Raspberry Pi Pico** اور **Arduino Leonardo / Pro Micro**)۔

---

[🌐 تمام زبانیں](../README.md) | [🏠 ہوم](01-Home-and-Getting-Started.md) | ← [ماؤس اور کی بورڈ ریکارڈر](04-Recorder-and-Playback.md) | [پائتھن منی اسکرپٹس اور انسانی رفتار](06-Python-Mini-Scripting.md) →
