[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Fare ve Klavye Kaydedici](04-Recorder-and-Playback.md) | [Python Mini-Betikler ve İnsanlaştırma](06-Python-Mini-Scripting.md) →

# 🧪 Test Laboratuvarı ve 22 Giriş Yöntemi

Tek bir yöntemi test edin veya **22 giriş yönteminin** tamamını katı doğrulamayla (`strict_no_fallback`) ön planda ya da arka planda tarayın.

## 6 Test Modu
1. **Makro / Hızlı Demo (`W -> Space`)**
2. **Metin / Akıllı Şablon**
3. **Tek Tuş**
4. **Tarama — Düzenlenebilir Python Betiği**
5. **Tarama — Değişken Şablonu** (`key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n`)
6. **Tarama — Kayıtlı Makro**

## 22 Yöntem Özeti
- **1–4:** `SendInput` (ScanCode, Hybrid, Unicode) ve `keybd_event`.
- **5–6:** `Stealth Humanized` ve `8-Byte 6KRO USB HID`.
- **7–8:** Ring-0 Çekirdek `Interception` (`LLKHF_INJECTED=0`) ve PS/2 Port `0x60/0x64` (`InpOutx64`).
- **9–10:** Atomik `INPUT[2]` ve Linux `/dev/uinput` / `XTest`.
- **11–20:** Arka plan `HWND` yöntemleri (`PostMessageW`, `SendMessageTimeoutW`, alt kontrol keşfi, `AttachThreadInput`, `WM_CHAR`, `WM_IME_KEYDOWN`).
- **21–22:** Harici USB HID donanımı (**Raspberry Pi Pico** ve **Arduino Leonardo / Pro Micro**).

---

[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Fare ve Klavye Kaydedici](04-Recorder-and-Playback.md) | [Python Mini-Betikler ve İnsanlaştırma](06-Python-Mini-Scripting.md) →
