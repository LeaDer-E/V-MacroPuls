#include "SettingsPage.h"
#include "Icons.h"
#include "Widgets.h"
#include "../core/AppState.h"
#include "../core/I18n.h"
#include "../core/Paths.h"
#include "../core/Theme.h"
#include "../engine/AutoPresserEngine.h"
#include "../engine/MacroEngine.h"
#include "../platform/MicrocontrollerBridge.h"
#include "../platform/Win32Input.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMetaObject>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QScrollArea>
#include <thread>

namespace vmacro {

// ============================================================================
// ColorSwatchButton
// ============================================================================

ColorSwatchButton::ColorSwatchButton(const QString& paletteId, QWidget* parent)
    : QWidget(parent), m_paletteId(paletteId) {
    setFixedSize(34, 34);
    setCursor(Qt::PointingHandCursor);
    for (const auto& pal : Theme::instance().palettes()) {
        if (pal.id == m_paletteId) {
            setToolTip(QStringLiteral("%1 (%2)").arg(I18n::isRtl() ? pal.arName : pal.enName, pal.accentHex));
            break;
        }
    }
}

void ColorSwatchButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_paletteId);
    }
}

void ColorSwatchButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QString accentHex;
    for (const auto& pal : Theme::instance().palettes()) {
        if (pal.id == m_paletteId) {
            accentHex = pal.accentHex;
            break;
        }
    }
    if (accentHex.isEmpty()) return;

    QColor c(accentHex);
    const bool active = (Theme::instance().paletteId() == m_paletteId);
    const QPointF center(width() / 2.0, height() / 2.0);

    if (active) {
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(c, 2.2));
        p.drawEllipse(center, 16.0, 16.0);
    }

    p.setBrush(c);
    p.setPen(Qt::NoPen);
    p.drawEllipse(center, 12.0, 12.0);

    if (active) {
        Icons::draw(&p, "check", QRectF(center.x() - 6.0, center.y() - 6.0, 12.0, 12.0), QColor("#FFFFFF"), 2.4);
    }
}

// ============================================================================
// SettingsPage
// ============================================================================

SettingsPage::SettingsPage(AppState* state, MacroEngine* engine, QWidget* parent)
    : QWidget(parent), m_state(state), m_engine(engine) {
    buildUi();
    if (m_state) {
        connect(m_state, &AppState::settingsChanged, this, &SettingsPage::onExternalSettingsChanged);
        connect(this, &SettingsPage::aboutRequested, m_state, [this]() {
            if (m_state) emit m_state->navigationRequested("about");
        });
    }
    connect(this, &SettingsPage::mcuDiagReady, this, &SettingsPage::applyMcuDiagOnMainThread);
    refreshKernelDiagnostics(false, false);
    refreshPs2Diagnostics(false, false);
    refreshMcuDiagnostics(false, false);
}

void SettingsPage::refreshUi() {
    refreshKernelDiagnostics(false, false);
    refreshPs2Diagnostics(false, false);
    refreshMcuDiagnostics(false, false);
}

