#include "PythonScriptRunner.h"
#include "../core/I18n.h"
#include "../core/Paths.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRandomGenerator>
#include <QTemporaryFile>
#include <QThread>

namespace vmacro {

QList<PythonPresetInfo> PythonScriptRunner::builtinPresets() {
    return {
        {
            "preset:test_log_line",
            trDual("1. طباعة اسم ورقم الطريقة + Enter (للاختبار)", "1. Print Method Num & ID + Enter (Diagnostic)"),
            "print(f\"Method {method_num}: [{method_id}] - OK\")\npress(\"Enter\")\nwait(120)\n",
            false
        },
        {
            "preset:human_3d_keys",
            trDual("2. ضغط أزرار ألعاب 3D طبيعي (Humanized 60-95ms)", "2. Humanized 3D Game Keys (60-95ms Anti-Cheat Safe)"),
            "# Humanized 3D game key taps (randomized hold & wait)\n"
            "human_press(\"1\")\n"
            "human_press(\"2\")\n"
            "human_press(\"Space\", min_hold=65, max_hold=105)\n",
            false
        },
        {
            "preset:wasd_movement",
            trDual("3. حركة مستمرة (WASD + قفز في الألعاب)", "3. 3D Movement & Jump (hold_key W + Space)"),
            "hold_key(\"W\", 350)\n"
            "human_wait(50, 90)\n"
            "human_press(\"Space\", min_hold=70, max_hold=110)\n",
            false
        },
        {
            "preset:s4_chat_message",
            trDual("4. فتح الشات وكتابة رسالة وإرسالها (Enter + Type + Enter)", "4. In-Game Chat Message (Enter + Type + Enter)"),
            "human_press(\"Enter\")\n"
            "human_wait(80, 140)\n"
            "type_text(f\"Method {method_num} [{method_id}] OK\", human=True)\n"
            "human_wait(60, 110)\n"
            "human_press(\"Enter\")\n",
            false
        },
        {
            "preset:mouse_3d_combat",
            trDual("5. كليك ماوس + تحريك كاميرا 3D داخل الجيم", "5. 3D Combat Click + Relative Camera Move"),
            "click(\"left\", human=True, in_game_3d=True)\n"
            "human_wait(70, 120)\n"
            "move_rel(120, 0, steps=6, duration_ms=70)\n",
            false
        },
        {
            "preset:combo_loop_3x",
            trDual("6. تكرار ماكرو 3 مرات بتوقيت بشري عشوائي", "6. 3x Humanized Loop Combo"),
            "for i in range(3):\n"
            "    human_press(\"Q\")\n"
            "    human_wait(110, 180)\n",
            false
        },
    };
}

QList<PythonPresetInfo> PythonScriptRunner::loadUserScripts() {
    QList<PythonPresetInfo> out;
    QFile f(Paths::pythonScriptsPath());
    if (!f.exists() || !f.open(QIODevice::ReadOnly)) return out;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return out;
    for (const auto& v : doc.array()) {
        if (!v.isObject()) continue;
        QJsonObject o = v.toObject();
        out.append(PythonPresetInfo{o.value("id").toString(), o.value("name").toString(), o.value("code").toString(), true});
    }
    return out;
}

static void saveUserScripts(const QList<PythonPresetInfo>& list) {
    QJsonArray arr;
    for (const auto& it : list) {
        QJsonObject o;
        o["id"] = it.key;
        o["name"] = it.name;
        o["code"] = it.code;
        arr.append(o);
    }
    QFile f(Paths::pythonScriptsPath());
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    }
}

QList<QPair<QString, QString>> PythonScriptRunner::dropdownItems() {
    QList<QPair<QString, QString>> items;
    for (const auto& u : loadUserScripts()) {
        items.append({u.key, "★ " + u.name});
    }
    for (const auto& b : builtinPresets()) {
        items.append({b.key, b.name});
    }
    return items;
}

QString PythonScriptRunner::codeByKey(const QString& key) {
    for (const auto& u : loadUserScripts()) if (u.key == key) return u.code;
    for (const auto& b : builtinPresets()) if (b.key == key) return b.code;
    return {};
}

QString PythonScriptRunner::nameByKey(const QString& key) {
    for (const auto& u : loadUserScripts()) if (u.key == key) return u.name;
    return {};
}

