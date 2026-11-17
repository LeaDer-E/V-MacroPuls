#include "AppState.h"
#include "I18n.h"
#include "Theme.h"
#include <QVariant>

namespace vmacro {

AppState::AppState(QObject* parent) : QObject(parent) {
    config = AppConfig::load();
    combosActive = config.combosEnabledGlobally;
    I18n::instance().setLanguage(config.language);
    Theme::instance().setTheme(config.themeMode, config.paletteId, config.customAccent);
}

void AppState::setLanguage(const QString& langCode) {
    if (config.language == langCode) return;
    config.language = langCode;
    config.save();
    I18n::instance().setLanguage(langCode);
    emit languageChanged(langCode);
    emit configChanged();
}

void AppState::setTheme(const QString& mode, const QString& paletteId, const QString& customAccent) {
    if (!mode.isEmpty()) config.themeMode = mode;
    if (!paletteId.isEmpty()) config.paletteId = paletteId;
    if (!customAccent.isEmpty()) config.customAccent = customAccent;
    config.save();
    Theme::instance().setTheme(config.themeMode, config.paletteId, config.customAccent);
    emit themeChanged();
    emit configChanged();
}

void AppState::updateSetting(const QString& key, const QVariant& value) {
    if (key == "input_method") config.inputMethod = value.toString();
    else if (key == "mcu_com_port") config.mcuComPort = value.toString();
    else if (key == "record_mouse_moves") config.recordMouseMoves = value.toBool();
    else if (key == "record_mouse_clicks") config.recordMouseClicks = value.toBool();
    else if (key == "record_mouse_scroll") config.recordMouseScroll = value.toBool();
    else if (key == "record_keyboard") config.recordKeyboard = value.toBool();
    else if (key == "default_playback_speed") config.defaultPlaybackSpeed = value.toDouble();
    else if (key == "repeat_mode") config.repeatMode = value.toString();
    else if (key == "default_repeat_count") config.defaultRepeatCount = value.toInt();
    else if (key == "repeat_duration_value") config.repeatDurationValue = value.toInt();
    else if (key == "repeat_duration_unit") config.repeatDurationUnit = value.toString();
    else if (key == "repeat_delay_enabled") config.repeatDelayEnabled = value.toBool();
    else if (key == "repeat_delay_ms") config.repeatDelayMs = value.toInt();
    else if (key == "repeat_delay_random") config.repeatDelayRandom = value.toBool();
    else if (key == "do_not_activate_on_stop") config.doNotActivateOnStop = value.toBool();
    else if (key == "show_floating_playback_pill") config.showFloatingPlaybackPill = value.toBool();
    else if (key == "playback_start_delay_ms") config.playbackStartDelayMs = value.toInt();
    else if (key == "shutdown_on_finish") config.shutdownOnFinish = value.toBool();
    else if (key == "hotkey_record_toggle") config.hotkeyRecordToggle = value.toString();
    else if (key == "hotkey_playback_toggle") config.hotkeyPlaybackToggle = value.toString();
    else if (key == "hotkey_combo_master_toggle") config.hotkeyComboMasterToggle = value.toString();
    else if (key == "hotkey_panic_stop") config.hotkeyPanicStop = value.toString();
    else if (key == "hotkey_autopresser_stop_all") config.hotkeyAutopresserStopAll = value.toString();
    else if (key == "combos_enabled_globally") {
        config.combosEnabledGlobally = value.toBool();
        combosActive = config.combosEnabledGlobally;
    }
    else if (key == "suppress_trigger_key") config.suppressTriggerKey = value.toBool();
    else if (key == "run_on_windows_startup") config.runOnWindowsStartup = value.toBool();
    else if (key == "minimize_to_tray") config.minimizeToTray = value.toBool();
    else if (key == "always_on_top") config.alwaysOnTop = value.toBool();
    else if (key == "sound_effects_enabled") config.soundEffectsEnabled = value.toBool();
    else if (key == "first_run_completed") config.firstRunCompleted = value.toBool();

    config.save();
    emit settingsChanged(key, value);
    emit configChanged();
}

void AppState::registerStopButtonRectProvider(std::function<QRect()> fn) {
    m_stopRectProviders.append(fn);
}

bool AppState::isPointOnStopButton(int x, int y) const {
    for (const auto& fn : m_stopRectProviders) {
        if (fn) {
            QRect r = fn();
            if (!r.isNull() && r.contains(x, y)) return true;
        }
    }
    return false;
}

} // namespace vmacro
