#pragma once
#include "../core/Models.h"
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QSet>
#include <QString>
#include <atomic>
#include <chrono>
#include <memory>

namespace vmacro {

class AppState;
class AutoPresserEngine;

class MacroEngine : public QObject {
    Q_OBJECT
public:
    explicit MacroEngine(AppState* state, QObject* parent = nullptr);
    ~MacroEngine() override;

    void setAutoPresserEngine(AutoPresserEngine* ap) { m_apEngine = ap; }

    void startHooks();
    void shutdown();

    void rebuildTriggerMap();
    void loadAllData();
    void saveCombos();
    void saveSavedMacros();

    bool updateSavedMacro(const QString& macroId, const QString& name, const QList<MacroEvent>& events);
    SavedMacro saveCurrentMacroToLibrary(const QString& name, const QString& hotkey = QString());
    bool updateSavedMacroHotkey(const QString& macroId, const QString& newHotkey);
    bool deleteSavedMacro(const QString& macroId);
    SmartCombo convertRecordingToSmartCombo(const QString& name, const QList<MacroEvent>& events);

    bool exportAllBackup(const QString& filePath) const;
    bool importAllBackup(const QString& filePath);

    void toggleRecording(bool viaMouse = false);
    void startRecording();
    void stopRecording(bool stoppedViaMouse = false);

    void togglePlayback();
    void startPlayback(const QList<MacroEvent>& events, double speed = 1.0, int repeatCount = 1, bool allowSelfTest = false);
    void toggleCombosMaster();

    void executeSingleStepAfter(const ComboStep& step, int delayMs = 2000, bool allowSelfTest = true);
    void executeSmartCombo(const SmartCombo& combo, const QList<QPair<int, bool>>& releaseChord = {}, int initialDelayMs = 0, bool allowSelfTest = false);
    void stopCombo(const QString& comboId);
    void stopAll();

private:
    using KeyId = QPair<int, bool>;

    bool onGlobalKey(int scan, bool ext, bool isDown, bool isInjected);
    void onGlobalMouse(const QString& evType, int x, int y, const QString& btnOrDir, int dx, int dy);

    void pruneStaleDownKeys(double nowSec);
    bool isKeyActiveForChord(const KeyId& k, double nowSec) const;
    bool chordMatchesDownEvent(const QList<KeyId>& chord, const KeyId& currentKid, double nowSec, bool ignoreExtraModifiers = true) const;

    void stripStopHotkeyFromRecording(const QList<KeyId>& recChord);
    void stripStopMouseClickFromRecording();
    void releaseAllPlaybackHeldInputs();
    void triggerSystemShutdown();

    bool sleepWithStop(double seconds, std::atomic<bool>* stopFlag);
    void tapOrHoldScancode(int scanCode, bool isExtended, int holdMs, const QString& fallbackLabel, std::atomic<bool>* stopFlag);
    bool executePythonMiniScript(const QString& code, const QString& method, std::atomic<bool>* stopFlag, bool allowSelfTest);
    void executeComboStep(const ComboStep& step, std::atomic<bool>* stopFlag, bool allowSelfTest);
    void executeMacroEvent(const MacroEvent& ev, std::atomic<bool>* stopFlag, bool allowSelfTest);

    static double steadyNowSec();

    AppState* m_state = nullptr;
    AutoPresserEngine* m_apEngine = nullptr;

    mutable QMutex m_lock;
    QMap<KeyId, double> m_downKeys;
    QSet<KeyId> m_suppressedDownKeys;

    QList<SmartCombo> m_activeCombos;
    QMap<QString, std::shared_ptr<std::atomic<bool>>> m_runningComboStops;
    QMap<QString, QList<KeyId>> m_activeToggleHoldChords;

    std::shared_ptr<std::atomic<bool>> m_playbackStop;
    QSet<KeyId> m_playbackHeldKeys;
    QSet<QString> m_playbackHeldMouse;

    double m_lastEventSec = 0.0;
    double m_lastMouseSampleSec = 0.0;
    double m_lastCountEmitSec = 0.0;
};

} // namespace vmacro
