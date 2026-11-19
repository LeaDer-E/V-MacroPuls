#include "TestPage.h"
#include "../Widgets.h"
#include "../../core/AppState.h"
#include "../../core/I18n.h"
#include "../../core/Theme.h"
#include "../../engine/AutoPresserEngine.h"
#include "../../engine/MacroEngine.h"
#include "../../engine/PythonScriptRunner.h"
#include "../../platform/Win32Input.h"

#include <QDateTime>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPointer>
#include <QThread>
#include <algorithm>
#include <thread>

namespace vmacro {

static const QString kDefaultTemplateSweepStr = QStringLiteral("key=\"Enter\" {method_num}: Hello V-MacroPlus worked with {method_id} \\n");

static QList<QPair<QString, QString>> sweepTemplateVariables() {
    return {
        {"key=\"Enter\"", "key=\"Enter\""},
        {"\\n", "\\n (Enter)"},
        {"\\t", "\\t (Tab)"},
        {"wait=100", "wait=100"},
        {"{method_num}", "{method_num}"},
        {"{method_id}", "{method_id}"},
        {"{method_name}", "{method_name}"},
        {"{category}", "{category}"},
        {"{target_app}", "{target_app}"},
        {"{target_hwnd}", "{target_hwnd}"},
        {"{mode}", "{mode}"},
        {"{time}", "{time}"},
        {"{date}", "{date}"},
    };
}

// ============================================================================
// LiveInputSandboxBox
// ============================================================================

LiveInputSandboxBox::LiveInputSandboxBox(AppState* state, LogEntryCb onLogEntry, QWidget* parent)
    : QWidget(parent), m_state(state), m_onLogEntry(std::move(onLogEntry)) {
    setFixedHeight(48);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);

    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(14, 6, 14, 6);
    lay->setSpacing(10);

    m_testInput = new QLineEdit(this);
    m_testInput->setPlaceholderText(I18n::t("sandbox_input_placeholder"));
    m_testInput->setFixedHeight(32);
    m_testInput->setMinimumWidth(290);
    m_testInput->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 8px;
            padding: 3px 10px;
            font-family: '%4';
            font-size: 12px;
            font-weight: bold;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border_hi"),
            I18n::langFontFamily()));

    m_lblStatus = new QLabel(I18n::t("sandbox_hint_status"), this);
    m_lblStatus->setFont(makeFont(11, QFont::ExtraBold));
    m_lblStatus->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    lay->addWidget(m_testInput, 1);
    lay->addWidget(m_lblStatus, 1);
}

EffectiveEngineInfo LiveInputSandboxBox::cachedEffInfo() {
    if (!m_hasCachedEff) {
        const QString req = m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode");
        m_cachedEff = Win32Input::resolveEffectiveInputEngine(req);
        m_hasCachedEff = true;
    }
    return m_cachedEff;
}

void LiveInputSandboxBox::mousePressEvent(QMouseEvent* event) {
    setFocus();
    const QString btnName = (event->button() == Qt::LeftButton)
        ? QStringLiteral("Left Click")
        : (event->button() == Qt::RightButton) ? QStringLiteral("Right Click") : QStringLiteral("Middle Click");
    const QPoint pos = event->globalPosition().toPoint();
    const EffectiveEngineInfo effInfo = cachedEffInfo();
    const QString fb = effInfo.isFallback
        ? QStringLiteral(" (%1 %2)").arg(I18n::t("log_fallback_from"), effInfo.requestedMethod)
        : QString();
    const QString plainMsg = QStringLiteral("%1 %2 (%3) %4 (%5, %6) — %7 %8%9")
        .arg(I18n::t("log_recv_mouse"), I18n::t("log_mouse_click_at"), btnName, I18n::t("log_at_coords"))
        .arg(pos.x()).arg(pos.y())
        .arg(I18n::t("log_actual_engine"), effInfo.effectiveMethod, fb);
    m_lblStatus->setText(plainMsg);

    if (m_onLogEntry) {
        QJsonObject obj;
        obj["kind"] = "mouse";
        obj["btn_name"] = btnName;
        obj["x"] = pos.x();
        obj["y"] = pos.y();
        obj["requested_method"] = m_state ? m_state->config.inputMethod : "sendinput_scancode";
        m_onLogEntry(obj, "#10B981");
    }
    QWidget::mousePressEvent(event);
}

void LiveInputSandboxBox::keyPressEvent(QKeyEvent* event) {
    const int hwSc = static_cast<int>(event->nativeScanCode()) & 0xFF;
    const QString lbl = hwSc > 0 ? Win32Input::labelForScancode(hwSc, false) : (event->text().isEmpty() ? "Key" : event->text());
    const int hidUsage = Win32Input::usbHidUsageForScancode(hwSc, false);
    const EffectiveEngineInfo effInfo = cachedEffInfo();
    const QString fb = effInfo.isFallback
        ? QStringLiteral(" (%1 %2)").arg(I18n::t("log_fallback_from"), effInfo.requestedMethod)
        : QString();
    const QString plainMsg = QStringLiteral("%1 [ %2 ] (ScanCode: 0x%3 | USB HID: 0x%4) — %5 %6%7")
        .arg(I18n::t("log_recv_key"), lbl)
        .arg(hwSc, 2, 16, QChar('0')).toUpper()
        .arg(hidUsage, 2, 16, QChar('0')).toUpper()
        .arg(I18n::t("log_actual_engine"), effInfo.effectiveMethod, fb);
    m_lblStatus->setText(plainMsg);

    if (m_onLogEntry) {
        QJsonObject obj;
        obj["kind"] = "key";
        obj["lbl"] = lbl;
        obj["hw_sc"] = hwSc;
        obj["hid_usage"] = hidUsage;
        obj["requested_method"] = m_state ? m_state->config.inputMethod : "sendinput_scancode";
        m_onLogEntry(obj, "#8B5CF6");
    }
    QWidget::keyPressEvent(event);
}

void LiveInputSandboxBox::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
    const bool focused = hasFocus() || (m_testInput && m_testInput->hasFocus());
    p.setBrush(Theme::instance().c(focused ? "surface_hi" : "bg1", 215));
    p.setPen(QPen(Theme::instance().c(focused ? "accent_hi" : "border_hi"), focused ? 1.6 : 1.1));
    p.drawRoundedRect(r, 12.0, 12.0);
}

// ============================================================================
// TestPage
// ============================================================================

TestPage::TestPage(AppState* state, MacroEngine* engine, QWidget* parent)
    : QWidget(parent), m_state(state), m_engine(engine) {
    connect(this, &TestPage::threadLogSignal, this, &TestPage::addStructuredLog);
    connect(this, &TestPage::sweepFinishedSignal, this, &TestPage::onSweepFinishedMainThread);

    buildUi();
    refreshTestTargetApps();
    populateTestTargets();
    restorePersistentLogs();

    if (m_state) {
        connect(m_state, &AppState::combosListUpdated, this, &TestPage::populateTestTargets);
        connect(m_state, &AppState::macrosLibraryUpdated, this, &TestPage::populateTestTargets);
        connect(m_state, &AppState::pythonScriptsUpdated, this, [this]() { refreshPyPresetsCombo(); });
    }
}

