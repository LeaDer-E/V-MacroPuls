#include "Win32Input.h"
#include "MicrocontrollerBridge.h"
#include "../core/I18n.h"
#include "../core/Paths.h"
#include <QCoreApplication>
#include <QCursor>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QProcess>
#include <QSettings>
#include <QThread>
#include <QUrl>
#include <atomic>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#else
#include <unistd.h>
#include <fcntl.h>
#ifdef VMACRO_HAS_XTEST
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#endif
#endif

namespace vmacro {

struct KeyTableEntry {
    int scan;
    bool ext;
    const char* label;
    int hid;
    int vk;
};

static const KeyTableEntry kKeyTable[] = {
    {0x01, false, "Esc", 0x29, 0x1B},
    {0x02, false, "1", 0x1E, '1'}, {0x03, false, "2", 0x1F, '2'}, {0x04, false, "3", 0x20, '3'},
    {0x05, false, "4", 0x21, '4'}, {0x06, false, "5", 0x22, '5'}, {0x07, false, "6", 0x23, '6'},
    {0x08, false, "7", 0x24, '7'}, {0x09, false, "8", 0x25, '8'}, {0x0A, false, "9", 0x26, '9'},
    {0x0B, false, "0", 0x27, '0'}, {0x0C, false, "-", 0x2D, 0xBD}, {0x0D, false, "=", 0x2E, 0xBB},
    {0x0E, false, "Backspace", 0x2A, 0x08}, {0x0F, false, "Tab", 0x2B, 0x09},
    {0x10, false, "Q", 0x14, 'Q'}, {0x11, false, "W", 0x1A, 'W'}, {0x12, false, "E", 0x08, 'E'},
    {0x13, false, "R", 0x15, 'R'}, {0x14, false, "T", 0x17, 'T'}, {0x15, false, "Y", 0x1C, 'Y'},
    {0x16, false, "U", 0x18, 'U'}, {0x17, false, "I", 0x0C, 'I'}, {0x18, false, "O", 0x12, 'O'},
    {0x19, false, "P", 0x13, 'P'}, {0x1A, false, "[", 0x2F, 0xDB}, {0x1B, false, "]", 0x30, 0xDD},
    {0x1C, false, "Enter", 0x28, 0x0D}, {0x1C, true, "NumEnter", 0x58, 0x0D},
    {0x1D, false, "Ctrl", 0xE0, 0xA2}, {0x1D, true, "RCtrl", 0xE4, 0xA3},
    {0x1E, false, "A", 0x04, 'A'}, {0x1F, false, "S", 0x16, 'S'}, {0x20, false, "D", 0x07, 'D'},
    {0x21, false, "F", 0x09, 'F'}, {0x22, false, "G", 0x0A, 'G'}, {0x23, false, "H", 0x0B, 'H'},
    {0x24, false, "J", 0x0D, 'J'}, {0x25, false, "K", 0x0E, 'K'}, {0x26, false, "L", 0x0F, 'L'},
    {0x27, false, ";", 0x33, 0xBA}, {0x28, false, "'", 0x34, 0xDE}, {0x29, false, "`", 0x35, 0xC0},
    {0x2A, false, "Shift", 0xE1, 0xA0}, {0x2B, false, "\\", 0x31, 0xDC},
    {0x2C, false, "Z", 0x1D, 'Z'}, {0x2D, false, "X", 0x1B, 'X'}, {0x2E, false, "C", 0x06, 'C'},
    {0x2F, false, "V", 0x19, 'V'}, {0x30, false, "B", 0x05, 'B'}, {0x31, false, "N", 0x11, 'N'},
    {0x32, false, "M", 0x10, 'M'}, {0x33, false, ",", 0x36, 0xBC}, {0x34, false, ".", 0x37, 0xBE},
    {0x35, false, "/", 0x38, 0xBF}, {0x35, true, "Num/", 0x54, 0x6F},
    {0x36, false, "RShift", 0xE5, 0xA1}, {0x37, false, "Num*", 0x55, 0x6A},
    {0x38, false, "Alt", 0xE2, 0xA4}, {0x38, true, "RAlt", 0xE6, 0xA5},
    {0x39, false, "Space", 0x2C, 0x20}, {0x3A, false, "CapsLock", 0x39, 0x14},
    {0x3B, false, "F1", 0x3A, 0x70}, {0x3C, false, "F2", 0x3B, 0x71}, {0x3D, false, "F3", 0x3C, 0x72},
    {0x3E, false, "F4", 0x3D, 0x73}, {0x3F, false, "F5", 0x3E, 0x74}, {0x40, false, "F6", 0x3F, 0x75},
    {0x41, false, "F7", 0x40, 0x76}, {0x42, false, "F8", 0x41, 0x77}, {0x43, false, "F9", 0x42, 0x78},
    {0x44, false, "F10", 0x43, 0x79}, {0x57, false, "F11", 0x44, 0x7A}, {0x58, false, "F12", 0x45, 0x7B},
    {0x45, false, "NumLock", 0x53, 0x90}, {0x46, false, "ScrollLock", 0x47, 0x91},
    {0x47, false, "Num7", 0x5F, 0x67}, {0x48, false, "Num8", 0x60, 0x68}, {0x49, false, "Num9", 0x61, 0x69},
    {0x4A, false, "Num-", 0x56, 0x6D}, {0x4B, false, "Num4", 0x5C, 0x64}, {0x4C, false, "Num5", 0x5D, 0x65},
    {0x4D, false, "Num6", 0x5E, 0x66}, {0x4E, false, "Num+", 0x57, 0x6B}, {0x4F, false, "Num1", 0x59, 0x61},
    {0x50, false, "Num2", 0x5A, 0x62}, {0x51, false, "Num3", 0x5B, 0x63}, {0x52, false, "Num0", 0x62, 0x60},
    {0x53, false, "Num.", 0x63, 0x6E},
    {0x47, true, "Home", 0x4A, 0x24}, {0x48, true, "Up", 0x52, 0x26}, {0x49, true, "PageUp", 0x4B, 0x21},
    {0x4B, true, "Left", 0x50, 0x25}, {0x4D, true, "Right", 0x4F, 0x27}, {0x4F, true, "End", 0x4D, 0x23},
    {0x50, true, "Down", 0x51, 0x28}, {0x51, true, "PageDown", 0x4E, 0x22}, {0x52, true, "Insert", 0x49, 0x2D},
    {0x53, true, "Delete", 0x4C, 0x2E}, {0x5B, true, "LWin", 0xE3, 0x5B}, {0x5C, true, "RWin", 0xE7, 0x5C},
    {0x5D, true, "Apps", 0x65, 0x5D},
};

quint32 Win32Input::ownPid() {
    return static_cast<quint32>(QCoreApplication::applicationPid());
}

bool Win32Input::isHwndOwnProcess(quintptr hwnd) {
#ifdef _WIN32
    if (!hwnd) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(reinterpret_cast<HWND>(hwnd), &pid);
    return pid != 0 && pid == ownPid();
#else
    Q_UNUSED(hwnd);
    return false;
#endif
}

bool Win32Input::isOwnAppForeground() {
#ifdef _WIN32
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    return isHwndOwnProcess(reinterpret_cast<quintptr>(fg));
#else
    return false;
#endif
}

QString Win32Input::getForegroundProcessName() {
#ifdef _WIN32
    HWND fg = GetForegroundWindow();
    if (!fg) return {};
    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    if (!pid) return {};
    HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hp) return {};
    wchar_t buf[512] = {0};
    DWORD sz = 511;
    QString out;
    if (QueryFullProcessImageNameW(hp, 0, buf, &sz)) {
        out = QFileInfo(QString::fromWCharArray(buf)).fileName().toLower();
    }
    CloseHandle(hp);
    return out;
#else
    return {};
#endif
}

