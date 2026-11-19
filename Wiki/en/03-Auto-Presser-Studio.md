[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Smart Keys & Custom Macros](02-Smart-Keys-and-Macros.md) | [Mouse & Keyboard Recorder](04-Recorder-and-Playback.md) →

# ⌨️ Multi-Tab Auto Presser Studio

The **Auto Presser** page lets you run **unlimited concurrent automation tabs**, each on its own background worker thread and bound to its own target application window (`HWND`).

## Tabs, Window Binding & Background Execution
- **Unlimited Tabs:** Create (`+ New Tab`), duplicate, rename, and close tabs independently.
- **Hotkeys:** Each tab has its own Start/Stop toggle shortcut (default `F6`), plus a global **Stop All Tabs** hotkey (`F7`).
- **Window Binding & `Switch` Button:** Pick any running window from the dropdown or window picker. Click the orange **Switch** button to restore and focus that window on demand.
- **Run in Background:** Sends keystrokes and mouse clicks directly to the target `HWND` without stealing focus from your active window.

## 36 Built-In Key Tiles & Custom Keys
- 3 rows × 12 built-in keys (`F1–F12`, `1–=`, `Num1–Num*`) plus **+ Add Custom Key** for any physical key.
- Each 2-row tile features:
  - **Row 1:** Enable toggle pill + **Smart Key Link (`⚡`)** — when enabled, runs the matching Smart Key macro on every interval tick instead of a plain key press.
  - **Row 2:** Repeat interval (`10 ms` to `1 hour`) + single-key live **Test (▶)** button.

## Multi-Point Mouse Clicker (5 Buttons & 8 Modes)
1. Switch to the **Mouse** sub-view.
2. Press **`F8`** anywhere on screen to capture coordinates and add or update a click point instantly.
3. Choose any of 5 mouse buttons (`left`, `right`, `middle`, `x1`, `x2`) and one of **8 mouse modes** (`auto`, `real`, `post`, `post_top`, `post_hittest_chain`, `attach_mouse_state`, `guithread_capture`, `sendinput_abs`).
4. Use **Test All / Sweep** to benchmark all key methods or mouse modes and inspect the structured 2-line **Activity Log**.

---

[🌐 All Languages](../README.md) | [🏠 Home](01-Home-and-Getting-Started.md) | ← [Smart Keys & Custom Macros](02-Smart-Keys-and-Macros.md) | [Mouse & Keyboard Recorder](04-Recorder-and-Playback.md) →