void TestPage::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 6, 16, 10);
    root->setSpacing(6);

    // Header Card
    auto* topCard = new GlassCard(true, this);
    auto* tLay = new QHBoxLayout(topCard);
    tLay->setContentsMargins(16, 7, 16, 7);
    tLay->setSpacing(12);

    auto* tCol = new QVBoxLayout();
    tCol->setSpacing(1);
    auto* title = new QLabel(I18n::t("test_title"), topCard);
    title->setFont(makeFont(14, QFont::ExtraBold));
    title->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    auto* sub = new QLabel(I18n::t("test_subtitle"), topCard);
    sub->setFont(makeFont(11, QFont::Medium));
    sub->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    tCol->addWidget(title);
    tCol->addWidget(sub);
    tLay->addLayout(tCol, 1);

    auto* btnClearLog = new Button(I18n::t("btn_clear_test_log"), "ghost", "trash", 30, topCard);
    connect(btnClearLog, &Button::clicked, this, &TestPage::clearLog);
    tLay->addWidget(btnClearLog);
    root->addWidget(topCard);

    // Method & Target Selection Card
    auto* ctrlCard = new GlassCard(false, this);
    auto* cLay = new QVBoxLayout(ctrlCard);
    cLay->setContentsMargins(16, 10, 16, 10);
    cLay->setSpacing(6);

    // Row 1: Input Simulation Method Selector (22 Engines)
    auto* r1 = new QHBoxLayout();
    r1->setSpacing(8);
    auto* lblM = new QLabel(I18n::t("lbl_input_method"), ctrlCard);
    lblM->setFont(makeFont(12, QFont::ExtraBold));
    lblM->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    const QString curMethod = m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode");
    m_methodCombo = new SearchCombo(AutoPresserEngine::localizedKeyMethodOptions(), curMethod, ctrlCard);
    m_methodCombo->setMinimumWidth(380);
    connect(m_methodCombo, &SearchCombo::currentChanged, this, &TestPage::onMethodChanged);

    const EffectiveEngineInfo initEff = Win32Input::resolveEffectiveInputEngine(curMethod);
    const QString initCol = initEff.isFallback ? QStringLiteral("#F59E0B") : Theme::instance().tokenHex("ok");
    m_lblHwStatus = new QLabel(QString::fromUtf8("⚡ %1").arg(initEff.reason), ctrlCard);
    m_lblHwStatus->setFont(makeFont(11, QFont::Bold));
    m_lblHwStatus->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(initCol));

    r1->addWidget(lblM);
    r1->addWidget(m_methodCombo, 1);
    r1->addWidget(m_lblHwStatus);
    cLay->addLayout(r1);

    // Row 1b: Optional Target App Selector + Switch Button + Background Test Toggle
    auto* rApp = new QHBoxLayout();
    rApp->setSpacing(8);

    auto* lblTapp = new QLabel(I18n::t("lbl_test_target_app_opt"), ctrlCard);
    lblTapp->setFont(makeFont(12, QFont::Bold));
    lblTapp->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    m_testAppCombo = new SearchCombo(ctrlCard);
    m_testAppCombo->setMinimumWidth(250);

    auto* btnRefTapp = new IconButton("search", I18n::t("toast_apps_refreshed"), 30, "glass", ctrlCard);
    connect(btnRefTapp, &IconButton::clicked, this, &TestPage::refreshTestTargetApps);

    m_btnTestSwitch = new Button(I18n::t("akp_btn_switch_win"), "glass", QString(), "#F59E0B", false, ctrlCard);
    m_btnTestSwitch->setFixedHeight(30);
    connect(m_btnTestSwitch, &Button::clicked, this, &TestPage::onSwitchToTestApp);

    auto* lblBgTest = new QLabel(I18n::t("lbl_test_bg_mode"), ctrlCard);
    lblBgTest->setFont(makeFont(11, QFont::Bold));
    lblBgTest->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    m_togTestBg = new ToggleSwitch(false, ctrlCard);

    rApp->addWidget(lblTapp);
    rApp->addWidget(m_testAppCombo, 1);
    rApp->addWidget(btnRefTapp);
    rApp->addWidget(m_btnTestSwitch);
    rApp->addSpacing(6);
    rApp->addWidget(lblBgTest);
    rApp->addWidget(m_togTestBg);
    cLay->addLayout(rApp);

    // Row 2: Action Mode Selector + Delay + Run Button
    auto* r2Mode = new QHBoxLayout();
    r2Mode->setSpacing(8);

    auto* lblAm = new QLabel(I18n::t("lbl_test_action_mode"), ctrlCard);
    lblAm->setFont(makeFont(12, QFont::Bold));
    lblAm->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_actionModeCombo = new SearchCombo(
        {
            {"macro", I18n::t("test_mode_macro")},
            {"type_text", I18n::t("test_mode_type_text")},
            {"simple_key", I18n::t("test_mode_simple_key")},
            {"sweep_20", I18n::t("test_mode_sweep_20")},
            {"sweep_20_template", I18n::t("test_mode_sweep_20_template")},
            {"sweep_20_macro", I18n::t("test_mode_sweep_20_macro")},
        },
        "macro",
        ctrlCard
    );
    m_actionModeCombo->setMinimumWidth(340);
    connect(m_actionModeCombo, &SearchCombo::currentChanged, this, &TestPage::onActionModeChanged);

    auto* lblDelay = new QLabel(I18n::t("lbl_run_after"), ctrlCard);
    lblDelay->setFont(makeFont(11, QFont::Bold));
    lblDelay->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));

    m_numTestDelay = new NumberInput(1000, 0, 30000, 500, "ms", 118, ctrlCard);
    m_numTestDelay->setFixedSize(118, 32);

    m_btnRunTest = new Button(I18n::t("btn_run_test_now"), "primary", "play", 34, ctrlCard);
    connect(m_btnRunTest, &Button::clicked, this, &TestPage::runSelectedTest);

    r2Mode->addWidget(lblAm);
    r2Mode->addWidget(m_actionModeCombo, 1);
    r2Mode->addWidget(lblDelay);
    r2Mode->addWidget(m_numTestDelay);
    r2Mode->addWidget(m_btnRunTest);
    cLay->addLayout(r2Mode);

    // Row 2b: Dynamic Payload Row
    m_payloadRowW = new QWidget(ctrlCard);
    auto* r2 = new QHBoxLayout(m_payloadRowW);
    r2->setContentsMargins(0, 0, 0, 0);
    r2->setSpacing(8);

    m_lblTarget = new QLabel(I18n::t("lbl_test_select_macro"), m_payloadRowW);
    m_lblTarget->setFont(makeFont(12, QFont::Bold));
    m_lblTarget->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    m_targetCombo = new SearchCombo(m_payloadRowW);
    m_targetCombo->setMinimumWidth(240);

    m_togMacroSweepPrefix = new ToggleSwitch(true, m_payloadRowW);
    m_lblMacroSweepPrefix = new QLabel(I18n::t("chk_macro_sweep_prefix"), m_payloadRowW);
    m_lblMacroSweepPrefix->setFont(makeFont(11, QFont::Bold));
    m_lblMacroSweepPrefix->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_lblCustomTxt = new QLabel(I18n::t("lbl_test_custom_text"), m_payloadRowW);
    m_lblCustomTxt->setFont(makeFont(12, QFont::Bold));
    m_lblCustomTxt->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    m_customTextEdit = new QLineEdit(kDefaultTemplateSweepStr, m_payloadRowW);
    m_customTextEdit->setPlaceholderText(I18n::t("ph_test_custom_text"));
    m_customTextEdit->setLayoutDirection(Qt::LeftToRight);
    m_customTextEdit->setFixedHeight(32);
    m_customTextEdit->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 8px;
            padding: 3px 10px;
            font-family: 'Consolas', '%4';
            font-size: 12px;
            font-weight: bold;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border_hi"),
            I18n::langFontFamily()));

    m_togPressEnter = new ToggleSwitch(true, m_payloadRowW);
    m_lblPressEnter = new QLabel(I18n::t("chk_test_press_enter"), m_payloadRowW);
    m_lblPressEnter->setFont(makeFont(11, QFont::Bold));
    m_lblPressEnter->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    m_lblSimpleK = new QLabel(I18n::t("lbl_test_simple_key"), m_payloadRowW);
    m_lblSimpleK->setFont(makeFont(12, QFont::Bold));
    m_lblSimpleK->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    m_simpleKeyBtn = new KeyCaptureButton(m_state, "Enter", false, m_payloadRowW);
    m_simpleKeyBtn->setFixedSize(160, 32);

    r2->addWidget(m_lblTarget);
    r2->addWidget(m_targetCombo, 1);
    r2->addWidget(m_togMacroSweepPrefix);
    r2->addWidget(m_lblMacroSweepPrefix);
    r2->addWidget(m_lblCustomTxt);
    r2->addWidget(m_customTextEdit, 1);
    r2->addWidget(m_togPressEnter);
    r2->addWidget(m_lblPressEnter);
    r2->addWidget(m_lblSimpleK);
    r2->addWidget(m_simpleKeyBtn);
    cLay->addWidget(m_payloadRowW);

    // Row 2c: Smart Variable Template Container
    m_templateSweepContainer = new QWidget(ctrlCard);
    auto* tplLay = new QVBoxLayout(m_templateSweepContainer);
    tplLay->setContentsMargins(0, 2, 0, 0);
    tplLay->setSpacing(4);

    auto* tplHdr = new QHBoxLayout();
    tplHdr->setSpacing(8);
    m_lblTplBox = new QLabel(I18n::t("lbl_template_sweep_box"), m_templateSweepContainer);
    m_lblTplBox->setFont(makeFont(11, QFont::ExtraBold));
    m_lblTplBox->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_btnResetTpl = new Button(I18n::t("btn_reset_template_sweep"), "ghost", "refresh", 26, m_templateSweepContainer);
    connect(m_btnResetTpl, &Button::clicked, this, &TestPage::onResetTemplateSweep);

    tplHdr->addWidget(m_lblTplBox, 1);
    tplHdr->addWidget(m_btnResetTpl);
    tplLay->addLayout(tplHdr);

    const QString savedTpl = (m_state && !m_state->testLabTemplateSweepStr.isEmpty())
        ? m_state->testLabTemplateSweepStr
        : kDefaultTemplateSweepStr;
    m_templateSweepEdit = new QLineEdit(savedTpl, m_templateSweepContainer);
    m_templateSweepEdit->setLayoutDirection(Qt::LeftToRight);
    m_templateSweepEdit->setFixedHeight(32);
    m_templateSweepEdit->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1.4px solid %3;
            border-radius: 8px;
            padding: 4px 10px;
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 12px;
            font-weight: bold;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("accent")));
    connect(m_templateSweepEdit, &QLineEdit::textChanged, this, &TestPage::onTemplateSweepChanged);
    tplLay->addWidget(m_templateSweepEdit);

    m_tplChipsScrollBox = new TintedScrollBox(QString(), m_templateSweepContainer);
    m_tplChipsScrollBox->setFixedHeight(42);
    auto* tcBoxLay = new QHBoxLayout(m_tplChipsScrollBox);
    tcBoxLay->setContentsMargins(6, 2, 6, 2);
    tcBoxLay->setSpacing(0);

    m_tplChipsScroll = new QScrollArea(m_tplChipsScrollBox);
    m_tplChipsScroll->setWidgetResizable(true);
    m_tplChipsScroll->setFrameShape(QFrame::NoFrame);
    m_tplChipsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tplChipsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_tplChipsScroll->setStyleSheet(modernScrollStyle());

    m_tplChipsContainer = new QWidget();
    m_tplChipsContainer->setLayoutDirection(Qt::LeftToRight);
    m_tplChipsContainer->setStyleSheet("background: transparent;");
    auto* chipsRow = new QHBoxLayout(m_tplChipsContainer);
    chipsRow->setContentsMargins(4, 2, 4, 2);
    chipsRow->setSpacing(8);

    int totalChipsW = 16;
    for (const auto& pair : sweepTemplateVariables()) {
        const QString tokenStr = pair.first;
        const QString dispLbl = pair.second;
        auto* chipBtn = new Button(QStringLiteral("+ %1").arg(dispLbl), "glass", QString(), 26, m_tplChipsContainer);
        chipBtn->setForceLtrText(true);
        const int btnW = chipBtn->naturalCompactWidth() + 14;
        chipBtn->setFixedWidth(btnW);
        totalChipsW += btnW + 8;
        connect(chipBtn, &Button::clicked, this, [this, tokenStr]() { insertTemplateToken(tokenStr); });
        chipsRow->addWidget(chipBtn);
    }
    chipsRow->addStretch(1);
    m_tplChipsContainer->setMinimumWidth(totalChipsW);
    m_tplChipsScroll->setWidget(m_tplChipsContainer);
    tcBoxLay->addWidget(m_tplChipsScroll);
    tplLay->addWidget(m_tplChipsScrollBox);
    cLay->addWidget(m_templateSweepContainer);

    // Row 2d: Editable Python Per-Method Test Script Container
    m_pySweepContainer = new QWidget(ctrlCard);
    auto* pyLay = new QVBoxLayout(m_pySweepContainer);
    pyLay->setContentsMargins(0, 2, 0, 0);
    pyLay->setSpacing(4);

    auto* pyHdrRow = new QHBoxLayout();
    pyHdrRow->setSpacing(6);
    m_lblPySweep = new QLabel(I18n::t("lbl_py_sweep_editor"), m_pySweepContainer);
    m_lblPySweep->setFont(makeFont(11, QFont::ExtraBold));
    m_lblPySweep->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_pyPresetCombo = new SearchCombo(PythonScriptRunner::dropdownItems(), "preset:chat_20_sweep", m_pySweepContainer);
    m_pyPresetCombo->setMinimumWidth(260);
    m_pyPresetCombo->setFixedHeight(28);
    connect(m_pyPresetCombo, &SearchCombo::currentChanged, this, &TestPage::onPyPresetSelected);

    m_pyScriptNameEdit = new QLineEdit(m_pySweepContainer);
    m_pyScriptNameEdit->setPlaceholderText(I18n::t("ph_py_script_name"));
    m_pyScriptNameEdit->setFixedWidth(150);
    m_pyScriptNameEdit->setFixedHeight(28);
    m_pyScriptNameEdit->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 7px;
            padding: 2px 8px;
            font-family: '%4';
            font-size: 11px;
            font-weight: bold;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border_hi"),
            I18n::langFontFamily()));

    m_btnSavePyScript = new Button(I18n::t("btn_save_py_script"), "glass", "save", 28, m_pySweepContainer);
    connect(m_btnSavePyScript, &Button::clicked, this, &TestPage::onSavePyScriptClicked);

    m_btnDelPyScript = new IconButton("trash", I18n::t("tip_del_py_script"), 28, "danger", m_pySweepContainer);
    connect(m_btnDelPyScript, &IconButton::clicked, this, &TestPage::onDeletePyScriptClicked);

    m_btnResetPy = new Button(I18n::t("btn_reset_py_sweep"), "ghost", "refresh", 28, m_pySweepContainer);
    connect(m_btnResetPy, &Button::clicked, this, &TestPage::onResetPySweepScript);

    pyHdrRow->addWidget(m_lblPySweep);
    pyHdrRow->addWidget(m_pyPresetCombo, 1);
    pyHdrRow->addWidget(m_pyScriptNameEdit);
    pyHdrRow->addWidget(m_btnSavePyScript);
    pyHdrRow->addWidget(m_btnDelPyScript);
    pyHdrRow->addWidget(m_btnResetPy);
    pyLay->addLayout(pyHdrRow);

    m_pySweepEditor = new QPlainTextEdit(m_pySweepContainer);
    m_pySweepEditor->setLayoutDirection(Qt::LeftToRight);
    m_pySweepEditor->setPlainText(PythonScriptRunner::codeByKey("preset:chat_20_sweep"));
    m_pySweepEditor->setFixedHeight(88);
    m_pySweepEditor->setStyleSheet(QStringLiteral(R"(
        QPlainTextEdit {
            background: %1;
            color: %2;
            border: 1.4px solid %3;
            border-radius: 9px;
            padding: 5px 10px;
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 12px;
            font-weight: 600;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("accent")));
    connect(m_pySweepEditor, &QPlainTextEdit::textChanged, this, &TestPage::onPySweepTextChanged);
    pyLay->addWidget(m_pySweepEditor);

    cLay->addWidget(m_pySweepContainer);
    root->addWidget(ctrlCard);
    onActionModeChanged("macro");

    // Live Sandbox Box + Event Log Card
    auto* sandboxCard = new GlassCard(false, this);
    auto* sLay = new QVBoxLayout(sandboxCard);
    sLay->setContentsMargins(14, 8, 14, 8);
    sLay->setSpacing(6);

    auto* lblSb = new QLabel(I18n::t("lbl_test_sandbox"), sandboxCard);
    lblSb->setFont(makeFont(11, QFont::ExtraBold));
    lblSb->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    sLay->addWidget(lblSb);

    m_sandboxBox = new LiveInputSandboxBox(
        m_state,
        [this](const QJsonObject& entry, const QString& accent) { addStructuredLog(entry, accent); },
        sandboxCard
    );
    sLay->addWidget(m_sandboxBox);

    m_logScroll = new QScrollArea(sandboxCard);
    m_logScroll->setMinimumHeight(185);
    m_logScroll->setWidgetResizable(true);
    m_logScroll->setFrameShape(QFrame::NoFrame);
    m_logScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_logScroll->setStyleSheet(modernScrollStyle());

    m_logContainer = new QWidget();
    m_logContainer->setStyleSheet("background: transparent;");
    m_logVbox = new QVBoxLayout(m_logContainer);
    m_logVbox->setContentsMargins(0, 0, 4, 0);
    m_logVbox->setSpacing(6);
    m_logVbox->addStretch(1);
    m_logScroll->setWidget(m_logContainer);
    sLay->addWidget(m_logScroll, 1);

    root->addWidget(sandboxCard, 1);
}