QVector<WindowAppInfo> Win32Input::listDetailedWindowsForPicker() {
    QVector<WindowAppInfo> res;
#ifdef _WIN32
    struct EnumCtx { QVector<WindowAppInfo>* list; quint32 myPid; };
    EnumCtx ctx{&res, ownPid()};
    EnumWindows([](HWND hwnd, LPARAM lp) -> BOOL {
        if (!IsWindowVisible(hwnd)) return TRUE;
        int len = GetWindowTextLengthW(hwnd);
        if (len <= 0) return TRUE;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        auto* c = reinterpret_cast<EnumCtx*>(lp);
        if (!pid || pid == c->myPid) return TRUE;
        wchar_t tbuf[512] = {0};
        GetWindowTextW(hwnd, tbuf, 511);
        QString title = QString::fromWCharArray(tbuf).trimmed();
        if (title.isEmpty()) return TRUE;
        QString exePath, procName = QString("PID-%1").arg(pid);
        HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hp) {
            wchar_t pbuf[1024] = {0};
            DWORD sz = 1023;
            if (QueryFullProcessImageNameW(hp, 0, pbuf, &sz)) {
                exePath = QString::fromWCharArray(pbuf);
                procName = QFileInfo(exePath).fileName();
            }
            CloseHandle(hp);
        }
        c->list->append({reinterpret_cast<quintptr>(hwnd), static_cast<quint32>(pid), procName, title, exePath});
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));
#endif
    return res;
}

