[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Test Laboratuvarı ve 22 Giriş Yöntemi](05-Test-Lab-and-22-Input-Methods.md) | [Çekirdek Sürücüleri ve USB HID Kartları](07-Hardware-Drivers-and-Microcontrollers.md) →

# 🐍 Python Mini-Betikler ve İnsanlaştırma

**Akıllı Tuşlar** ve **Test Laboratuvarı (Mod 4)** ortak yerleşik yorumlayıcıyı, 6 hazır şablonu ve kullanıcı betik kütüphanesini (`python_scripts.json`) paylaşır.

## Yerleşik Fonksiyonlar
- `press(key, hold_ms=65, human=False)` / `human_press(key, min_hold, max_hold, min_wait, max_wait)`
- `hold_key(key, duration_ms=300)` — 3D oyunlarda `WASD` hareketi için.
- `rand(min_val, max_val)`, `wait(ms)`, `human_wait(min_ms, max_ms)`
- `print(text, enter=False, human=False)` / `type_text(...)`
- `click(button="left", x=None, y=None, hold_ms=68, human=False, in_game_3d=False)`
- `move_rel(dx, dy, steps=6, duration_ms=60)` — 3D kamera dönüşü için göreli fare hareketi.

---

[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Test Laboratuvarı ve 22 Giriş Yöntemi](05-Test-Lab-and-22-Input-Methods.md) | [Çekirdek Sürücüleri ve USB HID Kartları](07-Hardware-Drivers-and-Microcontrollers.md) →
