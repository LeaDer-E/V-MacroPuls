[🌐 Toutes les Langues](../README.md) | [🏠 Accueil](01-Home-and-Getting-Started.md) | ← [Mini-Scripts Python et Humanisation](06-Python-Mini-Scripting.md) | [Paramètres, Thèmes et Raccourcis](08-Settings-Themes-and-Hotkeys.md) →

# 🔌 Pilotes Kernel et Microcontrôleurs USB HID

Pour les jeux 3D bloquant les entrées logicielles (`LLKHF_INJECTED = 1`), V-MacroPlus génère de véritables signaux matériels (`LLKHF_INJECTED = 0`) :

## 1. Raspberry Pi Pico (Méthode 21 — Installation en 1 Clic)
1. Maintenez **BOOTSEL** en branchant le Pico avec un **câble USB de données** (`RPI-RP2`).
2. Copiez le `.uf2` officiel **CircuitPython** sur `RPI-RP2` ; le lecteur redémarre en **`CIRCUITPY`**.
3. Dans **V-MacroPlus -> Paramètres**, cliquez sur **Installation en 1 clic sur Pico (`CIRCUITPY`)**.
4. Rebranchez une fois, cliquez sur **Détecter et Tester** et choisissez la **Méthode 21**.

## 2. Arduino Leonardo / Pro Micro (Méthode 22 — ATmega32U4)
1. Ouvrez `firmware/arduino_leonardo_promicro/arduino_hid_bridge.ino` dans **Arduino IDE**.
2. Choisissez **Board : `Arduino Leonardo`** et le port COM, puis cliquez sur **Upload**.
3. Dans **V-MacroPlus -> Paramètres**, testez la carte et sélectionnez la **Méthode 22**.

## 3. Interception (Méthode 7) & InpOutx64 PS/2 (Méthode 8)
Liez `interception.dll` (64-bit) ou `inpoutx64.dll` dans `drivers/` depuis Paramètres et redémarrez Windows une fois après `install-interception.exe`.

---

[🌐 Toutes les Langues](../README.md) | [🏠 Accueil](01-Home-and-Getting-Started.md) | ← [Mini-Scripts Python et Humanisation](06-Python-Mini-Scripting.md) | [Paramètres, Thèmes et Raccourcis](08-Settings-Themes-and-Hotkeys.md) →
