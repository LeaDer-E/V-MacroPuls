[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Building from Source (Windows & Linux)](09-Building-from-Source.md)

# ❓ Troubleshooting & FAQ

## Common Issues & Fixes

| Symptom | Cause & Solution |
| :--- | :--- |
| **Background methods (`11–20`) report `UIPI Rejected`** | The target app is running as Administrator while V-MacroPlus is Standard User. Click the **Standard User** badge in the top header to elevate. |
| **Methods `1–10` show `Skipped` in sweeps** | **Background Test** is ON while the target window is unfocused. Foreground methods are skipped to protect V-MacroPlus; disable Background Test or click `Switch`. |
| **Pico / Arduino not detected on COM port** | Ensure you are using a **USB Data Cable** (not charge-only) plugged directly into a PC USB port. |
| **Mouse clicks offset on High-DPI screens** | Use **`F8`** live picking, which reads raw physical Win32 `GetCursorPos()` coordinates directly. |
| **Interception shows unavailable after install** | Reboot Windows once after running `install-interception.exe` and verify the 64-bit `interception.dll` is inside `drivers/`. |

## Configuration Data Directory
- **Windows:** `%APPDATA%\V-Studio\V-MacroPlus\`
- **Linux:** `~/.local/share/V-Studio/V-MacroPlus/`
- **Factory Reset:** Click **Reset Defaults** in Settings, or quit the app and delete `settings.json`.

## Privacy & Offline Guarantee
- **Zero Telemetry / Zero Network:** V-MacroPlus never connects to the internet or transmits data. All configs, macros, and scripts stay 100% local on your machine.

---

[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Building from Source (Windows & Linux)](09-Building-from-Source.md)