void TestPage::insertTemplateToken(const QString& tokenStr) {
    if (!m_templateSweepEdit) return;
    const QString cur = m_templateSweepEdit->text();
    const QString sep = (!cur.isEmpty() && !cur.endsWith(' ')) ? QStringLiteral(" ") : QString();
    m_templateSweepEdit->setText(QStringLiteral("%1%2%3 ").arg(cur, sep, tokenStr));
    m_templateSweepEdit->setFocus();
}

void TestPage::onTemplateSweepChanged(const QString& txt) {
    if (m_state) m_state->testLabTemplateSweepStr = txt;
}

void TestPage::onResetTemplateSweep() {
    if (m_templateSweepEdit) m_templateSweepEdit->setText(kDefaultTemplateSweepStr);
    if (m_state) m_state->testLabTemplateSweepStr = kDefaultTemplateSweepStr;
}

void TestPage::refreshPyPresetsCombo(const QString& selectKey) {
    if (!m_pyPresetCombo) return;
    const QString cur = selectKey.isEmpty() ? m_pyPresetCombo->currentKey() : selectKey;
    m_pyPresetCombo->setItems(PythonScriptRunner::dropdownItems(), cur);
}

void TestPage::onPyPresetSelected(const QString& scriptKey) {
    const QString code = PythonScriptRunner::codeByKey(scriptKey);
    if (!code.isEmpty() && m_pySweepEditor) {
        m_pySweepEditor->setPlainText(code);
    }
    const QString savedNm = PythonScriptRunner::nameByKey(scriptKey);
    if (!savedNm.isEmpty() && m_pyScriptNameEdit) {
        m_pyScriptNameEdit->setText(savedNm);
    }
}

