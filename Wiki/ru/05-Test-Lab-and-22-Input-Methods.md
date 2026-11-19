[🌐 Все Языки](../README.md) | [🏠 Главная](01-Home-and-Getting-Started.md) | ← [Запись Мыши и Клавиатуры](04-Recorder-and-Playback.md) | [Мини-скрипты Python и Гуманизация](06-Python-Mini-Scripting.md) →

# 🧪 Тестовая Лаборатория и 22 Метода Ввода

Проверьте конкретный метод или запустите строгий прогон (`strict_no_fallback`) всех **22 методов ввода** в активном или фоновом режиме.

## 6 Режимов Тестирования
1. **Макрос / Быстрый тест (`W -> Space`)**
2. **Текст / Умный шаблон**
3. **Одна клавиша**
4. **Прогон — Редактируемый скрипт Python**
5. **Прогон — Шаблон переменных** (`key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n`)
6. **Прогон — Сохранённый макрос**

## Обзор 22 Методов
- **1–4:** `SendInput` (ScanCode, Hybrid, Unicode) и `keybd_event`.
- **5–6:** `Stealth Humanized` и `8-Byte 6KRO USB HID`.
- **7–8:** Драйвер ядра Ring-0 `Interception` (`LLKHF_INJECTED=0`) и порт PS/2 `0x60/0x64` (`InpOutx64`).
- **9–10:** Атомарный `INPUT[2]` и Linux `/dev/uinput` / `XTest`.
- **11–20:** Фоновые `HWND` методы (`PostMessageW`, `SendMessageTimeoutW`, дочерние контролы `Scintilla`/`Edit`, `AttachThreadInput`, `WM_CHAR`, `WM_IME_KEYDOWN`).
- **21–22:** Внешние USB HID контроллеры (**Raspberry Pi Pico** и **Arduino Leonardo / Pro Micro**).

---

[🌐 Все Языки](../README.md) | [🏠 Главная](01-Home-and-Getting-Started.md) | ← [Запись Мыши и Клавиатуры](04-Recorder-and-Playback.md) | [Мини-скрипты Python и Гуманизация](06-Python-Mini-Scripting.md) →
