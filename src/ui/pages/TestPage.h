#pragma once
#include <QPointer>
#include <QLabel>
#include "../../core/Models.h"
#include "../../platform/Win32Input.h"
#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>

namespace vmacro {

class AppState;
class MacroEngine;
class Button;
class GlassCard;
class IconButton;
class KeyCaptureButton;
class NumberInput;
class SearchCombo;
class TintedScrollBox;
class ToggleSwitch;

class LiveInputSandboxBox : public QWidget {
    Q_OBJECT
public:
    using LogEntryCb = std::function<void(const QJsonObject&, const QString&)>;

    explicit LiveInputSandboxBox(AppState* state, LogEntryCb onLogEntry, QWidget* parent = nullptr);
    QLineEdit* testInput() const { return m_testInput; }
    QLabel* statusLabel() const { return m_lblStatus; }
    void invalidateCache() { m_hasCachedEff = false; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    EffectiveEngineInfo cachedEffInfo();

    AppState* m_state = nullptr;
    LogEntryCb m_onLogEntry;
    QLineEdit* m_testInput = nullptr;
    QLabel* m_lblStatus = nullptr;
    bool m_hasCachedEff = false;
    EffectiveEngineInfo m_cachedEff;
};

class TestPage : public QWidget {
    Q_OBJECT
public:
    explicit TestPage(AppState* state, MacroEngine* engine, QWidget* parent = nullptr);

signals:
    void threadLogSignal(const QJsonObject& entry, const QString& accentHex);
    void sweepFinishedSignal();

private:
    void buildUi();
    void insertTemplateToken(const QString& tokenStr);
    void onTemplateSweepChanged(const QString& txt);
    void onResetTemplateSweep();
    void refreshPyPresetsCombo(const QString& selectKey = QString());
    void onPyPresetSelected(const QString& scriptKey);
    void onSavePyScriptClicked();
    void onDeletePyScriptClicked();
    void onPySweepTextChanged();
    void onResetPySweepScript();
    void refreshTestTargetApps();
    quintptr resolveSelectedTestHwnd();
    void onSwitchToTestApp();
    void populateTestTargets();
    void onMethodChanged(const QString& methodKey);
    QPair<QString, QString> formatEffectiveEngineBadge(const QString& requestedMethod) const;
    QString renderLogEntryHtml(const QJsonObject& entry) const;
    void onActionModeChanged(const QString& modeKey);
    void resolveSelectedMacroSteps(QString& outName, QList<ComboStep>& outSteps) const;
    void runSelectedTest();
    void onSweepFinishedMainThread();
    void runSelectedTestMacroImpl(bool runInBg = false);
    void addStructuredLog(const QJsonObject& entry, const QString& accentHex = "#8B5CF6");
    void restorePersistentLogs();
    void insertLogCard(const QJsonObject& item, bool atTop = true);
    void clearLog();

    AppState* m_state = nullptr;
    MacroEngine* m_engine = nullptr;
    int m_logCount = 0;
    QVector<WindowAppInfo> m_testWinCache;
    bool m_sweepRunning = false;

    SearchCombo* m_methodCombo = nullptr;
    QLabel* m_lblHwStatus = nullptr;
    SearchCombo* m_testAppCombo = nullptr;
    Button* m_btnTestSwitch = nullptr;
    ToggleSwitch* m_togTestBg = nullptr;

    SearchCombo* m_actionModeCombo = nullptr;
    NumberInput* m_numTestDelay = nullptr;
    Button* m_btnRunTest = nullptr;

    QWidget* m_payloadRowW = nullptr;
    QLabel* m_lblTarget = nullptr;
    SearchCombo* m_targetCombo = nullptr;
    ToggleSwitch* m_togMacroSweepPrefix = nullptr;
    QLabel* m_lblMacroSweepPrefix = nullptr;

    QLabel* m_lblCustomTxt = nullptr;
    QLineEdit* m_customTextEdit = nullptr;
    ToggleSwitch* m_togPressEnter = nullptr;
    QLabel* m_lblPressEnter = nullptr;

    QLabel* m_lblSimpleK = nullptr;
    KeyCaptureButton* m_simpleKeyBtn = nullptr;

    QWidget* m_templateSweepContainer = nullptr;
    QLabel* m_lblTplBox = nullptr;
    Button* m_btnResetTpl = nullptr;
    QLineEdit* m_templateSweepEdit = nullptr;
    TintedScrollBox* m_tplChipsScrollBox = nullptr;
    QScrollArea* m_tplChipsScroll = nullptr;
    QWidget* m_tplChipsContainer = nullptr;

    QWidget* m_pySweepContainer = nullptr;
    QLabel* m_lblPySweep = nullptr;
    SearchCombo* m_pyPresetCombo = nullptr;
    QLineEdit* m_pyScriptNameEdit = nullptr;
    Button* m_btnSavePyScript = nullptr;
    IconButton* m_btnDelPyScript = nullptr;
    Button* m_btnResetPy = nullptr;
    QPlainTextEdit* m_pySweepEditor = nullptr;

    LiveInputSandboxBox* m_sandboxBox = nullptr;
    QScrollArea* m_logScroll = nullptr;
    QWidget* m_logContainer = nullptr;
    QVBoxLayout* m_logVbox = nullptr;
};

} // namespace vmacro