void TestPage::onSavePyScriptClicked() {
    if (!m_pySweepEditor || !m_pyPresetCombo || !m_pyScriptNameEdit) return;
    const QString nm = m_pyScriptNameEdit->text().trimmed();
    const QString code = m_pySweepEditor->toPlainText();
    const QString curK = m_pyPresetCombo->currentKey();
    const auto saved = PythonScriptRunner::upsertUserScript(nm, code, nm.isEmpty() ? curK : QString());
    m_pyScriptNameEdit->setText(saved.second);
    refreshPyPresetsCombo(saved.first);
    if (m_state) {
        emit m_state->pythonScriptsUpdated();
        emit m_state->toastRequested(QStringLiteral("%1 «%2»").arg(I18n::t("toast_py_script_saved"), saved.second), "ok");
    }
}

void TestPage::onDeletePyScriptClicked() {
    if (!m_pyPresetCombo) return;
    const QString curK = m_pyPresetCombo->currentKey();
    if (PythonScriptRunner::deleteUserScript(curK)) {
        if (m_pyScriptNameEdit) m_pyScriptNameEdit->clear();
        refreshPyPresetsCombo("preset:chat_20_sweep");
        if (m_state) {
            emit m_state->pythonScriptsUpdated();
            emit m_state->toastRequested(I18n::t("toast_py_script_deleted"), "info");
        }
    }
}

void TestPage::onPySweepTextChanged() {}

void TestPage::onResetPySweepScript() {
    if (m_pySweepEditor) {
        m_pySweepEditor->setPlainText(PythonScriptRunner::codeByKey("preset:chat_20_sweep"));
    }
}

void TestPage::refreshTestTargetApps() {
    const QString curSel = m_testAppCombo ? m_testAppCombo->currentKey() : QString();
    m_testWinCache = Win32Input::listDetailedWindowsForPicker();
    QList<SearchComboItem> items;
    items.append(SearchComboItem(QString(), I18n::t("test_target_app_sandbox_free")));
    QSet<QString> seen;
    for (const auto& w : m_testWinCache) {
        const QString pname = w.procName;
        const QString kLow = pname.toLower();
        if (seen.contains(kLow)) continue;
        seen.insert(kLow);
        const QString disp = QStringLiteral("%1 — %2 (PID: %3)").arg(pname, w.title.left(42), QString::number(w.pid));
        const QString tint = (!curSel.isEmpty() && kLow == curSel.toLower()) ? QStringLiteral("#10B981") : QString();
        items.append(SearchComboItem(pname, disp, w.exePath, tint));
    }
    if (m_testAppCombo) m_testAppCombo->setRichItems(items, curSel);
}

quintptr TestPage::resolveSelectedTestHwnd() {
    const QString sel = m_testAppCombo ? m_testAppCombo->currentKey().trimmed().toLower() : QString();
    if (sel.isEmpty()) return 0;
    for (const auto& w : m_testWinCache) {
        if (w.procName.toLower() == sel) return w.hwnd;
    }
    for (const auto& w : Win32Input::listDetailedWindowsForPicker()) {
        if (w.procName.toLower() == sel) return w.hwnd;
    }
    return 0;
}

void TestPage::onSwitchToTestApp() {
    const QString sel = m_testAppCombo ? m_testAppCombo->currentKey().trimmed() : QString();
    if (sel.isEmpty()) {
        if (m_state) emit m_state->toastRequested(I18n::t("akp_err_must_select_window"), "info");
        return;
    }
    const quintptr hwnd = resolveSelectedTestHwnd();
    if (hwnd > 0) {
        Win32Input::focusAndBringHwndToFront(hwnd);
    } else if (m_state) {
        emit m_state->toastRequested(I18n::t("akp_err_window_not_found"), "danger");
    }
}

