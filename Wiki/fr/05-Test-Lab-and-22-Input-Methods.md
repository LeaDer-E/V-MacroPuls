[🌐 Toutes les Langues](../README.md) | [🏠 Accueil](01-Home-and-Getting-Started.md) | ← [Enregistreur Souris et Clavier](04-Recorder-and-Playback.md) | [Mini-Scripts Python et Humanisation](06-Python-Mini-Scripting.md) →

# 🧪 Laboratoire de Test et 22 Méthodes d'Entrée

Testez une méthode précise ou lancez un balayage strict (`strict_no_fallback`) sur les **22 moteurs d'entrée** en Premier Plan ou Arrière-Plan.

## Les 6 Modes de Test
1. **Macro / Démo rapide (`W -> Space`)**
2. **Texte / Modèle Intelligent**
3. **Touche unique**
4. **Balayage — Script Python éditable**
5. **Balayage — Modèle de Variables** (`key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} \n`)
6. **Balayage — Macro enregistrée**

## Aperçu des 22 Méthodes
- **1–4 :** `SendInput` (ScanCode, Hybrid, Unicode) & `keybd_event`.
- **5–6 :** `Stealth Humanized` & `8-Byte 6KRO USB HID`.
- **7–8 :** Pilote Kernel Ring-0 `Interception` (`LLKHF_INJECTED=0`) & Port PS/2 `0x60/0x64` (`InpOutx64`).
- **9–10 :** Rafale atomique `INPUT[2]` & Linux `/dev/uinput` / `XTest`.
- **11–20 :** Pipelines `HWND` en arrière-plan (`PostMessageW`, `SendMessageTimeoutW`, contrôles enfants `Scintilla`/`Edit`, `AttachThreadInput`, `WM_CHAR`, `WM_IME_KEYDOWN`).
- **21–22 :** Microcontrôleurs physiques USB HID (**Raspberry Pi Pico** & **Arduino Leonardo / Pro Micro**).

---

[🌐 Toutes les Langues](../README.md) | [🏠 Accueil](01-Home-and-Getting-Started.md) | ← [Enregistreur Souris et Clavier](04-Recorder-and-Playback.md) | [Mini-Scripts Python et Humanisation](06-Python-Mini-Scripting.md) →
