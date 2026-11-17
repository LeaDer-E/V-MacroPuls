#pragma once
#include <QPointer>
#include <QLabel>
#include "../../core/Models.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace vmacro {

class AppState;
class MacroEngine;
class Button;
class DragGripHandle;
class GlassCard;
class IconButton;
class KeyCaptureButton;
class NumberInput;
class ResponsiveFlowBar;
class SearchCombo;
class StepCard;
class StepEnableToggleButton;
class StepNoteIconButton;
class TintedScrollBox;
class ToggleSwitch;

class ComboStepRowWidget : public QWidget {
    Q_OBJECT
public:
    explicit ComboStepRowWidget(int index,
                                ComboStep* stepPtr,
                                AppState* state,
                                QWidget* parent = nullptr);

    void setStepIndex(int newIdx);
    int stepIndex() const { return m_index; }
    ComboStep* stepPtr() const { return m_step; }

signals:
    void changed();
    void removed(int idx);
    void moveUp(int idx);
    void moveDown(int idx);
    void testSingleStep(int idx);
    void dragMoveRequested(ComboStepRowWidget* row, int globalY);
    void dragFinished();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void onStepEnabledToggled(bool enabled);
    void onStepNoteChanged(const QString& noteTxt);
    void browseFolder();
    void browseProgramOrFile();
    void refreshStepPyPresetsCombo(const QString& selectKey = QString());
    void onStepPyPresetSelected(const QString& scriptKey);
    void onStepSavePyScript();
    void onStepDeletePyScript();
    void onStepChordCaptured(const QList<QPair<int, bool>>& chord, const QString& lbl);
    void startLiveXyPreview();
    void toggleLiveXyPicker();
    void tickLiveXy();
    void onF8Picked(int cx, int cy);

    int m_index = 0;
    ComboStep* m_step = nullptr;
    AppState* m_state = nullptr;
    bool m_pickingLiveXy = false;
    bool m_rowDragging = false;
    QString m_catHex = "#8B5CF6";

    StepCard* m_card = nullptr;
    DragGripHandle* m_grip = nullptr;
    QLabel* m_stepNum = nullptr;
    StepEnableToggleButton* m_btnEnableStep = nullptr;
    QLabel* m_lblPaused = nullptr;

    NumberInput* m_numMs = nullptr;
    SearchCombo* m_btnCombo = nullptr;
    NumberInput* m_numX = nullptr;
    NumberInput* m_numY = nullptr;
    Button* m_btnPickXy = nullptr;
    QLabel* m_lblLiveXy = nullptr;
    NumberInput* m_numHold = nullptr;
    QTimer* m_xyTimer = nullptr;

    SearchCombo* m_scrollDirCombo = nullptr;
    NumberInput* m_numScrollAmt = nullptr;

    QLineEdit* m_folderEdit = nullptr;
    QLineEdit* m_cmdEdit = nullptr;

    SearchCombo* m_pyPresetCombo = nullptr;
    QLineEdit* m_pyScriptNameEdit = nullptr;
    QPlainTextEdit* m_pyEdit = nullptr;

    KeyCaptureButton* m_keyBtn = nullptr;
    NumberInput* m_numDelay = nullptr;
    StepNoteIconButton* m_btnNote = nullptr;
};

class CombosPage : public QWidget {
    Q_OBJECT
public:
    static constexpr int LEFT_PANEL_WIDTH = 264;

