# Changelog

All notable changes to **V-MacroPlus** are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [3.9.0]

### Added
- **Three-Mode 20-Method Sweep in Test Lab (`app/ui/pages/test_page.py`, `app/domain/autopresser_engine.py`):**
  - **Mode 4 (`sweep_20`):** Editable Python Per-Method Test Script (`QPlainTextEdit`) where `print()`, `type_text()`, `press()`, `wait()`, and `click()` execute strictly through each method `1..20`.
  - **Mode 5 (`sweep_20_template`):** Smart Key & Variable Template engine (`expand_and_execute_method_template`) supporting inline key directives (`key="Enter"`, `[key=Space]`, `{key:F1}`), delays (`wait=120`), escape sequences (`\n`, `\t`), clickable variable chips, and 10 dynamic variables (`{method_num}`, `{method_num_02}`, `{method_id}`, `{method_name}`, `{category}`, `{target_app}`, `{target_hwnd}`, `{mode}`, `{time}`, `{date}`).
  - **Mode 6 (`sweep_20_macro`):** Per-Method Macro Sweep (`execute_per_method_macro_test`) allowing any user Smart Key (`ComboStep` sequence) or Saved Recording (`MacroEvent` timeline) to be tested sequentially across all 20 methods with an optional template prefix.
- **Background Test Mode (`tog_test_bg` in `TestPage` & `run_in_background` in `AutoKeyPresserPage`):**
  - Testing or sweeping methods in Background Mode never focuses the target window or steals focus from V-MacroPlus, and safely skips foreground-only methods (`1..10` and mouse `real`/`sendinput_abs`) when the target window is unfocused.
- **Two-Line Structured, BiDi-Safe Diagnostic Logs (`format_diagnostic_reason`, `dispatch_autopresser_mouse_detailed`):**
  - Upgraded both Auto Presser Activity Logs and Test Lab Logs to two-line cards displaying `[#01/20]` or `[#01/8]` index badges, localized status badges, target process + `HWND` + resolved inner child control class (`RichEditD2DPT`, `Scintilla`, `Edit`), Screen-to-Client mouse coordinates (`Screen(x, y) -> Client(cx, cy)`), millisecond latency, and localized diagnostic reasons across all 10 languages.
- **5-Button Mouse Support (`Back x1` / `Forward x2`) & Live Mouse Point Test Button:**
  - Added `Back (x1)` and `Forward (x2)` mouse buttons and a live per-row `Test` button in Auto Presser Multi-Point Mouse Clicks.
- **Collapsible Macro Sidebar & Tinted Horizontal Scroll Containers (`TintedScrollBox`):**
  - Added collapsible sidebar toggle buttons in Smart Keys and Recordings, and custom-painted `TintedScrollBox` containers for Auto Presser tabs and excluded applications.
- **36 Color Palettes & Custom RGB/Hex Color Picker:**
  - Expanded theme swatches in Settings to 36 palettes (3 rows x 12 swatches) plus a Custom Color Picker dialog, positioned as the first card at the top of Settings, with the Emergency Stop card placed in its own standalone section at the bottom of Settings.

### Changed
- **2-Row Auto Presser Key Tiles (`_KeyTileWidget`):**
  - Redesigned all 36 built-in key tiles and custom key tiles into a spacious `78 px` 2-row layout (Row 1: Key Pill + Smart Key `Lightning` toggle; Row 2: `ms` interval input + `Play` single-key test button).
- **Zero-Flicker In-Place Theme & Resize Optimization (`refresh_widget_tree_theme`):**
  - Eliminated page destruction/recreation during theme/palette switches and pooled `_KeyTileWidget` instances (`bind_item`) for smooth window resizing and tab switching.

### Fixed
- Fixed `dispatch_autopresser_key` routing Methods `11..20` to `sendinput_scancode` when `run_in_background=False` was passed from Test Lab.
- Enforced `strict_no_fallback=True` during all single-method tests and 20-method / 8-mode sweeps so missing optional drivers (`Interception`, `InpOutx64`, `Linux /dev/uinput`) never fake success via `SendInput` fallback.
- Fixed thread-safety in `TestPage` background sweeps by routing all background log insertions through `_thread_log_signal = Signal(dict, str)`.
- Fixed `_KeyMacroModeButton.setHasLinkedMacro` attribute lookup when closing or switching Auto Presser tabs.

## [3.6.0]

### Added
- Integrated the complete Multi-Tab **V-AutoKeyPresser** studio (`autopresser_page.py`, `autopresser_engine.py`) with 36 built-in keys, custom keys, 20 unified key methods, 8 mouse modes, per-tab profiles, and multi-tab workspaces.
- Added **Motherboard PS/2 Port `0x60/0x64` (`InpOutx64` / `i8042prt`)** 4-layer live diagnostic and 1-click file linking panel in Settings.
- Added **Python Mini-Scripting (`python_script`)** and **Direct Text Typing (`type_text`)** step adders in Smart Keys.

## [3.2.0]

### Added
- Added **Ring-0 Interception Kernel Driver (`Method 7`)** integration and 5-layer live `LLKHF_INJECTED` hardware pulse diagnostic panel in Settings.
- Added 10-language localization (`ar`, `en`, `es`, `fr`, `de`, `tr`, `ru`, `id`, `fa`, `ur`), lossless 1-to-1 recording-to-Smart-Key conversion, and stop-button mouse/hotkey exclusion.
