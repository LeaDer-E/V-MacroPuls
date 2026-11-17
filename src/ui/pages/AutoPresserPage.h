#pragma once
#include <QPointer>
#include <QLabel>
#include "../../core/Models.h"
#include "../Widgets.h"
#include "../../platform/Win32Input.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace vmacro {

class AppState;
class MacroEngine;
class AutoPresserEngine;

class KeyPillToggleButton : public QWidget {
    Q_OBJECT
public:
    explicit KeyPillToggleButton(const QString& keyLabel, bool checked = false, QWidget* parent = nullptr);
    void setKeyLabel(const QString& keyLabel);
    bool isChecked() const { return m_checked; }
    void setChecked(bool checked, bool emitSignal = false);

signals:
    void toggled(bool checked);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_keyLabel;
    bool m_checked = false;
    bool m_hovered = false;
};

class KeyMacroModeButton : public QWidget {
    Q_OBJECT
public:
    explicit KeyMacroModeButton(bool checked = false, bool hasLinkedMacro = false, QWidget* parent = nullptr);
    void setHasLinkedMacro(bool hasLinked);
    void setChecked(bool checked, bool emitSignal = false);

signals:
    void toggled(bool checked);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void syncTooltip();
    bool m_checked = false;
    bool m_hasLinked = false;
    bool m_hovered = false;
};

class CompactMsEdit : public QLineEdit {
    Q_OBJECT
public:
    explicit CompactMsEdit(int valueMs = 1000, QWidget* parent = nullptr);
    void setValueMs(int valMs);

signals:
    void msChanged(int valMs);

protected:
    void wheelEvent(QWheelEvent* event) override;

private:
    void commitText();
    int m_valMs = 1000;
};

class KeyTileWidget : public QWidget {
    Q_OBJECT
public:
    explicit KeyTileWidget(AutoPresserKeyItem* itemPtr, bool hasLinkedMacro = false, QWidget* parent = nullptr);
    void bindItem(AutoPresserKeyItem* itemPtr, bool hasLinkedMacro = false);

signals:
    void changed();
    void testClicked(const QString& keyLabel);
    void removeClicked(const QString& keyLabel);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    AutoPresserKeyItem* m_item = nullptr;
    bool m_hovered = false;
    KeyPillToggleButton* m_pillBtn = nullptr;
    KeyMacroModeButton* m_btnMacroMode = nullptr;
    IconButton* m_btnDel = nullptr;
    CompactMsEdit* m_msEdit = nullptr;
    IconButton* m_btnTest = nullptr;
};

class AddCustomKeyButton : public Button {
    Q_OBJECT
public:
    explicit AddCustomKeyButton(AppState* state, QWidget* parent = nullptr);

signals:
    void customKeyCaptured(const QString& keyStr);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    void toggleCapture();
    void stopCapture();
    void onHookKey(int scan, bool ext, bool isDown, const QString& label);

    AppState* m_state = nullptr;
    bool m_capturing = false;
};

class MousePointRowWidget : public QWidget {
    Q_OBJECT
public:
    explicit MousePointRowWidget(int index,
                                 AutoPresserMousePoint* mpPtr,
                                 bool isPicking,
                                 QWidget* parent = nullptr);

signals:
    void changed();
    void removed(int idx);
    void pickRequested(int idx);
    void testRequested(int idx);

private:
    int m_index = 0;
    AutoPresserMousePoint* m_mp = nullptr;
    StepCard* m_card = nullptr;
    ToggleSwitch* m_tog = nullptr;
    SearchCombo* m_btnCombo = nullptr;
    NumberInput* m_numX = nullptr;
    NumberInput* m_numY = nullptr;
    Button* m_btnPick = nullptr;
    NumberInput* m_numInt = nullptr;
    Button* m_btnTestMp = nullptr;
};

