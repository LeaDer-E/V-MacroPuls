#include "Models.h"
#include "Paths.h"
#include <QFile>
#include <QJsonDocument>

namespace vmacro {

const QStringList& builtin36Keys() {
    static const QStringList keys = {
        "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12",
        "1","2","3","4","5","6","7","8","9","0","-","=",
        "Num1","Num2","Num3","Num4","Num5","Num6","Num7","Num8","Num9","Num0","Num/","Num*"
    };
    return keys;
}

QJsonObject MacroEvent::toJson() const {
    QJsonObject o;
    o["kind"] = kind;
    o["delay_ms"] = delayMs;
    o["hold_ms"] = holdMs;
    o["key"] = key;
    o["scan_code"] = scanCode;
    o["is_extended"] = isExtended;
    o["x"] = x;
    o["y"] = y;
    o["button"] = button;
    o["scroll_dir"] = scrollDir;
    o["scroll_amount"] = scrollAmount;
    o["dx"] = dx;
    o["dy"] = dy;
    o["enabled"] = enabled;
    o["note"] = note;
    return o;
}

MacroEvent MacroEvent::fromJson(const QJsonObject& obj) {
    MacroEvent e;
    e.kind = obj.value("kind").toString("key_tap");
    e.delayMs = qMax(1, obj.value("delay_ms").toInt(40));
    e.holdMs = qMax(0, obj.value("hold_ms").toInt(0));
    e.key = obj.value("key").toString("Space");
    e.scanCode = obj.value("scan_code").toInt(0x39);
    e.isExtended = obj.value("is_extended").toBool(false);
    e.x = obj.value("x").toInt(500);
    e.y = obj.value("y").toInt(500);
    e.button = obj.value("button").toString("left");
    e.scrollDir = obj.value("scroll_dir").toString("down");
    e.scrollAmount = qMax(1, obj.value("scroll_amount").toInt(3));
    e.dx = obj.value("dx").toInt(0);
    e.dy = obj.value("dy").toInt(0);
    e.enabled = obj.value("enabled").toBool(true);
    e.note = obj.value("note").toString();
    return e;
}

QJsonObject ComboStep::toJson() const {
    QJsonObject o;
    o["step_type"] = stepType;
    o["value"] = value;
    o["scan_code"] = scanCode;
    o["is_extended"] = isExtended;
    o["delay_ms"] = delayMs;
    o["hold_ms"] = holdMs;
    o["scroll_amount"] = scrollAmount;
    o["x"] = x;
    o["y"] = y;
    o["use_coords"] = useCoords;
    o["enabled"] = enabled;
    o["note"] = note;
    return o;
}

ComboStep ComboStep::fromJson(const QJsonObject& obj) {
    ComboStep s;
    s.stepType = obj.value("step_type").toString("key_tap");
    s.value = obj.value("value").toString("Space");
    s.scanCode = obj.value("scan_code").toInt(0);
    s.isExtended = obj.value("is_extended").toBool(false);
    s.delayMs = qMax(1, obj.value("delay_ms").toInt(20));
    s.holdMs = qMax(0, obj.value("hold_ms").toInt(0));
    s.scrollAmount = qMax(1, obj.value("scroll_amount").toInt(3));
    s.x = obj.value("x").toInt(500);
    s.y = obj.value("y").toInt(500);
    s.useCoords = obj.value("use_coords").toBool(true);
    s.enabled = obj.value("enabled").toBool(true);
    s.note = obj.value("note").toString();
    return s;
}

ComboStep ComboStep::fromMacroEvent(const MacroEvent& ev) {
    ComboStep s;
    s.delayMs = qMax(1, ev.delayMs);
    s.holdMs = qMax(0, ev.holdMs);
    s.enabled = ev.enabled;
    s.note = ev.note;
    if (ev.kind == "wait") {
        s.stepType = "wait";
        s.value = "Wait";
    } else if (ev.kind == "key_down" || ev.kind == "key_up" || ev.kind == "key_tap" || ev.kind == "key_hold") {
        s.stepType = ev.kind;
        s.value = ev.key;
        s.scanCode = ev.scanCode;
        s.isExtended = ev.isExtended;
    } else if (ev.kind == "mouse_down" || ev.kind == "mouse_up" || ev.kind == "mouse_click") {
        s.stepType = ev.kind;
        s.value = ev.button.isEmpty() ? "left" : ev.button;
        s.x = ev.x;
        s.y = ev.y;
        s.useCoords = true;
    } else if (ev.kind == "mouse_scroll") {
        s.stepType = "mouse_scroll";
        s.value = ev.scrollDir.isEmpty() ? "down" : ev.scrollDir;
        s.scrollAmount = qMax(1, ev.scrollAmount);
        s.x = ev.x;
        s.y = ev.y;
        s.useCoords = false;
    } else if (ev.kind == "mouse_move") {
        s.stepType = "mouse_move";
        s.value = "move";
        s.x = ev.x;
        s.y = ev.y;
        s.useCoords = true;
    } else {
        s.stepType = "wait";
        s.value = "Wait";
    }
    return s;
}

QJsonObject SmartCombo::toJson() const {
    QJsonObject o;
    o["id"] = id;
    o["name"] = name;
    o["trigger_key"] = triggerKey;
    o["trigger_scan"] = triggerScan;
    o["trigger_extended"] = triggerExtended;
    QJsonArray chArr;
    for (const auto& p : triggerChord) {
        QJsonArray pair;
        pair.append(p.first);
        pair.append(p.second ? 1 : 0);
        chArr.append(pair);
    }
    o["trigger_chord"] = chArr;
    o["mode"] = mode;
    o["retrigger_policy"] = retriggerPolicy;
    o["target_process"] = targetProcess;
    QJsonArray exArr;
    for (const auto& e : excludedProcesses) exArr.append(e);
    o["excluded_processes"] = exArr;
    o["stop_hotkey"] = stopHotkey;
    o["start_delay_ms"] = startDelayMs;
    o["enabled"] = enabled;
    QJsonArray stArr;
    for (const auto& s : steps) stArr.append(s.toJson());
    o["steps"] = stArr;
    return o;
}

SmartCombo SmartCombo::fromJson(const QJsonObject& obj) {
    SmartCombo c;
    c.id = obj.value("id").toString("combo_1");
    c.name = obj.value("name").toString();
    c.triggerKey = obj.value("trigger_key").toString();
    c.triggerScan = obj.value("trigger_scan").toInt(0);
    c.triggerExtended = obj.value("trigger_extended").toBool(false);
    const auto chArr = obj.value("trigger_chord").toArray();
    for (const auto& v : chArr) {
        auto pair = v.toArray();
        if (pair.size() >= 2) {
            c.triggerChord.append({pair[0].toInt(), pair[1].toInt() != 0 || pair[1].toBool()});
        }
    }
    c.mode = obj.value("mode").toString("once");
    c.retriggerPolicy = obj.value("retrigger_policy").toString("ignore");
    c.targetProcess = obj.value("target_process").toString();
    for (const auto& v : obj.value("excluded_processes").toArray()) {
        QString s = v.toString().trimmed();
        if (!s.isEmpty()) c.excludedProcesses.append(s);
    }
    c.stopHotkey = obj.value("stop_hotkey").toString().trimmed();
    c.startDelayMs = obj.value("start_delay_ms").toInt(1000);
    c.enabled = obj.value("enabled").toBool(false);
    for (const auto& v : obj.value("steps").toArray()) {
        if (v.isObject()) c.steps.append(ComboStep::fromJson(v.toObject()));
    }
    return c;
}

QString SmartCombo::sidebarLabel(bool isAr) const {
    if (!triggerKey.isEmpty()) return triggerKey;
    if (!name.isEmpty()) return name;
    return isAr ? QStringLiteral("ماكرو يدوي") : QStringLiteral("Manual Macro");
}

QJsonObject SavedMacro::toJson() const {
    QJsonObject o;
    o["id"] = id;
    o["name"] = name;
    o["hotkey"] = hotkey;
    o["created_at"] = createdAt;
    QJsonArray arr;
    for (const auto& e : events) arr.append(e.toJson());
    o["events"] = arr;
    return o;
}

SavedMacro SavedMacro::fromJson(const QJsonObject& obj) {
    SavedMacro m;
    m.id = obj.value("id").toString("macro_1");
    m.name = obj.value("name").toString("Macro");
    m.hotkey = obj.value("hotkey").toString();
    m.createdAt = obj.value("created_at").toString();
    for (const auto& v : obj.value("events").toArray()) {
        if (v.isObject()) m.events.append(MacroEvent::fromJson(v.toObject()));
    }
    return m;
}

QJsonObject AutoPresserKeyItem::toJson() const {
    QJsonObject o;
    o["key"] = key;
    o["enabled"] = enabled;
    o["interval_ms"] = intervalMs;
    o["is_custom"] = isCustom;
    o["use_smart_combo"] = useSmartCombo;
    return o;
}

AutoPresserKeyItem AutoPresserKeyItem::fromJson(const QJsonObject& obj) {
    AutoPresserKeyItem k;
    k.key = obj.value("key").toString("F1");
    k.enabled = obj.value("enabled").toBool(false);
    k.intervalMs = qBound(10, obj.value("interval_ms").toInt(1000), 3600000);
    k.isCustom = obj.value("is_custom").toBool(false);
    k.useSmartCombo = obj.value("use_smart_combo").toBool(false);
    return k;
}

QJsonObject AutoPresserMousePoint::toJson() const {
    QJsonObject o;
    o["name"] = name;
    o["x"] = x;
    o["y"] = y;
    o["button"] = button;
    o["interval_ms"] = intervalMs;
    o["enabled"] = enabled;
    return o;
}

AutoPresserMousePoint AutoPresserMousePoint::fromJson(const QJsonObject& obj) {
    AutoPresserMousePoint m;
    m.name = obj.value("name").toString("Click 1");
    m.x = qBound(-32768, obj.value("x").toInt(500), 32768);
    m.y = qBound(-32768, obj.value("y").toInt(500), 32768);
    QString b = obj.value("button").toString("left").toLower();
    m.button = (b == "left" || b == "right" || b == "middle" || b == "x1" || b == "x2") ? b : "left";
    m.intervalMs = qBound(10, obj.value("interval_ms").toInt(1000), 3600000);
    m.enabled = obj.value("enabled").toBool(true);
    return m;
}

void AutoPresserTab::ensureBuiltinKeys() {
    QList<AutoPresserKeyItem> ordered;
    QSet<QString> used;
    for (const QString& bk : builtin36Keys()) {
        bool found = false;
        for (auto item : keys) {
            if (item.key == bk) {
                item.isCustom = false;
                ordered.append(item);
                used.insert(bk);
                found = true;
                break;
            }
        }
        if (!found) {
            AutoPresserKeyItem item;
            item.key = bk;
            ordered.append(item);
        }
    }
    for (auto item : keys) {
        if (!used.contains(item.key)) {
            item.isCustom = true;
            ordered.append(item);
        }
    }
    keys = ordered;
}

QJsonObject AutoPresserTab::toJson() const {
    QJsonObject o;
    o["tab_id"] = tabId;
    o["title"] = title;
    o["target_process"] = targetProcess;
    o["target_hwnd"] = static_cast<qint64>(targetHwnd);
    o["target_title"] = targetTitle;
    o["run_in_background"] = runInBackground;
    o["key_method"] = keyMethod;
    o["mouse_mode"] = mouseMode;
    o["hold_ms"] = holdMs;
    o["stop_hotkey"] = stopHotkey;
    o["mouse_enabled"] = mouseEnabled;
    QJsonArray kArr;
    for (const auto& k : keys) kArr.append(k.toJson());
    o["keys"] = kArr;
    QJsonArray mArr;
    for (const auto& m : mousePoints) mArr.append(m.toJson());
    o["mouse_points"] = mArr;
    return o;
}

AutoPresserTab AutoPresserTab::fromJson(const QJsonObject& obj) {
    AutoPresserTab t;
    t.tabId = obj.value("tab_id").toString("tab_1");
    t.title = obj.value("title").toString("Tab 1");
    t.targetProcess = obj.value("target_process").toString();
    t.targetHwnd = static_cast<quintptr>(obj.value("target_hwnd").toInteger(0));
    t.targetTitle = obj.value("target_title").toString();
    t.runInBackground = obj.value("run_in_background").toBool(true);
    t.keyMethod = obj.value("key_method").toString("attach_state");
    t.mouseMode = obj.value("mouse_mode").toString("auto");
    t.holdMs = qBound(10, obj.value("hold_ms").toInt(50), 2000);
    t.stopHotkey = obj.value("stop_hotkey").toString("F6");
    t.mouseEnabled = obj.value("mouse_enabled").toBool(false);
    for (const auto& v : obj.value("keys").toArray()) {
        if (v.isObject()) t.keys.append(AutoPresserKeyItem::fromJson(v.toObject()));
    }
    for (const auto& v : obj.value("mouse_points").toArray()) {
        if (v.isObject()) t.mousePoints.append(AutoPresserMousePoint::fromJson(v.toObject()));
    }
    t.ensureBuiltinKeys();
    return t;
}

QJsonObject AppConfig::toJson() const {
    QJsonObject o;
    o["schema_version"] = schemaVersion;
    o["language"] = language;
    o["theme_mode"] = themeMode;
    o["palette_id"] = paletteId;
    o["custom_accent"] = customAccent;
    o["first_run_completed"] = firstRunCompleted;
    o["input_method"] = inputMethod;
    o["mcu_com_port"] = mcuComPort;
    o["test_hotkey"] = testHotkey;
    o["record_mouse_moves"] = recordMouseMoves;
    o["record_mouse_clicks"] = recordMouseClicks;
    o["record_mouse_scroll"] = recordMouseScroll;
    o["record_keyboard"] = recordKeyboard;
    o["mouse_sample_interval_ms"] = mouseSampleIntervalMs;
    o["default_playback_speed"] = defaultPlaybackSpeed;
    o["repeat_mode"] = repeatMode;
    o["default_repeat_count"] = defaultRepeatCount;
    o["repeat_duration_value"] = repeatDurationValue;
    o["repeat_duration_unit"] = repeatDurationUnit;
    o["repeat_delay_enabled"] = repeatDelayEnabled;
    o["repeat_delay_ms"] = repeatDelayMs;
    o["repeat_delay_random"] = repeatDelayRandom;
    o["do_not_activate_on_stop"] = doNotActivateOnStop;
    o["show_floating_playback_pill"] = showFloatingPlaybackPill;
    o["playback_start_delay_ms"] = playbackStartDelayMs;
    o["shutdown_on_finish"] = shutdownOnFinish;
    o["hotkey_record_toggle"] = hotkeyRecordToggle;
    o["hotkey_playback_toggle"] = hotkeyPlaybackToggle;
    o["hotkey_combo_master_toggle"] = hotkeyComboMasterToggle;
    o["hotkey_panic_stop"] = hotkeyPanicStop;
    o["hotkey_autopresser_stop_all"] = hotkeyAutopresserStopAll;
    o["combos_enabled_globally"] = combosEnabledGlobally;
    o["suppress_trigger_key"] = suppressTriggerKey;
    o["run_on_windows_startup"] = runOnWindowsStartup;
    o["notifications_enabled"] = notificationsEnabled;
    o["sound_effects_enabled"] = soundEffectsEnabled;
    o["sound_volume"] = soundVolume;
    o["minimize_to_tray"] = minimizeToTray;
    o["always_on_top"] = alwaysOnTop;
    return o;
}

AppConfig AppConfig::fromJson(const QJsonObject& obj) {
    AppConfig c;
    c.language = obj.value("language").toString("ar");
    c.themeMode = obj.value("theme_mode").toString("dark");
    c.paletteId = obj.value("palette_id").toString("cyber_violet");
    c.customAccent = obj.value("custom_accent").toString("#8B5CF6");
    c.firstRunCompleted = obj.value("first_run_completed").toBool(false);
    c.inputMethod = obj.value("input_method").toString("sendinput_scancode");
    c.mcuComPort = obj.value("mcu_com_port").toString("auto");
    c.testHotkey = obj.value("test_hotkey").toString("F7");
    c.recordMouseMoves = obj.value("record_mouse_moves").toBool(false);
    c.recordMouseClicks = obj.value("record_mouse_clicks").toBool(true);
    c.recordMouseScroll = obj.value("record_mouse_scroll").toBool(true);
    c.recordKeyboard = obj.value("record_keyboard").toBool(true);
    c.mouseSampleIntervalMs = obj.value("mouse_sample_interval_ms").toInt(25);
    c.defaultPlaybackSpeed = obj.value("default_playback_speed").toDouble(1.0);
    c.repeatMode = obj.value("repeat_mode").toString("once");
    c.defaultRepeatCount = obj.value("default_repeat_count").toInt(5);
    c.repeatDurationValue = obj.value("repeat_duration_value").toInt(10);
    c.repeatDurationUnit = obj.value("repeat_duration_unit").toString("minutes");
    c.repeatDelayEnabled = obj.value("repeat_delay_enabled").toBool(false);
    c.repeatDelayMs = obj.value("repeat_delay_ms").toInt(1000);
    c.repeatDelayRandom = obj.value("repeat_delay_random").toBool(false);
    c.doNotActivateOnStop = obj.value("do_not_activate_on_stop").toBool(true);
    c.showFloatingPlaybackPill = obj.value("show_floating_playback_pill").toBool(false);
    c.playbackStartDelayMs = obj.value("playback_start_delay_ms").toInt(0);
    c.shutdownOnFinish = obj.value("shutdown_on_finish").toBool(false);
    c.hotkeyRecordToggle = obj.value("hotkey_record_toggle").toString("F9");
    c.hotkeyPlaybackToggle = obj.value("hotkey_playback_toggle").toString("F10");
    c.hotkeyComboMasterToggle = obj.value("hotkey_combo_master_toggle").toString("F11");
    c.hotkeyPanicStop = obj.value("hotkey_panic_stop").toString("Ctrl + Shift + Alt");
    c.hotkeyAutopresserStopAll = obj.value("hotkey_autopresser_stop_all").toString("F7");
    c.combosEnabledGlobally = obj.value("combos_enabled_globally").toBool(true);
    c.suppressTriggerKey = obj.value("suppress_trigger_key").toBool(true);
    c.runOnWindowsStartup = obj.value("run_on_windows_startup").toBool(false);
    c.notificationsEnabled = obj.value("notifications_enabled").toBool(false);
    c.soundEffectsEnabled = obj.value("sound_effects_enabled").toBool(false);
    c.soundVolume = obj.value("sound_volume").toInt(60);
    c.minimizeToTray = obj.value("minimize_to_tray").toBool(true);
    c.alwaysOnTop = obj.value("always_on_top").toBool(false);
    return c;
}

AppConfig AppConfig::load() {
    QFile f(Paths::configPath());
    if (!f.exists() || !f.open(QIODevice::ReadOnly)) {
        AppConfig cfg;
        cfg.save();
        return cfg;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) {
        AppConfig cfg;
        cfg.save();
        return cfg;
    }
    return fromJson(doc.object());
}

void AppConfig::save() const {
    QFile f(Paths::configPath());
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(toJson()).toJson(QJsonDocument::Indented));
    }
}

