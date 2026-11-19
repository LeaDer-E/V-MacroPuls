#pragma once
#include "Models.h"
#include <QObject>
#include <QRect>
#include <QJsonObject>
#include <functional>

namespace vmacro {

class AppState : public QObject {
    Q_OBJECT
public:
    explicit AppState(QObject* parent = nullptr);

    AppConfig config;
    bool isRecording = false;
    bool isPlaying = false;
    bool combosActive = true;
    bool isCapturingKey = false;
    bool combosSidebarCollapsed = false;
    bool recorderSidebarCollapsed = false;
    QString currentPageId = "combos";
    QString currentMacroName = "W -> Wait 1s -> Space";
    QString testLabTemplateSweepStr;

    QList<MacroEvent> recordedEvents;
    QList<SmartCombo> smartCombos;
    QList<SavedMacro> savedMacros;
    QList<QJsonObject> testLabLogs;

    void setLanguage(const QString& langCode);
    void setTheme(const QString& mode, const QString& paletteId = QString(), const QString& customAccent = QString());
    void updateSetting(const QString& key, const QVariant& value);

    void registerStopButtonRectProvider(std::function<QRect()> fn);
    bool isPointOnStopButton(int x, int y) const;
    void triggerCursorPick(int x, int y);

signals:
    void configChanged();
    void settingsChanged(const QString& key, const QVariant& val);
    void themeChanged();
    void languageChanged(const QString& lang);
    void navigationRequested(const QString& pageId);
    void toastRequested(const QString& message, const QString& kind);
    void soundRequested(const QString& kind);

    void recordingStateChanged(bool active);
    void playbackStateChanged(bool active);
    void recordingCountChanged(int count);
    void eventsUpdated();
    void combosListUpdated();
    void macrosLibraryUpdated();
    void combosMasterChanged(bool active);
    void pythonScriptsUpdated();

    void hookKeyEvent(int scanCode, bool isExtended, bool isDown, const QString& label);
    void cursorPickHotkeyPressed(int x, int y);

private:
    QList<std::function<QRect()>> m_stopRectProviders;
    qint64 m_lastCursorPickMs = 0;
};

} // namespace vmacro