void SettingsPage::buildUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(18, 8, 18, 12);
    outer->setSpacing(10);

    // Header Card
    auto* hdr = new GlassCard(true, this);
    auto* hLay = new QHBoxLayout(hdr);
    hLay->setContentsMargins(18, 12, 18, 12);

    auto* tCol = new QVBoxLayout();
    tCol->setSpacing(2);
    auto* title = new QLabel(I18n::t("settings_title"), hdr);
    title->setFont(makeFont(17, QFont::ExtraBold));
    title->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    auto* sub = new QLabel(I18n::t("settings_subtitle"), hdr);
    sub->setFont(makeFont(12, QFont::Medium));
    sub->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    tCol->addWidget(title);
    tCol->addWidget(sub);
    hLay->addLayout(tCol, 1);

    auto* btnTestLab = new Button(I18n::t("nav_test"), "primary", "test", 38, hdr);
    connect(btnTestLab, &Button::clicked, this, [this]() {
        if (m_state) emit m_state->navigationRequested("test");
    });

    auto* btnAbout = new Button(I18n::t("btn_about_app"), "glass", "about", 38, hdr);
    connect(btnAbout, &Button::clicked, this, &SettingsPage::aboutRequested);

    auto* btnReset = new Button(I18n::t("btn_reset_settings"), "ghost", "trash", 38, hdr);
    connect(btnReset, &Button::clicked, this, &SettingsPage::resetDefaults);

    hLay->addWidget(btnTestLab);
    hLay->addWidget(btnAbout);
    hLay->addWidget(btnReset);
    outer->addWidget(hdr);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(modernScrollStyle());

    auto* content = new QWidget();
    content->setStyleSheet("background: transparent;");
    auto* cLay = new QVBoxLayout(content);
    cLay->setContentsMargins(0, 0, 4, 12);
    cLay->setSpacing(12);

    // 1. Language Drop Panel & Appearance Card
    auto* cardApp = new GlassCard(false, content);
    auto* aLay = new QVBoxLayout(cardApp);
    aLay->setContentsMargins(20, 14, 20, 14);
    aLay->setSpacing(12);

    QList<QPair<QString, QString>> langItems;
    for (const auto& info : I18n::supportedLanguages()) {
        langItems.append(qMakePair(info.code, info.name));
    }
    m_langCombo = new SearchCombo(langItems, I18n::language(), cardApp);
    m_langCombo->setFixedWidth(240);
    connect(m_langCombo, &SearchCombo::currentChanged, this, [this](const QString& code) {
        if (m_state) m_state->setLanguage(code);
    });
    addRow(aLay, I18n::t("lbl_language"), m_langCombo);

    auto* modeSeg = new SegmentedControl(
        {{"dark", I18n::t("mode_dark")}, {"light", I18n::t("mode_light")}},
        Theme::instance().isDark() ? "dark" : "light",
        cardApp
    );
    connect(modeSeg, &SegmentedControl::changed, this, [this](const QString& m) {
        if (m_state) m_state->setTheme(m);
    });
    addRow(aLay, I18n::t("lbl_theme_mode"), modeSeg);

    auto* swatchW = new QWidget(cardApp);
    auto* swOuter = new QVBoxLayout(swatchW);
    swOuter->setContentsMargins(0, 0, 0, 0);
    swOuter->setSpacing(8);

    auto* swGrid = new QGridLayout();
    swGrid->setContentsMargins(0, 0, 0, 0);
    swGrid->setHorizontalSpacing(8);
    swGrid->setVerticalSpacing(8);

    const auto palettes = Theme::instance().palettes();
    const int cols = 12;
    for (int idx = 0; idx < palettes.size(); ++idx) {
        auto* sw = new ColorSwatchButton(palettes[idx].id, swatchW);
        connect(sw, &ColorSwatchButton::clicked, this, &SettingsPage::pickPalette);
        swGrid->addWidget(sw, idx / cols, idx % cols);
        m_swatches.append(sw);
    }
    swOuter->addLayout(swGrid);

    auto* customColRow = new QHBoxLayout();
    customColRow->setContentsMargins(0, 0, 0, 0);
    m_btnCustomAccent = new Button(
        I18n::t("btn_custom_color"),
        "glass",
        "palette",
        Theme::instance().tokenHex("accent"),
        false,
        swatchW
    );
    m_btnCustomAccent->setFixedHeight(32);
    connect(m_btnCustomAccent, &Button::clicked, this, &SettingsPage::pickCustomColorDialog);
    customColRow->addWidget(m_btnCustomAccent);
    customColRow->addStretch(1);
    swOuter->addLayout(customColRow);

    addRow(aLay, I18n::t("lbl_color_accent"), swatchW);
    cLay->addWidget(cardApp);

    // 2. Input Simulation Method & Live Interception / PS2 / MCU Diagnostic Card
    auto* cardInput = new GlassCard(true, content);
    auto* inLay = new QVBoxLayout(cardInput);
    inLay->setContentsMargins(20, 14, 20, 14);
    inLay->setSpacing(10);

    auto* lblMethod = new QLabel(I18n::t("lbl_input_method"), cardInput);
    lblMethod->setFont(makeFont(13, QFont::ExtraBold));
    lblMethod->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    inLay->addWidget(lblMethod);

    m_methodCombo = new SearchCombo(
        AutoPresserEngine::localizedKeyMethodOptions(),
        m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode"),
        cardInput
    );
    m_methodCombo->setMinimumWidth(520);
    m_methodCombo->setFixedHeight(38);
    connect(m_methodCombo, &SearchCombo::currentChanged, this, &SettingsPage::onInputMethodChanged);
    inLay->addWidget(m_methodCombo);

    // 2a. Ring-0 Interception Diagnostic Box
    auto* diagBox = new GlassCard(false, cardInput);
    auto* dLay = new QVBoxLayout(diagBox);
    dLay->setContentsMargins(16, 12, 16, 12);
    dLay->setSpacing(8);

    auto* lblDiagHdr = new QLabel(I18n::t("lbl_kernel_diag_title"), diagBox);
    lblDiagHdr->setFont(makeFont(13, QFont::ExtraBold));
    lblDiagHdr->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    dLay->addWidget(lblDiagHdr);

    m_lblKernelHeadline = new QLabel(diagBox);
    m_lblKernelHeadline->setWordWrap(true);
    m_lblKernelHeadline->setFont(makeFont(13, QFont::ExtraBold));
    dLay->addWidget(m_lblKernelHeadline);

    m_lblKernelDetails = new QLabel(diagBox);
    m_lblKernelDetails->setWordWrap(true);
    m_lblKernelDetails->setFont(makeFont(12, QFont::DemiBold));
    m_lblKernelDetails->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    dLay->addWidget(m_lblKernelDetails);

    m_btnCheckInterception = new Button(I18n::t("btn_check_interception"), "primary", "shield", 34, diagBox);
    connect(m_btnCheckInterception, &Button::clicked, this, [this]() {
        refreshKernelDiagnostics(true, true);
    });

    m_btnInstallInterception = new Button(I18n::t("btn_install_interception"), "glass", "bolt", "#10B981", false, diagBox);
    m_btnInstallInterception->setFixedHeight(34);
    connect(m_btnInstallInterception, &Button::clicked, this, &SettingsPage::onInstallOrLinkInterception);

    m_btnOpenDrivers = new Button(I18n::t("btn_open_drivers_dir"), "ghost", "folder", 34, diagBox);
    connect(m_btnOpenDrivers, &Button::clicked, this, []() {
        Win32Input::openFolderOrPath(Paths::driversDir());
    });

    auto* diagBtnsBar = new ResponsiveFlowBar(
        {m_btnCheckInterception, m_btnInstallInterception, m_btnOpenDrivers},
        34, 8, 8, diagBox
    );
    dLay->addWidget(diagBtnsBar);
    inLay->addWidget(diagBox);

    // 2b. PS/2 Port 0x60/0x64 (InpOutx64 / i8042prt) Diagnostic Box
    auto* ps2Box = new GlassCard(false, cardInput);
    auto* p2Lay = new QVBoxLayout(ps2Box);
    p2Lay->setContentsMargins(16, 12, 16, 12);
    p2Lay->setSpacing(8);

    auto* lblPs2Hdr = new QLabel(I18n::t("lbl_ps2_diag_title"), ps2Box);
    lblPs2Hdr->setFont(makeFont(13, QFont::ExtraBold));
    lblPs2Hdr->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    p2Lay->addWidget(lblPs2Hdr);

    m_lblPs2Headline = new QLabel(ps2Box);
    m_lblPs2Headline->setWordWrap(true);
    m_lblPs2Headline->setFont(makeFont(13, QFont::ExtraBold));
    p2Lay->addWidget(m_lblPs2Headline);

    m_lblPs2Details = new QLabel(ps2Box);
    m_lblPs2Details->setWordWrap(true);
    m_lblPs2Details->setFont(makeFont(12, QFont::DemiBold));
    m_lblPs2Details->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    p2Lay->addWidget(m_lblPs2Details);

    m_btnCheckPs2 = new Button(I18n::t("btn_check_ps2"), "primary", "shield", 34, ps2Box);
    connect(m_btnCheckPs2, &Button::clicked, this, [this]() {
        refreshPs2Diagnostics(true, true);
    });

    m_btnInstallPs2 = new Button(I18n::t("btn_install_ps2"), "glass", "bolt", "#06B6D4", false, ps2Box);
    m_btnInstallPs2->setFixedHeight(34);
    connect(m_btnInstallPs2, &Button::clicked, this, &SettingsPage::onInstallOrLinkPs2);

    m_btnOpenPs2Drivers = new Button(I18n::t("btn_open_drivers_dir"), "ghost", "folder", 34, ps2Box);
    connect(m_btnOpenPs2Drivers, &Button::clicked, this, []() {
        Win32Input::openFolderOrPath(Paths::driversDir());
    });

    auto* ps2BtnsBar = new ResponsiveFlowBar(
        {m_btnCheckPs2, m_btnInstallPs2, m_btnOpenPs2Drivers},
        34, 8, 8, ps2Box
    );
    p2Lay->addWidget(ps2BtnsBar);
    inLay->addWidget(ps2Box);

    // 2c. External Hardware Microcontroller Bridge Box (Pico & Arduino)
    auto* mcuBox = new GlassCard(false, cardInput);
    auto* mLay = new QVBoxLayout(mcuBox);
    mLay->setContentsMargins(16, 12, 16, 12);
    mLay->setSpacing(8);

    auto* lblMcuHdr = new QLabel(I18n::t("lbl_mcu_diag_title"), mcuBox);
    lblMcuHdr->setFont(makeFont(13, QFont::ExtraBold));
    lblMcuHdr->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    mLay->addWidget(lblMcuHdr);

    auto* portRow = new QHBoxLayout();
    portRow->setSpacing(10);
    auto* lblPort = new QLabel(I18n::t("lbl_mcu_com_port"), mcuBox);
    lblPort->setFont(makeFont(12, QFont::Bold));
    lblPort->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    const QString curPort = (m_state && !m_state->config.mcuComPort.isEmpty()) ? m_state->config.mcuComPort : QStringLiteral("auto");
    m_mcuPortCombo = new SearchCombo(buildMcuPortDropdownItems(), curPort, mcuBox);
    m_mcuPortCombo->setMinimumWidth(310);
    m_mcuPortCombo->setFixedHeight(34);
    connect(m_mcuPortCombo, &SearchCombo::currentChanged, this, &SettingsPage::onMcuPortChanged);

    portRow->addWidget(lblPort);
    portRow->addWidget(m_mcuPortCombo, 1);
    mLay->addLayout(portRow);

    m_lblMcuHeadline = new QLabel(mcuBox);
    m_lblMcuHeadline->setWordWrap(true);
    m_lblMcuHeadline->setFont(makeFont(13, QFont::ExtraBold));
    mLay->addWidget(m_lblMcuHeadline);

    m_lblMcuDetails = new QLabel(mcuBox);
    m_lblMcuDetails->setWordWrap(true);
    m_lblMcuDetails->setFont(makeFont(12, QFont::DemiBold));
    m_lblMcuDetails->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    mLay->addWidget(m_lblMcuDetails);

    m_btnCheckMcu = new Button(I18n::t("btn_check_mcu"), "primary", "bolt", 34, mcuBox);
    connect(m_btnCheckMcu, &Button::clicked, this, [this]() {
        refreshMcuDiagnostics(true, true);
    });

    m_btnAutoInstallPico = new Button(I18n::t("btn_auto_install_pico"), "glass", "shield", "#10B981", false, mcuBox);
    m_btnAutoInstallPico->setFixedHeight(34);
    connect(m_btnAutoInstallPico, &Button::clicked, this, &SettingsPage::onAutoInstallPicoCircuitPython);

    m_btnExtractMcuFw = new Button(I18n::t("btn_extract_mcu_firmware"), "glass", "folder", "#F59E0B", false, mcuBox);
    m_btnExtractMcuFw->setFixedHeight(34);
    connect(m_btnExtractMcuFw, &Button::clicked, this, &SettingsPage::onExtractMcuFirmware);

    auto* mcuBtnsBar = new ResponsiveFlowBar(
        {m_btnCheckMcu, m_btnAutoInstallPico, m_btnExtractMcuFw},
        34, 8, 8, mcuBox
    );
    mLay->addWidget(mcuBtnsBar);
    inLay->addWidget(mcuBox);

    cLay->addWidget(cardInput);

    // 3. Full Backup Card
    auto* cardBackup = new GlassCard(true, content);
    auto* bkLay = new QHBoxLayout(cardBackup);
    bkLay->setContentsMargins(20, 14, 20, 14);
    bkLay->setSpacing(12);

    auto* bkLbl = new QLabel(I18n::t("lbl_backup_section"), cardBackup);
    bkLbl->setFont(makeFont(13, QFont::ExtraBold));
    bkLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    bkLay->addWidget(bkLbl, 1);

    auto* btnExpAll = new Button(I18n::t("btn_export_all"), "primary", "save", 36, cardBackup);
    connect(btnExpAll, &Button::clicked, this, &SettingsPage::exportFullBackup);

    auto* btnImpAll = new Button(I18n::t("btn_import_all"), "glass", "folder", 36, cardBackup);
    connect(btnImpAll, &Button::clicked, this, &SettingsPage::importFullBackup);

    bkLay->addWidget(btnExpAll);
    bkLay->addWidget(btnImpAll);
    cLay->addWidget(cardBackup);

    // 4. System Behavior, Windows Startup & Multi-Key Global Hotkeys Card
    auto* cardBeh = new GlassCard(false, content);
    auto* bLay = new QVBoxLayout(cardBeh);
    bLay->setContentsMargins(20, 14, 20, 14);
    bLay->setSpacing(12);

    const bool adminOk = Win32Input::isRunningAsAdmin();
    auto* adminRow = new QHBoxLayout();
    adminRow->setSpacing(10);
    const QString adminTxt = I18n::t(adminOk ? "lbl_admin_ok" : "lbl_admin_warn");
    auto* adminLbl = new QLabel(adminTxt, cardBeh);
    adminLbl->setWordWrap(true);
    adminLbl->setFont(makeFont(12, QFont::Bold));
    adminLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex(adminOk ? "ok" : "accent_hi")));
    adminRow->addWidget(adminLbl, 1);
    if (!adminOk) {
        auto* btnElevate = new Button(I18n::t("btn_relaunch_admin"), "glass", "shield", "#10B981", false, cardBeh);
        btnElevate->setFixedHeight(32);
        connect(btnElevate, &Button::clicked, this, []() { Win32Input::relaunchAsAdmin(); });
        adminRow->addWidget(btnElevate);
    }
    bLay->addLayout(adminRow);

    const bool startupActive = Win32Input::isWindowsStartupEnabled() || (m_state && m_state->config.runOnWindowsStartup);
    auto* togStartup = new ToggleSwitch(startupActive, cardBeh);
    connect(togStartup, &ToggleSwitch::toggled, this, &SettingsPage::onStartupToggled);
    addRow(bLay, I18n::t("lbl_run_on_startup"), togStartup);

    auto* togSup = new ToggleSwitch(m_state ? m_state->config.suppressTriggerKey : true, cardBeh);
    connect(togSup, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("suppress_trigger_key", v);
    });
    addRow(bLay, I18n::t("lbl_suppress_key"), togSup);

    auto* togTray = new ToggleSwitch(m_state ? m_state->config.minimizeToTray : true, cardBeh);
    connect(togTray, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("minimize_to_tray", v);
    });
    addRow(bLay, I18n::t("lbl_min_to_tray"), togTray);

    auto* togTop = new ToggleSwitch(m_state ? m_state->config.alwaysOnTop : false, cardBeh);
    connect(togTop, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("always_on_top", v);
    });
    addRow(bLay, I18n::t("lbl_always_on_top"), togTop);

    auto* togSnd = new ToggleSwitch(m_state ? m_state->config.soundEffectsEnabled : false, cardBeh);
    connect(togSnd, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("sound_effects_enabled", v);
    });
    addRow(bLay, I18n::t("lbl_sounds"), togSnd);

    auto* hkRec = new KeyCaptureButton(m_state, m_state ? m_state->config.hotkeyRecordToggle : "F9", true, cardBeh);
    hkRec->setFixedWidth(170);
    connect(hkRec, &KeyCaptureButton::keyCaptured, this, [this](const QString& k, int, bool) {
        if (m_state) m_state->updateSetting("hotkey_record_toggle", k.isEmpty() ? "F9" : k);
    });
    addRow(bLay, I18n::t("lbl_hk_rec"), hkRec);

    auto* hkPlay = new KeyCaptureButton(m_state, m_state ? m_state->config.hotkeyPlaybackToggle : "F10", true, cardBeh);
    hkPlay->setFixedWidth(170);
    connect(hkPlay, &KeyCaptureButton::keyCaptured, this, [this](const QString& k, int, bool) {
        if (m_state) m_state->updateSetting("hotkey_playback_toggle", k.isEmpty() ? "F10" : k);
    });
    addRow(bLay, I18n::t("lbl_hk_play"), hkPlay);

    auto* hkCombo = new KeyCaptureButton(m_state, m_state ? m_state->config.hotkeyComboMasterToggle : "F11", true, cardBeh);
    hkCombo->setFixedWidth(170);
    connect(hkCombo, &KeyCaptureButton::keyCaptured, this, [this](const QString& k, int, bool) {
        if (m_state) m_state->updateSetting("hotkey_combo_master_toggle", k.isEmpty() ? "F11" : k);
    });
    addRow(bLay, I18n::t("lbl_hk_combo"), hkCombo);

    cLay->addWidget(cardBeh);

    // 5. Dedicated Auto Key Presser Studio Hotkeys Card
    auto* cardAkpHk = new GlassCard(true, content);
    auto* akpHkLay = new QVBoxLayout(cardAkpHk);
    akpHkLay->setContentsMargins(20, 14, 20, 14);
    akpHkLay->setSpacing(10);

    auto* lblAkpHkTitle = new QLabel(I18n::t("settings_akp_hotkeys_title"), cardAkpHk);
    lblAkpHkTitle->setFont(makeFont(13, QFont::ExtraBold));
    lblAkpHkTitle->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    akpHkLay->addWidget(lblAkpHkTitle);

    m_hkAkpStopAll = new KeyCaptureButton(m_state, m_state ? m_state->config.hotkeyAutopresserStopAll : "F7", true, cardAkpHk);
    m_hkAkpStopAll->setFixedWidth(220);
    connect(m_hkAkpStopAll, &KeyCaptureButton::keyCaptured, this, [this](const QString& k, int, bool) {
        if (m_state) m_state->updateSetting("hotkey_autopresser_stop_all", k.isEmpty() ? "F7" : k);
    });
    addRow(akpHkLay, I18n::t("akp_lbl_stop_all_hotkey"), m_hkAkpStopAll);

    auto* lblAkpNote = new QLabel(I18n::t("settings_akp_hotkeys_note"), cardAkpHk);
    lblAkpNote->setWordWrap(true);
    lblAkpNote->setFont(makeFont(12, QFont::DemiBold));
    lblAkpNote->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    akpHkLay->addWidget(lblAkpNote);

    cLay->addWidget(cardAkpHk);

    // 6. Emergency Stop Card at the Bottom
    auto* panicCard = new GlassCard(false, content);
    auto* pOuter = new QVBoxLayout(panicCard);
    pOuter->setContentsMargins(20, 14, 20, 14);
    pOuter->setSpacing(8);

    auto* lblPanicHdr = new QLabel(QString::fromUtf8("⛔ %1").arg(I18n::t("lbl_panic_section_title")), panicCard);
    lblPanicHdr->setFont(makeFont(14, QFont::ExtraBold));
    lblPanicHdr->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("danger")));
    pOuter->addWidget(lblPanicHdr);

    auto* lblPanicDesc = new QLabel(I18n::t("lbl_panic_section_desc"), panicCard);
    lblPanicDesc->setWordWrap(true);
    lblPanicDesc->setFont(makeFont(12, QFont::Medium));
    lblPanicDesc->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    pOuter->addWidget(lblPanicDesc);

    auto* pLay = new QHBoxLayout();
    pLay->setSpacing(12);
    auto* lblPanic = new QLabel(QString::fromUtf8("⛔ %1").arg(I18n::t("lbl_hk_panic_all")), panicCard);
    lblPanic->setFont(makeFont(13, QFont::ExtraBold));
    lblPanic->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("danger")));

    auto* hkStopPlay = new KeyCaptureButton(m_state, m_state ? m_state->config.hotkeyPanicStop : "Ctrl + Shift + Alt", true, panicCard);
    hkStopPlay->setFixedWidth(220);
    connect(hkStopPlay, &KeyCaptureButton::keyCaptured, this, [this](const QString& k, int, bool) {
        if (m_state) m_state->updateSetting("hotkey_panic_stop", k.isEmpty() ? "Ctrl + Shift + Alt" : k);
    });
    pLay->addWidget(lblPanic, 1);
    pLay->addWidget(hkStopPlay);
    pOuter->addLayout(pLay);

    cLay->addWidget(panicCard);
    cLay->addStretch(1);

    scroll->setWidget(content);
    outer->addWidget(scroll, 1);
}