class AutoPresserPage : public QWidget {
    Q_OBJECT
public:
    explicit AutoPresserPage(AppState* state, MacroEngine* engine, QWidget* parent = nullptr);
    ~AutoPresserPage() override;

signals:
    void sweepDoneSignal(const QString& which, const QString& summaryMsg, const QString& toastKind);

private:
    AutoPresserTab& currentTab();
    void buildUi();
    void scheduleSaveWorkspace();
    void onThemeUpdated();
    void onExternalSettingsChanged(const QString& key, const QVariant& val);
    void onStopAllHotkeyChanged(const QString& newKey);
    void onTabHotkeyChanged(const QString& newKey);
    void refreshTabsBar();
    void selectTab(int index);
    void loadCurrentTabIntoUi();
    void refreshProcessDropdown(const QString& selectedProc = QString(), bool forceOsScan = false);
    void refreshProcessDropdownAndOpen();
    void onProcessSelected(const QString& procName);
    bool hasMatchingSmartCombo(const QString& keyLabel, SmartCombo* outCombo = nullptr) const;
    void rebuildKeysGrid();
    void rebuildMousePointsList();
    void onTestSingleMousePoint(int idx);
    QString renderLogHtml(const QJsonObject& entry) const;
    QString computeLogSig(const QString& tabId) const;
    void rebuildActivityLog();
    void onSubViewChanged(const QString& viewKey);
    void onTabNameEdited(const QString& txt);
    void onAddTab();
    void onDupTab();
    void onCloseCurrentTab();
    void onBgToggled(bool val);
    void onMethodChanged(const QString& methodKey);
    void onHoldChanged(int val);
    quintptr ensureTargetWindowAndFocus(int focusWindowMode = -1);
    void onSwitchToTargetWindow();
    void onTestActiveMethod();
    void onSweepAllKeyMethods();
    void onSweepAllMouseModes();
    void onSweepDoneMainThread(const QString& which, const QString& summaryMsg, const QString& toastKind);
    void onTestSingleKey(const QString& keyLabel);
    void onCustomKeyCaptured(const QString& keyStr);
    void onRemoveCustomKey(const QString& keyStr);
    void setAllKeysEnabled(bool enabled);
    void onMouseMasterToggled(bool val);
    void onMouseModeChanged(const QString& modeKey);
    void onAddMousePoint();
    void onRemoveMousePoint(int idx);
    void onPickMousePoint(int idx);
    void onF8Captured(int x, int y);
    void onToggleCurrentTab();
    void onStopAllTabs();
    void syncStartButtonState();
    void tickUiStatus();
    void onClearTabLog();
    void onSaveWorkspace();
    void onLoadWorkspace();
    void onSaveTabProfile();
    void onLoadTabProfile();

    AppState* m_state = nullptr;
    MacroEngine* m_macroEngine = nullptr;
    AutoPresserEngine* m_akp = nullptr;
    bool m_ownsAkp = false;

    QString m_subView = "keys";
    int m_pickingMouseIdx = -1;
    QVector<WindowAppInfo> m_windowCache;
    QString m_lastLogSig;
    bool m_sweepRunning = false;
    QList<KeyTileWidget*> m_builtinTiles;

    QTimer* m_saveTimer = nullptr;
    QTimer* m_statusTimer = nullptr;

    KeyCaptureButton* m_hkStopAllTabs = nullptr;
    Button* m_btnStopAll = nullptr;

    TintedScrollBox* m_tabsScrollBox = nullptr;
    QScrollArea* m_tabsScroll = nullptr;
    QWidget* m_tabsBtnContainer = nullptr;
    QHBoxLayout* m_tabsBtnLayout = nullptr;
    QLineEdit* m_tabNameEdit = nullptr;

    Button* m_btnChooseProc = nullptr;
    SearchCombo* m_procCombo = nullptr;
    Button* m_btnSwitchWin = nullptr;
    ToggleSwitch* m_togBg = nullptr;
    KeyCaptureButton* m_hkTabToggle = nullptr;
    Button* m_btnStartTab = nullptr;

    SearchCombo* m_methodCombo = nullptr;
    NumberInput* m_numHold = nullptr;
    Button* m_btnSweepAll = nullptr;

    SegmentedControl* m_subSeg = nullptr;
    AddCustomKeyButton* m_btnAddCustomKey = nullptr;

    GlassCard* m_bodyCard = nullptr;
    QScrollArea* m_keysScroll = nullptr;
    QWidget* m_keysContainer = nullptr;
    QVBoxLayout* m_keysOuterVbox = nullptr;
    QLabel* m_lblBuiltinHdr = nullptr;
    QWidget* m_builtinGridW = nullptr;
    QGridLayout* m_keysGrid = nullptr;
    QLabel* m_lblCustomHdr = nullptr;
    QWidget* m_customGridW = nullptr;
    QGridLayout* m_customKeysGrid = nullptr;

    QWidget* m_mousePanel = nullptr;
    ToggleSwitch* m_togMouseMaster = nullptr;
    SearchCombo* m_mouseModeCombo = nullptr;
    Button* m_btnSweepMouse = nullptr;
    QScrollArea* m_mouseScroll = nullptr;
    QWidget* m_mouseListW = nullptr;
    QVBoxLayout* m_mouseVbox = nullptr;

    QWidget* m_logPanel = nullptr;
    QScrollArea* m_logScroll = nullptr;
    QWidget* m_logListW = nullptr;
    QVBoxLayout* m_logVbox = nullptr;
};

} // namespace vmacro
