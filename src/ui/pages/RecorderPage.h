#pragma once
#include <QPointer>
#include <QLabel>
#include "../../core/Models.h"
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
class Button;
class DragGripHandle;
class GlassCard;
class IconButton;
class KeyCaptureButton;
class NumberInput;
class ResponsiveFlowBar;
class SearchCombo;
class SegmentedControl;
class StepCard;
class StepEnableToggleButton;
class StepNoteIconButton;
class ToggleSwitch;

class EditableEventRow : public QWidget {
    Q_OBJECT
public:
    explicit EditableEventRow(int index,
                              MacroEvent* evPtr,
                              AppState* state,
                              QWidget* parent = nullptr);

    void setStepIndex(int newIdx);
    int stepIndex() const { return m_index; }

signals:
    void deleteClicked(int idx);
    void dragMoveRequested(EditableEventRow* row, int globalY);
    void dragFinished();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void onEvEnabledToggled(bool enabled);
    void onKeyEdited(int sc, bool ext, const QString& lbl);
    void startLiveXyPreview();
    void toggleLiveXyPicker();
    void tickLiveXy();
    void onF8Picked(int cx, int cy);

    int m_index = 0;
    MacroEvent* m_ev = nullptr;
    AppState* m_state = nullptr;
    bool m_pickingLiveXy = false;
    bool m_rowDragging = false;
    QString m_catHex = "#8B5CF6";

    StepCard* m_card = nullptr;
    DragGripHandle* m_grip = nullptr;
    QLabel* m_idxLbl = nullptr;
    StepEnableToggleButton* m_btnEnableStep = nullptr;
    QLabel* m_lblPaused = nullptr;

    KeyCaptureButton* m_keyBtn = nullptr;
    NumberInput* m_numHold = nullptr;
    SearchCombo* m_scrollDirCombo = nullptr;
    NumberInput* m_numScrollAmt = nullptr;
    SearchCombo* m_btnCombo = nullptr;
    NumberInput* m_numX = nullptr;
    NumberInput* m_numY = nullptr;
    Button* m_btnPickXy = nullptr;
    QLabel* m_lblLiveXy = nullptr;
    QTimer* m_xyTimer = nullptr;

    NumberInput* m_numDelay = nullptr;
    StepNoteIconButton* m_btnNote = nullptr;
};

class RecorderPage : public QWidget {
    Q_OBJECT
public:
    explicit RecorderPage(AppState* state, MacroEngine* engine, QWidget* parent = nullptr);

private:
    void buildUi();
    void connectSignals();
    QRect getStopBtnScreenRect() const;
    void toggleRecSidebar();
    void onRepeatModeChanged(const QString& mode);
    void onDelayBetweenToggled(bool val);
    void syncRepeatModeInputs();
    void onLiveRecordingCount(int count);
    void syncTransportButtons();
    void onPlayClicked();
    void convertCurrentToCombo();
    void pushUndoSnapshot();
    void syncUndoRedoButtons();
    void undoSteps();
    void redoSteps();
    void setStepsEditorVisible(bool visible);
    void saveEditsToCurrentMacro();
    void saveEditsAsNewMacro();
    void clearEvents();
    void insertStep(const QString& kind);
    void deleteEventAt(int idx);
    void refreshTimeline();
    void onEventDragMoved(EditableEventRow* draggedRow, int globalY);
    void refreshSavedLibrary();
    void loadSavedIntoTimeline(const SavedMacro& m);
    void deleteSavedWithUndo(const SavedMacro& m);
    void undoDeleteMacro();
    void exportJson(const SavedMacro& m);
    void exportFullBackup();
    void importJsonOrBackup();

    AppState* m_state = nullptr;
    MacroEngine* m_engine = nullptr;
    bool m_hasDeletedMacro = false;
    SavedMacro m_lastDeletedMacro;
    QList<EditableEventRow*> m_eventRows;
    bool m_sidebarCollapsed = false;
    bool m_stepsEditorVisible = false;
    QString m_editingMacroId;
    QList<QList<MacroEvent>> m_undoStack;
    QList<QList<MacroEvent>> m_redoStack;

    Button* m_btnRec = nullptr;
    Button* m_btnPlay = nullptr;
    Button* m_btnToCombo = nullptr;
    Button* m_btnClear = nullptr;
    Button* m_btnToggleSidebar = nullptr;

    GlassCard* m_libCard = nullptr;
    QLineEdit* m_nameEdit = nullptr;
    QWidget* m_libContainer = nullptr;
    QVBoxLayout* m_libVbox = nullptr;
    IconButton* m_btnUndoLib = nullptr;

    NumberInput* m_numSpeed = nullptr;
    SegmentedControl* m_repModeSeg = nullptr;
    NumberInput* m_numRepeatCount = nullptr;
    NumberInput* m_numDurationVal = nullptr;
    SearchCombo* m_comboDurationUnit = nullptr;

    ToggleSwitch* m_togDelayBetween = nullptr;
    NumberInput* m_numDelayBetween = nullptr;
    ToggleSwitch* m_togDelayRandom = nullptr;
    ToggleSwitch* m_togNoActivate = nullptr;
    ToggleSwitch* m_togShutdown = nullptr;
    NumberInput* m_numPlayStartDelay = nullptr;
    ToggleSwitch* m_togFloatingPill = nullptr;

    QLabel* m_lblLiveRecBanner = nullptr;

    QWidget* m_stepsHiddenHolder = nullptr;
    GlassCard* m_stepsHiddenCard = nullptr;
    QLabel* m_lblStepsHidden = nullptr;
    Button* m_btnOpenStepsEditor = nullptr;

    GlassCard* m_recEditToolbar = nullptr;
    QLabel* m_lblEditingRecBadge = nullptr;
    Button* m_btnRecUndo = nullptr;
    Button* m_btnRecRedo = nullptr;
    Button* m_btnRecSaveCurr = nullptr;
    Button* m_btnRecSaveNew = nullptr;
    Button* m_btnRecHideSteps = nullptr;

    QScrollArea* m_scroll = nullptr;
    QWidget* m_tlContainer = nullptr;
    QVBoxLayout* m_tlVbox = nullptr;
    ResponsiveFlowBar* m_recAdderBar = nullptr;
};

} // namespace vmacro
