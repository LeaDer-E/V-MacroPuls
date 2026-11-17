#include "MacroEngine.h"
#include "AutoPresserEngine.h"
#include "PythonScriptRunner.h"
#include "../core/AppState.h"
#include "../core/I18n.h"
#include "../core/Paths.h"
#include "../platform/Win32Input.h"
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QProcess>
#include <QRandomGenerator>
#include <QUuid>
#include <algorithm>
#include <thread>

namespace vmacro {

MacroEngine::MacroEngine(AppState* state, QObject* parent)
    : QObject(parent), m_state(state) {
    m_playbackStop = std::make_shared<std::atomic<bool>>(true);
}

MacroEngine::~MacroEngine() {
    shutdown();
}

double MacroEngine::steadyNowSec() {
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count();
}

void MacroEngine::startHooks() {
    Win32Input::startGlobalHooks(
        [this](int scan, bool ext, bool isDown, bool isInjected) -> bool {
            return onGlobalKey(scan, ext, isDown, isInjected);
        },
        [this](const QString& evType, int x, int y, const QString& btnOrDir, int dx, int dy) {
            onGlobalMouse(evType, x, y, btnOrDir, dx, dy);
        }
    );
}

void MacroEngine::shutdown() {
    stopAll();
    Win32Input::stopGlobalHooks();
}

void MacroEngine::loadAllData() {
    // Load Smart Combos
    QList<SmartCombo> combos;
    QFile cf(Paths::combosFile());
    if (cf.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(cf.readAll());
        if (doc.isArray()) {
            for (const QJsonValue& v : doc.array()) {
                if (v.isObject()) combos.append(SmartCombo::fromJson(v.toObject()));
            }
        }
    }
    if (combos.isEmpty()) {
        combos = getDefaultSmartCombos();
    }

    // Load Saved Macros
    QList<SavedMacro> macros;
    QFile mf(Paths::macrosFile());
    if (mf.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(mf.readAll());
        if (doc.isArray()) {
            for (const QJsonValue& v : doc.array()) {
                if (v.isObject()) macros.append(SavedMacro::fromJson(v.toObject()));
            }
        }
    }

    if (m_state) {
        m_state->smartCombos = combos;
        m_state->savedMacros = macros;
        m_state->recordedEvents = getStarterRecordedMacro();
        rebuildTriggerMap();
        emit m_state->combosListUpdated();
        emit m_state->macrosLibraryUpdated();
        emit m_state->eventsUpdated();
    }
}

void MacroEngine::saveCombos() {
    if (!m_state) return;
    QJsonArray arr;
    for (const auto& c : m_state->smartCombos) {
        arr.append(c.toJson());
    }
    QFile f(Paths::combosFile());
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    }
    rebuildTriggerMap();
}

void MacroEngine::saveSavedMacros() {
    if (!m_state) return;
    QJsonArray arr;
    for (const auto& m : m_state->savedMacros) {
        arr.append(m.toJson());
    }
    QFile f(Paths::macrosFile());
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    }
}

bool MacroEngine::updateSavedMacro(const QString& macroId, const QString& name, const QList<MacroEvent>& events) {
    if (!m_state) return false;
    for (auto& m : m_state->savedMacros) {
        if (m.id == macroId) {
            if (!name.trimmed().isEmpty()) m.name = name.trimmed();
            m.events = events;
            saveSavedMacros();
            emit m_state->macrosLibraryUpdated();
            return true;
        }
    }
    return false;
}

SavedMacro MacroEngine::saveCurrentMacroToLibrary(const QString& name, const QString& hotkey) {
    SavedMacro sm;
    sm.id = QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
    sm.name = name.trimmed().isEmpty() ? QStringLiteral("Macro") : name.trimmed();
    sm.hotkey = hotkey.trimmed();
    sm.createdAt = QDateTime::currentDateTime().toString(Qt::ISODate);
    if (m_state) {
        sm.events = m_state->recordedEvents;
        m_state->savedMacros.prepend(sm);
        saveSavedMacros();
        emit m_state->macrosLibraryUpdated();
    }
    return sm;
}

bool MacroEngine::updateSavedMacroHotkey(const QString& macroId, const QString& newHotkey) {
    if (!m_state) return false;
    for (auto& m : m_state->savedMacros) {
        if (m.id == macroId) {
            m.hotkey = newHotkey.trimmed();
            saveSavedMacros();
            emit m_state->macrosLibraryUpdated();
            return true;
        }
    }
    return false;
}

bool MacroEngine::deleteSavedMacro(const QString& macroId) {
    if (!m_state) return false;
    for (int i = 0; i < m_state->savedMacros.size(); ++i) {
        if (m_state->savedMacros[i].id == macroId) {
            m_state->savedMacros.removeAt(i);
            saveSavedMacros();
            emit m_state->macrosLibraryUpdated();
            return true;
        }
    }
    return false;
}

