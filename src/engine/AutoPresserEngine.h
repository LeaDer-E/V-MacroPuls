#pragma once
#include "../core/Models.h"
#include "../platform/Win32Input.h"
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QPair>
#include <QString>
#include <atomic>
#include <memory>
#include <thread>

namespace vmacro {

class AppState;
class MacroEngine;

struct MethodDef {
    QString id;
    QString trKey;
    bool isBackground;
};

class AutoPresserEngine {
public:
    explicit AutoPresserEngine(AppState* state = nullptr);
    ~AutoPresserEngine();

    static const QList<MethodDef>& all22KeyMethods();
    static const QList<QPair<QString, QString>>& all8MouseModes();
    static QList<QPair<QString, QString>> localizedKeyMethodOptions();
    static QList<QPair<QString, QString>> localizedMouseModeOptions();

    static QString formatDiagnosticReason(const QJsonObject& entry);
    static quintptr resolveTargetHwnd(AutoPresserTab& tab);

    static bool dispatchKey(
        const QString& keyLabel,
        const QString& method,
        int holdMs = 50,
        quintptr targetHwnd = 0,
        bool runInBackground = true,
        bool strictNoFallback = false,
        bool allowSelfTest = false,
        QString* outReason = nullptr
    );

    static bool dispatchTextViaMethod(
        const QString& text,
        const QString& method,
        quintptr targetHwnd = 0,
        bool runInBackground = false,
        bool strictNoFallback = true,
        bool human = false,
        int charDelayMs = 0,
        bool allowSelfTest = false,
        QString* outReason = nullptr
    );

    static bool dispatchMouseDetailed(
        const AutoPresserMousePoint& pt,
        const QString& mouseMode,
        quintptr targetHwnd = 0,
        bool runInBackground = true,
        bool strictNoFallback = false,
        bool allowSelfTest = false,
        QJsonObject* outMeta = nullptr
    );

    static bool expandAndExecuteMethodTemplate(
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
    );

    static bool executePerMethodPythonTest(
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
    );

    static bool executePerMethodMacroTest(
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
    );

    void setMacroEngine(MacroEngine* me) { m_macroEngine = me; }
    bool loadWorkspace(const QString& filePath = QString());
    bool saveWorkspace(const QString& filePath = QString());
    AutoPresserTab& addNewTab(int duplicateFrom = -1);
    void closeTab(int index);

    bool isTabRunning(const QString& tabId) const;
    bool anyTabRunning() const;
    bool toggleTab(AutoPresserTab& tab);
    bool startTab(AutoPresserTab& tab);
    void stopTab(const QString& tabId);
    void stopAllTabs();

    QList<QJsonObject> getTabLogs(const QString& tabId) const;
    void clearTabLogs(const QString& tabId);
    void appendTabLog(const QString& tabId, const QJsonObject& entry, const QString& accentHex = "#8B5CF6");

    void sweepAllKeyMethods(AutoPresserTab& tab, const QString& testKey, int& outOk, int& outFail);
    void sweepAllMouseModes(AutoPresserTab& tab, int& outOk, int& outFail);

    QList<AutoPresserTab> tabs;
    int activeTabIndex = 0;

private:
    AppState* m_state = nullptr;
    MacroEngine* m_macroEngine = nullptr;
    QMap<QString, std::shared_ptr<std::atomic<bool>>> m_runningTabs;
    QMap<QString, QList<QJsonObject>> m_tabLogs;
};

} // namespace vmacro
