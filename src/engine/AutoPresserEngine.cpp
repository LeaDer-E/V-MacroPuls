#include "AutoPresserEngine.h"
#include "MacroEngine.h"
#include "PythonScriptRunner.h"
#include "../core/AppState.h"
#include "../core/I18n.h"
#include "../core/Paths.h"
#include "../platform/MicrocontrollerBridge.h"
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QThread>

#ifdef _WIN32
#include <windows.h>
#endif

namespace vmacro {

const QList<MethodDef>& AutoPresserEngine::all22KeyMethods() {
    static const QList<MethodDef> list = {
        {"sendinput_scancode", "method_scancode", false},
        {"sendinput_hybrid", "method_hybrid", false},
        {"keybd_mouse_event", "method_keybd_event", false},
        {"unicode", "akp_method_unicode", false},
        {"cpp_stealth_humanized", "method_cpp_stealth", false},
        {"cpp_vhid_ioctl_bridge", "method_cpp_vhid", false},
        {"cpp_interception_driver", "method_cpp_interception", false},
        {"cpp_winring0_ps2_port", "method_cpp_ps2_port", false},
        {"cpp_sendinput_batch_atomic", "method_cpp_batch_atomic", false},
        {"linux_uinput_xtest", "method_linux_uinput", false},
        {"postmessage", "akp_method_postmessage", true},
        {"sendmessage", "akp_method_sendmessage", true},
        {"post_child", "akp_method_post_child", true},
        {"post_children", "akp_method_post_children", true},
        {"post_active", "akp_method_post_active", true},
        {"attach_state", "akp_method_attach_state", true},
        {"guithread_focus", "akp_method_guithread_focus", true},
        {"wm_char_triplet", "akp_method_wm_char_triplet", true},
        {"ime_pipeline", "akp_method_ime_pipeline", true},
        {"sendnotify_tree", "akp_method_sendnotify_tree", true},
        {"pico_usb_hid", "method_pico_hid", false},
        {"arduino_usb_hid", "method_arduino_hid", false},
    };
    return list;
}

const QList<QPair<QString, QString>>& AutoPresserEngine::all8MouseModes() {
    static const QList<QPair<QString, QString>> list = {
        {"auto", "akp_mouse_mode_auto"},
        {"real", "akp_mouse_mode_real"},
        {"post", "akp_mouse_mode_post"},
        {"post_top", "akp_mouse_mode_post_top"},
        {"post_hittest_chain", "akp_mouse_mode_hittest"},
        {"attach_mouse_state", "akp_mouse_mode_attach"},
        {"guithread_capture", "akp_mouse_mode_guithread"},
        {"sendinput_abs", "akp_mouse_mode_abs"},
    };
    return list;
}

QList<QPair<QString, QString>> AutoPresserEngine::localizedKeyMethodOptions() {
    QList<QPair<QString, QString>> out;
    for (const auto& m : all22KeyMethods()) out.append({m.id, trKey(m.trKey)});
    return out;
}

QList<QPair<QString, QString>> AutoPresserEngine::localizedMouseModeOptions() {
    QList<QPair<QString, QString>> out;
    for (const auto& m : all8MouseModes()) out.append({m.first, trKey(m.second)});
    return out;
}

QString AutoPresserEngine::formatDiagnosticReason(const QJsonObject& entry) {
    QString rc = entry.value("reason_code").toString();
    QString engine = entry.value("method_id").toString(entry.value("method").toString());
    QString child = entry.value("child_cls").toString("TopWindow");
    if (rc == "delivered_fg") return trKey("log_reason_delivered_fg").replace("{engine}", engine);
    if (rc == "delivered_bg") return trKey("log_reason_delivered_bg").replace("{engine}", engine).replace("{child}", child);
    if (rc == "skipped_fg_in_bg") return trKey("log_reason_skipped_fg_in_bg");
    if (rc == "interception_missing") return trKey("log_reason_interception_missing");
    if (rc == "ps2_missing") return trKey("log_reason_ps2_missing");
    if (rc == "linux_on_win") return trKey("log_reason_linux_on_win");
    if (rc == "pico_missing") return trKey("log_reason_pico_missing");
    if (rc == "arduino_missing") return trKey("log_reason_arduino_missing");
    if (rc == "self_app_protected") return trKey("log_reason_self_app_protected");
    if (rc == "hwnd_rejected") return trKey("log_reason_hwnd_rejected");
    return entry.value("reason").toString();
}

quintptr AutoPresserEngine::resolveTargetHwnd(AutoPresserTab& tab) {
#ifdef _WIN32
    if (tab.targetHwnd && IsWindow(reinterpret_cast<HWND>(tab.targetHwnd))) {
        if (Win32Input::isHwndOwnProcess(tab.targetHwnd)) {
            tab.targetHwnd = 0;
            return 0;
        }
        return tab.targetHwnd;
    }
    if (tab.targetProcess.isEmpty()) {
        HWND fg = GetForegroundWindow();
        quintptr h = reinterpret_cast<quintptr>(fg);
        if (h && Win32Input::isHwndOwnProcess(h)) return 0;
        return h;
    }
    QString tLow = tab.targetProcess.toLower().trimmed();
    for (const auto& w : Win32Input::listDetailedWindowsForPicker()) {
        if (w.procName.toLower() == tLow) {
            tab.targetHwnd = w.hwnd;
            tab.targetTitle = w.title;
            return w.hwnd;
        }
    }
#else
    Q_UNUSED(tab);
#endif
    return 0;
}

#ifdef _WIN32
static HWND findBestInputChildHwnd(HWND parent) {
    if (!parent) return parent;
    struct Ctx { QList<HWND> children; } ctx;
    EnumChildWindows(parent, [](HWND ch, LPARAM lp) -> BOOL {
        auto* c = reinterpret_cast<Ctx*>(lp);
        if (c->children.size() < 96) c->children.append(ch);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&ctx));
    if (ctx.children.isEmpty()) return parent;

    const QStringList prio = {"richeditd2dpt", "richedit", "scintilla", "edit", "chrome_renderwidgethosthwnd", "unitywndclass", "unrealwindow"};
    for (const QString& tok : prio) {
        for (HWND ch : ctx.children) {
            if (!IsWindowVisible(ch)) continue;
            wchar_t cls[256] = {0};
            GetClassNameW(ch, cls, 255);
            if (QString::fromWCharArray(cls).toLower().contains(tok)) return ch;
        }
    }
    return parent;
}
#endif

bool AutoPresserEngine::dispatchKey(
    const QString& keyLabel,
    const QString& method,
    int holdMs,
    quintptr targetHwnd,
    bool runInBackground,
    bool strictNoFallback,
    bool allowSelfTest,
    QString* outReason
) {
    auto setR = [&](const QString& r) { if (outReason) *outReason = r; };
    if (!allowSelfTest && targetHwnd && Win32Input::isHwndOwnProcess(targetHwnd)) {
        setR("Self-Protection — Blocked injecting into V-MacroPlus window");
        return false;
    }
    auto scExt = Win32Input::scancodeForLabel(keyLabel);
    int sc = scExt.first;
    bool ext = scExt.second;
    int holdBounded = qBound(10, holdMs, 2000);

    bool isBgMethod = false;
    for (const auto& m : all22KeyMethods()) {
        if (m.id == method) { isBgMethod = m.isBackground; break; }
    }

    if (!isBgMethod) {
        if (strictNoFallback) {
            if (method == "cpp_interception_driver") {
                auto d = Win32Input::diagnoseKernelDrivers(false);
                if (!d.contextCreated) { setR("Interception Ring-0 Driver not installed"); return false; }
            }
            if (method == "cpp_winring0_ps2_port") {
                auto d = Win32Input::diagnosePs2PortDriver(false);
                if (!d.driverOpen) { setR("InpOutx64 PS/2 Port 0x60 Driver not loaded"); return false; }
            }
#ifdef _WIN32
            if (method == "linux_uinput_xtest") { setR("Linux /dev/uinput not available on Windows"); return false; }
#endif
            if (method == "pico_usb_hid" && !MicrocontrollerBridge::instance().isBoardReady("pico")) {
                setR("Raspberry Pi Pico USB HID board not connected");
                return false;
            }
            if (method == "arduino_usb_hid" && !MicrocontrollerBridge::instance().isBoardReady("arduino")) {
                setR("Arduino Leonardo/Pro Micro USB HID board not connected");
                return false;
            }
        }
        if (runInBackground && targetHwnd && !Win32Input::isHwndOrChildForeground(targetHwnd)) {
            setR("Foreground-only method skipped in Background Mode (Target window not focused)");
            return false;
        }
        if (targetHwnd && !runInBackground && !Win32Input::isHwndOrChildForeground(targetHwnd)) {
            Win32Input::focusAndBringHwndToFront(targetHwnd);
        }
        if (!allowSelfTest && Win32Input::isOwnAppForeground()) {
            setR("Self-Protection — V-MacroPlus window is in the foreground");
            return false;
        }
        if (method == "unicode" && keyLabel.trimmed().size() == 1) {
            Win32Input::sendUnicodeText(keyLabel.trimmed());
            setR("Delivered via unicode");
            return true;
        }
        bool ok1 = Win32Input::sendScancodeKey(sc, ext, false, method, strictNoFallback);
        QThread::msleep(static_cast<unsigned long>(holdBounded));
        bool ok2 = Win32Input::sendScancodeKey(sc, ext, true, method, strictNoFallback);
        setR("Delivered via " + method);
        return ok1 && ok2;
    }

#ifdef _WIN32
    HWND hwnd = targetHwnd ? reinterpret_cast<HWND>(targetHwnd) : GetForegroundWindow();
    if (!hwnd || !IsWindow(hwnd)) {
        setR("No valid target HWND available");
        return false;
    }
    if (!allowSelfTest && Win32Input::isHwndOwnProcess(reinterpret_cast<quintptr>(hwnd))) {
        setR("Self-Protection — Blocked injecting into V-MacroPlus window");
        return false;
    }
    HWND dest = findBestInputChildHwnd(hwnd);
    int vk = Win32Input::vkForScancode(sc, ext);
    LPARAM lparamDn = 1 | ((sc & 0xFF) << 16) | (ext ? (1 << 24) : 0);
    LPARAM lparamUp = lparamDn | (1 << 30) | (1 << 31);

    if (method == "sendmessage") {
        DWORD_PTR res = 0;
        LRESULT r1 = SendMessageTimeoutW(dest, WM_KEYDOWN, vk, lparamDn, SMTO_ABORTIFHUNG, 80, &res);
        QThread::msleep(static_cast<unsigned long>(holdBounded));
        LRESULT r2 = SendMessageTimeoutW(dest, WM_KEYUP, vk, lparamUp, SMTO_ABORTIFHUNG, 80, &res);
        setR("Delivered via sendmessage");
        return r1 != 0 && r2 != 0;
    }
    if (method == "sendnotify_tree") {
        BOOL r1 = SendNotifyMessageW(dest, WM_KEYDOWN, vk, lparamDn);
        QThread::msleep(static_cast<unsigned long>(holdBounded));
        BOOL r2 = SendNotifyMessageW(dest, WM_KEYUP, vk, lparamUp);
        setR("Delivered via sendnotify_tree");
        return r1 && r2;
    }
    if (method == "attach_state") {
        DWORD curTid = GetCurrentThreadId();
        DWORD tgTid = GetWindowThreadProcessId(dest, nullptr);
        bool canAttach = (tgTid && tgTid != curTid && !Win32Input::isHwndOwnProcess(reinterpret_cast<quintptr>(dest)));
        BYTE kbd[256]{};
        if (canAttach && AttachThreadInput(curTid, tgTid, TRUE)) {
            GetKeyboardState(kbd); kbd[vk & 0xFF] |= 0x80; SetKeyboardState(kbd);
            AttachThreadInput(curTid, tgTid, FALSE);
        }
        BOOL ok1 = PostMessageW(dest, WM_KEYDOWN, vk, lparamDn);
        QThread::msleep(static_cast<unsigned long>(holdBounded));
        if (canAttach && AttachThreadInput(curTid, tgTid, TRUE)) {
            GetKeyboardState(kbd); kbd[vk & 0xFF] &= ~0x80; SetKeyboardState(kbd);
            AttachThreadInput(curTid, tgTid, FALSE);
        }
        BOOL ok2 = PostMessageW(dest, WM_KEYUP, vk, lparamUp);
        setR("Delivered via attach_state");
        return ok1 && ok2;
    }
    if (method == "post_active") {
        PostMessageW(hwnd, WM_ACTIVATE, 1, 0);
        PostMessageW(dest, WM_SETFOCUS, 0, 0);
    }
    UINT msgDn = (method == "ime_pipeline") ? WM_IME_KEYDOWN : WM_KEYDOWN;
    UINT msgUp = (method == "ime_pipeline") ? WM_IME_KEYUP : WM_KEYUP;
    BOOL ok1 = PostMessageW(dest, msgDn, vk, lparamDn);
    QThread::msleep(static_cast<unsigned long>(holdBounded));
    BOOL ok2 = PostMessageW(dest, msgUp, vk, lparamUp);
    setR("Delivered via " + method);
    return ok1 && ok2;
#else
    QThread::msleep(static_cast<unsigned long>(holdBounded));
    setR("Delivered via " + method);
    return true;
#endif
}

bool AutoPresserEngine::dispatchTextViaMethod(
    const QString& text,
    const QString& method,
    quintptr targetHwnd,
    bool runInBackground,
    bool strictNoFallback,
    bool human,
    int charDelayMs,
    bool allowSelfTest,
    QString* outReason
) {
    if (text.isEmpty()) return true;
    auto setR = [&](const QString& r) { if (outReason) *outReason = r; };
    if (!allowSelfTest && targetHwnd && Win32Input::isHwndOwnProcess(targetHwnd)) {
        setR("Self-Protection — Blocked injecting into V-MacroPlus window");
        return false;
    }

    bool isBgMethod = false;
    for (const auto& m : all22KeyMethods()) {
        if (m.id == method) { isBgMethod = m.isBackground; break; }
    }

    if (!isBgMethod) {
        if (method == "unicode") {
            if (!allowSelfTest && Win32Input::isOwnAppForeground()) {
                setR("Self-Protection — V-MacroPlus window is in the foreground");
                return false;
            }
            Win32Input::sendUnicodeText(text);
            setR("Delivered via unicode");
            return true;
        }
        for (QChar ch : text) {
            if (ch == '\n') {
                if (!dispatchKey("Enter", method, 10, targetHwnd, runInBackground, strictNoFallback, allowSelfTest, outReason)) return false;
                continue;
            }
            if (ch == '\t') {
                if (!dispatchKey("Tab", method, 10, targetHwnd, runInBackground, strictNoFallback, allowSelfTest, outReason)) return false;
                continue;
            }
            if (ch == ' ') {
                if (!dispatchKey("Space", method, 10, targetHwnd, runInBackground, strictNoFallback, allowSelfTest, outReason)) return false;
                continue;
            }
            int hold = human ? QRandomGenerator::global()->bounded(42, 78) : 10;
            if (!dispatchKey(QString(ch).toUpper(), method, hold, targetHwnd, runInBackground, strictNoFallback, allowSelfTest, outReason)) return false;
            int gap = human ? QRandomGenerator::global()->bounded(45, 92) : qMax(2, charDelayMs);
            if (gap > 0) QThread::msleep(static_cast<unsigned long>(gap));
        }
        setR("Delivered via " + method);
        return true;
    }

#ifdef _WIN32
    HWND hwnd = targetHwnd ? reinterpret_cast<HWND>(targetHwnd) : GetForegroundWindow();
    if (!hwnd || !IsWindow(hwnd)) { setR("No valid target HWND available"); return false; }
    HWND dest = findBestInputChildHwnd(hwnd);
    for (QChar ch : text) {
        if (ch == '\n') {
            if (!dispatchKey("Enter", method, 10, targetHwnd, true, strictNoFallback, allowSelfTest, outReason)) return false;
            continue;
        }
        if (ch == '\t') {
            if (!dispatchKey("Tab", method, 10, targetHwnd, true, strictNoFallback, allowSelfTest, outReason)) return false;
            continue;
        }
        PostMessageW(dest, (method == "ime_pipeline") ? WM_IME_CHAR : WM_CHAR, ch.unicode(), 1);
        QThread::msleep(static_cast<unsigned long>(human ? QRandomGenerator::global()->bounded(38, 75) : qMax(2, charDelayMs)));
    }
#endif
    setR("Delivered via " + method);
    return true;
}

bool AutoPresserEngine::dispatchMouseDetailed(
    const AutoPresserMousePoint& pt,
    const QString& mouseMode,
    quintptr targetHwnd,
    bool runInBackground,
    bool strictNoFallback,
    bool allowSelfTest,
    QJsonObject* outMeta
) {
    QString eff = mouseMode;
    if (eff == "auto") eff = (runInBackground && targetHwnd) ? "post" : "real";
    QJsonObject meta;
    meta["mode"] = mouseMode;
    meta["effective_mode"] = eff;
    meta["btn"] = pt.button;
    meta["screen_x"] = pt.x;
    meta["screen_y"] = pt.y;

    if (!allowSelfTest && targetHwnd && Win32Input::isHwndOwnProcess(targetHwnd)) {
        meta["reason_code"] = "self_app_protected";
        if (outMeta) *outMeta = meta;
        return false;
    }
    if (eff == "real" || eff == "sendinput_abs" || !targetHwnd) {
        if (runInBackground && targetHwnd && !Win32Input::isHwndOrChildForeground(targetHwnd)) {
            meta["reason_code"] = "mouse_skipped_bg";
            if (outMeta) *outMeta = meta;
            return false;
        }
        if (!allowSelfTest && Win32Input::isOwnAppForeground()) {
            meta["reason_code"] = "self_app_protected";
            if (outMeta) *outMeta = meta;
            return false;
        }
        Win32Input::sendMouseButton(pt.x, pt.y, pt.button, "click", true, 30, "sendinput_scancode");
        meta["reason_code"] = "mouse_fg_ok";
        if (outMeta) *outMeta = meta;
        return true;
    }

#ifdef _WIN32
    HWND hwnd = reinterpret_cast<HWND>(targetHwnd);
    if (!IsWindow(hwnd)) {
        meta["reason_code"] = "hwnd_rejected";
        if (outMeta) *outMeta = meta;
        return false;
    }
    POINT cpt{pt.x, pt.y};
    ScreenToClient(hwnd, &cpt);
    LPARAM lp = ((cpt.y & 0xFFFF) << 16) | (cpt.x & 0xFFFF);
    UINT dn = WM_LBUTTONDOWN, up = WM_LBUTTONUP;
    WPARAM mk = MK_LBUTTON;
    if (pt.button == "right") { dn = WM_RBUTTONDOWN; up = WM_RBUTTONUP; mk = MK_RBUTTON; }
    else if (pt.button == "middle") { dn = WM_MBUTTONDOWN; up = WM_MBUTTONUP; mk = MK_MBUTTON; }
    BOOL ok1 = PostMessageW(hwnd, dn, mk, lp);
    QThread::msleep(25);
    BOOL ok2 = PostMessageW(hwnd, up, 0, lp);
    meta["client_x"] = static_cast<int>(cpt.x);
    meta["client_y"] = static_cast<int>(cpt.y);
    meta["reason_code"] = (ok1 && ok2) ? "mouse_bg_ok" : "hwnd_rejected";
    if (outMeta) *outMeta = meta;
    return ok1 && ok2;
#else
    Q_UNUSED(strictNoFallback);
    meta["reason_code"] = "mouse_bg_ok";
    if (outMeta) *outMeta = meta;
    return true;
#endif
}

bool AutoPresserEngine::expandAndExecuteMethodTemplate(
    const QString& templateStr,
    int methodNum,
    const QString& methodId,
    const QString& methodName,
    quintptr targetHwnd,
    const QString& targetApp,
    bool runInBackground,
    bool extraEnter,
    bool allowSelfTest,
    QString& outReason,
    QString& outPreview
) {
    QString tpl = templateStr.trimmed().isEmpty()
        ? QStringLiteral("key=\"Enter\" {method_num}: Hello V-MacroPlus worked with {method_id} \\n")
        : templateStr.trimmed();

    QString cat = "Foreground";
    for (const auto& m : all22KeyMethods()) if (m.id == methodId && m.isBackground) { cat = "Background"; break; }

    auto subVars = [&](QString s) {
        s.replace("{method_num}", QString::number(methodNum));
        s.replace("{method_num_02}", QString("%1").arg(methodNum, 2, 10, QChar('0')));
        s.replace("{method_id}", methodId);
        s.replace("{method_name}", methodName);
        s.replace("{category}", cat);
        s.replace("{target_app}", targetApp.isEmpty() ? "Sandbox" : targetApp);
        s.replace("{target_hwnd}", targetHwnd ? QString("0x%1").arg(static_cast<qulonglong>(targetHwnd), 0, 16).toUpper() : "0x0");
        s.replace("{mode}", runInBackground ? "Background" : "Foreground");
        s.replace("{time}", QDateTime::currentDateTime().toString("HH:mm:ss"));
        s.replace("{date}", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
        return s;
    };

    static const QRegularExpression re(
        R"re((?:\bkey\s*=\s*(?:"([^"]+)"|'([^']+)'|([^\s,;]+)))|(?:\bwait\s*=\s*(\d+))|(\\n|\\t))re",
        QRegularExpression::CaseInsensitiveOption
    );

    bool anyOk = false;
    bool endedEnter = false;
    outPreview.clear();
    outReason = "Delivered via " + methodId;

    int pos = 0;
    auto it = re.globalMatch(tpl);
    while (it.hasNext()) {
        auto m = it.next();
        if (m.capturedStart() > pos) {
            QString rawSeg = tpl.mid(pos, m.capturedStart() - pos);
            if (pos > 0 && rawSeg.startsWith(' ') && rawSeg.size() > 1) rawSeg = rawSeg.mid(1);
            QString seg = subVars(rawSeg);
            if (!seg.isEmpty()) {
                if (!dispatchTextViaMethod(seg, methodId, targetHwnd, runInBackground, true, false, 0, allowSelfTest, &outReason)) return false;
                anyOk = true; endedEnter = false; outPreview += seg;
            }
        }
        QString kv = !m.captured(1).isEmpty() ? m.captured(1) : (!m.captured(2).isEmpty() ? m.captured(2) : m.captured(3));
        QString wv = m.captured(4);
        QString ev = m.captured(5).toLower();
        if (!kv.isEmpty()) {
            if (!dispatchKey(kv.trimmed(), methodId, 16, targetHwnd, runInBackground, true, allowSelfTest, &outReason)) return false;
            anyOk = true; endedEnter = (kv.trimmed().compare("enter", Qt::CaseInsensitive) == 0);
            outPreview += "[" + kv.trimmed() + "]";
        } else if (!wv.isEmpty()) {
            QThread::msleep(static_cast<unsigned long>(qBound(0, wv.toInt(), 10000)));
            outPreview += "(" + wv + "ms)";
        } else if (!ev.isEmpty()) {
            QString kEsc = (ev == "\\n") ? "Enter" : "Tab";
            if (!dispatchKey(kEsc, methodId, 15, targetHwnd, runInBackground, true, allowSelfTest, &outReason)) return false;
            anyOk = true; endedEnter = (kEsc == "Enter"); outPreview += "[" + kEsc + "]";
        }
        pos = m.capturedEnd();
    }
    if (pos < tpl.size()) {
        QString rawTail = tpl.mid(pos);
        if (pos > 0 && rawTail.startsWith(' ') && rawTail.size() > 1) rawTail = rawTail.mid(1);
        QString tail = subVars(rawTail);
        if (!tail.isEmpty()) {
            if (!dispatchTextViaMethod(tail, methodId, targetHwnd, runInBackground, true, false, 0, allowSelfTest, &outReason)) return false;
            anyOk = true; endedEnter = false; outPreview += tail;
        }
    }
    if (extraEnter && !endedEnter) {
        if (!dispatchKey("Enter", methodId, 15, targetHwnd, runInBackground, true, allowSelfTest, &outReason)) return false;
        anyOk = true; outPreview += "[Enter]";
    }
    return anyOk;
}

bool AutoPresserEngine::executePerMethodPythonTest(
    const QString& scriptCode,
    int methodNum,
    const QString& methodId,
    const QString& methodName,
    quintptr targetHwnd,
    bool runInBackground,
    const QString& targetApp,
    bool allowSelfTest,
    std::atomic<bool>* stopFlag,
    QString& outReason
) {
    QString cat = "Foreground";
    for (const auto& m : all22KeyMethods()) if (m.id == methodId && m.isBackground) { cat = "Background"; break; }

    return PythonScriptRunner::executeScript(
        scriptCode,
        methodNum,
        methodId,
        methodName,
        cat,
        targetApp.isEmpty() ? "Sandbox" : targetApp,
        targetHwnd,
        runInBackground ? "Background" : "Foreground",
        [&](const QString& cmd, const QJsonObject& args, QString& actReason) -> bool {
            if (cmd == "press") {
                return dispatchKey(args.value("key").toString("Enter"), methodId, args.value("hold_ms").toInt(65), targetHwnd, runInBackground, true, allowSelfTest, &actReason);
            }
            if (cmd == "type_text") {
                bool ok = dispatchTextViaMethod(args.value("text").toString(), methodId, targetHwnd, runInBackground, true, args.value("human").toBool(false), args.value("delay_ms").toInt(0), allowSelfTest, &actReason);
                if (ok && args.value("enter").toBool(false)) {
                    return dispatchKey("Enter", methodId, 60, targetHwnd, runInBackground, true, allowSelfTest, &actReason);
                }
                return ok;
            }
            if (cmd == "click") {
                AutoPresserMousePoint mp;
                QPoint cur = Win32Input::getCursorPos();
                mp.x = args.value("has_xy").toBool(false) ? args.value("x").toInt(cur.x()) : cur.x();
                mp.y = args.value("has_xy").toBool(false) ? args.value("y").toInt(cur.y()) : cur.y();
                mp.button = args.value("button").toString("left");
                return dispatchMouseDetailed(mp, (runInBackground && targetHwnd) ? "post" : "real", targetHwnd, runInBackground, true, allowSelfTest);
            }
            if (cmd == "move_rel") {
                if (runInBackground && targetHwnd && !Win32Input::isHwndOrChildForeground(targetHwnd)) {
                    actReason = "Foreground-only 3D camera move skipped in Background Mode";
                    return false;
                }
                return Win32Input::sendMouseRelativeMove(args.value("dx").toInt(), args.value("dy").toInt(), args.value("steps").toInt(6), args.value("duration_ms").toInt(60), methodId);
            }
            return true;
        },
        stopFlag,
        outReason
    );
}

bool AutoPresserEngine::executePerMethodMacroTest(
    const QList<ComboStep>& steps,
    int methodNum,
    const QString& methodId,
    const QString& methodName,
    quintptr targetHwnd,
    const QString& targetApp,
    bool runInBackground,
    const QString& prefixTemplate,
    bool allowSelfTest,
    QString& outReason,
    int& outExecutedCount
) {
    outExecutedCount = 0;
    bool anyOk = false;
    outReason = "Delivered via " + methodId;
    if (!prefixTemplate.trimmed().isEmpty()) {
        QString prev;
        if (!expandAndExecuteMethodTemplate(prefixTemplate, methodNum, methodId, methodName, targetHwnd, targetApp, runInBackground, false, allowSelfTest, outReason, prev)) {
            return false;
        }
        anyOk = true;
        outExecutedCount++;
    }
    for (const auto& st : steps) {
        if (!st.enabled) continue;
        if (st.stepType == "wait") {
            QThread::msleep(static_cast<unsigned long>(qBound(1, st.delayMs, 1500)));
            outExecutedCount++;
            continue;
        }
        if (st.stepType == "key_tap" || st.stepType == "key_hold" || st.stepType == "key_down" || st.stepType == "key_up") {
            if (!dispatchKey(st.value, methodId, qMax(10, st.holdMs), targetHwnd, runInBackground, true, allowSelfTest, &outReason)) return false;
            anyOk = true; outExecutedCount++;
        } else if (st.stepType == "type_text") {
            QString prev;
            if (!expandAndExecuteMethodTemplate(st.value, methodNum, methodId, methodName, targetHwnd, targetApp, runInBackground, false, allowSelfTest, outReason, prev)) return false;
            anyOk = true; outExecutedCount++;
        } else if (st.stepType == "python_script") {
            if (!executePerMethodPythonTest(st.value, methodNum, methodId, methodName, targetHwnd, runInBackground, targetApp, allowSelfTest, nullptr, outReason)) return false;
            anyOk = true; outExecutedCount++;
        } else if (st.stepType == "mouse_click" || st.stepType == "mouse_down" || st.stepType == "mouse_up") {
            AutoPresserMousePoint mp{"MacroClick", st.x, st.y, st.value.isEmpty() ? "left" : st.value, 100, true};
            if (!dispatchMouseDetailed(mp, (runInBackground && targetHwnd) ? "post" : "real", targetHwnd, runInBackground, true, allowSelfTest)) {
                outReason = "Mouse click rejected";
                return false;
            }
            anyOk = true; outExecutedCount++;
        }
        if (st.delayMs > 0) QThread::msleep(static_cast<unsigned long>(qMin(250, st.delayMs)));
    }
    return anyOk;
}

AutoPresserEngine::AutoPresserEngine(AppState* state) : m_state(state) {
    loadWorkspace();
}

AutoPresserEngine::~AutoPresserEngine() {
    stopAllTabs();
}

bool AutoPresserEngine::loadWorkspace(const QString& filePath) {
    QString fp = filePath.isEmpty() ? Paths::autopresserWorkspacePath() : filePath;
    QFile f(fp);
    if (f.exists() && f.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        if (doc.isObject()) {
            QJsonObject root = doc.object();
            QList<AutoPresserTab> loaded;
            for (const auto& v : root.value("tabs").toArray()) {
                if (v.isObject()) loaded.append(AutoPresserTab::fromJson(v.toObject()));
            }
            if (!loaded.isEmpty()) {
                tabs = loaded;
                activeTabIndex = qBound(0, root.value("active_tab_index").toInt(0), tabs.size() - 1);
                return true;
            }
        }
    }
    if (tabs.isEmpty()) {
        AutoPresserTab t1;
        t1.ensureBuiltinKeys();
        tabs.append(t1);
        activeTabIndex = 0;
        saveWorkspace();
    }
    return true;
}

bool AutoPresserEngine::saveWorkspace(const QString& filePath) {
    QString fp = filePath.isEmpty() ? Paths::autopresserWorkspacePath() : filePath;
    QJsonObject root;
    root["schema_version"] = 6;
    root["active_tab_index"] = activeTabIndex;
    QJsonArray arr;
    for (const auto& t : tabs) arr.append(t.toJson());
    root["tabs"] = arr;
    QFile f(fp);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        return true;
    }
    return false;
}

AutoPresserTab& AutoPresserEngine::addNewTab(int duplicateFrom) {
    QString newId = QString("tab_%1_%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(tabs.size() + 1);
    AutoPresserTab nt;
    if (duplicateFrom >= 0 && duplicateFrom < tabs.size()) {
        nt = tabs[duplicateFrom];
        nt.tabId = newId;
        nt.title += " (Copy)";
    } else {
        nt.tabId = newId;
        nt.title = QString("Tab %1").arg(tabs.size() + 1);
        nt.ensureBuiltinKeys();
    }
    tabs.append(nt);
    activeTabIndex = tabs.size() - 1;
    saveWorkspace();
    return tabs.last();
}

void AutoPresserEngine::closeTab(int index) {
    if (tabs.size() <= 1 || index < 0 || index >= tabs.size()) return;
    stopTab(tabs[index].tabId);
    tabs.removeAt(index);
    activeTabIndex = qBound(0, activeTabIndex, tabs.size() - 1);
    saveWorkspace();
}

bool AutoPresserEngine::isTabRunning(const QString& tabId) const {
    auto it = m_runningTabs.constFind(tabId);
    return it != m_runningTabs.constEnd() && !it.value()->load();
}

bool AutoPresserEngine::anyTabRunning() const {
    for (const auto& k : m_runningTabs.keys()) if (isTabRunning(k)) return true;
    return false;
}

bool AutoPresserEngine::toggleTab(AutoPresserTab& tab) {
    if (isTabRunning(tab.tabId)) { stopTab(tab.tabId); return false; }
    return startTab(tab);
}

bool AutoPresserEngine::startTab(AutoPresserTab& tab) {
    if (isTabRunning(tab.tabId)) return true;
    bool hasWork = false;
    for (const auto& k : tab.keys) if (k.enabled) { hasWork = true; break; }
    if (!hasWork && tab.mouseEnabled) {
        for (const auto& m : tab.mousePoints) if (m.enabled) { hasWork = true; break; }
    }
    if (!hasWork) {
        QJsonObject e; e["kind"] = "no_keys_armed";
        appendTabLog(tab.tabId, e, "#F59E0B");
        return false;
    }

    auto stopEv = std::make_shared<std::atomic<bool>>(false);
    m_runningTabs[tab.tabId] = stopEv;
    QString tid = tab.tabId;
    AutoPresserTab tabCopy = tab;

    std::thread([this, tid, tabCopy, stopEv]() mutable {
        QMap<QString, qint64> nextKeyMs;
        QMap<int, qint64> nextMouseMs;
        while (!stopEv->load()) {
            qint64 now = QDateTime::currentMSecsSinceEpoch();
            quintptr hwnd = resolveTargetHwnd(tabCopy);
            for (const auto& k : tabCopy.keys) {
                if (stopEv->load()) break;
                if (!k.enabled) continue;
                if (now >= nextKeyMs.value(k.key, 0)) {
                    QString reason;
                    bool ok = dispatchKey(k.key, tabCopy.keyMethod, tabCopy.holdMs, hwnd, tabCopy.runInBackground, false, false, &reason);
                    nextKeyMs[k.key] = QDateTime::currentMSecsSinceEpoch() + qMax(10, k.intervalMs);
                    QJsonObject e;
                    e["kind"] = "sent_key"; e["key"] = k.key; e["interval_ms"] = k.intervalMs; e["method"] = tabCopy.keyMethod; e["ok"] = ok;
                    appendTabLog(tid, e, ok ? "#8B5CF6" : "#EF4444");
                }
            }
            if (tabCopy.mouseEnabled) {
                for (int i = 0; i < tabCopy.mousePoints.size(); ++i) {
                    if (stopEv->load()) break;
                    const auto& mp = tabCopy.mousePoints[i];
                    if (!mp.enabled) continue;
                    if (now >= nextMouseMs.value(i, 0)) {
                        QJsonObject meta;
                        bool okM = dispatchMouseDetailed(mp, tabCopy.mouseMode, hwnd, tabCopy.runInBackground, false, false, &meta);
                        nextMouseMs[i] = QDateTime::currentMSecsSinceEpoch() + qMax(10, mp.intervalMs);
                        QJsonObject e;
                        e["kind"] = "sent_mouse"; e["btn"] = mp.button; e["x"] = mp.x; e["y"] = mp.y;
                        e["mode"] = tabCopy.mouseMode; e["ok"] = okM;
                        appendTabLog(tid, e, okM ? "#06B6D4" : "#EF4444");
                    }
                }
            }
            QThread::msleep(15);
        }
        m_runningTabs.remove(tid);
        QJsonObject s; s["kind"] = "stopped";
        appendTabLog(tid, s, "#F59E0B");
    }).detach();
    return true;
}

void AutoPresserEngine::stopTab(const QString& tabId) {
    if (m_runningTabs.contains(tabId)) {
        m_runningTabs[tabId]->store(true);
        m_runningTabs.remove(tabId);
    }
}

void AutoPresserEngine::stopAllTabs() {
    for (const auto& k : m_runningTabs.keys()) m_runningTabs[k]->store(true);
    m_runningTabs.clear();
}

QList<QJsonObject> AutoPresserEngine::getTabLogs(const QString& tabId) const {
    return m_tabLogs.value(tabId);
}

void AutoPresserEngine::clearTabLogs(const QString& tabId) {
    m_tabLogs[tabId].clear();
}

void AutoPresserEngine::appendTabLog(const QString& tabId, const QJsonObject& entry, const QString& accentHex) {
    QJsonObject item = entry;
    item["ts"] = QDateTime::currentDateTime().toString("HH:mm:ss");
    item["accent"] = accentHex;
    auto& list = m_tabLogs[tabId];
    list.prepend(item);
    if (list.size() > 200) list.removeLast();
}

void AutoPresserEngine::sweepAllKeyMethods(AutoPresserTab& tab, const QString& testKey, int& outOk, int& outFail) {
    outOk = 0; outFail = 0;
    quintptr hwnd = resolveTargetHwnd(tab);
    if (hwnd && !tab.runInBackground) Win32Input::focusAndBringHwndToFront(hwnd);

    const auto& methods = all22KeyMethods();
    for (int i = 0; i < methods.size(); ++i) {
        const auto& m = methods[i];
        QString reason;
        bool ok = dispatchKey(testKey, m.id, 15, hwnd, tab.runInBackground, true, false, &reason);
        if (ok) outOk++; else outFail++;
        QJsonObject e;
        e["kind"] = "sweep_item";
        e["idx"] = i + 1;
        e["total"] = methods.size();
        e["method_id"] = m.id;
        e["method_tr_key"] = m.trKey;
        e["key"] = testKey;
        e["target_app"] = tab.targetProcess.isEmpty() ? "Active Window" : tab.targetProcess;
        e["is_bg"] = tab.runInBackground;
        e["ok"] = ok;
        e["reason"] = reason;
        appendTabLog(tab.tabId, e, ok ? "#10B981" : "#EF4444");
        QThread::msleep(35);
    }
}

void AutoPresserEngine::sweepAllMouseModes(AutoPresserTab& tab, int& outOk, int& outFail) {
    outOk = 0; outFail = 0;
    quintptr hwnd = resolveTargetHwnd(tab);
    if (hwnd && !tab.runInBackground) Win32Input::focusAndBringHwndToFront(hwnd);

    AutoPresserMousePoint pt;
    if (!tab.mousePoints.isEmpty()) pt = tab.mousePoints.first();
    else {
        QPoint cur = Win32Input::getCursorPos();
        pt.x = cur.x(); pt.y = cur.y();
    }

    const auto& modes = all8MouseModes();
    for (int i = 0; i < modes.size(); ++i) {
        QJsonObject meta;
        bool ok = dispatchMouseDetailed(pt, modes[i].first, hwnd, tab.runInBackground, true, false, &meta);
        if (ok) outOk++; else outFail++;
        QJsonObject e;
        e["kind"] = "sweep_item";
        e["idx"] = i + 1;
        e["total"] = modes.size();
        e["method_id"] = modes[i].first;
        e["method_tr_key"] = modes[i].second;
        e["ok"] = ok;
        e["reason"] = meta.value("reason_code").toString();
        appendTabLog(tab.tabId, e, ok ? "#10B981" : "#EF4444");
        QThread::msleep(35);
    }
}

} // namespace vmacro