void TestPage::populateTestTargets() {
    if (!m_targetCombo || !m_state) return;
    QList<QPair<QString, QString>> items;
    items.append(qMakePair(QStringLiteral("quick_demo"), I18n::t("test_quick_demo_label")));
    const bool isAr = I18n::isRtl();
    for (int idx = 0; idx < m_state->smartCombos.size(); ++idx) {
        const QString title = m_state->smartCombos[idx].sidebarLabel(isAr);
        items.append(qMakePair(QStringLiteral("combo:%1").arg(idx), QStringLiteral("%1 %2").arg(I18n::t("test_prefix_smart_key"), title)));
    }
    for (int idx = 0; idx < m_state->savedMacros.size(); ++idx) {
        const QString nm = m_state->savedMacros[idx].name.isEmpty() ? QStringLiteral("Recording %1").arg(idx + 1) : m_state->savedMacros[idx].name;
        items.append(qMakePair(QStringLiteral("rec:%1").arg(idx), QStringLiteral("%1 %2").arg(I18n::t("test_prefix_saved_rec"), nm)));
    }
    const QString curKey = m_targetCombo->currentKey().isEmpty() ? QStringLiteral("quick_demo") : m_targetCombo->currentKey();
    m_targetCombo->setItems(items, curKey);
}

void TestPage::onMethodChanged(const QString& methodKey) {
    if (m_state) m_state->updateSetting("input_method", methodKey);
    if (m_sandboxBox) m_sandboxBox->invalidateCache();
    const EffectiveEngineInfo eff = Win32Input::resolveEffectiveInputEngine(methodKey);
    const QString col = eff.isFallback ? QStringLiteral("#F59E0B") : Theme::instance().tokenHex("ok");
    if (m_lblHwStatus) {
        m_lblHwStatus->setText(QString::fromUtf8("⚡ %1").arg(eff.reason));
        m_lblHwStatus->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(col));
    }
    QJsonObject obj;
    obj["kind"] = "method_switch";
    obj["requested_method"] = methodKey;
    addStructuredLog(obj, eff.isFallback ? "#F59E0B" : "#06B6D4");
}

QPair<QString, QString> TestPage::formatEffectiveEngineBadge(const QString& requestedMethod) const {
    const EffectiveEngineInfo eff = Win32Input::resolveEffectiveInputEngine(requestedMethod);
    QString effName = eff.effectiveMethod;
    for (const auto& pair : AutoPresserEngine::localizedKeyMethodOptions()) {
        if (pair.first == eff.effectiveMethod) { effName = pair.second; break; }
    }
    if (eff.isFallback) {
        const QString html = QStringLiteral("<b style='color:#D97706;'>%1</b> <span style='color:#EF4444;'>(%2 %3)</span>")
            .arg(effName, I18n::t("log_auto_fallback_from"), eff.requestedMethod);
        return qMakePair(html, QStringLiteral("#F59E0B"));
    }
    return qMakePair(QStringLiteral("<b style='color:#10B981;'>%1</b>").arg(effName), QStringLiteral("#10B981"));
}

QString TestPage::renderLogEntryHtml(const QJsonObject& entry) const {
    const QString kind = entry.value("kind").toString("raw");
    if (kind == "sweep_item") {
        const bool ok = entry.value("ok").toBool();
        const int idx = entry.value("idx").toInt(1);
        const int total = entry.value("total").toInt(22);
        const QString trK = entry.value("method_tr_key").toString();
        const QString mId = entry.value("method_id").toString();
        const QString mName = !trK.isEmpty() ? I18n::t(trK) : mId;
        const QString dtMs = I18n::fmtNum(QString::number(entry.value("dt_ms").toInt(1)));
        const QString badgeCol = ok ? "#10B981" : "#EF4444";
        const QString badgeTxt = I18n::t(ok ? "akp_log_sweep_item_ok" : "akp_log_sweep_item_fail");
        const QString targetApp = entry.value("target_app").toString("Sandbox");
        const QString hwndStr = entry.value("hwnd").toString("0x0");
        const QString childCls = entry.value("child_cls").toString("TopWindow");
        const QString modeLbl = I18n::t(entry.value("is_bg").toBool() ? "log_lbl_mode_bg" : "log_lbl_mode_fg");
        const QString actionDesc = entry.value("action_desc").toString(entry.value("key").toString("Enter"));
        const QString reasonStr = AutoPresserEngine::formatDiagnosticReason(entry);

        const QString line1 = QStringLiteral("<span dir='ltr' style='color:#06B6D4; font-weight:800;'>[#%1/%2]</span> <b style='color:%3;'>%4</b> — <b>%5</b> <span dir='ltr' style='color:#3B82F6; font-weight:bold;'>(%6 ms)</span>")
            .arg(idx, 2, 10, QChar('0')).arg(total).arg(badgeCol, badgeTxt, mName, dtMs);
        const QString line2 = QString::fromUtf8("<span style='color:#94A3B8; font-size:11px;'>🎯 <b>%1</b> <span dir='ltr'>%2 [%3 &rarr; %4]</span> (%5) &nbsp;|&nbsp; ⚡ <b>%6</b> <span dir='ltr' style='color:#A78BFA;'>%7</span> &nbsp;|&nbsp; 📌 <b>%8</b> <span style='color:%9;'>%10</span></span>")
            .arg(I18n::t("log_lbl_target"), targetApp, hwndStr, childCls, modeLbl, I18n::t("log_lbl_action"), actionDesc, I18n::t("log_lbl_details"), badgeCol, reasonStr);
        return QStringLiteral("<div>%1<br>%2</div>").arg(line1, line2);
    }
    if (kind == "mouse") {
        const EffectiveEngineInfo effInfo = Win32Input::resolveEffectiveInputEngine(entry.value("requested_method").toString());
        const QString fb = effInfo.isFallback ? QStringLiteral(" (%1 %2)").arg(I18n::t("log_fallback_from"), effInfo.requestedMethod) : QString();
        const QString effCol = effInfo.isFallback ? "#D97706" : "#10B981";
        return QStringLiteral("%1 <b style='color:#10B981;'>%2 (%3)</b> %4 <b dir='ltr' style='color:#3B82F6;'>(%5, %6)</b> — %7 <b style='color:%8;'>%9%10</b>")
            .arg(I18n::t("log_recv_mouse"), I18n::t("log_mouse_click_at"), entry.value("btn_name").toString(), I18n::t("log_at_coords"))
            .arg(entry.value("x").toInt()).arg(entry.value("y").toInt())
            .arg(I18n::t("log_actual_engine"), effCol, effInfo.effectiveMethod, fb);
    }
    if (kind == "key") {
        const EffectiveEngineInfo effInfo = Win32Input::resolveEffectiveInputEngine(entry.value("requested_method").toString());
        const QString fb = effInfo.isFallback ? QStringLiteral(" (%1 %2)").arg(I18n::t("log_fallback_from"), effInfo.requestedMethod) : QString();
        const QString effCol = effInfo.isFallback ? "#D97706" : "#10B981";
        const int hwSc = entry.value("hw_sc").toInt();
        const int hid = entry.value("hid_usage").toInt();
        return QStringLiteral("%1 <b dir='ltr' style='color:#8B5CF6;'>[ %2 ]</b> &nbsp;|&nbsp; <span dir='ltr' style='color:#06B6D4; font-weight:bold;'>ScanCode: 0x%3</span> &nbsp;|&nbsp; <span dir='ltr' style='color:#3B82F6; font-weight:bold;'>USB HID: 0x%4</span> — %5 <b style='color:%6;'>%7%8</b>")
            .arg(I18n::t("log_recv_key"), entry.value("lbl").toString())
            .arg(hwSc, 2, 16, QChar('0')).toUpper()
            .arg(hid, 2, 16, QChar('0')).toUpper()
            .arg(I18n::t("log_actual_engine"), effCol, effInfo.effectiveMethod, fb);
    }
    if (kind == "method_switch") {
        const QString req = entry.value("requested_method").toString();
        QString reqLbl = req;
        for (const auto& pair : AutoPresserEngine::localizedKeyMethodOptions()) {
            if (pair.first == req) { reqLbl = pair.second; break; }
        }
        const EffectiveEngineInfo eff = Win32Input::resolveEffectiveInputEngine(req);
        const QString badgeCol = eff.isFallback ? "#D97706" : "#10B981";
        return QStringLiteral("%1 <b style='color:#8B5CF6;'>%2</b><br><span style='color:%3; font-weight:bold;'>%4 %5</span>")
            .arg(I18n::t("log_selected_prefix"), reqLbl, badgeCol, I18n::t("log_actual_status"), eff.reason);
    }
    if (kind == "run_combo") {
        const auto badge = formatEffectiveEngineBadge(entry.value("requested_method").toString());
        const int delayMs = entry.value("delay_ms").toInt();
        return QStringLiteral("%1 <b style='color:#3B82F6;'>%2 ms</b> %3 %4")
            .arg(I18n::t("log_run_combo_prefix"), I18n::fmtNum(QString::number(delayMs)), I18n::t("log_via_actual_engine"), badge.first);
    }
    if (kind == "run_rec") {
        const auto badge = formatEffectiveEngineBadge(entry.value("requested_method").toString());
        return QStringLiteral("%1 <b style='color:#8B5CF6;'>«%2»</b> %3 %4")
            .arg(I18n::t("log_run_rec_prefix"), entry.value("target_name").toString(), I18n::t("log_via_actual_engine"), badge.first);
    }
    if (kind == "run_quick") {
        const auto badge = formatEffectiveEngineBadge(entry.value("requested_method").toString());
        const int delayMs = entry.value("delay_ms").toInt();
        return QStringLiteral("%1 <b style='color:#8B5CF6;'>(W -> Space)</b> %2 <b style='color:#3B82F6;'>%3 ms</b> %4 %5")
            .arg(I18n::t("log_run_quick_prefix"), I18n::t("log_in_delay"), I18n::fmtNum(QString::number(delayMs)), I18n::t("log_via_actual_engine"), badge.first);
    }
    return entry.value("text").toString();
}

