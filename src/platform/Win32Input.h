#pragma once
#include <QList>
#include <QPair>
#include <QPoint>
#include <QString>
#include <QVector>
#include <functional>

namespace vmacro {

struct WindowAppInfo {
    quintptr hwnd = 0;
    quint32 pid = 0;
    QString procName;
    QString title;
    QString exePath;
};

struct EffectiveEngineInfo {
    QString requestedMethod;
    QString effectiveMethod;
    bool isFallback = false;
    QString statusKind = "ok";
    QString reason;
};

struct KernelDriverDiagnostics {
    QString stateCode = "fallback";
    bool isAdmin = false;
    bool dllFound = false;
    QString dllPath;
    bool contextCreated = false;
    int kbDeviceId = 0;
    int mouseDeviceId = 0;
    bool driverInstalled = false;
    QString serviceState;
    QString driversDir;
};

struct Ps2Diagnostics {
    QString stateCode = "fallback";
    bool isAdmin = false;
    bool dllFound = false;
    QString dllPath;
    bool driverOpen = false;
    int port64Status = -1;
    QString driversDir;
};

class Win32Input {
public:
    static quint32 ownPid();
    static bool isHwndOwnProcess(quintptr hwnd);
    static bool isOwnAppForeground();
    static QString getForegroundProcessName();
    static QVector<WindowAppInfo> listDetailedWindowsForPicker();
    static QVector<WindowAppInfo> listRunningApps();
    static bool focusAndBringHwndToFront(quintptr hwnd);
    static bool isHwndOrChildForeground(quintptr hwnd);

    static QPair<int, bool> scancodeForLabel(const QString& label);
    static QString labelForScancode(int scanCode, bool isExtended);
    static int vkForScancode(int scanCode, bool isExtended);
    static int usbHidUsageForScancode(int scanCode, bool isExtended);
    static int usbHidModifierBit(int scanCode, bool isExtended);
    static bool isModifierScancode(int scanCode);
    static QList<QPair<int, bool>> parseChordString(const QString& chordStr);

    static EffectiveEngineInfo resolveEffectiveInputEngine(const QString& method);
    static bool sendScancodeKey(int scanCode, bool isExtended, bool keyUp, const QString& method = "sendinput_scancode", bool strictNoFallback = false);
    static bool sendUnicodeText(const QString& text);
    static QPoint getCursorPos();
    static void sendMouseButton(int x, int y, const QString& button = "left", const QString& action = "click", bool moveCursor = true, int holdMs = 0, const QString& method = "sendinput_scancode");
    static bool sendMouseRelativeMove(int dx, int dy, int steps = 1, int durationMs = 0, const QString& method = "sendinput_scancode");
    static void sendMouseScroll(const QString& direction = "down", int amount = 3, int dx = 0, int dy = 0);

    static void openFolderOrPath(const QString& path);
    static void runShellCommand(const QString& shellType, const QString& cmdText);
    static bool isRunningAsAdmin();
    static bool relaunchAsAdmin();
    static bool isWindowsStartupEnabled();
    static bool setWindowsStartupEnabled(bool enable);

    static KernelDriverDiagnostics diagnoseKernelDrivers(bool runLiveProbe = false);
    static void formatKernelDiagnosticReport(const KernelDriverDiagnostics& diag, bool isArabic, QString& outHeadline, QString& outDetails, QString& outKind);
    static bool installOrImportInterceptionFile(const QString& filePath, QString& outMsg);

    static Ps2Diagnostics diagnosePs2PortDriver(bool runLiveProbe = false);
    static void formatPs2DiagnosticReport(const Ps2Diagnostics& diag, bool isArabic, QString& outHeadline, QString& outDetails, QString& outKind);
    static bool installOrImportPs2File(const QString& filePath, QString& outMsg);

    // Global Low-Level Hooks (Windows WH_KEYBOARD_LL / WH_MOUSE_LL; Linux X11/evdev poll)
    using KeyHookCb = std::function<bool(int scan, bool ext, bool isDown, bool isInjected)>;
    using MouseHookCb = std::function<void(const QString& evType, int x, int y, const QString& btnOrDir, int dx, int dy)>;
    static void startGlobalHooks(KeyHookCb onKey, MouseHookCb onMouse);
    static void stopGlobalHooks();
};

} // namespace vmacro
