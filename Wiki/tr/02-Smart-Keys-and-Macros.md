[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Ana Sayfa ve Başlangıç](01-Home-and-Getting-Started.md) | [Çok Sekmeli Otomatik Basıcı](03-Auto-Presser-Studio.md) →

# ⚡ Akıllı Tuşlar ve Özel Makrolar

Herhangi bir fiziksel tuşu, 3 tuşa kadar kombinasyonu (`Ctrl + Shift + K`), 5 fare tuşundan birini, kaydırma tekerleğini veya medya tuşlarını çok adımlı bir makroya bağlayın.

## Çalışma Modları
| Mod | Davranış | Durdurma |
| :--- | :--- | :--- |
| **Bir Kez (`once`)** | Adımları sırayla bir kez çalıştırır. | Otomatik veya `Ctrl + Shift + Alt`. |
| **Basılı Tut (`repeat_hold`)** | Tuş basılı tutulduğu sürece tekrarlar. | Bırakıldığında veya **Zorunlu Durdurma Kısayolu**. |
| **Aç/Kapat Döngü (`toggle_loop`)** | İlk basışta döngüyü başlatır, ikinci basışta durdurur. | Tekrar basış, **Durdurma Kısayolu** veya Acil Durdurma. |
| **Manuel (Tuşsuz)** | Yan paneldeki **▶** düğmesiyle çalışır. | Otomatik veya Acil Durdurma. |

## 9 Adım Türü (Sınırsız — 10.000+ Adım Desteği)
1. **Tuş Bas / Basılı Tut** (`key_tap` / `key_hold`)
2. **Bekle** (`wait`, `1 ms`–`10 dk`)
3. **Fare Tıklaması** (`mouse_click`, 5 tuş + **`F8`** ile canlı koordinat alma)
4. **Fare Tekerleği** (`mouse_scroll`, 4 yön)
5. **Unicode Metin Yaz** (`type_text`)
6. **Python Mini-Betik** (`python_script`)
7. **Klasör / Program Aç** (`open_folder`)
8. **PowerShell Komutu** (`powershell`)
9. **CMD Komutu** (`cmd`)

Akıcı sürükle-bırak sıralama (`165 ms`), **Geri Al/Yinele (`Ctrl+Z` / `Ctrl+Y`)**, adım duraklatma ve anlık üzerine gelme notları içerir.

---

[🌐 Tüm Diller](../README.md) | [🏠 Ana Sayfa](01-Home-and-Getting-Started.md) | ← [Ana Sayfa ve Başlangıç](01-Home-and-Getting-Started.md) | [Çok Sekmeli Otomatik Basıcı](03-Auto-Presser-Studio.md) →
