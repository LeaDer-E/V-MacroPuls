[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Python Mini-Betikler ve İnsanlaştırma](06-Python-Mini-Scripting.md) | [Ayarlar, Temalar ve Kısayollar](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 Çekirdek Sürücüleri ve Harici USB HID Kartları

Yazılım girişlerini (`LLKHF_INJECTED = 1`) engelleyen 3D oyunlar için gerçek donanım girişi (`LLKHF_INJECTED = 0`):

## 1. Raspberry Pi Pico (Yöntem 21 — Tek Tıkla Kurulum)
1. **BOOTSEL** düğmesine basılı tutarak Pico'yu **USB veri kablosu** ile bağlayın (`RPI-RP2`).
2. Resmi **CircuitPython `.uf2`** dosyasını `RPI-RP2` içine kopyalayın (`CIRCUITPY` olarak açılır).
3. **V-MacroPlus -> Ayarlar** bölümünden **Pico'ya Tek Tıkla Kur (`CIRCUITPY`)** düğmesine basın.
4. Kabloyu çıkarıp tekrar takın, kartı test edin ve **Yöntem 21**'i seçin.

## 2. Arduino Leonardo / Pro Micro (Yöntem 22 — ATmega32U4)
1. `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` dosyasını **Arduino IDE** ile açın.
2. **Board: `Arduino Leonardo`** ve COM portunu seçip **Upload** tıklayın.
3. Ayarlardan kartı test edin ve **Yöntem 22**'yi seçin.

## 3. Interception (Yöntem 7) ve InpOutx64 PS/2 (Yöntem 8)
Ayarlar üzerinden `interception.dll` (64-bit) veya `inpoutx64.dll` dosyalarını `drivers/` klasörüne bağlayın ve `install-interception.exe` sonrası Windows'u bir kez yeniden başlatın.

---

[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Python Mini-Betikler ve İnsanlaştırma](06-Python-Mini-Scripting.md) | [Ayarlar, Temalar ve Kısayollar](08-Settings-Themes-and-Hotkeys.md) →