QVector<WindowAppInfo> Win32Input::listRunningApps() {
    auto all = listDetailedWindowsForPicker();
    QVector<WindowAppInfo> uniq;
    QSet<QString> seen;
    for (const auto& w : all) {
        QString low = w.procName.toLower();
        if (low == "explorer.exe" || low == "textinputhost.exe" || low == "v-macroplus.exe") continue;
        if (!seen.contains(low)) {
            seen.insert(low);
            uniq.append(w);
        }
    }
    return uniq;
}

bool Win32Input::focusAndBringHwndToFront(quintptr hwnd) {
#ifdef _WIN32
    if (!hwnd) return true;
    HWND h = reinterpret_cast<HWND>(hwnd);
    if (!IsWindow(h) || isHwndOwnProcess(hwnd)) return false;
    if (IsIconic(h)) ShowWindow(h, SW_RESTORE);
    DWORD curTid = GetCurrentThreadId();
    HWND fg = GetForegroundWindow();
    DWORD fgTid = (fg && !isHwndOwnProcess(reinterpret_cast<quintptr>(fg))) ? GetWindowThreadProcessId(fg, nullptr) : 0;
    DWORD tgTid = GetWindowThreadProcessId(h, nullptr);
    bool aFg = (fgTid && fgTid != curTid) ? AttachThreadInput(curTid, fgTid, TRUE) : false;
    bool aTg = (tgTid && tgTid != curTid && tgTid != fgTid) ? AttachThreadInput(curTid, tgTid, TRUE) : false;
    BringWindowToTop(h);
    SetForegroundWindow(h);
    SetActiveWindow(h);
    if (aTg) AttachThreadInput(curTid, tgTid, FALSE);
    if (aFg) AttachThreadInput(curTid, fgTid, FALSE);
    QThread::msleep(60);
    return true;
#else
    Q_UNUSED(hwnd);
    return true;
#endif
}

bool Win32Input::isHwndOrChildForeground(quintptr hwnd) {
#ifdef _WIN32
    if (!hwnd) return true;
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    HWND tg = reinterpret_cast<HWND>(hwnd);
    if (fg == tg || IsChild(tg, fg)) return true;
    DWORD p1 = 0, p2 = 0;
    GetWindowThreadProcessId(fg, &p1);
    GetWindowThreadProcessId(tg, &p2);
    return p1 != 0 && p1 == p2;
#else
    Q_UNUSED(hwnd);
    return true;
#endif
}

QPair<int, bool> Win32Input::scancodeForLabel(const QString& label) {
    QString u = label.trimmed().toUpper();
    if (u == "ESCAPE") return {0x01, false};
    if (u == "RETURN") return {0x1C, false};
    if (u == "NUMDIVIDE") return {0x35, true};
    if (u == "NUMMULTIPLY") return {0x37, false};
    for (const auto& e : kKeyTable) {
        if (QString::fromLatin1(e.label).toUpper() == u) return {e.scan, e.ext};
    }
    return {0x39, false};
}

QString Win32Input::labelForScancode(int scanCode, bool isExtended) {
    for (const auto& e : kKeyTable) {
        if (e.scan == scanCode && e.ext == isExtended) return QString::fromLatin1(e.label);
    }
    for (const auto& e : kKeyTable) {
        if (e.scan == scanCode) return QString::fromLatin1(e.label);
    }
    return QString("0x%1").arg(scanCode, 2, 16, QChar('0')).toUpper();
}

int Win32Input::vkForScancode(int scanCode, bool isExtended) {
    for (const auto& e : kKeyTable) {
        if (e.scan == scanCode && e.ext == isExtended) return e.vk;
    }
    for (const auto& e : kKeyTable) {
        if (e.scan == scanCode) return e.vk;
    }
    return 0;
}

int Win32Input::usbHidUsageForScancode(int scanCode, bool isExtended) {
    for (const auto& e : kKeyTable) {
        if (e.scan == scanCode && e.ext == isExtended) return e.hid;
    }
    for (const auto& e : kKeyTable) {
        if (e.scan == scanCode) return e.hid;
    }
    return 0;
}

int Win32Input::usbHidModifierBit(int scanCode, bool isExtended) {
    if (scanCode == 0x1D && !isExtended) return 0x01;
    if (scanCode == 0x2A && !isExtended) return 0x02;
    if (scanCode == 0x38 && !isExtended) return 0x04;
    if (scanCode == 0x5B) return 0x08;
    if (scanCode == 0x1D && isExtended) return 0x10;
    if (scanCode == 0x36) return 0x20;
    if (scanCode == 0x38 && isExtended) return 0x40;
    if (scanCode == 0x5C) return 0x80;
    return 0;
}