QPair<QString, QString> PythonScriptRunner::upsertUserScript(const QString& name, const QString& code, const QString& existingKey) {
    auto list = loadUserScripts();
    QString cleanName = name.trimmed().isEmpty() ? QString("Script %1").arg(list.size() + 1) : name.trimmed();
    QString targetKey = existingKey.startsWith("user:") ? existingKey : QString("user:%1").arg(QDateTime::currentMSecsSinceEpoch());
    bool updated = false;
    for (auto& item : list) {
        if (item.key == targetKey) {
            item.name = cleanName;
            item.code = code;
            updated = true;
            break;
        }
    }
    if (!updated) list.prepend({targetKey, cleanName, code, true});
    saveUserScripts(list);
    return {targetKey, cleanName};
}

bool PythonScriptRunner::deleteUserScript(const QString& key) {
    if (!key.startsWith("user:")) return false;
    auto list = loadUserScripts();
    int before = list.size();
    list.erase(std::remove_if(list.begin(), list.end(), [&](const PythonPresetInfo& p) { return p.key == key; }), list.end());
    if (list.size() != before) {
        saveUserScripts(list);
        return true;
    }
    return false;
}

static const char* kPyBridgeRunnerScript = R"PY(
import json, random, sys, time

ctx = json.loads(sys.stdin.readline())

def _rpc(cmd, **kwargs):
    sys.stdout.write(json.dumps({"cmd": cmd, "args": kwargs}, ensure_ascii=False) + "\n")
    sys.stdout.flush()
    resp = sys.stdin.readline()
    if not resp:
        raise RuntimeError("Bridge closed")
    d = json.loads(resp)
    if not d.get("ok", False):
        raise RuntimeError(d.get("reason", "Action failed"))
    return True

def rand(a=60, b=110):
    return random.randint(int(min(a, b)), int(max(a, b)))

def wait(ms=100):
    time.sleep(max(0.0, min(15.0, float(ms) / 1000.0)))

def human_wait(min_ms=65, max_ms=130):
    ms = max(5.0, random.uniform(float(min_ms), float(max_ms)) + random.gauss(0.0, 2.5))
    wait(ms)

def type_text(text, enter=False, human=False, delay_ms=0):
    return _rpc("type_text", text=str(text), enter=bool(enter), human=bool(human), delay_ms=int(delay_ms))

def print_fn(*args, sep=" ", end="", enter=False, human=False, delay_ms=0):
    return type_text(sep.join(str(a) for a in args) + end, enter=enter, human=human, delay_ms=delay_ms)

def press(key="Enter", hold_ms=65, human=False):
    if isinstance(hold_ms, str) and hold_ms.lower() == "human":
        human = True
        eff = random.randint(58, 95)
    elif human:
        eff = max(45, int((hold_ms if isinstance(hold_ms, (int, float)) and hold_ms > 25 else 72) + random.randint(-12, 16)))
    else:
        eff = max(15, int(hold_ms) if isinstance(hold_ms, (int, float)) else 65)
    return _rpc("press", key=str(key), hold_ms=eff)

def human_press(key="Enter", min_hold=58, max_hold=95, min_wait=65, max_wait=130):
    ok = press(key, hold_ms=rand(min_hold, max_hold))
    if ok and max_wait > 0:
        human_wait(min_wait, max_wait)
    return ok

def hold_key(key="W", duration_ms=300):
    return press(key, hold_ms=max(40, int(duration_ms)))

def click(arg1="left", arg2=None, button="left", hold_ms=68, human=False, in_game_3d=False):
    if isinstance(arg1, (int, float)) and isinstance(arg2, (int, float)):
        return _rpc("click", x=int(arg1), y=int(arg2), has_xy=True, button=str(button), hold_ms=int(hold_ms), human=bool(human), in_game_3d=bool(in_game_3d))
    return _rpc("click", x=0, y=0, has_xy=False, button=str(arg1 or button or "left"), hold_ms=int(hold_ms), human=bool(human), in_game_3d=bool(in_game_3d))

def move_rel(dx, dy, steps=6, duration_ms=60):
    return _rpc("move_rel", dx=int(dx), dy=int(dy), steps=int(steps), duration_ms=int(duration_ms))