void SettingsPage::addRow(QVBoxLayout* parentLayout, const QString& labelText, QWidget* control) {
    auto* row = new QHBoxLayout();
    row->setSpacing(12);
    auto* lbl = new QLabel(labelText, this);
    lbl->setFont(makeFont(13, QFont::DemiBold));
    lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    row->addWidget(lbl, 1);
    row->addWidget(control);
    parentLayout->addLayout(row);
}

QList<QPair<QString, QString>> SettingsPage::buildMcuPortDropdownItems() const {
    QList<QPair<QString, QString>> items;
    items.append(qMakePair(QStringLiteral("auto"), I18n::t("mcu_port_auto")));
    for (const auto& pInfo : MicrocontrollerBridge::instance().listSerialPorts()) {
        const QString bTag = !pInfo.boardType.isEmpty() ? QStringLiteral(" [%1]").arg(pInfo.boardType.toUpper()) : QString();
        items.append(qMakePair(pInfo.port, QStringLiteral("%1 — %2%3").arg(pInfo.port, pInfo.description, bTag)));
    }
    return items;
}

void SettingsPage::onExternalSettingsChanged(const QString& key, const QVariant& val) {
    if (key == "hotkey_autopresser_stop_all" && m_hkAkpStopAll) {
        m_hkAkpStopAll->setKeyString(val.toString().isEmpty() ? "F7" : val.toString());
    } else if (key == "input_method" && m_methodCombo) {
        m_methodCombo->setSelectedKey(val.toString(), false);
    }
}