void TestPage::onActionModeChanged(const QString& modeKey) {
    const bool showMacroPicker = (modeKey == "macro" || modeKey == "sweep_20_macro");
    if (m_lblTarget) m_lblTarget->setVisible(showMacroPicker);
    if (m_targetCombo) m_targetCombo->setVisible(showMacroPicker);
    if (m_togMacroSweepPrefix) m_togMacroSweepPrefix->setVisible(modeKey == "sweep_20_macro");
    if (m_lblMacroSweepPrefix) m_lblMacroSweepPrefix->setVisible(modeKey == "sweep_20_macro");

    if (m_lblCustomTxt) m_lblCustomTxt->setVisible(modeKey == "type_text");
    if (m_customTextEdit) m_customTextEdit->setVisible(modeKey == "type_text");
    if (m_togPressEnter) m_togPressEnter->setVisible(modeKey == "type_text");
    if (m_lblPressEnter) m_lblPressEnter->setVisible(modeKey == "type_text");

    if (m_lblSimpleK) m_lblSimpleK->setVisible(modeKey == "simple_key");
    if (m_simpleKeyBtn) m_simpleKeyBtn->setVisible(modeKey == "simple_key");

    if (m_payloadRowW) {
        m_payloadRowW->setVisible(modeKey == "macro" || modeKey == "type_text" || modeKey == "simple_key" || modeKey == "sweep_20_macro");
    }
    if (m_templateSweepContainer) {
        m_templateSweepContainer->setVisible(modeKey == "sweep_20_template" || modeKey == "sweep_20_macro");
    }
    if (m_pySweepContainer) {
        m_pySweepContainer->setVisible(modeKey == "sweep_20");
    }
}

void TestPage::resolveSelectedMacroSteps(QString& outName, QList<ComboStep>& outSteps) const {
    outSteps.clear();
    const QString key = m_targetCombo ? m_targetCombo->currentKey() : QStringLiteral("quick_demo");
    if (m_state && key.startsWith("combo:")) {
        const int idx = key.mid(6).toInt();
        if (idx >= 0 && idx < m_state->smartCombos.size()) {
            const SmartCombo& c = m_state->smartCombos[idx];
            outName = !c.name.isEmpty() ? c.name : (!c.triggerKey.isEmpty() ? c.triggerKey : QStringLiteral("Macro %1").arg(idx + 1));
            outSteps = c.steps;
            return;
        }
    } else if (m_state && key.startsWith("rec:")) {
        const int idx = key.mid(4).toInt();
        if (idx >= 0 && idx < m_state->savedMacros.size()) {
            const SavedMacro& m = m_state->savedMacros[idx];
            outName = !m.name.isEmpty() ? m.name : QStringLiteral("Recording %1").arg(idx + 1);
            for (const auto& ev : m.events) {
                outSteps.append(ComboStep::fromMacroEvent(ev));
            }
            return;
        }
    }

    outName = QStringLiteral("Quick Demo (W -> Space)");
    ComboStep s1; s1.stepType = "key_tap"; s1.value = "W"; s1.scanCode = 0x11; s1.delayMs = 40; s1.holdMs = 20;
    ComboStep s2; s2.stepType = "key_tap"; s2.value = "Space"; s2.scanCode = 0x39; s2.delayMs = 40; s2.holdMs = 20;
    outSteps = {s1, s2};
}

