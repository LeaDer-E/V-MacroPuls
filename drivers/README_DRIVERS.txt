V-MacroPlus — Ring-0 Kernel & Hardware Drivers Folder
=====================================================
To enable true Ring-0 Kernel injection (Method 7: Interception) without Fallback:
1. Place the 64-bit `interception.dll` (and optionally `install-interception.exe`) in this folder.
2. In V-MacroPlus -> Settings (الإعدادات), click:
   - [تثبيت / ربط ملفات Interception...] to install the kernel driver and register UpperFilters.
   - Restart Windows once after installing `install-interception.exe /install`.
   - Click [فحص حالة Interception والكيرنل الآن] in Settings to verify `Ring-0 Active (LLKHF_INJECTED = 0)`.

For Method 8 (Motherboard PS/2 Port 0x60/0x64):
- Place `inpoutx64.dll` in this folder.