void SettingsPage::onInputMethodChanged(const QString& methodKey) {
    if (m_state) m_state->updateSetting("input_method", methodKey);
    refreshKernelDiagnostics(false, false);
    refreshPs2Diagnostics(false, false);
    refreshMcuDiagnostics(methodKey == "pico_usb_hid" || methodKey == "arduino_usb_hid", false);
}

void SettingsPage::onMcuPortChanged(const QString& portKey) {
    const QString clean = portKey.trimmed().isEmpty() ? QStringLiteral("auto") : portKey.trimmed();
    if (m_state) m_state->updateSetting("mcu_com_port", clean);
    MicrocontrollerBridge::instance().setPreferredPort(clean);
    refreshMcuDiagnostics(true, true);
}

void SettingsPage::refreshKernelDiagnostics(bool runLiveProbe, bool showToast) {
    const KernelDriverDiagnostics diag = Win32Input::diagnoseKernelDrivers(runLiveProbe);
    QString headline, details, kind;
    Win32Input::formatKernelDiagnosticReport(diag, I18n::isRtl(), headline, details, kind);
    const QString colHex = (kind == "ok")
        ? Theme::instance().tokenHex("ok")
        : (kind == "warn") ? QStringLiteral("#F59E0B") : Theme::instance().tokenHex("accent_hi");

    if (m_lblKernelHeadline) {
        m_lblKernelHeadline->setText(headline);
        m_lblKernelHeadline->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(colHex));
    }
    if (m_lblKernelDetails) {
        m_lblKernelDetails->setText(details);
    }
    if (showToast && m_state) {
        emit m_state->toastRequested(headline, kind == "ok" ? "ok" : "info");
    }
}

