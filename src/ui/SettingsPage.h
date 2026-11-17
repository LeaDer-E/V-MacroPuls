#pragma once
#include <QPointer>
#include <QLabel>
#include "../platform/MicrocontrollerBridge.h"
#include <QLabel>
#include <QList>
#include <QPair>
#include <QVBoxLayout>
#include <QWidget>

namespace vmacro {

class AppState;
class MacroEngine;
class Button;
class KeyCaptureButton;
class SearchCombo;

class ColorSwatchButton : public QWidget {
    Q_OBJECT
public:
    explicit ColorSwatchButton(const QString& paletteId, QWidget* parent = nullptr);
    QString paletteId() const { return m_paletteId; }

signals:
    void clicked(const QString& paletteId);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_paletteId;
};

class SettingsPage : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPage(AppState* state, MacroEngine* engine, QWidget* parent = nullptr);
    void refreshUi();

signals:
    void aboutRequested();
    void mcuDiagReady(const vmacro::McuDiagnostics& diag, bool showToast);

private:
    void buildUi();
    void addRow(QVBoxLayout* parentLayout, const QString& labelText, QWidget* control);
    QList<QPair<QString, QString>> buildMcuPortDropdownItems() const;
    void onExternalSettingsChanged(const QString& key, const QVariant& val);
    void onInputMethodChanged(const QString& methodKey);
    void onMcuPortChanged(const QString& portKey);
    void refreshKernelDiagnostics(bool runLiveProbe = true, bool showToast = true);
    void refreshPs2Diagnostics(bool runLiveProbe = true, bool showToast = true);
    void refreshMcuDiagnostics(bool runLivePing = true, bool showToast = true);
    void applyMcuDiagOnMainThread(const vmacro::McuDiagnostics& diag, bool showToast);
    void onInstallOrLinkInterception();
    void onInstallOrLinkPs2();
    void onAutoInstallPicoCircuitPython();
    void onExtractMcuFirmware();
    void exportFullBackup();
    void importFullBackup();
    void onStartupToggled(bool val);
    void pickPalette(const QString& pid);
    void pickCustomColorDialog();
    void resetDefaults();

    AppState* m_state = nullptr;
    MacroEngine* m_engine = nullptr;

    SearchCombo* m_langCombo = nullptr;
    SearchCombo* m_methodCombo = nullptr;
    SearchCombo* m_mcuPortCombo = nullptr;
    Button* m_btnCustomAccent = nullptr;
    QList<ColorSwatchButton*> m_swatches;

    QLabel* m_lblKernelHeadline = nullptr;
    QLabel* m_lblKernelDetails = nullptr;
    Button* m_btnCheckInterception = nullptr;
    Button* m_btnInstallInterception = nullptr;
    Button* m_btnOpenDrivers = nullptr;

    QLabel* m_lblPs2Headline = nullptr;
    QLabel* m_lblPs2Details = nullptr;
    Button* m_btnCheckPs2 = nullptr;
    Button* m_btnInstallPs2 = nullptr;
    Button* m_btnOpenPs2Drivers = nullptr;

    QLabel* m_lblMcuHeadline = nullptr;
    QLabel* m_lblMcuDetails = nullptr;
    Button* m_btnCheckMcu = nullptr;
    Button* m_btnAutoInstallPico = nullptr;
    Button* m_btnExtractMcuFw = nullptr;

    KeyCaptureButton* m_hkAkpStopAll = nullptr;
};

} // namespace vmacro
