[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Mouse & Keyboard Recorder](04-Recorder-and-Playback.md) | [Python Mini-Scripting & Humanization](06-Python-Mini-Scripting.md) →

# 🧪 Test Lab & 22 Input Methods

The **Test Lab** lets you test any single input engine or run an automated sweep across all **22 input methods** against either the built-in sandbox or any external application window.

## Strict Non-Fallback Verification
In normal daily use, V-MacroPlus falls back safely to `SendInput` if an optional driver or microcontroller is disconnected. Inside **Test Lab**, `strict_no_fallback` is enforced so every method runs strictly through its own protocol and reports honest pass/fail diagnostics.

## The 6 Test Lab Action Modes
| # | Mode | What It Does |
| :--- | :--- | :--- |
| **1** | **Run Macro / Quick Demo** | Runs a saved Smart Key, Recording, or `W -> Space` demo via the selected method. |
| **2** | **Type Text / Smart Template** | Parses and dispatches text or variable templates via the selected method. |
| **3** | **Single Key** | Dispatches one captured key via the selected method. |
| **4** | **Sweep — Editable Python Script** | Runs your custom Python script sequentially through each method. |
| **5** | **Sweep — Smart Variable Template** | Types a diagnostic line identifying each working method inside the target window. |
| **6** | **Sweep — Saved Macro** | Executes every step of a chosen macro through each method in sequence. |

## Smart Variable Template Syntax
```text
key="Enter" {method_num}: Hello V-MacroPlus worked with {method_id} 

```
- **Variables:** `{method_num}`, `{method_num_02}`, `{method_id}`, `{method_name}`, `{category}`, `{target_app}`, `{target_hwnd}`, `{mode}`, `{time}`, `{date}`.
- **Directives:** `key="<KeyName>"` (92+ keys supported, case-insensitive), `
` (Enter), `	` (Tab), and `wait=<ms>`.

## All 22 Input Methods at a Glance
| Methods | Category | Mechanism |
| :--- | :--- | :--- |
| **1 – 4** | Standard Foreground | `SendInput` (ScanCode, Hybrid VK+Scan, Unicode) and legacy `keybd_event`. |
| **5 – 6** | Advanced Foreground | `Stealth Humanized` (`dwExtraInfo=0`, 1000Hz tick alignment) & `8-Byte 6KRO USB HID`. |
| **7 – 8** | Kernel & Motherboard Port | **Method 7:** Ring-0 `Interception` (`LLKHF_INJECTED=0`); **Method 8:** PS/2 Port `0x60/0x64` (`InpOutx64`). |
| **9 – 10** | Atomic Burst & Linux | **Method 9:** Atomic `INPUT[2]` burst; **Method 10:** Linux `/dev/uinput` + `XTest`. |
| **11 – 20** | Background `HWND` | `PostMessageW`, `SendMessageTimeoutW`, inner child editor discovery (`Scintilla`, `Edit`, `RichEditD2DPT`), `AttachThreadInput + SetKeyboardState`, `GetGUIThreadInfo`, `WM_CHAR Triplet`, `WM_IME_KEYDOWN`, `SendNotifyMessageW`. |
| **21 – 22** | External USB HID Hardware | **Method 21:** **Raspberry Pi Pico** (`RP2040`/`RP2350`); **Method 22:** **Arduino Leonardo / Pro Micro** (`ATmega32U4`). |

---

[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Mouse & Keyboard Recorder](04-Recorder-and-Playback.md) | [Python Mini-Scripting & Humanization](06-Python-Mini-Scripting.md) →