void SettingsPage::refreshPs2Diagnostics(bool runLiveProbe, bool showToast) {
    const Ps2Diagnostics diag = Win32Input::diagnosePs2PortDriver(runLiveProbe);
    QString headline, details, kind;
    Win32Input::formatPs2DiagnosticReport(diag, I18n::isRtl(), headline, details, kind);
    const QString colHex = (kind == "ok")
        ? Theme::instance().tokenHex("ok")
        : (kind == "warn") ? QStringLiteral("#F59E0B") : Theme::instance().tokenHex("accent_hi");

    if (m_lblPs2Headline) {
        m_lblPs2Headline->setText(headline);
        m_lblPs2Headline->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(colHex));
    }
    if (m_lblPs2Details) {
        m_lblPs2Details->setText(details);
    }
    if (showToast && m_state) {
        emit m_state->toastRequested(headline, kind == "ok" ? "ok" : "info");
    }
}

void SettingsPage::refreshMcuDiagnostics(bool runLivePing, bool showToast) {
    const QString prefPort = (m_state && !m_state->config.mcuComPort.isEmpty()) ? m_state->config.mcuComPort : QStringLiteral("auto");
    if (m_mcuPortCombo) {
        m_mcuPortCombo->setItems(buildMcuPortDropdownItems(), prefPort);
    }
    if (!runLivePing) {
        const McuDiagnostics diag = MicrocontrollerBridge::instance().diagnose(prefPort, false);
        applyMcuDiagOnMainThread(diag, showToast);
        return;
    }
    if (m_btnCheckMcu) m_btnCheckMcu->setBusy(true);

    QPointer<SettingsPage> self(this);
    std::thread([self, prefPort, showToast]() {
        const McuDiagnostics d = MicrocontrollerBridge::instance().diagnose(prefPort, true);
        if (self) {
            QMetaObject::invokeMethod(self, [self, d, showToast]() {
                if (self) self->applyMcuDiagOnMainThread(d, showToast);
            }, Qt::QueuedConnection);
        }
    }).detach();
}

