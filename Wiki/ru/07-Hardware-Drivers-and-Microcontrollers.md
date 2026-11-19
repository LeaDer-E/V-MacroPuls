[🌐 Все Языки](../README.md) | [🏠 Главная](01-Home-and-Getting-Started.md) | ← [Мини-скрипты Python и Гуманизация](06-Python-Mini-Scripting.md) | [Настройки, Темы и Горячие Клавиши](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 Драйверы Ядра и Внешние USB HID Контроллеры

Для защищённых 3D-игр, блокирующих программный ввод (`LLKHF_INJECTED = 1`), используйте реальное железо (`LLKHF_INJECTED = 0`):

## 1. Raspberry Pi Pico (Метод 21 — Установка в 1 клик)
1. Зажмите **BOOTSEL** при подключении Pico **USB-кабелем для передачи данных** (`RPI-RP2`).
2. Скопируйте официальный `.uf2` **CircuitPython** на `RPI-RP2` (появится диск **`CIRCUITPY`**).
3. В **V-MacroPlus -> Настройки** нажмите **Установить в 1 клик на Pico (`CIRCUITPY`)**.
4. Переподключите кабель, проверьте плату и выберите **Метод 21**.

## 2. Arduino Leonardo / Pro Micro (Метод 22 — ATmega32U4)
1. Откройте `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` в **Arduino IDE**.
2. Выберите **Board: `Arduino Leonardo`** и COM-порт, затем нажмите **Upload**.
3. В Настройках проверьте плату и выберите **Метод 22**.

## 3. Interception (Метод 7) и InpOutx64 PS/2 (Метод 8)
Привяжите `interception.dll` (64-bit) или `inpoutx64.dll` в `drivers/` через Настройки и перезагрузите Windows после `install-interception.exe`.

---

[🌐 Все Языки](../README.md) | [🏠 Главная](01-Home-and-Getting-Started.md) | ← [Мини-скрипты Python и Гуманизация](06-Python-Mini-Scripting.md) | [Настройки, Темы и Горячие Клавиши](08-Settings-Themes-and-Hotkeys.md) →