void TestPage::runSelectedTest() {
    const QString actionMode = m_actionModeCombo ? m_actionModeCombo->currentKey() : QStringLiteral("macro");
    const int delayMs = m_numTestDelay ? m_numTestDelay->value() : 1000;
    const QString reqMethod = m_state ? m_state->config.inputMethod : QStringLiteral("sendinput_scancode");
    const bool runInBg = m_togTestBg && m_togTestBg->isChecked();

    const QString selApp = m_testAppCombo ? m_testAppCombo->currentKey().trimmed() : QString();
    quintptr targetHwnd = 0;
    if (!selApp.isEmpty()) {
        targetHwnd = resolveSelectedTestHwnd();
#ifdef _WIN32
        if (targetHwnd == 0) {
            if (m_state) emit m_state->toastRequested(I18n::t("akp_err_window_not_found"), "danger");
            return;
        }
#endif
        if (targetHwnd > 0 && !runInBg) {
            Win32Input::focusAndBringHwndToFront(targetHwnd);
        }
    } else {
        if (runInBg) {
            if (m_state) emit m_state->toastRequested(I18n::t("akp_err_must_select_window"), "info");
            return;
        }
        if (m_sandboxBox && m_sandboxBox->testInput()) {
            m_sandboxBox->testInput()->setFocus();
            targetHwnd = static_cast<quintptr>(m_sandboxBox->testInput()->winId());
        }
    }

    const bool allowSelfTest = selApp.isEmpty();
    const QString targetAppLabel = selApp.isEmpty() ? QStringLiteral("Sandbox") : selApp;
    const auto allMethods = AutoPresserEngine::all22KeyMethods();

    // Mode 2: Type Custom Text or Smart Variable Template
    if (actionMode == "type_text") {
        const QString txt = m_customTextEdit ? m_customTextEdit->text() : QString();
        const bool pressEnter = m_togPressEnter && m_togPressEnter->isChecked();
        int mIdx = 1;
        QString mTr;
        bool isBgMethod = false;
        for (int i = 0; i < allMethods.size(); ++i) {
            if (allMethods[i].id == reqMethod) {
                mIdx = i + 1;
                mTr = allMethods[i].trKey;
                isBgMethod = allMethods[i].isBackground;
                break;
            }
        }
        const QString mTitle = !mTr.isEmpty() ? I18n::t(mTr) : reqMethod;
        const int totalMethods = allMethods.size();

        QPointer<TestPage> self(this);
        std::thread([self, delayMs, txt, mIdx, reqMethod, mTr, mTitle, targetHwnd, targetAppLabel, runInBg, pressEnter, allowSelfTest, isBgMethod, totalMethods]() {
            if (delayMs > 0) QThread::msleep(delayMs);
            QElapsedTimer timer;
            timer.start();
            QString reasonT, previewT;
            const bool okT = AutoPresserEngine::expandAndExecuteMethodTemplate(
                txt, mIdx, reqMethod, mTitle, targetHwnd, targetAppLabel, runInBg, pressEnter, allowSelfTest, reasonT, previewT
            );
            const int dtMs = std::max(1, static_cast<int>(timer.elapsed()));
            if (self) {
                QJsonObject entry;
                entry["kind"] = "sweep_item";
                entry["idx"] = mIdx;
                entry["total"] = totalMethods;
                entry["method_id"] = reqMethod;
                entry["method_tr_key"] = mTr;
                entry["target_app"] = targetAppLabel;
                entry["hwnd"] = targetHwnd ? QStringLiteral("0x%1").arg(targetHwnd, 0, 16).toUpper() : "0x0";
                entry["child_cls"] = "SandboxEdit";
                entry["is_bg"] = runInBg;
                entry["action_desc"] = QStringLiteral("Text: %1").arg(previewT.left(38));
                entry["ok"] = okT;
                entry["dt_ms"] = dtMs;
                entry["reason_code"] = okT ? (isBgMethod ? "delivered_bg" : "delivered_fg") : "";
                entry["reason"] = reasonT;
                emit self->threadLogSignal(entry, okT ? "#10B981" : "#EF4444");
            }
        }).detach();
        return;
    }

    // Mode 3: Single Key Press
    if (actionMode == "simple_key") {
        const QString kLbl = (m_simpleKeyBtn && !m_simpleKeyBtn->keyString().isEmpty()) ? m_simpleKeyBtn->keyString() : QStringLiteral("Enter");
        int mIdx = 1;
        QString mTr;
        bool isBgMethod = false;
        for (int i = 0; i < allMethods.size(); ++i) {
            if (allMethods[i].id == reqMethod) {
                mIdx = i + 1;
                mTr = allMethods[i].trKey;
                isBgMethod = allMethods[i].isBackground;
                break;
            }
        }
        const int totalMethods = allMethods.size();

        QPointer<TestPage> self(this);
        std::thread([self, delayMs, kLbl, reqMethod, targetHwnd, runInBg, allowSelfTest, mIdx, totalMethods, mTr, targetAppLabel, isBgMethod]() {
            if (delayMs > 0) QThread::msleep(delayMs);
            QElapsedTimer timer;
            timer.start();
            QString reasonK;
            const bool okK = AutoPresserEngine::dispatchKey(kLbl, reqMethod, 25, targetHwnd, runInBg, true, allowSelfTest, &reasonK);
            const int dtMs = std::max(1, static_cast<int>(timer.elapsed()));
            if (self) {
                QJsonObject entry;
                entry["kind"] = "sweep_item";
                entry["idx"] = mIdx;
                entry["total"] = totalMethods;
                entry["method_id"] = reqMethod;
                entry["method_tr_key"] = mTr;
                entry["target_app"] = targetAppLabel;
                entry["hwnd"] = targetHwnd ? QStringLiteral("0x%1").arg(targetHwnd, 0, 16).toUpper() : "0x0";
                entry["child_cls"] = "SandboxEdit";
                entry["is_bg"] = runInBg;
                entry["action_desc"] = QStringLiteral("Key [%1]").arg(kLbl);
                entry["ok"] = okK;
                entry["dt_ms"] = dtMs;
                entry["reason_code"] = okK ? (isBgMethod ? "delivered_bg" : "delivered_fg") : "";
                entry["reason"] = reasonK;
                emit self->threadLogSignal(entry, okK ? "#10B981" : "#EF4444");
            }
        }).detach();
        return;
    }

    // Modes 4, 5, 6: Sweep All 22 Methods
    if (actionMode == "sweep_20" || actionMode == "sweep_20_template" || actionMode == "sweep_20_macro") {
        if (m_sweepRunning) return;
        m_sweepRunning = true;
        if (m_btnRunTest) m_btnRunTest->setBusy(true);

        const QString pyCode = m_pySweepEditor ? m_pySweepEditor->toPlainText() : QString();
        const QString tplStr = m_templateSweepEdit ? m_templateSweepEdit->text() : kDefaultTemplateSweepStr;
        const bool useMacroPrefix = m_togMacroSweepPrefix && m_togMacroSweepPrefix->isChecked();
        QString macroName;
        QList<ComboStep> macroSteps;
        resolveSelectedMacroSteps(macroName, macroSteps);

        QPointer<TestPage> self(this);
        std::thread([self, delayMs, actionMode, runInBg, targetAppLabel, targetHwnd, allowSelfTest, pyCode, tplStr, useMacroPrefix, macroName, macroSteps, allMethods]() {
            if (delayMs > 0) QThread::msleep(delayMs);
            const QString modeTag = I18n::t(runInBg ? "log_lbl_mode_bg" : "log_lbl_mode_fg");
            const QString sweepKindLbl = (actionMode == "sweep_20")
                ? QStringLiteral("Python Per-Method Script")
                : (actionMode == "sweep_20_template")
                    ? QStringLiteral("Smart Template (%1...)").arg(tplStr.left(32))
                    : QStringLiteral("Macro Sweep «%1» (%2 steps)").arg(macroName).arg(macroSteps.size());

            if (self) {
                QJsonObject hdr;
                hdr["kind"] = "raw";
                hdr["text"] = QString::fromUtf8("<b style='color:#06B6D4;'>⚡ %1 <span dir='ltr'>[%2 | HWND: 0x%3 &rarr; SandboxEdit]</span> (%4) — %5</b>")
                    .arg(I18n::t("akp_log_sweep_header"), targetAppLabel)
                    .arg(targetHwnd, 0, 16).toUpper()
                    .arg(modeTag, sweepKindLbl);
                emit self->threadLogSignal(hdr, "#06B6D4");
            }

            int okC = 0;
            int failC = 0;
            const int total = allMethods.size();

            for (int i = 0; i < total; ++i) {
                const int idx = i + 1;
                const QString mId = allMethods[i].id;
                const QString trK = allMethods[i].trKey;
                const bool isBgMethod = allMethods[i].isBackground;
                const QString mTitle = I18n::t(trK);

                QElapsedTimer timer;
                timer.start();
                bool ok = false;
                QString reason;
                QString actDesc;

                if (actionMode == "sweep_20") {
                    ok = AutoPresserEngine::executePerMethodPythonTest(
                        pyCode, idx, mId, mTitle, targetHwnd, runInBg, targetAppLabel, allowSelfTest, nullptr, reason
                    );
                    actDesc = QStringLiteral("Python (#%1 %2)").arg(idx).arg(mId);
                } else if (actionMode == "sweep_20_template") {
                    QString prev;
                    ok = AutoPresserEngine::expandAndExecuteMethodTemplate(
                        tplStr, idx, mId, mTitle, targetHwnd, targetAppLabel, runInBg, false, allowSelfTest, reason, prev
                    );
                    actDesc = QStringLiteral("Template: %1").arg(prev.left(32));
                } else {
                    int stepCnt = 0;
                    ok = AutoPresserEngine::executePerMethodMacroTest(
                        macroSteps, idx, mId, mTitle, targetHwnd, targetAppLabel, runInBg, useMacroPrefix ? tplStr : QString(), allowSelfTest, reason, stepCnt
                    );
                    actDesc = QStringLiteral("Macro «%1» (%2 ops)").arg(macroName).arg(stepCnt);
                }

                const int dtMs = std::max(1, static_cast<int>(timer.elapsed()));
                if (ok) ++okC;
                else ++failC;

                if (self) {
                    QJsonObject item;
                    item["kind"] = "sweep_item";
                    item["idx"] = idx;
                    item["total"] = total;
                    item["method_id"] = mId;
                    item["method_tr_key"] = trK;
                    item["target_app"] = targetAppLabel;
                    item["hwnd"] = targetHwnd ? QStringLiteral("0x%1").arg(targetHwnd, 0, 16).toUpper() : "0x0";
                    item["child_cls"] = "SandboxEdit";
                    item["is_bg"] = runInBg;
                    item["action_desc"] = actDesc;
                    item["ok"] = ok;
                    item["dt_ms"] = dtMs;
                    item["reason_code"] = ok ? (isBgMethod ? "delivered_bg" : "delivered_fg") : "";
                    item["reason"] = reason;
                    emit self->threadLogSignal(item, ok ? "#10B981" : "#EF4444");
                }
                QThread::msleep(40);
            }

            if (self) {
                QString summaryMsg = I18n::t("akp_log_sweep_summary");
                summaryMsg.replace("{ok}", I18n::fmtNum(QString::number(okC)));
                summaryMsg.replace("{fail}", I18n::fmtNum(QString::number(failC)));
                QJsonObject sumObj;
                sumObj["kind"] = "raw";
                sumObj["text"] = QString::fromUtf8("<b style='color:#10B981;'>🏁 %1 — <span dir='ltr'>%2</span> (%3)</b>")
                    .arg(summaryMsg, targetAppLabel, modeTag);
                emit self->threadLogSignal(sumObj, okC > 0 ? "#10B981" : "#F59E0B");
                emit self->sweepFinishedSignal();
            }
        }).detach();
        return;
    }

    runSelectedTestMacroImpl(runInBg);
}