void SettingsPage::applyMcuDiagOnMainThread(const McuDiagnostics& diag, bool showToast) {
    if (m_btnCheckMcu) m_btnCheckMcu->setBusy(false);
    QString headline, details, kind;
    MicrocontrollerBridge::instance().formatDiagnosticReport(diag, I18n::isRtl(), headline, details, kind);
    const QString colHex = (kind == "ok")
        ? Theme::instance().tokenHex("ok")
        : (kind == "warn") ? QStringLiteral("#F59E0B") : Theme::instance().tokenHex("accent_hi");

    if (m_lblMcuHeadline) {
        m_lblMcuHeadline->setText(headline);
        m_lblMcuHeadline->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(colHex));
    }
    if (m_lblMcuDetails) {
        m_lblMcuDetails->setText(details);
    }
    if (showToast && m_state) {
        emit m_state->toastRequested(headline, kind == "ok" ? "ok" : "info");
    }
}

void SettingsPage::onInstallOrLinkInterception() {
    const QString drvDir = Paths::driversDir();
    bool hasFiles = QFileInfo::exists(drvDir + "/install-interception.exe") ||
                    QFileInfo::exists(drvDir + "/interception.dll") ||
                    QFileInfo::exists(drvDir + "/inpoutx64.dll");
    QString pickedPath;
    if (!hasFiles) {
        pickedPath = QFileDialog::getOpenFileName(
            this,
            I18n::t("btn_install_interception"),
            drvDir,
            "Driver & Bridge Files (interception.dll install-interception.exe inpoutx64.dll *.dll *.exe);;All Files (*.*)"
        );
        if (pickedPath.isEmpty()) return;
    }
    QString msg;
    const bool ok = Win32Input::installOrImportInterceptionFile(pickedPath, msg);
    refreshKernelDiagnostics(true, false);
    if (m_state) emit m_state->toastRequested(msg, ok ? "ok" : "danger");
}

