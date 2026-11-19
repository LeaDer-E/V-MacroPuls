#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>

namespace vmacro {

struct MacroEvent {
    QString kind = "key_tap";       // key_down, key_up, key_tap, key_hold, mouse_move, mouse_down, mouse_up, mouse_click, mouse_scroll, wait
    int delayMs = 40;
    int holdMs = 0;
    QString key = "Space";
    int scanCode = 0x39;
    bool isExtended = false;
    int x = 500;
    int y = 500;
    QString button = "left";
    QString scrollDir = "down";
    int scrollAmount = 3;
    int dx = 0;
    int dy = 0;
    bool enabled = true;
    QString note;

    QJsonObject toJson() const;
    static MacroEvent fromJson(const QJsonObject& obj);
};

struct ComboStep {
    QString stepType = "key_tap";   // key_tap, key_hold, key_down, key_up, wait, mouse_click, mouse_down, mouse_up, mouse_move, mouse_scroll, open_folder, powershell, cmd, type_text, python_script
    QString value = "Space";
    int scanCode = 0x39;
    bool isExtended = false;
    int delayMs = 20;
    int holdMs = 0;
    int scrollAmount = 3;
    int x = 500;
    int y = 500;
    bool useCoords = true;
    bool enabled = true;
    QString note;

    QJsonObject toJson() const;
    static ComboStep fromJson(const QJsonObject& obj);
    static ComboStep fromMacroEvent(const MacroEvent& ev);
};

struct SmartCombo {
    QString id;
    QString name;
    QString triggerKey;
    int triggerScan = 0;
    bool triggerExtended = false;
    QList<QPair<int, bool>> triggerChord;
    QString mode = "once";                  // once, repeat_hold, toggle_loop
    QString retriggerPolicy = "ignore";     // ignore, restart, queue
    QString targetProcess;
    QStringList excludedProcesses;
    QString stopHotkey;
    int startDelayMs = 1000;
    bool enabled = false;
    QList<ComboStep> steps;

    QJsonObject toJson() const;
    static SmartCombo fromJson(const QJsonObject& obj);
    QString sidebarLabel(bool isAr = true) const;
};

struct SavedMacro {
    QString id;
    QString name;
    QString hotkey;
    QString createdAt;
    QList<MacroEvent> events;

    QJsonObject toJson() const;
    static SavedMacro fromJson(const QJsonObject& obj);
};

struct AutoPresserKeyItem {
    QString key = "F1";
    bool enabled = false;
    int intervalMs = 1000;
    bool isCustom = false;
    bool useSmartCombo = false;

    QJsonObject toJson() const;
    static AutoPresserKeyItem fromJson(const QJsonObject& obj);
};

struct AutoPresserMousePoint {
    QString name = "Click 1";
    int x = 500;
    int y = 500;
    QString button = "left";
    int intervalMs = 1000;
    bool enabled = true;

    QJsonObject toJson() const;
    static AutoPresserMousePoint fromJson(const QJsonObject& obj);
};

struct AutoPresserTab {
    QString tabId = "tab_1";
    QString title = "Tab 1";
    QString targetProcess;
    quintptr targetHwnd = 0;
    QString targetTitle;
    bool runInBackground = true;
    QString keyMethod = "attach_state";
    QString mouseMode = "auto";
    int holdMs = 50;
    QString stopHotkey = "F6";
    bool mouseEnabled = false;
    QList<AutoPresserKeyItem> keys;
    QList<AutoPresserMousePoint> mousePoints;

    void ensureBuiltinKeys();
    QJsonObject toJson() const;
    static AutoPresserTab fromJson(const QJsonObject& obj);
};

struct AppConfig {
    int schemaVersion = 9;
    QString language = "ar";
    QString themeMode = "dark";
    QString paletteId = "cyber_violet";
    QString customAccent = "#8B5CF6";
    bool firstRunCompleted = false;

    QString inputMethod = "sendinput_scancode";
    QString mcuComPort = "auto";
    QString testHotkey = "F7";

    bool recordMouseMoves = false;
    bool recordMouseClicks = true;
    bool recordMouseScroll = true;
    bool recordKeyboard = true;
    int mouseSampleIntervalMs = 25;

    double defaultPlaybackSpeed = 1.0;
    QString repeatMode = "once";
    int defaultRepeatCount = 5;
    int repeatDurationValue = 10;
    QString repeatDurationUnit = "minutes";

    bool repeatDelayEnabled = false;
    int repeatDelayMs = 1000;
    bool repeatDelayRandom = false;

    bool doNotActivateOnStop = true;
    bool showFloatingPlaybackPill = false;
    int playbackStartDelayMs = 0;
    bool shutdownOnFinish = false;

    QString hotkeyRecordToggle = "F9";
    QString hotkeyPlaybackToggle = "F10";
    QString hotkeyComboMasterToggle = "F11";
    QString hotkeyPanicStop = "Ctrl + Shift + Alt";
    QString hotkeyAutopresserStopAll = "F7";

    bool combosEnabledGlobally = true;
    bool suppressTriggerKey = true;
    bool runOnWindowsStartup = false;

    bool notificationsEnabled = false;
    bool soundEffectsEnabled = false;
    int soundVolume = 60;
    bool minimizeToTray = true;
    bool alwaysOnTop = false;

    QJsonObject toJson() const;
    static AppConfig fromJson(const QJsonObject& obj);
    static AppConfig load();
    void save() const;
};

QList<SmartCombo> getDefaultSmartCombos();
QList<MacroEvent> getStarterRecordedMacro();
const QStringList& builtin36Keys();

} // namespace vmacro