void TestPage::onSweepFinishedMainThread() {
    m_sweepRunning = false;
    if (m_btnRunTest) m_btnRunTest->setBusy(false);
}

void TestPage::runSelectedTestMacroImpl(bool runInBg) {
    if (!m_state || !m_engine) return;
    const QString key = m_targetCombo ? m_targetCombo->currentKey() : QStringLiteral("quick_demo");
    const int delayMs = m_numTestDelay ? m_numTestDelay->value() : 1000;
    const QString reqMethod = m_state->config.inputMethod;
    const auto badge = formatEffectiveEngineBadge(reqMethod);

    const QString selApp = m_testAppCombo ? m_testAppCombo->currentKey().trimmed() : QString();
    const bool allowSelfTest = selApp.isEmpty();
    if (!selApp.isEmpty()) {
        const quintptr hwnd = resolveSelectedTestHwnd();
        if (hwnd > 0 && !runInBg) Win32Input::focusAndBringHwndToFront(hwnd);
    } else if (m_sandboxBox && m_sandboxBox->testInput()) {
        m_sandboxBox->testInput()->setFocus();
    }

    if (key.startsWith("combo:")) {
        const int idx = key.mid(6).toInt();
        if (idx >= 0 && idx < m_state->smartCombos.size()) {
            QJsonObject entry;
            entry["kind"] = "run_combo";
            entry["delay_ms"] = delayMs;
            entry["requested_method"] = reqMethod;
            addStructuredLog(entry, badge.second);
            m_engine->executeSmartCombo(m_state->smartCombos[idx], {}, delayMs, allowSelfTest);
            return;
        }
    } else if (key.startsWith("rec:")) {
        const int idx = key.mid(4).toInt();
        if (idx >= 0 && idx < m_state->savedMacros.size()) {
            const SavedMacro& m = m_state->savedMacros[idx];
            QJsonObject entry;
            entry["kind"] = "run_rec";
            entry["target_name"] = m.name;
            entry["requested_method"] = reqMethod;
            addStructuredLog(entry, badge.second);
            m_engine->startPlayback(m.events, 1.0, 1, allowSelfTest);
            return;
        }
    }

    SmartCombo demoCombo;
    demoCombo.id = "test_lab_quick_demo";
    demoCombo.name = "Quick Test";
    demoCombo.mode = "once";
    ComboStep s1; s1.stepType = "key_tap"; s1.value = "W"; s1.scanCode = 0x11; s1.delayMs = 40;
    ComboStep s2; s2.stepType = "key_tap"; s2.value = "Space"; s2.scanCode = 0x39; s2.delayMs = 40;
    demoCombo.steps = {s1, s2};

    QJsonObject entry;
    entry["kind"] = "run_quick";
    entry["delay_ms"] = delayMs;
    entry["requested_method"] = reqMethod;
    addStructuredLog(entry, badge.second);
    m_engine->executeSmartCombo(demoCombo, {}, delayMs, allowSelfTest);
}

void TestPage::addStructuredLog(const QJsonObject& entry, const QString& accentHex) {
    QJsonObject item = entry;
    item["ts"] = QDateTime::currentDateTime().toString("HH:mm:ss");
    item["accent"] = accentHex;
    if (m_state) {
        m_state->testLabLogs.prepend(item);
        while (m_state->testLabLogs.size() > 200) m_state->testLabLogs.removeLast();
    }
    insertLogCard(item, true);
}

void TestPage::restorePersistentLogs() {
    if (!m_state) return;
    for (int i = m_state->testLabLogs.size() - 1; i >= 0; --i) {
        insertLogCard(m_state->testLabLogs[i], true);
    }
}

void TestPage::insertLogCard(const QJsonObject& item, bool atTop) {
    if (!m_logVbox || !m_logContainer) return;
    const QString accentHex = item.value("accent").toString("#8B5CF6");
    const QString ts = item.value("ts").toString();
    const QString html = renderLogEntryHtml(item);

    auto* card = new StepCard(m_logCount++, accentHex, m_logContainer);
    auto* lay = new QHBoxLayout(card);
    lay->setContentsMargins(12, 6, 12, 6);
    lay->setSpacing(10);

    auto* lblTs = new QLabel(ts, card);
    lblTs->setFont(makeFont(11, QFont::Bold));
    lblTs->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(accentHex));

    auto* lblMsg = new QLabel(html, card);
    lblMsg->setTextFormat(Qt::RichText);
    lblMsg->setWordWrap(true);
    lblMsg->setFont(makeFont(11, QFont::DemiBold));
    lblMsg->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    lay->addWidget(lblTs);
    lay->addWidget(lblMsg, 1);

    if (atTop) {
        m_logVbox->insertWidget(0, card);
    } else {
        m_logVbox->insertWidget(std::max(0, m_logVbox->count() - 1), card);
    }
}

void TestPage::clearLog() {
    if (m_state) m_state->testLabLogs.clear();
    if (m_sandboxBox) {
        if (m_sandboxBox->testInput()) m_sandboxBox->testInput()->clear();
        if (m_sandboxBox->statusLabel()) m_sandboxBox->statusLabel()->setText(I18n::t("sandbox_hint_status"));
    }
    if (!m_logVbox) return;
    while (m_logVbox->count() > 1) {
        if (QLayoutItem* it = m_logVbox->takeAt(0)) {
            if (QWidget* w = it->widget()) {
                w->hide();
                w->deleteLater();
            }
            delete it;
        }
    }
}

} // namespace vmacro