void SettingsPage::onInstallOrLinkPs2() {
    const QString drvDir = Paths::driversDir();
    bool hasFiles = QFileInfo::exists(drvDir + "/inpoutx64.dll") ||
                    QFileInfo::exists(drvDir + "/InpOutx64.dll") ||
                    QFileInfo::exists(drvDir + "/WinRing0x64.dll") ||
                    QFileInfo::exists(drvDir + "/InstallDriver.exe");
    QString pickedPath;
    if (!hasFiles) {
        pickedPath = QFileDialog::getOpenFileName(
            this,
            I18n::t("btn_install_ps2"),
            drvDir,
            "PS/2 & InpOutx64 Files (inpoutx64.dll InstallDriver.exe WinRing0x64.dll *.dll *.exe);;All Files (*.*)"
        );
        if (pickedPath.isEmpty()) return;
    }
    QString msg;
    const bool ok = Win32Input::installOrImportPs2File(pickedPath, msg);
    refreshPs2Diagnostics(true, false);
    refreshKernelDiagnostics(false, false);
    if (m_state) emit m_state->toastRequested(msg, ok ? "ok" : "danger");
}

void SettingsPage::onAutoInstallPicoCircuitPython() {
    QString msg;
    const bool ok = MicrocontrollerBridge::instance().autoInstallCircuitPythonToPico(msg);
    refreshMcuDiagnostics(false, false);
    if (m_state) emit m_state->toastRequested(msg, ok ? "ok" : "info");
}