SmartCombo MacroEngine::convertRecordingToSmartCombo(const QString& name, const QList<MacroEvent>& events) {
    SmartCombo sc;
    sc.id = QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
    sc.name = name.trimmed().isEmpty() ? QStringLiteral("Converted Macro") : name.trimmed();
    sc.mode = QStringLiteral("once");
    sc.enabled = true;
    for (const auto& ev : events) {
        sc.steps.append(ComboStep::fromMacroEvent(ev));
    }
    if (m_state) {
        m_state->smartCombos.prepend(sc);
        saveCombos();
        emit m_state->combosListUpdated();
    }
    return sc;
}

bool MacroEngine::exportAllBackup(const QString& filePath) const {
    if (!m_state) return false;
    QJsonObject root;
    root["version"] = QStringLiteral("3.9.0");
    root["exported_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["config"] = m_state->config.toJson();

    QJsonArray combosArr;
    for (const auto& c : m_state->smartCombos) combosArr.append(c.toJson());
    root["smart_combos"] = combosArr;

    QJsonArray macrosArr;
    for (const auto& m : m_state->savedMacros) macrosArr.append(m.toJson());
    root["saved_macros"] = macrosArr;

    QJsonArray scriptsArr;
    for (const auto& s : PythonScriptRunner::loadUserScripts()) {
        QJsonObject obj;
        obj["key"] = s.key;
        obj["name"] = s.name;
        obj["code"] = s.code;
        scriptsArr.append(obj);
    }
    root["python_scripts"] = scriptsArr;

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool MacroEngine::importAllBackup(const QString& filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) return false;
    QJsonObject root = doc.object();

    if (m_state) {
        if (root.contains("config") && root["config"].isObject()) {
            m_state->config = AppConfig::fromJson(root["config"].toObject());
            m_state->config.save();
            emit m_state->configChanged();
            emit m_state->themeChanged();
            emit m_state->languageChanged(m_state->config.language);
        }
        if (root.contains("smart_combos") && root["smart_combos"].isArray()) {
            QList<SmartCombo> combos;
            for (const auto& v : root["smart_combos"].toArray()) {
                if (v.isObject()) combos.append(SmartCombo::fromJson(v.toObject()));
            }
            m_state->smartCombos = combos;
            saveCombos();
            emit m_state->combosListUpdated();
        }
        if (root.contains("saved_macros") && root["saved_macros"].isArray()) {
            QList<SavedMacro> macros;
            for (const auto& v : root["saved_macros"].toArray()) {
                if (v.isObject()) macros.append(SavedMacro::fromJson(v.toObject()));
            }
            m_state->savedMacros = macros;
            saveSavedMacros();
            emit m_state->macrosLibraryUpdated();
        }
        if (root.contains("python_scripts") && root["python_scripts"].isArray()) {
            for (const auto& v : root["python_scripts"].toArray()) {
                if (v.isObject()) {
                    QJsonObject obj = v.toObject();
                    PythonScriptRunner::upsertUserScript(
                        obj["name"].toString(),
                        obj["code"].toString(),
                        obj["key"].toString()
                    );
                }
            }
            emit m_state->pythonScriptsUpdated();
        }
    }
    return true;
}

void MacroEngine::rebuildTriggerMap() {
    QMutexLocker locker(&m_lock);
    m_activeCombos.clear();
    if (!m_state) return;
    for (auto c : m_state->smartCombos) {
        if (!c.enabled) continue;
        if (c.triggerChord.isEmpty() && !c.triggerKey.trimmed().isEmpty()) {
            c.triggerChord = Win32Input::parseChordString(c.triggerKey);
        }
        if (c.triggerChord.isEmpty() && c.triggerScan > 0) {
            c.triggerChord.append({c.triggerScan, c.triggerExtended});
        }
        if (!c.triggerChord.isEmpty()) {
            m_activeCombos.append(c);
        }
    }
}

void MacroEngine::pruneStaleDownKeys(double nowSec) {
    QList<KeyId> toRemove;
    for (auto it = m_downKeys.cbegin(); it != m_downKeys.cend(); ++it) {
        if (nowSec - it.value() > 10.0) {
            toRemove.append(it.key());
        }
    }
    for (const auto& k : toRemove) {
        m_downKeys.remove(k);
        m_suppressedDownKeys.remove(k);
    }
}

bool MacroEngine::isKeyActiveForChord(const KeyId& k, double nowSec) const {
    auto it = m_downKeys.constFind(k);
    if (it == m_downKeys.constEnd()) return false;
    return (nowSec - it.value()) <= 5.0;
}

bool MacroEngine::chordMatchesDownEvent(const QList<KeyId>& chord, const KeyId& currentKid, double nowSec, bool ignoreExtraModifiers) const {
    if (chord.isEmpty()) return false;
    bool containsCurrent = false;
    for (const auto& ck : chord) {
        if (ck == currentKid) {
            containsCurrent = true;
        } else if (!isKeyActiveForChord(ck, nowSec)) {
            return false;
        }
    }
    if (!containsCurrent) return false;
    if (!ignoreExtraModifiers) {
        for (auto it = m_downKeys.cbegin(); it != m_downKeys.cend(); ++it) {
            if ((nowSec - it.value()) <= 5.0 && Win32Input::isModifierScancode(it.key().first)) {
                if (!chord.contains(it.key())) return false;
            }
        }
    }
    return true;
}

bool MacroEngine::onGlobalKey(int scan, bool ext, bool isDown, bool isInjected) {
    if (!m_state || isInjected) return false;
    if (m_state->isCapturingKey) return false;

    const double now = steadyNowSec();
    const KeyId kid(scan, ext);

    QMutexLocker locker(&m_lock);
    pruneStaleDownKeys(now);

    if (isDown) {
        m_downKeys[kid] = now;
    } else {
        m_downKeys.remove(kid);
        if (m_suppressedDownKeys.remove(kid)) {
            return true;
        }
    }

    // 1. Panic Stop Hotkey
    const auto panicChord = Win32Input::parseChordString(m_state->config.hotkeyPanicStop);
    if (isDown && !panicChord.isEmpty() && chordMatchesDownEvent(panicChord, kid, now, true)) {
        locker.unlock();
        QMetaObject::invokeMethod(this, [this]() { stopAll(); }, Qt::QueuedConnection);
        return true;
    }

    // 2. Autopresser Stop-All Hotkey
    const auto apStopChord = Win32Input::parseChordString(m_state->config.hotkeyAutopresserStopAll);
    if (isDown && !apStopChord.isEmpty() && chordMatchesDownEvent(apStopChord, kid, now, true)) {
        if (m_apEngine && m_apEngine->anyTabRunning()) {
            locker.unlock();
            QMetaObject::invokeMethod(this, [this]() {
                if (m_apEngine) m_apEngine->stopAllTabs();
            }, Qt::QueuedConnection);
            return true;
        }
    }

    // 3. Record / Playback / Master Combo Hotkeys
    const auto recChord = Win32Input::parseChordString(m_state->config.hotkeyRecordToggle);
    if (isDown && !recChord.isEmpty() && chordMatchesDownEvent(recChord, kid, now, true)) {
        bool wasRec = m_state->isRecording;
        if (wasRec) stripStopHotkeyFromRecording(recChord);
        locker.unlock();
        QMetaObject::invokeMethod(this, [this]() { toggleRecording(false); }, Qt::QueuedConnection);
        return true;
    }

    const auto playChord = Win32Input::parseChordString(m_state->config.hotkeyPlaybackToggle);
    if (isDown && !playChord.isEmpty() && chordMatchesDownEvent(playChord, kid, now, true)) {
        locker.unlock();
        QMetaObject::invokeMethod(this, [this]() { togglePlayback(); }, Qt::QueuedConnection);
        return true;
    }

    const auto masterChord = Win32Input::parseChordString(m_state->config.hotkeyComboMasterToggle);
    if (isDown && !masterChord.isEmpty() && chordMatchesDownEvent(masterChord, kid, now, true)) {
        locker.unlock();
        QMetaObject::invokeMethod(this, [this]() { toggleCombosMaster(); }, Qt::QueuedConnection);
        return true;
    }

    // 4. Priority Stop Hotkeys for currently running Smart Combos
    if (isDown) {
        for (const auto& c : m_activeCombos) {
            if (!m_runningComboStops.contains(c.id)) continue;
            if (c.stopHotkey.trimmed().isEmpty()) continue;
            auto stopChord = Win32Input::parseChordString(c.stopHotkey);
            if (!stopChord.isEmpty() && chordMatchesDownEvent(stopChord, kid, now, true)) {
                QString cid = c.id;
                locker.unlock();
                stopCombo(cid);
                return true;
            }
        }
    }

    // 5. Recording capture
    if (m_state->isRecording && m_state->config.recordKeyboard) {
        int delayMs = (m_lastEventSec > 0.0) ? qMax(1, static_cast<int>((now - m_lastEventSec) * 1000.0)) : 20;
        m_lastEventSec = now;
        MacroEvent ev;
        ev.kind = isDown ? QStringLiteral("key_down") : QStringLiteral("key_up");
        ev.delayMs = delayMs;
        ev.scanCode = scan;
        ev.isExtended = ext;
        ev.key = Win32Input::labelForScancode(scan, ext);
        m_state->recordedEvents.append(ev);
        if (now - m_lastCountEmitSec > 0.15) {
            m_lastCountEmitSec = now;
            int cnt = m_state->recordedEvents.size();
            QMetaObject::invokeMethod(m_state, [s = m_state, cnt]() {
                emit s->recordingCountChanged(cnt);
            }, Qt::QueuedConnection);
        }
        return false;
    }

    // 6. Smart Combos dispatch (never trigger on self-app!)
    if (!m_state->combosActive || !m_state->config.combosEnabledGlobally) return false;
    if (Win32Input::isOwnAppForeground()) return false;

    const QString fgProc = Win32Input::getForegroundProcessName().toLower();

    // Release check for repeat_hold
    if (!isDown) {
        QList<QString> toStop;
        for (auto it = m_activeToggleHoldChords.cbegin(); it != m_activeToggleHoldChords.cend(); ++it) {
            if (it.value().contains(kid)) {
                toStop.append(it.key());
            }
        }
        for (const auto& cid : toStop) {
            m_activeToggleHoldChords.remove(cid);
            if (m_runningComboStops.contains(cid)) {
                m_runningComboStops[cid]->store(true);
            }
        }
    }

    if (isDown) {
        for (const auto& c : m_activeCombos) {
            if (!chordMatchesDownEvent(c.triggerChord, kid, now, false)) continue;

            // Process filter
            if (!c.targetProcess.trimmed().isEmpty()) {
                QString target = c.targetProcess.trimmed().toLower();
                if (!fgProc.contains(target)) continue;
            }
            bool excluded = false;
            for (const auto& ex : c.excludedProcesses) {
                if (!ex.trimmed().isEmpty() && fgProc.contains(ex.trimmed().toLower())) {
                    excluded = true;
                    break;
                }
            }
            if (excluded) continue;

            // Toggle loop stop if already running
            if (c.mode == QStringLiteral("toggle_loop") && m_runningComboStops.contains(c.id)) {
                m_runningComboStops[c.id]->store(true);
                m_runningComboStops.remove(c.id);
                if (m_state->config.suppressTriggerKey) {
                    m_suppressedDownKeys.insert(kid);
                    return true;
                }
                return false;
            }

            if (c.retriggerPolicy == QStringLiteral("ignore") && m_runningComboStops.contains(c.id)) {
                return false;
            }
            if (c.retriggerPolicy == QStringLiteral("restart") && m_runningComboStops.contains(c.id)) {
                m_runningComboStops[c.id]->store(true);
            }

            if (c.mode == QStringLiteral("repeat_hold")) {
                m_activeToggleHoldChords[c.id] = c.triggerChord;
            }

            SmartCombo copy = c;
            locker.unlock();
            executeSmartCombo(copy, copy.triggerChord, 0, false);
            if (m_state->config.suppressTriggerKey) {
                QMutexLocker lk2(&m_lock);
                m_suppressedDownKeys.insert(kid);
                return true;
            }
            return false;
        }
    }

    return false;
}

void MacroEngine::onGlobalMouse(const QString& evType, int x, int y, const QString& btnOrDir, int dx, int dy) {
    if (!m_state || !m_state->isRecording) return;
    const double now = steadyNowSec();

    QMutexLocker locker(&m_lock);
    if (evType == QStringLiteral("move")) {
        if (!m_state->config.recordMouseMoves) return;
        int minMs = qMax(5, m_state->config.mouseSampleIntervalMs);
        if ((now - m_lastMouseSampleSec) * 1000.0 < minMs) return;
        m_lastMouseSampleSec = now;
    } else if (evType == QStringLiteral("down") || evType == QStringLiteral("up")) {
        if (!m_state->config.recordMouseClicks) return;
    } else if (evType == QStringLiteral("scroll")) {
        if (!m_state->config.recordMouseScroll) return;
    }

    int delayMs = (m_lastEventSec > 0.0) ? qMax(1, static_cast<int>((now - m_lastEventSec) * 1000.0)) : 20;
    m_lastEventSec = now;

    MacroEvent ev;
    ev.delayMs = delayMs;
    ev.x = x;
    ev.y = y;
    if (evType == QStringLiteral("move")) {
        ev.kind = QStringLiteral("mouse_move");
    } else if (evType == QStringLiteral("down")) {
        ev.kind = QStringLiteral("mouse_down");
        ev.button = btnOrDir;
    } else if (evType == QStringLiteral("up")) {
        ev.kind = QStringLiteral("mouse_up");
        ev.button = btnOrDir;
    } else if (evType == QStringLiteral("scroll")) {
        ev.kind = QStringLiteral("mouse_scroll");
        ev.scrollDir = btnOrDir;
        ev.scrollAmount = qMax(1, qAbs(dy > 0 ? dy : (dx != 0 ? dx : 3)));
    }
    m_state->recordedEvents.append(ev);

    if (now - m_lastCountEmitSec > 0.15) {
        m_lastCountEmitSec = now;
        int cnt = m_state->recordedEvents.size();
        QMetaObject::invokeMethod(m_state, [s = m_state, cnt]() {
            emit s->recordingCountChanged(cnt);
        }, Qt::QueuedConnection);
    }
}

void MacroEngine::stripStopHotkeyFromRecording(const QList<KeyId>& recChord) {
    if (!m_state || recChord.isEmpty()) return;
    while (!m_state->recordedEvents.isEmpty()) {
        const auto& last = m_state->recordedEvents.last();
        if (last.kind == QStringLiteral("key_down") || last.kind == QStringLiteral("key_up")) {
            if (recChord.contains(KeyId(last.scanCode, last.isExtended))) {
                m_state->recordedEvents.removeLast();
                continue;
            }
        }
        break;
    }
}

void MacroEngine::stripStopMouseClickFromRecording() {
    if (!m_state) return;
    int removed = 0;
    while (!m_state->recordedEvents.isEmpty() && removed < 2) {
        const auto& last = m_state->recordedEvents.last();
        if (last.kind == QStringLiteral("mouse_down") || last.kind == QStringLiteral("mouse_up")) {
            m_state->recordedEvents.removeLast();
            ++removed;
            continue;
        }
        break;
    }
}

void MacroEngine::toggleRecording(bool viaMouse) {
    if (!m_state) return;
    if (m_state->isRecording) {
        stopRecording(viaMouse);
    } else {
        startRecording();
    }
}

void MacroEngine::startRecording() {
    if (!m_state || m_state->isRecording) return;
    if (m_state->isPlaying) stopAll();
    {
        QMutexLocker locker(&m_lock);
        m_state->recordedEvents.clear();
        m_lastEventSec = steadyNowSec();
        m_lastMouseSampleSec = m_lastEventSec;
        m_lastCountEmitSec = m_lastEventSec;
    }
    m_state->isRecording = true;
    emit m_state->recordingStateChanged(true);
    emit m_state->eventsUpdated();
    emit m_state->soundRequested(QStringLiteral("start"));
}

void MacroEngine::stopRecording(bool stoppedViaMouse) {
    if (!m_state || !m_state->isRecording) return;
    {
        QMutexLocker locker(&m_lock);
        m_state->isRecording = false;
        if (stoppedViaMouse) {
            stripStopMouseClickFromRecording();
        }
    }
    emit m_state->recordingStateChanged(false);
    emit m_state->eventsUpdated();
    emit m_state->soundRequested(QStringLiteral("stop"));
}

void MacroEngine::togglePlayback() {
    if (!m_state) return;
    if (m_state->isPlaying) {
        m_playbackStop->store(true);
    } else {
        int rep = 1;
        if (m_state->config.repeatMode == QStringLiteral("count")) {
            rep = qMax(1, m_state->config.defaultRepeatCount);
        } else if (m_state->config.repeatMode == QStringLiteral("duration")) {
            rep = 999999;
        }
        startPlayback(m_state->recordedEvents, m_state->config.defaultPlaybackSpeed, rep, false);
    }
}

void MacroEngine::releaseAllPlaybackHeldInputs() {
    QMutexLocker locker(&m_lock);
    for (const auto& k : m_playbackHeldKeys) {
        Win32Input::sendScancodeKey(k.first, k.second, true, m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode"));
    }
    m_playbackHeldKeys.clear();
    for (const auto& b : m_playbackHeldMouse) {
        Win32Input::sendMouseButton(0, 0, b, QStringLiteral("up"), false, 0, m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode"));
    }
    m_playbackHeldMouse.clear();
}

void MacroEngine::triggerSystemShutdown() {
#ifdef _WIN32
    QProcess::startDetached(QStringLiteral("shutdown"), {QStringLiteral("/s"), QStringLiteral("/t"), QStringLiteral("5")});
#else
    QProcess::startDetached(QStringLiteral("systemctl"), {QStringLiteral("poweroff")});
#endif
}

void MacroEngine::startPlayback(const QList<MacroEvent>& events, double speed, int repeatCount, bool allowSelfTest) {
    if (!m_state || events.isEmpty() || m_state->isPlaying) return;
    if (m_state->isRecording) stopRecording(true);

    auto stopFlag = std::make_shared<std::atomic<bool>>(false);
    m_playbackStop = stopFlag;
    m_state->isPlaying = true;
    emit m_state->playbackStateChanged(true);
    emit m_state->soundRequested(QStringLiteral("start"));

    const AppConfig cfg = m_state->config;
    const double spd = qMax(0.1, speed);

    std::thread([this, events, spd, repeatCount, allowSelfTest, stopFlag, cfg]() {
        // Optional start delay
        if (cfg.playbackStartDelayMs > 0) {
            if (!sleepWithStop(cfg.playbackStartDelayMs / 1000.0, stopFlag.get())) {
                // cancelled during start delay
            }
        }

        double maxDurationSec = -1.0;
        if (cfg.repeatMode == QStringLiteral("duration")) {
            double mult = 60.0;
            if (cfg.repeatDurationUnit == QStringLiteral("seconds")) mult = 1.0;
            else if (cfg.repeatDurationUnit == QStringLiteral("hours")) mult = 3600.0;
            maxDurationSec = qMax(1.0, cfg.repeatDurationValue * mult);
        }
        const double startTime = steadyNowSec();

        for (int r = 0; r < repeatCount; ++r) {
            if (stopFlag->load()) break;
            if (maxDurationSec > 0 && (steadyNowSec() - startTime) >= maxDurationSec) break;

            for (const auto& ev : events) {
                if (stopFlag->load()) break;
                if (!ev.enabled) continue;
                if (maxDurationSec > 0 && (steadyNowSec() - startTime) >= maxDurationSec) break;

                if (ev.delayMs > 0) {
                    if (!sleepWithStop((ev.delayMs / 1000.0) / spd, stopFlag.get())) break;
                }
                if (stopFlag->load()) break;
                executeMacroEvent(ev, stopFlag.get(), allowSelfTest);
            }

            if (r + 1 < repeatCount && !stopFlag->load() && cfg.repeatDelayEnabled) {
                int delayMs = cfg.repeatDelayMs;
                if (cfg.repeatDelayRandom && delayMs > 0) {
                    delayMs = QRandomGenerator::global()->bounded(0, delayMs + 1);
                }
                if (delayMs > 0) {
                    sleepWithStop(delayMs / 1000.0, stopFlag.get());
                }
            }
        }

        releaseAllPlaybackHeldInputs();
        bool completedNaturally = !stopFlag->load();
        stopFlag->store(true);

        if (m_state) {
            QMetaObject::invokeMethod(m_state, [s = m_state, completedNaturally, cfg, this]() {
                s->isPlaying = false;
                emit s->playbackStateChanged(false);
                emit s->soundRequested(QStringLiteral("stop"));
                if (completedNaturally && cfg.shutdownOnFinish) {
                    triggerSystemShutdown();
                }
            }, Qt::QueuedConnection);
        }
    }).detach();
}

void MacroEngine::toggleCombosMaster() {
    if (!m_state) return;
    m_state->combosActive = !m_state->combosActive;
    m_state->config.combosEnabledGlobally = m_state->combosActive;
    m_state->config.save();
    emit m_state->combosMasterChanged(m_state->combosActive);
    emit m_state->toastRequested(
        I18n::tr(m_state->combosActive ? QStringLiteral("toast_combos_enabled") : QStringLiteral("toast_combos_paused")),
        QStringLiteral("info")
    );
}

void MacroEngine::executeSingleStepAfter(const ComboStep& step, int delayMs, bool allowSelfTest) {
    auto stopFlag = std::make_shared<std::atomic<bool>>(false);
    std::thread([this, step, delayMs, allowSelfTest, stopFlag]() {
        if (delayMs > 0) sleepWithStop(delayMs / 1000.0, stopFlag.get());
        if (!stopFlag->load() && step.enabled) {
            executeComboStep(step, stopFlag.get(), allowSelfTest);
        }
    }).detach();
}

void MacroEngine::executeSmartCombo(const SmartCombo& combo, const QList<QPair<int, bool>>& releaseChord, int initialDelayMs, bool allowSelfTest) {
    auto stopFlag = std::make_shared<std::atomic<bool>>(false);
    {
        QMutexLocker locker(&m_lock);
        m_runningComboStops[combo.id] = stopFlag;
    }

    std::thread([this, combo, releaseChord, initialDelayMs, allowSelfTest, stopFlag]() {
        if (initialDelayMs > 0) {
            sleepWithStop(initialDelayMs / 1000.0, stopFlag.get());
        } else if (combo.startDelayMs > 0 && allowSelfTest) {
            sleepWithStop(combo.startDelayMs / 1000.0, stopFlag.get());
        }

        // Wait briefly for trigger chord release so modifiers don't bleed into combo
        if (!releaseChord.isEmpty()) {
            const double deadline = steadyNowSec() + 0.35;
            while (steadyNowSec() < deadline && !stopFlag->load()) {
                bool anyDown = false;
                {
                    QMutexLocker lk(&m_lock);
                    for (const auto& k : releaseChord) {
                        if (m_downKeys.contains(k)) { anyDown = true; break; }
                    }
                }
                if (!anyDown) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        const bool loop = (combo.mode == QStringLiteral("repeat_hold") || combo.mode == QStringLiteral("toggle_loop"));
        do {
            for (const auto& st : combo.steps) {
                if (stopFlag->load()) break;
                if (!st.enabled) continue;
                if (st.delayMs > 0) {
                    if (!sleepWithStop(st.delayMs / 1000.0, stopFlag.get())) break;
                }
                if (stopFlag->load()) break;
                executeComboStep(st, stopFlag.get(), allowSelfTest);
            }
            if (loop && !stopFlag->load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }
        } while (loop && !stopFlag->load());

        {
            QMutexLocker locker(&m_lock);
            if (m_runningComboStops.value(combo.id) == stopFlag) {
                m_runningComboStops.remove(combo.id);
            }
        }
    }).detach();
}

void MacroEngine::stopCombo(const QString& comboId) {
    QMutexLocker locker(&m_lock);
    if (m_runningComboStops.contains(comboId)) {
        m_runningComboStops[comboId]->store(true);
        m_runningComboStops.remove(comboId);
    }
    m_activeToggleHoldChords.remove(comboId);
}

void MacroEngine::stopAll() {
    if (m_state && m_state->isRecording) stopRecording(true);
    if (m_playbackStop) m_playbackStop->store(true);
    {
        QMutexLocker locker(&m_lock);
        for (auto& st : m_runningComboStops) {
            if (st) st->store(true);
        }
        m_runningComboStops.clear();
        m_activeToggleHoldChords.clear();
    }
    if (m_apEngine) {
        m_apEngine->stopAllTabs();
    }
    releaseAllPlaybackHeldInputs();
    if (m_state) {
        m_state->isPlaying = false;
        emit m_state->playbackStateChanged(false);
    }
}

bool MacroEngine::sleepWithStop(double seconds, std::atomic<bool>* stopFlag) {
    if (seconds <= 0.0) return !(stopFlag && stopFlag->load());
    const double deadline = steadyNowSec() + seconds;
    while (true) {
        if (stopFlag && stopFlag->load()) return false;
        double rem = deadline - steadyNowSec();
        if (rem <= 0.0) return true;
        int sleepMs = qMin(25, qMax(1, static_cast<int>(rem * 1000.0)));
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
    }
}

void MacroEngine::tapOrHoldScancode(int scanCode, bool isExtended, int holdMs, const QString& fallbackLabel, std::atomic<bool>* stopFlag) {
    QString method = m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode");
    int sc = scanCode;
    bool ext = isExtended;
    if (sc <= 0 && !fallbackLabel.isEmpty()) {
        auto pair = Win32Input::scancodeForLabel(fallbackLabel);
        sc = pair.first;
        ext = pair.second;
    }
    if (sc <= 0) return;

    Win32Input::sendScancodeKey(sc, ext, false, method);
    if (holdMs > 0) {
        sleepWithStop(holdMs / 1000.0, stopFlag);
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    Win32Input::sendScancodeKey(sc, ext, true, method);
}

bool MacroEngine::executePythonMiniScript(const QString& code, const QString& method, std::atomic<bool>* stopFlag, bool allowSelfTest) {
    QString reason;
    return PythonScriptRunner::executeScript(
        code,
        1,
        method,
        method,
        QStringLiteral("smart_combo"),
        QString(),
        0,
        QStringLiteral("foreground"),
        [method, allowSelfTest](const QString& cmd, const QJsonObject& args, QString& outR) -> bool {
            if (!allowSelfTest && Win32Input::isOwnAppForeground()) {
                outR = QStringLiteral("Blocked: V-MacroPlus foreground self-immunity");
                return false;
            }
            if (cmd == QStringLiteral("print")) {
                QString txt = args["text"].toString();
                bool enter = args["enter"].toBool(false);
                bool human = args["human"].toBool(false);
                int charDelay = args["char_delay_ms"].toInt(0);
                bool ok = AutoPresserEngine::dispatchTextViaMethod(txt, method, 0, false, false, human, charDelay, allowSelfTest, &outR);
                if (ok && enter) {
                    AutoPresserEngine::dispatchKey(QStringLiteral("Enter"), method, 35, 0, false, false, allowSelfTest, &outR);
                }
                return ok;
            } else if (cmd == QStringLiteral("type_text")) {
                return AutoPresserEngine::dispatchTextViaMethod(args["text"].toString(), method, 0, false, false, false, 0, allowSelfTest, &outR);
            } else if (cmd == QStringLiteral("press")) {
                return AutoPresserEngine::dispatchKey(args["key"].toString(), method, args["hold_ms"].toInt(20), 0, false, false, allowSelfTest, &outR);
            } else if (cmd == QStringLiteral("key_down") || cmd == QStringLiteral("key_up")) {
                auto sc = Win32Input::scancodeForLabel(args["key"].toString());
                if (sc.first <= 0) return false;
                return Win32Input::sendScancodeKey(sc.first, sc.second, cmd == QStringLiteral("key_up"), method);
            } else if (cmd == QStringLiteral("click")) {
                AutoPresserMousePoint pt;
                pt.x = args["x"].toInt(-1);
                pt.y = args["y"].toInt(-1);
                pt.button = args["button"].toString("left");
                if (pt.x < 0 || pt.y < 0) {
                    QPoint cur = Win32Input::getCursorPos();
                    pt.x = cur.x();
                    pt.y = cur.y();
                }
                Win32Input::sendMouseButton(pt.x, pt.y, pt.button, QStringLiteral("click"), true, args["hold_ms"].toInt(20), method);
                return true;
            }
            return true;
        },
        stopFlag,
        reason
    );
}

void MacroEngine::executeComboStep(const ComboStep& step, std::atomic<bool>* stopFlag, bool allowSelfTest) {
    if (!allowSelfTest && Win32Input::isOwnAppForeground()) return;

    const QString method = m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode");
    const QString st = step.stepType;

    if (st == QStringLiteral("wait")) {
        return;
    } else if (st == QStringLiteral("key_tap") || st == QStringLiteral("key_hold")) {
        int h = (st == QStringLiteral("key_hold")) ? qMax(10, step.holdMs) : step.holdMs;
        tapOrHoldScancode(step.scanCode, step.isExtended, h, step.value, stopFlag);
    } else if (st == QStringLiteral("key_down")) {
        auto sc = step.scanCode > 0 ? KeyId(step.scanCode, step.isExtended) : Win32Input::scancodeForLabel(step.value);
        if (sc.first > 0) Win32Input::sendScancodeKey(sc.first, sc.second, false, method);
    } else if (st == QStringLiteral("key_up")) {
        auto sc = step.scanCode > 0 ? KeyId(step.scanCode, step.isExtended) : Win32Input::scancodeForLabel(step.value);
        if (sc.first > 0) Win32Input::sendScancodeKey(sc.first, sc.second, true, method);
    } else if (st == QStringLiteral("mouse_click") || st == QStringLiteral("mouse_down") || st == QStringLiteral("mouse_up")) {
        QString act = st == QStringLiteral("mouse_click") ? QStringLiteral("click")
                    : (st == QStringLiteral("mouse_down") ? QStringLiteral("down") : QStringLiteral("up"));
        Win32Input::sendMouseButton(step.x, step.y, step.value, act, step.useCoords, step.holdMs, method);
    } else if (st == QStringLiteral("mouse_move")) {
        Win32Input::sendMouseButton(step.x, step.y, QStringLiteral("left"), QStringLiteral("move"), true, 0, method);
    } else if (st == QStringLiteral("mouse_scroll")) {
        Win32Input::sendMouseScroll(step.value, qMax(1, step.scrollAmount));
    } else if (st == QStringLiteral("open_folder")) {
        Win32Input::openFolderOrPath(step.value);
    } else if (st == QStringLiteral("powershell") || st == QStringLiteral("cmd")) {
        Win32Input::runShellCommand(st, step.value);
    } else if (st == QStringLiteral("type_text")) {
        Win32Input::sendUnicodeText(step.value);
    } else if (st == QStringLiteral("python_script")) {
        executePythonMiniScript(step.value, method, stopFlag, allowSelfTest);
    }
}

void MacroEngine::executeMacroEvent(const MacroEvent& ev, std::atomic<bool>* stopFlag, bool allowSelfTest) {
    if (!allowSelfTest && Win32Input::isOwnAppForeground()) return;

    const QString method = m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode");
    const QString k = ev.kind;

    if (k == QStringLiteral("wait")) {
        return;
    } else if (k == QStringLiteral("key_down")) {
        auto sc = ev.scanCode > 0 ? KeyId(ev.scanCode, ev.isExtended) : Win32Input::scancodeForLabel(ev.key);
        if (sc.first > 0) {
            {
                QMutexLocker lk(&m_lock);
                m_playbackHeldKeys.insert(sc);
            }
            Win32Input::sendScancodeKey(sc.first, sc.second, false, method);
        }
    } else if (k == QStringLiteral("key_up")) {
        auto sc = ev.scanCode > 0 ? KeyId(ev.scanCode, ev.isExtended) : Win32Input::scancodeForLabel(ev.key);
        if (sc.first > 0) {
            {
                QMutexLocker lk(&m_lock);
                m_playbackHeldKeys.remove(sc);
            }
            Win32Input::sendScancodeKey(sc.first, sc.second, true, method);
        }
    } else if (k == QStringLiteral("key_tap") || k == QStringLiteral("key_hold")) {
        int h = (k == QStringLiteral("key_hold")) ? qMax(10, ev.holdMs) : ev.holdMs;
        tapOrHoldScancode(ev.scanCode, ev.isExtended, h, ev.key, stopFlag);
    } else if (k == QStringLiteral("mouse_move")) {
        Win32Input::sendMouseButton(ev.x, ev.y, QStringLiteral("left"), QStringLiteral("move"), true, 0, method);
    } else if (k == QStringLiteral("mouse_down")) {
        {
            QMutexLocker lk(&m_lock);
            m_playbackHeldMouse.insert(ev.button);
        }
        Win32Input::sendMouseButton(ev.x, ev.y, ev.button, QStringLiteral("down"), true, 0, method);
    } else if (k == QStringLiteral("mouse_up")) {
        {
            QMutexLocker lk(&m_lock);
            m_playbackHeldMouse.remove(ev.button);
        }
        Win32Input::sendMouseButton(ev.x, ev.y, ev.button, QStringLiteral("up"), true, 0, method);
    } else if (k == QStringLiteral("mouse_click")) {
        Win32Input::sendMouseButton(ev.x, ev.y, ev.button, QStringLiteral("click"), true, ev.holdMs, method);
    } else if (k == QStringLiteral("mouse_scroll")) {
        Win32Input::sendMouseScroll(ev.scrollDir, qMax(1, ev.scrollAmount), ev.dx, ev.dy);
    }
}

} // namespace vmacro