    explicit CombosPage(AppState* state, MacroEngine* engine, QWidget* parent = nullptr);

private:
    void buildUi();
    void connectSignals();
    SmartCombo* currentCombo();
    void autoSave();
    void toggleMacrosSidebar();
    void exportFullBackup();
    void importFullBackup();
    void onCombosUpdated();
    void refreshLeftList();
    void runComboFromSidebar(int idx);
    void pushComboUndo();
    void syncComboUndoRedo();
    void undoComboSteps();
    void redoComboSteps();
    void setComboStepsVisible(bool visible);
    void saveComboChangesNow();
    void saveComboAsNew();
    void selectCombo(int idx);
    void toggleListItem(int idx, bool enabled);
    void loadSelectedIntoEditor();
    void rebuildStepsUi();
    void onStepDragMoved(ComboStepRowWidget* draggedRow, int globalY);
    void onCustomNameEdited(const QString& txt);
    void onTriggerChordCaptured(const QList<QPair<int, bool>>& chord, const QString& lbl);
    void clearTriggerForManualMode();
    void addStep(const QString& stepType, const QString& value, int delayMs, int holdMs = 0, int scrollAmount = 3);
    void removeStepAt(int idx);
    void moveStepUp(int idx);
    void moveStepDown(int idx);
    void testSingleStepAt(int idx);
    void onStartDelayChanged(int ms);
    void createNewBlankCombo();
    void deleteSelectedCombo();
    void undoLastDelete();
    void refreshRunningAppsDropdown();
    void rebuildExcludedAppsChips();
    void onAddExcludedApp();
    void onRemoveExcludedApp(const QString& exeName);
    void onClearExcludedApps();
    void onTargetAppChanged(const QString& exeName);
    void syncModeStopHotkeyUi(const QString& modeKey);
    void onStopHotkeyCaptured(const QList<QPair<int, bool>>& chord, const QString& lbl);
    void onModeChanged(const QString& modeKey);
    void onRetriggerPolicyChanged(const QString& policy);
    void onCurrentEnabledToggled(bool val);

    AppState* m_state = nullptr;
    MacroEngine* m_engine = nullptr;
    int m_selectedIdx = 0;
    QList<ComboStepRowWidget*> m_stepRows;
    bool m_hasDeletedCombo = false;
    SmartCombo m_lastDeletedCombo;
    bool m_sidebarCollapsed = false;
    bool m_stepsEditorVisible = false;
    QList<QList<ComboStep>> m_undoStepsStack;
    QList<QList<ComboStep>> m_redoStepsStack;

    ToggleSwitch* m_masterTog = nullptr;
    Button* m_btnToggleSidebar = nullptr;

    GlassCard* m_leftCard = nullptr;
    Button* m_btnNew = nullptr;
    IconButton* m_btnUndoDelete = nullptr;
    QWidget* m_listContainer = nullptr;
    QVBoxLayout* m_listVbox = nullptr;

    GlassCard* m_emptyStateCard = nullptr;
    Button* m_btnEmptyNew = nullptr;

    GlassCard* m_rightCard = nullptr;
    Button* m_btnNoKey = nullptr;
    QLineEdit* m_nameEdit = nullptr;
    KeyCaptureButton* m_heroKeyBtn = nullptr;

    SearchCombo* m_appCombo = nullptr;
    SearchCombo* m_modeSeg = nullptr;
    QLabel* m_retrigLbl = nullptr;
    SearchCombo* m_retriggerSeg = nullptr;
    QLabel* m_lblStopHk = nullptr;
    KeyCaptureButton* m_stopHkBtn = nullptr;

    SearchCombo* m_excludeAppCombo = nullptr;
    IconButton* m_btnAddExclApp = nullptr;
    TintedScrollBox* m_excludedScrollBox = nullptr;
    QScrollArea* m_excludedChipsScroll = nullptr;
    QWidget* m_excludedChipsW = nullptr;
    QHBoxLayout* m_excludedChipsLay = nullptr;
    IconButton* m_btnClearExclApps = nullptr;

    QWidget* m_comboStepsHiddenHolder = nullptr;
    GlassCard* m_comboStepsHiddenCard = nullptr;
    QLabel* m_lblComboStepsHidden = nullptr;
    Button* m_btnComboOpenSteps = nullptr;

    GlassCard* m_comboEditToolbar = nullptr;
    QLabel* m_s2Lbl = nullptr;
    Button* m_btnComboUndo = nullptr;
    Button* m_btnComboRedo = nullptr;
    Button* m_btnComboSaveCurr = nullptr;
    Button* m_btnComboSaveNew = nullptr;
    Button* m_btnComboHideSteps = nullptr;

    ResponsiveFlowBar* m_stepAdderBar = nullptr;
    QScrollArea* m_stepsScroll = nullptr;
    QWidget* m_stepsContainer = nullptr;
    QVBoxLayout* m_stepsVbox = nullptr;

    ToggleSwitch* m_comboEnableTog = nullptr;
    NumberInput* m_numStartDelay = nullptr;
    Button* m_btnDelCombo = nullptr;
};

} // namespace vmacro