bool Win32Input::isModifierScancode(int scanCode) {
    return scanCode == 0x1D || scanCode == 0x2A || scanCode == 0x36 || scanCode == 0x38 || scanCode == 0x5B || scanCode == 0x5C;
}

QList<QPair<int, bool>> Win32Input::parseChordString(const QString& chordStr) {
    QList<QPair<int, bool>> res;
    for (const QString& part : chordStr.split('+', Qt::SkipEmptyParts)) {
        auto pair = scancodeForLabel(part.trimmed());
        if (pair.first > 0 && !res.contains(pair)) res.append(pair);
    }
    return res;
}

// ===========================================================================
// Interception & PS/2 Port 0x60 Dynamic DLL Bridges (Windows)
// ===========================================================================
#ifdef _WIN32
static HMODULE findAndLoadDriverDll(const QStringList& names, QString& outPath) {
    for (const QString& dir : Paths::driverSearchDirs()) {
        for (const QString& n : names) {
            QString fp = dir + "/" + n;
            if (QFile::exists(fp)) {
                HMODULE h = LoadLibraryW(reinterpret_cast<const wchar_t*>(fp.utf16()));
                if (h) { outPath = fp; return h; }
            }
        }
    }
    return nullptr;
}
#endif

EffectiveEngineInfo Win32Input::resolveEffectiveInputEngine(const QString& method) {
    EffectiveEngineInfo info;
    info.requestedMethod = method;
    if (method == "pico_usb_hid") {
        if (MicrocontrollerBridge::instance().isBoardReady("pico")) {
            info.effectiveMethod = method;
            info.reason = trKey("eng_status_pico_active") + " [" + MicrocontrollerBridge::instance().activePort() + "]";
        } else {
            info.effectiveMethod = "sendinput_scancode";
            info.isFallback = true;
            info.statusKind = "warn";
            info.reason = trKey("eng_status_pico_fallback");
        }
        return info;
    }
    if (method == "arduino_usb_hid") {
        if (MicrocontrollerBridge::instance().isBoardReady("arduino")) {
            info.effectiveMethod = method;
            info.reason = trKey("eng_status_arduino_active") + " [" + MicrocontrollerBridge::instance().activePort() + "]";
        } else {
            info.effectiveMethod = "sendinput_scancode";
            info.isFallback = true;
            info.statusKind = "warn";
            info.reason = trKey("eng_status_arduino_fallback");
        }
        return info;
    }
#ifdef _WIN32
    if (method == "linux_uinput_xtest") {
        info.effectiveMethod = "sendinput_scancode";
        info.isFallback = true;
        info.statusKind = "warn";
        info.reason = trKey("eng_status_linux_win_fallback");
        return info;
    }
    if (method == "cpp_interception_driver") {
        auto d = diagnoseKernelDrivers(false);
        if (d.contextCreated && d.kbDeviceId > 0) {
            info.effectiveMethod = method;
            info.reason = trKey("eng_status_interception_active");
        } else {
            info.effectiveMethod = "sendinput_scancode";
            info.isFallback = true;
            info.statusKind = "warn";
            info.reason = trKey("eng_status_interception_fallback");
        }
        return info;
    }
    if (method == "cpp_winring0_ps2_port") {
        auto d = diagnosePs2PortDriver(false);
        if (d.driverOpen) {
            info.effectiveMethod = method;
            info.reason = trKey("eng_status_ps2_active");
        } else {
            info.effectiveMethod = "sendinput_scancode";
            info.isFallback = true;
            info.statusKind = "warn";
            info.reason = trKey("eng_status_ps2_fallback");
        }
        return info;
    }
#endif
    info.effectiveMethod = method;
    info.reason = trKey("eng_status_native_active") + " (" + method + ")";
    return info;
}