env = {
    "__builtins__": {"len": len, "range": range, "str": str, "int": int, "float": float, "bool": bool, "min": min, "max": max, "abs": abs, "round": round, "enumerate": enumerate, "True": True, "False": False, "None": None},
    "method_num": ctx["method_num"],
    "method_num_02": ctx["method_num_02"],
    "method_id": ctx["method_id"],
    "method_name": ctx["method_name"],
    "category": ctx["category"],
    "target_app": ctx["target_app"],
    "target_hwnd": ctx["target_hwnd"],
    "mode": ctx["mode"],
    "rand": rand, "randint": rand,
    "print": print_fn, "type_text": type_text,
    "press": press, "send_key": press, "human_press": human_press, "hold_key": hold_key,
    "wait": wait, "sleep": wait, "sleep_ms": wait, "human_wait": human_wait,
    "click": click, "move_rel": move_rel,
}
try:
    exec(compile(ctx["code"], "<v_macroplus_python>", "exec"), env, {})
    sys.stdout.write(json.dumps({"cmd": "__done__", "ok": True}) + "\n")
    sys.stdout.flush()
except Exception as exc:
    sys.stdout.write(json.dumps({"cmd": "__done__", "ok": False, "reason": str(exc)}) + "\n")
    sys.stdout.flush()
)PY";

bool PythonScriptRunner::executeScript(
    const QString& userCode,
    int methodNum,
    const QString& methodId,
    const QString& methodName,
    const QString& category,
    const QString& targetApp,
    quintptr targetHwnd,
    const QString& mode,
    ActionCallback onAction,
    std::atomic<bool>* stopFlag,
    QString& outReason
) {
    QProcess proc;
    QStringList pyBins;
#ifdef _WIN32
    pyBins << "python" << "py";
#else
    pyBins << "python3" << "python";
#endif
    bool started = false;
    for (const QString& bin : pyBins) {
        proc.start(bin, {"-u", "-c", QString::fromUtf8(kPyBridgeRunnerScript)});
        if (proc.waitForStarted(1500)) {
            started = true;
            break;
        }
    }
    if (!started) {
        outReason = "Python runtime (python / python3) not found in PATH";
        return false;
    }

    QJsonObject ctx;
    ctx["method_num"] = methodNum;
    ctx["method_num_02"] = QString("%1").arg(methodNum, 2, 10, QChar('0'));
    ctx["method_id"] = methodId;
    ctx["method_name"] = methodName;
    ctx["category"] = category;
    ctx["target_app"] = targetApp;
    ctx["target_hwnd"] = QString("0x%1").arg(static_cast<qulonglong>(targetHwnd), 0, 16).toUpper();
    ctx["mode"] = mode;
    ctx["code"] = userCode.trimmed().isEmpty()
        ? QString("print(f\"Method {method_num}: [{method_id}] - OK\")\npress(\"Enter\")\nwait(120)\n")
        : userCode;

    proc.write(QJsonDocument(ctx).toJson(QJsonDocument::Compact) + "\n");
    proc.waitForBytesWritten(500);

    bool anyOk = false;
    outReason = "Delivered via " + methodId;

    while (proc.state() != QProcess::NotRunning || proc.canReadLine()) {
        if (stopFlag && stopFlag->load()) {
            proc.kill();
            proc.waitForFinished(500);
            outReason = "Stopped by user";
            return false;
        }
        if (!proc.canReadLine()) {
            proc.waitForReadyRead(40);
            continue;
        }
        QByteArray line = proc.readLine().trimmed();
        if (line.isEmpty()) continue;
        QJsonDocument d = QJsonDocument::fromJson(line);
        if (!d.isObject()) continue;
        QJsonObject obj = d.object();
        QString cmd = obj.value("cmd").toString();
        if (cmd == "__done__") {
            bool okDone = obj.value("ok").toBool(true);
            if (!okDone) {
                outReason = obj.value("reason").toString("Python Script Error");
                proc.waitForFinished(500);
                return false;
            }
            break;
        }
        QString actReason;
        bool actOk = onAction ? onAction(cmd, obj.value("args").toObject(), actReason) : true;
        if (actOk) anyOk = true;
        else outReason = actReason;

        QJsonObject ack;
        ack["ok"] = actOk;
        ack["reason"] = actReason;
        proc.write(QJsonDocument(ack).toJson(QJsonDocument::Compact) + "\n");
        proc.waitForBytesWritten(200);
    }

    proc.waitForFinished(500);
    return anyOk;
}

} // namespace vmacro