void SettingsPage::onExtractMcuFirmware() {
    const QString defaultDir = Paths::firmwareDir();
    const QString picked = QFileDialog::getExistingDirectory(this, I18n::t("btn_extract_mcu_firmware"), defaultDir);
    if (picked.isEmpty()) {
        QString dummy;
        MicrocontrollerBridge::instance().extractAllFirmwareFiles(defaultDir, dummy);
        Win32Input::openFolderOrPath(defaultDir);
        return;
    }
    QString msg;
    const bool ok = MicrocontrollerBridge::instance().extractAllFirmwareFiles(picked, msg);
    if (m_state) emit m_state->toastRequested(msg, ok ? "ok" : "danger");
    if (ok) Win32Input::openFolderOrPath(picked);
}

void SettingsPage::exportFullBackup() {
    if (!m_engine) return;
    const QString path = QFileDialog::getSaveFileName(this, I18n::t("btn_export_all"), "V-MacroPlus_Full_Backup.json", "JSON Backup (*.json)");
    if (!path.isEmpty()) m_engine->exportAllBackup(path);
}

void SettingsPage::importFullBackup() {
    if (!m_engine) return;
    const QString path = QFileDialog::getOpenFileName(this, I18n::t("btn_import_all"), QString(), "JSON Backup (*.json)");
    if (!path.isEmpty()) m_engine->importAllBackup(path);
}

void SettingsPage::onStartupToggled(bool val) {
    Win32Input::setWindowsStartupEnabled(val);
    if (m_state) m_state->updateSetting("run_on_windows_startup", val);
}

void SettingsPage::pickPalette(const QString& pid) {
    if (m_state) m_state->setTheme(Theme::instance().mode(), pid);
    for (auto* sw : m_swatches) sw->update();
}

void SettingsPage::pickCustomColorDialog() {
    const QColor initial(Theme::instance().tokenHex("accent"));
    const QColor picked = QColorDialog::getColor(initial, this, I18n::t("btn_custom_color"));
    if (picked.isValid()) {
        if (m_state) m_state->setTheme(Theme::instance().mode(), "custom", picked.name());
        for (auto* sw : m_swatches) sw->update();
    }
}

void SettingsPage::resetDefaults() {
    if (!m_state) return;
    AppConfig fresh;
    fresh.firstRunCompleted = true;
    m_state->config = fresh;
    m_state->config.save();
    m_state->setTheme(fresh.themeMode, fresh.paletteId, fresh.customAccent);
    emit m_state->toastRequested(I18n::t("toast_settings_reset"), "ok");
}

} // namespace vmacro