QList<SmartCombo> getDefaultSmartCombos() {
    SmartCombo c1;
    c1.id = "combo_example_n";
    c1.name = "N";
    c1.triggerKey = "N";
    c1.triggerScan = 0x31;
    c1.triggerChord = {{0x31, false}};
    c1.mode = "once";
    c1.enabled = false;
    c1.steps = {
        ComboStep{"key_tap", "W", 0x11, false, 20, 0, 3, 500, 500, true, true, ""},
        ComboStep{"wait", "Wait", 0, false, 1000, 0, 3, 500, 500, true, true, ""},
        ComboStep{"key_tap", "Space", 0x39, false, 20, 0, 3, 500, 500, true, true, ""},
    };
    return {c1};
}

QList<MacroEvent> getStarterRecordedMacro() {
    return {
        MacroEvent{"key_down", 40, 0, "W", 0x11, false, 500, 500, "left", "down", 3, 0, 0, true, ""},
        MacroEvent{"key_up", 250, 0, "W", 0x11, false, 500, 500, "left", "down", 3, 0, 0, true, ""},
        MacroEvent{"wait", 1000, 0, "Wait", 0, false, 500, 500, "left", "down", 3, 0, 0, true, ""},
        MacroEvent{"key_tap", 30, 0, "Space", 0x39, false, 500, 500, "left", "down", 3, 0, 0, true, ""},
    };
}

} // namespace vmacro