bool Win32Input::sendScancodeKey(int scanCode, bool isExtended, bool keyUp, const QString& method, bool strictNoFallback) {
    if (scanCode <= 0) return false;
    if (method == "pico_usb_hid" || method == "arduino_usb_hid") {
        QString bTarget = (method == "pico_usb_hid") ? "pico" : "arduino";
        int hid = usbHidUsageForScancode(scanCode, isExtended);
        int mod = usbHidModifierBit(scanCode, isExtended);
        if ((hid > 0 || mod > 0) && MicrocontrollerBridge::instance().sendKeyEvent(hid, !keyUp, mod, bTarget)) {
            return true;
        }
        if (strictNoFallback) return false;
    }
#ifdef _WIN32
    if (method == "linux_uinput_xtest" && strictNoFallback) return false;
    int vk = vkForScancode(scanCode, isExtended);
    if (method == "keybd_mouse_event") {
        DWORD flg = (isExtended ? KEYEVENTF_EXTENDEDKEY : 0) | (keyUp ? KEYEVENTF_KEYUP : 0);
        keybd_event(static_cast<BYTE>(vk), static_cast<BYTE>(scanCode), flg, 0);
        return true;
    }
    INPUT inp{};
    inp.type = INPUT_KEYBOARD;
    inp.ki.wVk = (method == "sendinput_hybrid" || method == "cpp_vhid_ioctl_bridge") ? static_cast<WORD>(vk) : 0;
    inp.ki.wScan = static_cast<WORD>(scanCode);
    inp.ki.dwFlags = KEYEVENTF_SCANCODE | (isExtended ? KEYEVENTF_EXTENDEDKEY : 0) | (keyUp ? KEYEVENTF_KEYUP : 0);
    inp.ki.time = 0;
    inp.ki.dwExtraInfo = 0;
    return SendInput(1, &inp, sizeof(INPUT)) > 0;
#else
    Q_UNUSED(strictNoFallback);
#ifdef VMACRO_HAS_XTEST
    Display* dpy = XOpenDisplay(nullptr);
    if (dpy) {
        QString lbl = labelForScancode(scanCode, isExtended);
        KeySym ks = XStringToKeysym(lbl.toLatin1().constData());
        if (ks == NoSymbol && lbl == "Enter") ks = XK_Return;
        if (ks == NoSymbol && lbl == "Space") ks = XK_space;
        if (ks != NoSymbol) {
            KeyCode kc = XKeysymToKeycode(dpy, ks);
            if (kc) XTestFakeKeyEvent(dpy, kc, !keyUp, CurrentTime);
        }
        XFlush(dpy);
        XCloseDisplay(dpy);
        return true;
    }
#endif
    return true;
#endif
}

bool Win32Input::sendUnicodeText(const QString& text) {
#ifdef _WIN32
    if (text.isEmpty()) return false;
    for (QChar ch : text) {
        ushort code = ch.unicode();
        INPUT inps[2]{};
        inps[0].type = INPUT_KEYBOARD;
        inps[0].ki.wScan = code;
        inps[0].ki.dwFlags = KEYEVENTF_UNICODE;
        inps[1].type = INPUT_KEYBOARD;
        inps[1].ki.wScan = code;
        inps[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        SendInput(2, inps, sizeof(INPUT));
    }
    return true;
#else
    Q_UNUSED(text);
    return true;
#endif
}

QPoint Win32Input::getCursorPos() {
#ifdef _WIN32
    POINT pt{};
    if (GetCursorPos(&pt)) return QPoint(pt.x, pt.y);
#endif
    return QCursor::pos();
}

void Win32Input::sendMouseButton(int x, int y, const QString& button, const QString& action, bool moveCursor, int holdMs, const QString& method) {
    if (method == "pico_usb_hid" || method == "arduino_usb_hid") {
        QString bTarget = (method == "pico_usb_hid") ? "pico" : "arduino";
#ifdef _WIN32
        if (moveCursor) SetCursorPos(x, y);
#endif
        if (MicrocontrollerBridge::instance().sendMouseAction(button, action, holdMs, bTarget)) return;
    }
#ifdef _WIN32
    if (moveCursor) SetCursorPos(x, y);
    QString b = button.toLower();
    DWORD dn = MOUSEEVENTF_LEFTDOWN, up = MOUSEEVENTF_LEFTUP, mData = 0;
    if (b == "right") { dn = MOUSEEVENTF_RIGHTDOWN; up = MOUSEEVENTF_RIGHTUP; }
    else if (b == "middle") { dn = MOUSEEVENTF_MIDDLEDOWN; up = MOUSEEVENTF_MIDDLEUP; }
    else if (b == "x1" || b == "back") { dn = MOUSEEVENTF_XDOWN; up = MOUSEEVENTF_XUP; mData = XBUTTON1; }
    else if (b == "x2" || b == "forward") { dn = MOUSEEVENTF_XDOWN; up = MOUSEEVENTF_XUP; mData = XBUTTON2; }

    auto emitFlg = [&](DWORD flg) {
        INPUT inp{};
        inp.type = INPUT_MOUSE;
        inp.mi.dwFlags = flg;
        inp.mi.mouseData = mData;
        SendInput(1, &inp, sizeof(INPUT));
    };
    if (action == "down") emitFlg(dn);
    else if (action == "up") emitFlg(up);
    else {
        emitFlg(dn);
        if (holdMs > 0) QThread::msleep(static_cast<unsigned long>(qMax(5, holdMs)));
        emitFlg(up);
    }
#else
    Q_UNUSED(x); Q_UNUSED(y); Q_UNUSED(button); Q_UNUSED(action); Q_UNUSED(moveCursor); Q_UNUSED(holdMs);
#endif
}

bool Win32Input::sendMouseRelativeMove(int dx, int dy, int steps, int durationMs, const QString& method) {
    int n = qBound(1, steps, 120);
    int stepMs = (durationMs > 0) ? qMax(1, durationMs / n) : 2;
    int rx = dx, ry = dy;
    for (int i = 0; i < n; ++i) {
        int left = n - i;
        int sx = qRound(double(rx) / left);
        int sy = qRound(double(ry) / left);
        rx -= sx; ry -= sy;
        if (method == "pico_usb_hid" || method == "arduino_usb_hid") {
            QString bTarget = (method == "pico_usb_hid") ? "pico" : "arduino";
            if (MicrocontrollerBridge::instance().sendMouseMoveRel(sx, sy, bTarget)) {
                if (i + 1 < n) QThread::msleep(static_cast<unsigned long>(stepMs));
                continue;
            }
        }
#ifdef _WIN32
        INPUT inp{};
        inp.type = INPUT_MOUSE;
        inp.mi.dx = sx;
        inp.mi.dy = sy;
        inp.mi.dwFlags = MOUSEEVENTF_MOVE;
        SendInput(1, &inp, sizeof(INPUT));
#endif
        if (i + 1 < n) QThread::msleep(static_cast<unsigned long>(stepMs));
    }
    return true;
}

void Win32Input::sendMouseScroll(const QString& direction, int amount, int dx, int dy) {
#ifdef _WIN32
    int steps = (dx == 0 && dy == 0) ? qMax(1, qAbs(amount)) : 1;
    QString d = direction.trimmed().toLower();
    DWORD flg = (d == "left" || d == "right") ? MOUSEEVENTF_HWHEEL : MOUSEEVENTF_WHEEL;
    int delta = (d == "up" || d == "right") ? (120 * steps) : (-120 * steps);
    INPUT inp{};
    inp.type = INPUT_MOUSE;
    inp.mi.dwFlags = flg;
    inp.mi.mouseData = static_cast<DWORD>(delta);
    SendInput(1, &inp, sizeof(INPUT));
#else
    Q_UNUSED(direction); Q_UNUSED(amount); Q_UNUSED(dx); Q_UNUSED(dy);
#endif
}

void Win32Input::openFolderOrPath(const QString& path) {
    QString clean = path.trimmed();
    if (clean.startsWith('"') && clean.endsWith('"')) clean = clean.mid(1, clean.size() - 2);
    if (clean.isEmpty()) return;
#ifdef _WIN32
    QFileInfo fi(clean);
    if (fi.exists() && fi.isFile()) {
        ShellExecuteW(nullptr, L"open", reinterpret_cast<const wchar_t*>(clean.utf16()),
                      nullptr, reinterpret_cast<const wchar_t*>(fi.absolutePath().utf16()), SW_SHOWNORMAL);
        return;
    }
#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(clean));
}

void Win32Input::runShellCommand(const QString& shellType, const QString& cmdText) {
    QString clean = cmdText.trimmed();
    if (clean.isEmpty()) return;
    if (shellType == "powershell") {
        QProcess::startDetached("powershell", {"-NoProfile", "-ExecutionPolicy", "Bypass", "-Command", clean});
    } else {
#ifdef _WIN32
        QProcess::startDetached("cmd.exe", {"/c", clean});
#else
        QProcess::startDetached("sh", {"-c", clean});
#endif
    }
}

bool Win32Input::isRunningAsAdmin() {
#ifdef _WIN32
    BOOL admin = FALSE;
    PSID sid = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuth = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0,0,0,0,0,0, &sid)) {
        CheckTokenMembership(nullptr, sid, &admin);
        FreeSid(sid);
    }
    return admin == TRUE;
#else
    return geteuid() == 0;
#endif
}

bool Win32Input::relaunchAsAdmin() {
#ifdef _WIN32
    if (isRunningAsAdmin()) return false;
    QString exe = QCoreApplication::applicationFilePath();
    HINSTANCE r = ShellExecuteW(nullptr, L"runas", reinterpret_cast<const wchar_t*>(exe.utf16()), L"--relaunch-admin", nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<intptr_t>(r) > 32) {
        QCoreApplication::quit();
        return true;
    }
#endif
    return false;
}

bool Win32Input::isWindowsStartupEnabled() {
#ifdef _WIN32
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    return reg.contains("V-MacroPlus");
#else
    return false;
#endif
}

bool Win32Input::setWindowsStartupEnabled(bool enable) {
#ifdef _WIN32
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (enable) reg.setValue("V-MacroPlus", "\"" + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + "\"");
    else reg.remove("V-MacroPlus");
    return true;
#else
    Q_UNUSED(enable);
    return false;
#endif
}

KernelDriverDiagnostics Win32Input::diagnoseKernelDrivers(bool runLiveProbe) {
    Q_UNUSED(runLiveProbe);
    KernelDriverDiagnostics d;
    d.isAdmin = isRunningAsAdmin();
    d.driversDir = Paths::driversDir();
#ifdef _WIN32
    QString dllP;
    HMODULE h = findAndLoadDriverDll({"interception.dll"}, dllP);
    if (h) {
        d.dllFound = true;
        d.dllPath = dllP;
        using CreateCtxFn = void* (*)();
        using DestroyCtxFn = void (*)(void*);
        auto fnCreate = reinterpret_cast<CreateCtxFn>(GetProcAddress(h, "interception_create_context"));
        auto fnDestroy = reinterpret_cast<DestroyCtxFn>(GetProcAddress(h, "interception_destroy_context"));
        if (fnCreate) {
            void* ctx = fnCreate();
            if (ctx) {
                d.contextCreated = true;
                d.kbDeviceId = 1;
                d.stateCode = "ring0_active";
                if (fnDestroy) fnDestroy(ctx);
            } else {
                d.stateCode = "dll_only_no_driver";
            }
        }
    }
#endif
    return d;
}

void Win32Input::formatKernelDiagnosticReport(const KernelDriverDiagnostics& diag, bool isArabic, QString& outHeadline, QString& outDetails, QString& outKind) {
    if (diag.stateCode == "ring0_active") {
        outHeadline = isArabic ? "🟢 درايفر الكيرنل Ring-0 Interception نشط ومتصل فعلياً!" : "🟢 Ring-0 Interception Kernel Driver ACTIVE!";
        outKind = "ok";
    } else if (diag.dllFound) {
        outHeadline = isArabic ? "🟡 ملف interception.dll موجود — شغّل install-interception.exe كمسؤول لتفعيل الكيرنل" : "🟡 interception.dll found — Run install-interception.exe as Admin to activate kernel driver";
        outKind = "warn";
    } else {
        outHeadline = isArabic ? "🔵 الطريقة 7 تعمل حالياً على البديل الآمن (SendInput ScanCode) — اربط interception.dll بالأسفل لتفعيل Ring-0" : "🔵 Method 7 uses safe SendInput ScanCode fallback — Link interception.dll below for Ring-0";
        outKind = "info";
    }
    outDetails = (isArabic ? "• مجلد الدرايفرات: " : "• Drivers Directory: ") + diag.driversDir;
}

bool Win32Input::installOrImportInterceptionFile(const QString& filePath, QString& outMsg) {
    if (filePath.isEmpty() || !QFile::exists(filePath)) {
        outMsg = trDual("لم يتم العثور على الملف المحدد", "Selected file not found");
        return false;
    }
    QFileInfo fi(filePath);
    QString dst = Paths::driversDir() + "/" + fi.fileName();
    QFile::remove(dst);
    QFile::copy(filePath, dst);
    outMsg = trDual("✓ تم نسخ الملف إلى مجلد drivers بنجاح!", "✓ Copied driver file to drivers/ successfully!");
    return true;
}

Ps2Diagnostics Win32Input::diagnosePs2PortDriver(bool runLiveProbe) {
    Q_UNUSED(runLiveProbe);
    Ps2Diagnostics d;
    d.isAdmin = isRunningAsAdmin();
    d.driversDir = Paths::driversDir();
#ifdef _WIN32
    QString dllP;
    HMODULE h = findAndLoadDriverDll({"inpoutx64.dll", "InpOutx64.dll"}, dllP);
    if (h) {
        d.dllFound = true;
        d.dllPath = dllP;
        using IsOpenFn = BOOL (__stdcall *)();
        auto fnOpen = reinterpret_cast<IsOpenFn>(GetProcAddress(h, "IsInpOutDriverOpen"));
        if (fnOpen && fnOpen()) {
            d.driverOpen = true;
            d.stateCode = "ps2_active";
        } else {
            d.stateCode = "dll_only_driver_closed";
        }
    }
#endif
    return d;
}

void Win32Input::formatPs2DiagnosticReport(const Ps2Diagnostics& diag, bool isArabic, QString& outHeadline, QString& outDetails, QString& outKind) {
    if (diag.driverOpen) {
        outHeadline = isArabic ? "🟢 بورت البوردة PS/2 Port 0x60/0x64 (InpOutx64) نشط ومتصل!" : "🟢 Motherboard PS/2 Port 0x60/0x64 (InpOutx64) ACTIVE!";
        outKind = "ok";
    } else if (diag.dllFound) {
        outHeadline = isArabic ? "🟡 ملف inpoutx64.dll موجود — شغّل البرنامج كمسؤول مرة واحدة لتفعيله" : "🟡 inpoutx64.dll loaded — Run as Admin once to initialize";
        outKind = "warn";
    } else {
        outHeadline = isArabic ? "🔵 الطريقة 8 تعمل حالياً على البديل الآمن — اربط inpoutx64.dll بالأسفل" : "🔵 Method 8 uses safe SendInput fallback — Link inpoutx64.dll below";
        outKind = "info";
    }
    outDetails = (isArabic ? "• مجلد الدرايفرات: " : "• Drivers Directory: ") + diag.driversDir;
}

bool Win32Input::installOrImportPs2File(const QString& filePath, QString& outMsg) {
    return installOrImportInterceptionFile(filePath, outMsg);
}

// ===========================================================================
// Global Low-Level Keyboard & Mouse Hooks (`WH_KEYBOARD_LL` / `WH_MOUSE_LL`)
// ===========================================================================
static Win32Input::KeyHookCb g_keyCb;
static Win32Input::MouseHookCb g_mouseCb;
static std::atomic<bool> g_hooksRunning{false};

#ifdef _WIN32
static HHOOK g_kbHook = nullptr;
static HHOOK g_msHook = nullptr;
static DWORD g_hookThreadId = 0;

static LRESULT CALLBACK llKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_keyCb) {
        auto* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isExt = (kb->flags & LLKHF_EXTENDED) != 0;
        bool isInj = (kb->flags & LLKHF_INJECTED) != 0;
        if (g_keyCb(static_cast<int>(kb->scanCode), isExt, isDown, isInj)) {
            return 1;
        }
    }
    return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
}

static LRESULT CALLBACK llMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_mouseCb) {
        auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        if ((ms->flags & LLMHF_INJECTED) == 0) {
            int x = ms->pt.x, y = ms->pt.y;
            if (wParam == WM_MOUSEMOVE) g_mouseCb("mouse_move", x, y, "move", 0, 0);
            else if (wParam == WM_LBUTTONDOWN) g_mouseCb("mouse_down", x, y, "left", 0, 0);
            else if (wParam == WM_LBUTTONUP) g_mouseCb("mouse_up", x, y, "left", 0, 0);
            else if (wParam == WM_RBUTTONDOWN) g_mouseCb("mouse_down", x, y, "right", 0, 0);
            else if (wParam == WM_RBUTTONUP) g_mouseCb("mouse_up", x, y, "right", 0, 0);
            else if (wParam == WM_MBUTTONDOWN) g_mouseCb("mouse_down", x, y, "middle", 0, 0);
            else if (wParam == WM_MBUTTONUP) g_mouseCb("mouse_up", x, y, "middle", 0, 0);
            else if (wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP) {
                QString b = (HIWORD(ms->mouseData) == XBUTTON1) ? "x1" : "x2";
                g_mouseCb(wParam == WM_XBUTTONDOWN ? "mouse_down" : "mouse_up", x, y, b, 0, 0);
            } else if (wParam == WM_MOUSEWHEEL) {
                short d = static_cast<short>(HIWORD(ms->mouseData)) / 120;
                g_mouseCb("mouse_scroll", x, y, d >= 0 ? "up" : "down", 0, d);
            } else if (wParam == WM_MOUSEHWHEEL) {
                short d = static_cast<short>(HIWORD(ms->mouseData)) / 120;
                g_mouseCb("mouse_scroll", x, y, d >= 0 ? "right" : "left", d, 0);
            }
        }
    }
    return CallNextHookEx(g_msHook, nCode, wParam, lParam);
}
#endif

void Win32Input::startGlobalHooks(KeyHookCb onKey, MouseHookCb onMouse) {
    if (g_hooksRunning.exchange(true)) return;
    g_keyCb = std::move(onKey);
    g_mouseCb = std::move(onMouse);
#ifdef _WIN32
    std::thread([]() {
        g_hookThreadId = GetCurrentThreadId();
        g_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, llKeyboardProc, GetModuleHandleW(nullptr), 0);
        g_msHook = SetWindowsHookExW(WH_MOUSE_LL, llMouseProc, GetModuleHandleW(nullptr), 0);
        MSG msg{};
        while (g_hooksRunning.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (g_kbHook) { UnhookWindowsHookEx(g_kbHook); g_kbHook = nullptr; }
        if (g_msHook) { UnhookWindowsHookEx(g_msHook); g_msHook = nullptr; }
    }).detach();
#endif
}

void Win32Input::stopGlobalHooks() {
    g_hooksRunning.store(false);
#ifdef _WIN32
    if (g_hookThreadId) PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
#endif
}

} // namespace vmacro
