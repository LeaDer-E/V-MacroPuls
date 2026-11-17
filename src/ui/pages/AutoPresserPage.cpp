#include "AutoPresserPage.h"
#include "../../core/AppState.h"
#include "../../core/I18n.h"
#include "../../core/Theme.h"
#include "../../engine/AutoPresserEngine.h"
#include "../../engine/MacroEngine.h"
#include "../../platform/Win32Input.h"

#include <QElapsedTimer>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QIntValidator>
#include <QJsonDocument>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QScrollBar>
#include <QWheelEvent>
#include <algorithm>
#include <thread>

namespace vmacro {

// ============================================================================
// KeyPillToggleButton
// ============================================================================

KeyPillToggleButton::KeyPillToggleButton(const QString& keyLabel, bool checked, QWidget* parent)
    : QWidget(parent), m_keyLabel(keyLabel), m_checked(checked) {
    setFixedHeight(30);
    setMinimumWidth(36);
    setCursor(Qt::PointingHandCursor);
}

void KeyPillToggleButton::setKeyLabel(const QString& keyLabel) {
    if (m_keyLabel != keyLabel) {
        m_keyLabel = keyLabel;
        update();
    }
}

void KeyPillToggleButton::setChecked(bool checked, bool emitSignal) {
    if (m_checked == checked) return;
    m_checked = checked;
    update();
    if (emitSignal) emit toggled(m_checked);
}

void KeyPillToggleButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        setChecked(!m_checked, true);
        return;
    }
    QWidget::mousePressEvent(event);
}

void KeyPillToggleButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void KeyPillToggleButton::enterEvent(QEnterEvent*) {
    m_hovered = true;
    update();
}

void KeyPillToggleButton::leaveEvent(QEvent*) {
    m_hovered = false;
    update();
}

void KeyPillToggleButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);

    QColor bg, borderC, fg;
    QString txt;
    if (m_checked) {
        bg = Theme::instance().c("accent");
        borderC = Theme::instance().c("accent_hi");
        fg = Theme::instance().c("on_accent");
        txt = QStringLiteral("✓ %1").arg(m_keyLabel);
    } else {
        bg = Theme::instance().c(m_hovered ? "surface_hi" : "bg2", 225);
        borderC = Theme::instance().c(m_hovered ? "accent" : "border_hi");
        fg = Theme::instance().c("text");
        txt = m_keyLabel;
    }

    p.setBrush(bg);
    p.setPen(QPen(borderC, 1.2));
    p.drawRoundedRect(r, 7.0, 7.0);

    p.setFont(makeFont(10, QFont::ExtraBold));
    p.setPen(fg);
    p.drawText(r, Qt::AlignCenter, txt);
}

// ============================================================================
// KeyMacroModeButton
// ============================================================================

KeyMacroModeButton::KeyMacroModeButton(bool checked, bool hasLinkedMacro, QWidget* parent)
    : QWidget(parent), m_checked(checked), m_hasLinked(hasLinkedMacro) {
    setFixedSize(26, 28);
    setCursor(Qt::PointingHandCursor);
    syncTooltip();
}

void KeyMacroModeButton::setHasLinkedMacro(bool hasLinked) {
    if (m_hasLinked != hasLinked) {
        m_hasLinked = hasLinked;
        update();
    }
}

void KeyMacroModeButton::setChecked(bool checked, bool emitSignal) {
    if (m_checked == checked) return;
    m_checked = checked;
    syncTooltip();
    update();
    if (emitSignal) emit toggled(m_checked);
}

void KeyMacroModeButton::syncTooltip() {
    setToolTip(I18n::t(m_checked ? "akp_tip_key_mode_macro" : "akp_tip_key_mode_normal"));
}

void KeyMacroModeButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        setChecked(!m_checked, true);
        return;
    }
    QWidget::mousePressEvent(event);
}

void KeyMacroModeButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void KeyMacroModeButton::enterEvent(QEnterEvent*) {
    m_hovered = true;
    update();
}

void KeyMacroModeButton::leaveEvent(QEvent*) {
    m_hovered = false;
    update();
}

void KeyMacroModeButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    QColor bg, borderC, fg;
    if (m_checked) {
        bg = QColor("#F59E0B");
        borderC = QColor("#FBBF24");
        fg = QColor("#FFFFFF");
    } else {
        bg = Theme::instance().c(m_hovered ? "surface_hi" : "bg2", 210);
        borderC = m_hasLinked ? QColor("#F59E0B") : Theme::instance().c(m_hovered ? "border_hi" : "border");
        fg = (m_hasLinked || m_hovered) ? QColor("#F59E0B") : Theme::instance().c("text2");
    }
    p.setBrush(bg);
    p.setPen(QPen(borderC, 1.1));
    p.drawRoundedRect(r, 5.0, 5.0);
    p.setFont(makeFont(9, QFont::ExtraBold));
    p.setPen(fg);
    p.drawText(r, Qt::AlignCenter, QString::fromUtf8("⚡"));
}

// ============================================================================
// CompactMsEdit
// ============================================================================

CompactMsEdit::CompactMsEdit(int valueMs, QWidget* parent)
    : QLineEdit(parent), m_valMs(qBound(10, valueMs, 3600000)) {
    setFixedHeight(25);
    setAlignment(Qt::AlignCenter);
    setValidator(new QIntValidator(10, 3600000, this));
    setText(QString::number(m_valMs));
    setToolTip("Interval (ms) — Type value or scroll mouse wheel");
    setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 0px 2px;
            font-family: '%4';
            font-size: 11px;
            font-weight: bold;
        }
        QLineEdit:focus {
            border: 1px solid %5;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border"),
            I18n::langFontFamily(),
            Theme::instance().tokenHex("accent")));
    connect(this, &QLineEdit::editingFinished, this, &CompactMsEdit::commitText);
}

void CompactMsEdit::setValueMs(int valMs) {
    const int v = qBound(10, valMs, 3600000);
    if (v != m_valMs || text() != QString::number(v)) {
        m_valMs = v;
        setText(QString::number(v));
    }
}

void CompactMsEdit::commitText() {
    bool ok = false;
    int v = text().trimmed().toInt(&ok);
    if (!ok) v = m_valMs;
    v = qBound(10, v, 3600000);
    m_valMs = v;
    setText(QString::number(v));
    emit msChanged(v);
}

void CompactMsEdit::wheelEvent(QWheelEvent* event) {
    const int delta = event->angleDelta().y();
    if (delta != 0) {
        const int step = std::abs(delta) >= 120 ? 50 : 10;
        const int newV = qBound(10, m_valMs + (delta > 0 ? step : -step), 3600000);
        if (newV != m_valMs) {
            m_valMs = newV;
            setText(QString::number(newV));
            emit msChanged(newV);
        }
        event->accept();
        return;
    }
    QLineEdit::wheelEvent(event);
}

// ============================================================================
// KeyTileWidget
// ============================================================================

KeyTileWidget::KeyTileWidget(AutoPresserKeyItem* itemPtr, bool hasLinkedMacro, QWidget* parent)
    : QWidget(parent), m_item(itemPtr) {
    setFixedHeight(78);
    setMinimumWidth(70);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(6, 5, 6, 5);
    lay->setSpacing(5);

    auto* top = new QHBoxLayout();
    top->setContentsMargins(0, 0, 0, 0);
    top->setSpacing(4);

    m_pillBtn = new KeyPillToggleButton(m_item ? m_item->key : "F1", m_item ? m_item->enabled : false, this);
    connect(m_pillBtn, &KeyPillToggleButton::toggled, this, [this](bool val) {
        if (m_item) m_item->enabled = val;
        update();
        emit changed();
    });
    top->addWidget(m_pillBtn, 1);

    m_btnMacroMode = new KeyMacroModeButton(m_item ? m_item->useSmartCombo : false, hasLinkedMacro, this);
    connect(m_btnMacroMode, &KeyMacroModeButton::toggled, this, [this](bool val) {
        if (m_item) m_item->useSmartCombo = val;
        update();
        emit changed();
    });
    top->addWidget(m_btnMacroMode);

    if (m_item && m_item->isCustom) {
        m_btnDel = new IconButton("close", I18n::t("btn_delete_rec"), 22, "danger", this);
        connect(m_btnDel, &IconButton::clicked, this, [this]() {
            if (m_item) emit removeClicked(m_item->key);
        });
        top->addWidget(m_btnDel);
    }

    lay->addLayout(top);

    auto* bot = new QHBoxLayout();
    bot->setContentsMargins(0, 0, 0, 0);
    bot->setSpacing(3);
    m_msEdit = new CompactMsEdit(m_item ? m_item->intervalMs : 1000, this);
    connect(m_msEdit, &CompactMsEdit::msChanged, this, [this](int val) {
        if (m_item) m_item->intervalMs = std::max(10, val);
        emit changed();
    });
    auto* lblMs = new QLabel("ms", this);
    lblMs->setFont(makeFont(9, QFont::Bold));
    lblMs->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));

    m_btnTest = new IconButton("play", I18n::t("akp_test_single_key"), 23, "glass", this);
    connect(m_btnTest, &IconButton::clicked, this, [this]() {
        if (m_item) emit testClicked(m_item->key);
    });
    bot->addWidget(m_msEdit, 1);
    bot->addWidget(lblMs);
    bot->addWidget(m_btnTest);
    lay->addLayout(bot);
}

void KeyTileWidget::bindItem(AutoPresserKeyItem* itemPtr, bool hasLinkedMacro) {
    m_item = itemPtr;
    if (!m_item) return;
    m_pillBtn->setKeyLabel(m_item->key);
    m_pillBtn->setChecked(m_item->enabled, false);
    m_btnMacroMode->setHasLinkedMacro(hasLinkedMacro);
    m_btnMacroMode->setChecked(m_item->useSmartCombo, false);
    m_msEdit->setValueMs(m_item->intervalMs);
    update();
}

void KeyTileWidget::enterEvent(QEnterEvent*) {
    m_hovered = true;
    update();
}

void KeyTileWidget::leaveEvent(QEvent*) {
    m_hovered = false;
    update();
}

void KeyTileWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(0.8, 0.8, -0.8, -0.8);
    QColor bg, borderC;
    double penW = 1.0;
    if (m_item && m_item->enabled) {
        bg = Theme::instance().c("accent", Theme::instance().isDark() ? 46 : 34);
        borderC = Theme::instance().c("accent");
        penW = 1.7;
    } else {
        bg = Theme::instance().c(m_hovered ? "surface_hi" : "bg1", 215);
        borderC = Theme::instance().c(m_hovered ? "border_hi" : "border");
        penW = 1.0;
    }
    p.setBrush(bg);
    p.setPen(QPen(borderC, penW));
    p.drawRoundedRect(r, 9.0, 9.0);
}

// ============================================================================
// AddCustomKeyButton
// ============================================================================

AddCustomKeyButton::AddCustomKeyButton(AppState* state, QWidget* parent)
    : Button(I18n::t("akp_btn_add_custom_key"), "primary", "plus", 32, parent),
      m_state(state) {
    connect(this, &Button::clicked, this, &AddCustomKeyButton::toggleCapture);
    if (m_state) {
        connect(m_state, &AppState::hookKeyEvent, this, &AddCustomKeyButton::onHookKey);
    }
}

void AddCustomKeyButton::toggleCapture() {
    if (m_capturing) {
        stopCapture();
    } else {
        m_capturing = true;
        if (m_state) m_state->isCapturingKey = true;
        setText(I18n::t("akp_btn_capturing_custom_key"));
        setVariant("danger");
    }
}

void AddCustomKeyButton::stopCapture() {
    m_capturing = false;
    if (m_state) m_state->isCapturingKey = false;
    setText(I18n::t("akp_btn_add_custom_key"));
    setVariant("primary");
}

void AddCustomKeyButton::mousePressEvent(QMouseEvent* event) {
    if (m_capturing) {
        if (event->button() == Qt::BackButton || event->button() == Qt::XButton1) {
            event->accept();
            stopCapture();
            emit customKeyCaptured("MouseBack");
            return;
        }
        if (event->button() == Qt::ForwardButton || event->button() == Qt::XButton2) {
            event->accept();
            stopCapture();
            emit customKeyCaptured("MouseForward");
            return;
        }
    }
    Button::mousePressEvent(event);
}

void AddCustomKeyButton::onHookKey(int, bool, bool isDown, const QString& label) {
    if (!m_capturing || !isDown) return;
    if (label.compare("Esc", Qt::CaseInsensitive) == 0 || label.compare("Escape", Qt::CaseInsensitive) == 0) {
        stopCapture();
        return;
    }
    const QString clean = label.trimmed();
    stopCapture();
    if (!clean.isEmpty()) emit customKeyCaptured(clean);
}

// ============================================================================
// MousePointRowWidget
// ============================================================================

MousePointRowWidget::MousePointRowWidget(int index, AutoPresserMousePoint* mpPtr, bool isPicking, QWidget* parent)
    : QWidget(parent), m_index(index), m_mp(mpPtr) {
    m_card = new StepCard(index, "#06B6D4", this);
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(m_card);

    auto* lay = new QHBoxLayout(m_card);
    lay->setContentsMargins(12, 6, 12, 6);
    lay->setSpacing(8);

    m_tog = new ToggleSwitch(m_mp ? m_mp->enabled : true, m_card);
    connect(m_tog, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_mp) m_mp->enabled = v;
        emit changed();
    });
    lay->addWidget(m_tog);

    auto* lblIdx = new QLabel(QStringLiteral("#%1").arg(I18n::fmtNum(QString::number(index + 1))), m_card);
    lblIdx->setFont(makeFont(12, QFont::ExtraBold));
    lblIdx->setStyleSheet("color: #06B6D4; background: transparent;");
    lay->addWidget(lblIdx);

    m_btnCombo = new SearchCombo(
        {
            {"left", I18n::t("mouse_btn_left")},
            {"right", I18n::t("mouse_btn_right")},
            {"middle", I18n::t("mouse_btn_middle")},
            {"x1", I18n::t("mouse_btn_x1")},
            {"x2", I18n::t("mouse_btn_x2")},
        },
        m_mp ? m_mp->button : "left",
        m_card
    );
    m_btnCombo->setFixedWidth(170);
    connect(m_btnCombo, &SearchCombo::currentChanged, this, [this](const QString& k) {
        if (m_mp) m_mp->button = k;
        emit changed();
    });
    lay->addWidget(m_btnCombo);

    auto* lblX = new QLabel("X:", m_card);
    lblX->setFont(makeFont(11, QFont::Bold));
    lblX->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    m_numX = new NumberInput(m_mp ? m_mp->x : 500, -32768, 32768, 10, QString(), 92, m_card);
    m_numX->setFixedSize(92, 30);
    connect(m_numX, &NumberInput::valueChanged, this, [this](int v) {
        if (m_mp) m_mp->x = v;
        emit changed();
    });

    auto* lblY = new QLabel("Y:", m_card);
    lblY->setFont(makeFont(11, QFont::Bold));
    lblY->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    m_numY = new NumberInput(m_mp ? m_mp->y : 500, -32768, 32768, 10, QString(), 92, m_card);
    m_numY->setFixedSize(92, 30);
    connect(m_numY, &NumberInput::valueChanged, this, [this](int v) {
        if (m_mp) m_mp->y = v;
        emit changed();
    });

    m_btnPick = new Button(
        I18n::t(isPicking ? "press_f8_live" : "btn_pick_live_xy"),
        isPicking ? "primary" : "glass",
        "crosshair",
        "#10B981",
        false,
        m_card
    );
    m_btnPick->setFixedHeight(30);
    connect(m_btnPick, &Button::clicked, this, [this]() { emit pickRequested(m_index); });

    auto* lblInt = new QLabel(I18n::t("lbl_pre_delay"), m_card);
    lblInt->setFont(makeFont(11, QFont::Bold));
    lblInt->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    m_numInt = new NumberInput(m_mp ? m_mp->intervalMs : 1000, 10, 3600000, 100, "ms", 118, m_card);
    m_numInt->setFixedSize(118, 30);
    connect(m_numInt, &NumberInput::valueChanged, this, [this](int v) {
        if (m_mp) m_mp->intervalMs = std::max(10, v);
        emit changed();
    });

    m_btnTestMp = new Button(I18n::t("akp_btn_test_mouse_pt"), "glass", "play", "#10B981", false, m_card);
    m_btnTestMp->setFixedHeight(30);
    m_btnTestMp->setToolTip(I18n::t("akp_tip_test_mouse_pt"));
    connect(m_btnTestMp, &Button::clicked, this, [this]() { emit testRequested(m_index); });

    auto* btnDel = new IconButton("trash", I18n::t("btn_delete_rec"), 30, "danger", m_card);
    connect(btnDel, &IconButton::clicked, this, [this]() { emit removed(m_index); });

    lay->addWidget(lblX);
    lay->addWidget(m_numX);
    lay->addWidget(lblY);
    lay->addWidget(m_numY);
    lay->addWidget(m_btnPick);
    lay->addWidget(m_btnTestMp);
    lay->addStretch(1);
    lay->addWidget(lblInt);
    lay->addWidget(m_numInt);
    lay->addWidget(btnDel);
}

// ============================================================================
// AutoPresserPage
// ============================================================================

AutoPresserPage::AutoPresserPage(AppState* state, MacroEngine* engine, QWidget* parent)
    : QWidget(parent), m_state(state), m_macroEngine(engine) {
    m_akp = new AutoPresserEngine(m_state);
    m_ownsAkp = true;
    m_akp->setMacroEngine(m_macroEngine);
    if (m_macroEngine) m_macroEngine->setAutoPresserEngine(m_akp);

    m_saveTimer = new QTimer(this);
    m_saveTimer->setSingleShot(true);
    m_saveTimer->setInterval(220);
    connect(m_saveTimer, &QTimer::timeout, this, [this]() {
        if (m_akp) m_akp->saveWorkspace();
    });

    buildUi();
    refreshTabsBar();
    loadCurrentTabIntoUi();

    if (m_state) {
        connect(m_state, &AppState::cursorPickHotkeyPressed, this, &AutoPresserPage::onF8Captured);
        connect(m_state, &AppState::settingsChanged, this, &AutoPresserPage::onExternalSettingsChanged);
    }
    connect(&Theme::instance(), &Theme::themeChanged, this, &AutoPresserPage::onThemeUpdated);
    connect(this, &AutoPresserPage::sweepDoneSignal, this, &AutoPresserPage::onSweepDoneMainThread);

    m_statusTimer = new QTimer(this);
    m_statusTimer->setInterval(350);
    connect(m_statusTimer, &QTimer::timeout, this, &AutoPresserPage::tickUiStatus);
    m_statusTimer->start();
}

AutoPresserPage::~AutoPresserPage() {
    if (m_ownsAkp && m_akp) {
        if (m_macroEngine) m_macroEngine->setAutoPresserEngine(nullptr);
        delete m_akp;
        m_akp = nullptr;
    }
}

AutoPresserTab& AutoPresserPage::currentTab() {
    if (m_akp->tabs.isEmpty()) {
        m_akp->addNewTab();
    }
    const int idx = qBound(0, m_akp->activeTabIndex, static_cast<int>(m_akp->tabs.size()) - 1);
    m_akp->activeTabIndex = idx;
    return m_akp->tabs[idx];
}

void AutoPresserPage::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(14, 6, 14, 8);
    root->setSpacing(6);

    // 1. Header Card
    auto* hdrCard = new GlassCard(true, this);
    auto* hLay = new QHBoxLayout(hdrCard);
    hLay->setContentsMargins(14, 7, 14, 7);
    hLay->setSpacing(8);

    auto* tCol = new QVBoxLayout();
    tCol->setSpacing(1);
    auto* title = new QLabel(I18n::t("akp_title"), hdrCard);
    title->setFont(makeFont(14, QFont::ExtraBold));
    title->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    auto* sub = new QLabel(I18n::t("akp_subtitle"), hdrCard);
    sub->setFont(makeFont(11, QFont::Medium));
    sub->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    tCol->addWidget(title);
    tCol->addWidget(sub);
    hLay->addLayout(tCol, 1);

    auto* lblStopAllHk = new QLabel(I18n::t("akp_lbl_stop_all_hotkey"), hdrCard);
    lblStopAllHk->setFont(makeFont(11, QFont::Bold));
    lblStopAllHk->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("danger")));

    m_hkStopAllTabs = new KeyCaptureButton(
        m_state,
        (m_state && !m_state->config.hotkeyAutopresserStopAll.isEmpty()) ? m_state->config.hotkeyAutopresserStopAll : "F7",
        true,
        hdrCard
    );
    m_hkStopAllTabs->setFixedSize(128, 30);
    connect(m_hkStopAllTabs, &KeyCaptureButton::keyCaptured, this, [this](const QString& k, int, bool) {
        onStopAllHotkeyChanged(k);
    });

    m_btnStopAll = new Button(I18n::t("akp_btn_stop_all_tabs"), "danger", "stop", 30, hdrCard);
    connect(m_btnStopAll, &Button::clicked, this, &AutoPresserPage::onStopAllTabs);

    auto* btnSaveWs = new Button(I18n::t("akp_btn_save_workspace"), "glass", "save", 30, hdrCard);
    connect(btnSaveWs, &Button::clicked, this, &AutoPresserPage::onSaveWorkspace);

    auto* btnLoadWs = new Button(I18n::t("akp_btn_load_workspace"), "ghost", "folder", 30, hdrCard);
    connect(btnLoadWs, &Button::clicked, this, &AutoPresserPage::onLoadWorkspace);

    hLay->addWidget(lblStopAllHk);
    hLay->addWidget(m_hkStopAllTabs);
    hLay->addWidget(m_btnStopAll);
    hLay->addWidget(btnSaveWs);
    hLay->addWidget(btnLoadWs);
    root->addWidget(hdrCard);

    // 2. Multi-Tab Strip Card
    auto* tabCard = new GlassCard(false, this);
    auto* tbLay = new QHBoxLayout(tabCard);
    tbLay->setContentsMargins(12, 5, 12, 5);
    tbLay->setSpacing(6);

    m_tabsScrollBox = new TintedScrollBox(QString(), tabCard);
    m_tabsScrollBox->setFixedHeight(44);
    auto* tsBoxLay = new QHBoxLayout(m_tabsScrollBox);
    tsBoxLay->setContentsMargins(6, 3, 6, 3);
    tsBoxLay->setSpacing(0);

    m_tabsScroll = new QScrollArea(m_tabsScrollBox);
    m_tabsScroll->setWidgetResizable(true);
    m_tabsScroll->setFrameShape(QFrame::NoFrame);
    m_tabsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_tabsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_tabsScroll->setStyleSheet(modernScrollStyle());

    m_tabsBtnContainer = new QWidget();
    m_tabsBtnContainer->setStyleSheet("background: transparent;");
    m_tabsBtnLayout = new QHBoxLayout(m_tabsBtnContainer);
    m_tabsBtnLayout->setContentsMargins(4, 2, 4, 2);
    m_tabsBtnLayout->setSpacing(6);
    m_tabsScroll->setWidget(m_tabsBtnContainer);
    tsBoxLay->addWidget(m_tabsScroll);
    tbLay->addWidget(m_tabsScrollBox, 1);

    m_tabNameEdit = new QLineEdit(tabCard);
    m_tabNameEdit->setPlaceholderText(I18n::t("akp_ph_tab_name"));
    m_tabNameEdit->setFixedWidth(130);
    m_tabNameEdit->setFixedHeight(28);
    m_tabNameEdit->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 8px;
            padding: 2px 8px;
            font-family: '%4';
            font-size: 12px;
            font-weight: bold;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border_hi"),
            I18n::langFontFamily()));
    connect(m_tabNameEdit, &QLineEdit::textEdited, this, &AutoPresserPage::onTabNameEdited);

    auto* btnNewTab = new Button(I18n::t("akp_btn_new_tab"), "primary", "plus", 28, tabCard);
    connect(btnNewTab, &Button::clicked, this, &AutoPresserPage::onAddTab);

    auto* btnDupTab = new Button(I18n::t("akp_btn_dup_tab"), "glass", "copy", 28, tabCard);
    connect(btnDupTab, &Button::clicked, this, &AutoPresserPage::onDupTab);

    auto* btnCloseTab = new IconButton("close", I18n::t("akp_btn_close_tab"), 28, "danger", tabCard);
    connect(btnCloseTab, &IconButton::clicked, this, &AutoPresserPage::onCloseCurrentTab);

    tbLay->addWidget(m_tabNameEdit);
    tbLay->addWidget(btnNewTab);
    tbLay->addWidget(btnDupTab);
    tbLay->addWidget(btnCloseTab);
    root->addWidget(tabCard);

    // 3. Control Card
    auto* ctrlCard = new GlassCard(false, this);
    auto* cLay = new QVBoxLayout(ctrlCard);
    cLay->setContentsMargins(12, 7, 12, 7);
    cLay->setSpacing(6);

    // Row A
    auto* rProc = new QHBoxLayout();
    rProc->setSpacing(6);

    m_btnChooseProc = new Button(I18n::t("akp_btn_choose_process"), "glass", QString(), "#06B6D4", false, ctrlCard);
    m_btnChooseProc->setFixedHeight(32);
    connect(m_btnChooseProc, &Button::clicked, this, &AutoPresserPage::refreshProcessDropdownAndOpen);

    m_procCombo = new SearchCombo(ctrlCard);
    m_procCombo->setMinimumWidth(230);
    connect(m_procCombo, &SearchCombo::currentChanged, this, &AutoPresserPage::onProcessSelected);

    auto* btnRefProc = new IconButton("search", I18n::t("toast_apps_refreshed"), 30, "glass", ctrlCard);
    connect(btnRefProc, &IconButton::clicked, this, [this]() { refreshProcessDropdown(QString(), true); });

    m_btnSwitchWin = new Button(I18n::t("akp_btn_switch_win"), "glass", QString(), "#F59E0B", false, ctrlCard);
    m_btnSwitchWin->setFixedHeight(30);
    connect(m_btnSwitchWin, &Button::clicked, this, &AutoPresserPage::onSwitchToTargetWindow);

    auto* lblBg = new QLabel(I18n::t("akp_lbl_run_in_bg"), ctrlCard);
    lblBg->setFont(makeFont(11, QFont::Bold));
    lblBg->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    m_togBg = new ToggleSwitch(true, ctrlCard);
    connect(m_togBg, &ToggleSwitch::toggled, this, &AutoPresserPage::onBgToggled);

    auto* lblTabHk = new QLabel(I18n::t("akp_lbl_tab_hotkey"), ctrlCard);
    lblTabHk->setFont(makeFont(11, QFont::Bold));
    lblTabHk->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_hkTabToggle = new KeyCaptureButton(m_state, "F6", true, ctrlCard);
    m_hkTabToggle->setFixedSize(115, 30);
    connect(m_hkTabToggle, &KeyCaptureButton::keyCaptured, this, [this](const QString& k, int, bool) {
        onTabHotkeyChanged(k);
    });

    m_btnStartTab = new Button(I18n::t("akp_btn_start_tab"), "primary", "play", 34, ctrlCard);
    connect(m_btnStartTab, &Button::clicked, this, &AutoPresserPage::onToggleCurrentTab);

    rProc->addWidget(m_btnChooseProc);
    rProc->addWidget(m_procCombo, 1);
    rProc->addWidget(btnRefProc);
    rProc->addWidget(m_btnSwitchWin);
    rProc->addSpacing(4);
    rProc->addWidget(lblBg);
    rProc->addWidget(m_togBg);
    rProc->addSpacing(6);
    rProc->addWidget(lblTabHk);
    rProc->addWidget(m_hkTabToggle);
    rProc->addSpacing(4);
    rProc->addWidget(m_btnStartTab);
    cLay->addLayout(rProc);

    // Row B
    auto* rMeth = new QHBoxLayout();
    rMeth->setSpacing(6);

    auto* lblM = new QLabel(I18n::t("akp_lbl_method"), ctrlCard);
    lblM->setFont(makeFont(11, QFont::Bold));
    lblM->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_methodCombo = new SearchCombo(AutoPresserEngine::localizedKeyMethodOptions(), "attach_state", ctrlCard);
    m_methodCombo->setMinimumWidth(250);
    connect(m_methodCombo, &SearchCombo::currentChanged, this, &AutoPresserPage::onMethodChanged);

    auto* lblHold = new QLabel(I18n::t("lbl_hold_duration"), ctrlCard);
    lblHold->setFont(makeFont(11, QFont::Bold));
    lblHold->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));

    m_numHold = new NumberInput(50, 10, 2000, 10, "ms", 106, ctrlCard);
    m_numHold->setFixedSize(106, 30);
    connect(m_numHold, &NumberInput::valueChanged, this, &AutoPresserPage::onHoldChanged);

    auto* btnTestM = new Button(I18n::t("akp_btn_test_method"), "glass", "bolt", "#10B981", false, ctrlCard);
    btnTestM->setFixedHeight(30);
    connect(btnTestM, &Button::clicked, this, &AutoPresserPage::onTestActiveMethod);

    m_btnSweepAll = new Button(I18n::t("akp_btn_test_all_methods"), "glass", "shield", "#06B6D4", false, ctrlCard);
    m_btnSweepAll->setFixedHeight(30);
    connect(m_btnSweepAll, &Button::clicked, this, &AutoPresserPage::onSweepAllKeyMethods);

    auto* btnSaveProf = new Button(I18n::t("akp_btn_save_profile"), "ghost", "save", 30, ctrlCard);
    connect(btnSaveProf, &Button::clicked, this, &AutoPresserPage::onSaveTabProfile);

    auto* btnLoadProf = new Button(I18n::t("akp_btn_load_profile"), "ghost", "folder", 30, ctrlCard);
    connect(btnLoadProf, &Button::clicked, this, &AutoPresserPage::onLoadTabProfile);

    rMeth->addWidget(lblM);
    rMeth->addWidget(m_methodCombo, 1);
    rMeth->addWidget(lblHold);
    rMeth->addWidget(m_numHold);
    rMeth->addWidget(btnTestM);
    rMeth->addWidget(m_btnSweepAll);
    rMeth->addWidget(btnSaveProf);
    rMeth->addWidget(btnLoadProf);
    cLay->addLayout(rMeth);

    root->addWidget(ctrlCard);

    // 4. Sub-View Bar
    auto* subBar = new QHBoxLayout();
    subBar->setSpacing(8);

    m_subSeg = new SegmentedControl(
        {
            {"keys", I18n::t("akp_sub_keys")},
            {"mouse", I18n::t("akp_sub_mouse")},
            {"log", I18n::t("akp_sub_log")},
        },
        "keys",
        this
    );
    connect(m_subSeg, &SegmentedControl::changed, this, &AutoPresserPage::onSubViewChanged);
    subBar->addWidget(m_subSeg);
    subBar->addStretch(1);

    m_btnAddCustomKey = new AddCustomKeyButton(m_state, this);
    connect(m_btnAddCustomKey, &AddCustomKeyButton::customKeyCaptured, this, &AutoPresserPage::onCustomKeyCaptured);

    auto* btnArmAll = new Button(I18n::t("akp_btn_arm_all"), "ghost", QString(), 32, this);
    connect(btnArmAll, &Button::clicked, this, [this]() { setAllKeysEnabled(true); });

    auto* btnDisarmAll = new Button(I18n::t("akp_btn_disarm_all"), "ghost", QString(), 32, this);
    connect(btnDisarmAll, &Button::clicked, this, [this]() { setAllKeysEnabled(false); });

    subBar->addWidget(m_btnAddCustomKey);
    subBar->addWidget(btnArmAll);
    subBar->addWidget(btnDisarmAll);
    root->addLayout(subBar);

    // 5. Main Body Card
    m_bodyCard = new GlassCard(false, this);
    auto* bLay = new QVBoxLayout(m_bodyCard);
    bLay->setContentsMargins(10, 8, 10, 8);
    bLay->setSpacing(6);

    // 5a. Keys Grid Scroll Area
    m_keysScroll = new QScrollArea(m_bodyCard);
    m_keysScroll->setWidgetResizable(true);
    m_keysScroll->setFrameShape(QFrame::NoFrame);
    m_keysScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_keysScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_keysScroll->setStyleSheet(modernScrollStyle());

    m_keysContainer = new QWidget();
    m_keysContainer->setStyleSheet("background: transparent;");
    m_keysOuterVbox = new QVBoxLayout(m_keysContainer);
    m_keysOuterVbox->setContentsMargins(2, 2, 4, 2);
    m_keysOuterVbox->setSpacing(6);

    m_lblBuiltinHdr = new QLabel(I18n::t("lbl_builtin_36_section"), m_keysContainer);
    m_lblBuiltinHdr->setFont(makeFont(11, QFont::Bold));
    m_lblBuiltinHdr->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    m_keysOuterVbox->addWidget(m_lblBuiltinHdr);

    m_builtinGridW = new QWidget(m_keysContainer);
    m_builtinGridW->setStyleSheet("background: transparent;");
    m_keysGrid = new QGridLayout(m_builtinGridW);
    m_keysGrid->setContentsMargins(0, 0, 0, 0);
    m_keysGrid->setHorizontalSpacing(7);
    m_keysGrid->setVerticalSpacing(7);
    for (int colIdx = 0; colIdx < 12; ++colIdx) {
        m_keysGrid->setColumnStretch(colIdx, 1);
    }
    m_keysOuterVbox->addWidget(m_builtinGridW);

    m_lblCustomHdr = new QLabel(I18n::t("lbl_custom_keys_section"), m_keysContainer);
    m_lblCustomHdr->setFont(makeFont(11, QFont::Bold));
    m_lblCustomHdr->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    m_keysOuterVbox->addWidget(m_lblCustomHdr);

    m_customGridW = new QWidget(m_keysContainer);
    m_customGridW->setStyleSheet("background: transparent;");
    m_customKeysGrid = new QGridLayout(m_customGridW);
    m_customKeysGrid->setContentsMargins(0, 0, 0, 0);
    m_customKeysGrid->setHorizontalSpacing(6);
    m_customKeysGrid->setVerticalSpacing(6);
    for (int colIdx = 0; colIdx < 8; ++colIdx) {
        m_customKeysGrid->setColumnStretch(colIdx, 1);
    }
    m_keysOuterVbox->addWidget(m_customGridW);
    m_keysOuterVbox->addStretch(1);

    m_keysScroll->setWidget(m_keysContainer);
    bLay->addWidget(m_keysScroll, 1);

    // 5b. Multi-Point Mouse Clicks Container
    m_mousePanel = new QWidget(m_bodyCard);
    m_mousePanel->setStyleSheet("background: transparent;");
    auto* mpLay = new QVBoxLayout(m_mousePanel);
    mpLay->setContentsMargins(0, 0, 0, 0);
    mpLay->setSpacing(8);

    auto* mpTop = new QHBoxLayout();
    mpTop->setSpacing(8);
    m_togMouseMaster = new ToggleSwitch(false, m_mousePanel);
    connect(m_togMouseMaster, &ToggleSwitch::toggled, this, &AutoPresserPage::onMouseMasterToggled);
    auto* lblMm = new QLabel(I18n::t("akp_lbl_enable_mouse"), m_mousePanel);
    lblMm->setFont(makeFont(13, QFont::ExtraBold));
    lblMm->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    auto* lblMmode = new QLabel(I18n::t("akp_lbl_mouse_mode"), m_mousePanel);
    lblMmode->setFont(makeFont(12, QFont::Bold));
    lblMmode->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    m_mouseModeCombo = new SearchCombo(AutoPresserEngine::localizedMouseModeOptions(), "auto", m_mousePanel);
    m_mouseModeCombo->setMinimumWidth(220);
    connect(m_mouseModeCombo, &SearchCombo::currentChanged, this, &AutoPresserPage::onMouseModeChanged);

    m_btnSweepMouse = new Button(I18n::t("akp_btn_test_all_mouse_modes"), "glass", "shield", "#06B6D4", false, m_mousePanel);
    m_btnSweepMouse->setFixedHeight(32);
    connect(m_btnSweepMouse, &Button::clicked, this, &AutoPresserPage::onSweepAllMouseModes);

    auto* btnAddMp = new Button(I18n::t("akp_btn_add_mouse_pt"), "primary", "plus", 32, m_mousePanel);
    connect(btnAddMp, &Button::clicked, this, &AutoPresserPage::onAddMousePoint);

    mpTop->addWidget(m_togMouseMaster);
    mpTop->addWidget(lblMm);
    mpTop->addSpacing(8);
    mpTop->addWidget(lblMmode);
    mpTop->addWidget(m_mouseModeCombo, 1);
    mpTop->addWidget(m_btnSweepMouse);
    mpTop->addWidget(btnAddMp);
    mpLay->addLayout(mpTop);

    m_mouseScroll = new QScrollArea(m_mousePanel);
    m_mouseScroll->setWidgetResizable(true);
    m_mouseScroll->setFrameShape(QFrame::NoFrame);
    m_mouseScroll->setStyleSheet(modernScrollStyle());

    m_mouseListW = new QWidget();
    m_mouseListW->setStyleSheet("background: transparent;");
    m_mouseVbox = new QVBoxLayout(m_mouseListW);
    m_mouseVbox->setContentsMargins(0, 0, 4, 0);
    m_mouseVbox->setSpacing(6);
    m_mouseScroll->setWidget(m_mouseListW);
    mpLay->addWidget(m_mouseScroll, 1);

    m_mousePanel->hide();
    bLay->addWidget(m_mousePanel, 1);

    // 5c. Per-Tab Activity Log Container
    m_logPanel = new QWidget(m_bodyCard);
    m_logPanel->setStyleSheet("background: transparent;");
    auto* lgLay = new QVBoxLayout(m_logPanel);
    lgLay->setContentsMargins(0, 0, 0, 0);
    lgLay->setSpacing(6);

    auto* lgTop = new QHBoxLayout();
    auto* lblLg = new QLabel(I18n::t("akp_lbl_activity_log"), m_logPanel);
    lblLg->setFont(makeFont(13, QFont::ExtraBold));
    lblLg->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    auto* btnClrLg = new Button(I18n::t("btn_clear_test_log"), "ghost", "trash", 30, m_logPanel);
    connect(btnClrLg, &Button::clicked, this, &AutoPresserPage::onClearTabLog);
    lgTop->addWidget(lblLg, 1);
    lgTop->addWidget(btnClrLg);
    lgLay->addLayout(lgTop);

    m_logScroll = new QScrollArea(m_logPanel);
    m_logScroll->setWidgetResizable(true);
    m_logScroll->setFrameShape(QFrame::NoFrame);
    m_logScroll->setStyleSheet(modernScrollStyle());

    m_logListW = new QWidget();
    m_logListW->setStyleSheet("background: transparent;");
    m_logVbox = new QVBoxLayout(m_logListW);
    m_logVbox->setContentsMargins(0, 0, 4, 0);
    m_logVbox->setSpacing(6);
    m_logScroll->setWidget(m_logListW);
    lgLay->addWidget(m_logScroll, 1);

    m_logPanel->hide();
    bLay->addWidget(m_logPanel, 1);

    root->addWidget(m_bodyCard, 1);
}

void AutoPresserPage::scheduleSaveWorkspace() {
    if (m_saveTimer) m_saveTimer->start();
}

void AutoPresserPage::onThemeUpdated() {
    if (m_tabsScrollBox) m_tabsScrollBox->update();
    if (m_tabsScroll) m_tabsScroll->setStyleSheet(modernScrollStyle());
    if (m_keysScroll) m_keysScroll->setStyleSheet(modernScrollStyle());
    update();
}

void AutoPresserPage::onExternalSettingsChanged(const QString& key, const QVariant& val) {
    if (key == "hotkey_autopresser_stop_all" && m_hkStopAllTabs) {
        m_hkStopAllTabs->setKeyString(val.toString().isEmpty() ? "F7" : val.toString());
    }
}

void AutoPresserPage::onStopAllHotkeyChanged(const QString& newKey) {
    const QString clean = newKey.trimmed().isEmpty() ? QStringLiteral("F7") : newKey.trimmed();
    if (m_state) m_state->updateSetting("hotkey_autopresser_stop_all", clean);
}

void AutoPresserPage::onTabHotkeyChanged(const QString& newKey) {
    AutoPresserTab& tab = currentTab();
    tab.stopHotkey = newKey.trimmed();
    m_akp->saveWorkspace();
    refreshTabsBar();
}

void AutoPresserPage::refreshTabsBar() {
    if (!m_tabsBtnContainer || !m_tabsBtnLayout || !m_akp) return;
    m_tabsBtnContainer->setUpdatesEnabled(false);
    while (QLayoutItem* it = m_tabsBtnLayout->takeAt(0)) {
        if (QWidget* w = it->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete it;
    }

    for (int idx = 0; idx < m_akp->tabs.size(); ++idx) {
        const AutoPresserTab& tab = m_akp->tabs[idx];
        const bool running = m_akp->isTabRunning(tab.tabId);
        const bool active = (idx == m_akp->activeTabIndex);
        const QString prefix = running ? QString::fromUtf8("🟢 ") : QString();
        const QString hkBadge = !tab.stopHotkey.isEmpty() ? QStringLiteral(" [%1]").arg(tab.stopHotkey) : QString();
        auto* btn = new Button(
            QStringLiteral("%1%2%3").arg(prefix, tab.title, hkBadge),
            active ? "primary" : "glass",
            running ? "play" : QString(),
            28,
            m_tabsBtnContainer
        );
        btn->setFixedWidth(btn->naturalCompactWidth() + 8);
        connect(btn, &Button::clicked, this, [this, idx]() { selectTab(idx); });
        m_tabsBtnLayout->addWidget(btn);
    }
    m_tabsBtnLayout->addStretch(1);
    m_tabsBtnContainer->setUpdatesEnabled(true);
}

void AutoPresserPage::selectTab(int index) {
    m_akp->activeTabIndex = index;
    scheduleSaveWorkspace();
    refreshTabsBar();
    loadCurrentTabIntoUi();
}

void AutoPresserPage::loadCurrentTabIntoUi() {
    setUpdatesEnabled(false);
    AutoPresserTab& tab = currentTab();
    if (m_tabNameEdit) m_tabNameEdit->setText(tab.title);
    if (m_togBg) m_togBg->setChecked(tab.runInBackground, false);
    if (m_hkTabToggle) m_hkTabToggle->setKeyString(tab.stopHotkey.isEmpty() ? "F6" : tab.stopHotkey);
    if (m_methodCombo) m_methodCombo->setSelectedKey(tab.keyMethod, false);
    if (m_numHold) m_numHold->setValue(tab.holdMs, false);
    if (m_togMouseMaster) m_togMouseMaster->setChecked(tab.mouseEnabled, false);
    if (m_mouseModeCombo) m_mouseModeCombo->setSelectedKey(tab.mouseMode, false);
    refreshProcessDropdown(tab.targetProcess, false);
    rebuildKeysGrid();
    rebuildMousePointsList();
    if (m_subView == "log") rebuildActivityLog();
    syncStartButtonState();
    setUpdatesEnabled(true);
}

void AutoPresserPage::refreshProcessDropdown(const QString& selectedProc, bool forceOsScan) {
    AutoPresserTab& tab = currentTab();
    const QString sel = selectedProc.isNull() ? tab.targetProcess : selectedProc;
    if (forceOsScan || m_windowCache.isEmpty()) {
        m_windowCache = Win32Input::listDetailedWindowsForPicker();
    }

    QMap<QString, QStringList> otherBoundMap;
    for (const auto& tObj : m_akp->tabs) {
        if (!tObj.targetProcess.isEmpty() && tObj.tabId != tab.tabId) {
            otherBoundMap[tObj.targetProcess.toLower()].append(tObj.title);
        }
    }

    QList<SearchComboItem> items;
    items.append(SearchComboItem(QString(), I18n::t("all_apps")));
    QSet<QString> seen;
    const QString selLow = sel.toLower().trimmed();

    for (const auto& w : m_windowCache) {
        const QString pname = w.procName;
        const QString keyLow = pname.toLower();
        if (seen.contains(keyLow)) continue;
        seen.insert(keyLow);

        const QStringList otherOwners = otherBoundMap.value(keyLow);
        const bool isThisTab = (!selLow.isEmpty() && keyLow == selLow);
        QString badge, rowTint;
        if (!otherOwners.isEmpty() && isThisTab) {
            badge = QString::fromUtf8("  [🟢 %1 + 🔶 %2: %3]")
                .arg(I18n::t("akp_badge_used_in_this_tab"), I18n::t("akp_badge_used_in_other_tab"), otherOwners.join(", "));
            rowTint = "#F59E0B";
        } else if (!otherOwners.isEmpty()) {
            badge = QString::fromUtf8("  [🔶 %1: %2]")
                .arg(I18n::t("akp_badge_used_in_other_tab"), otherOwners.join(", "));
            rowTint = "#F59E0B";
        } else if (isThisTab) {
            badge = QString::fromUtf8("  [✓ %1]").arg(I18n::t("akp_badge_used_in_this_tab"));
            rowTint = "#10B981";
        }
        const QString disp = QStringLiteral("%1 — %2 (PID: %3)%4")
            .arg(pname, w.title.left(38), QString::number(w.pid), badge);
        items.append(SearchComboItem(pname, disp, w.exePath, rowTint));
    }

    if (!sel.isEmpty() && !seen.contains(selLow)) {
        const QStringList otherOwners = otherBoundMap.value(selLow);
        const QString rowTint = !otherOwners.isEmpty() ? "#F59E0B" : "#10B981";
        items.append(SearchComboItem(sel, sel, QPixmap(), rowTint));
    }

    if (m_procCombo) m_procCombo->setRichItems(items, sel);
}

void AutoPresserPage::refreshProcessDropdownAndOpen() {
    refreshProcessDropdown(QString(), true);
    if (m_procCombo) {
        QMouseEvent ev(QEvent::MouseButtonRelease, QPointF(10, 10), m_procCombo->mapToGlobal(QPoint(10, 10)), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QCoreApplication::sendEvent(m_procCombo, &ev);
    }
}

void AutoPresserPage::onProcessSelected(const QString& procName) {
    AutoPresserTab& tab = currentTab();
    tab.targetProcess = procName;
    tab.targetHwnd = 0;
    tab.targetTitle.clear();
    for (const auto& w : m_windowCache) {
        if (w.procName.compare(procName, Qt::CaseInsensitive) == 0) {
            tab.targetHwnd = w.hwnd;
            tab.targetTitle = w.title;
            break;
        }
    }
    m_akp->saveWorkspace();
    refreshProcessDropdown(procName, false);
}

bool AutoPresserPage::hasMatchingSmartCombo(const QString& keyLabel, SmartCombo* outCombo) const {
    if (!m_state) return false;
    for (const auto& c : m_state->smartCombos) {
        if (c.triggerKey.compare(keyLabel, Qt::CaseInsensitive) == 0 ||
            c.name.compare(keyLabel, Qt::CaseInsensitive) == 0) {
            if (outCombo) *outCombo = c;
            return true;
        }
    }
    return false;
}

void AutoPresserPage::rebuildKeysGrid() {
    AutoPresserTab& tab = currentTab();
    tab.ensureBuiltinKeys();
    m_keysContainer->setUpdatesEnabled(false);

    QList<AutoPresserKeyItem*> builtinItems;
    QList<AutoPresserKeyItem*> customItems;
    for (int i = 0; i < tab.keys.size(); ++i) {
        if (!tab.keys[i].isCustom && builtinItems.size() < 36) {
            builtinItems.append(&tab.keys[i]);
        } else if (tab.keys[i].isCustom) {
            customItems.append(&tab.keys[i]);
        }
    }

    const int colsBuiltin = 12;
    if (m_builtinTiles.size() != builtinItems.size()) {
        while (QLayoutItem* it = m_keysGrid->takeAt(0)) {
            if (QWidget* w = it->widget()) { w->hide(); w->deleteLater(); }
            delete it;
        }
        m_builtinTiles.clear();
        for (int idx = 0; idx < builtinItems.size(); ++idx) {
            AutoPresserKeyItem* kItem = builtinItems[idx];
            const bool hasMacro = hasMatchingSmartCombo(kItem->key);
            auto* tile = new KeyTileWidget(kItem, hasMacro, m_builtinGridW);
            connect(tile, &KeyTileWidget::changed, this, &AutoPresserPage::scheduleSaveWorkspace);
            connect(tile, &KeyTileWidget::testClicked, this, &AutoPresserPage::onTestSingleKey);
            m_keysGrid->addWidget(tile, idx / colsBuiltin, idx % colsBuiltin);
            m_builtinTiles.append(tile);
        }
    } else {
        for (int idx = 0; idx < builtinItems.size(); ++idx) {
            AutoPresserKeyItem* kItem = builtinItems[idx];
            const bool hasMacro = hasMatchingSmartCombo(kItem->key);
            m_builtinTiles[idx]->bindItem(kItem, hasMacro);
        }
    }

    const int builtinRows = std::max(1, static_cast<int>((builtinItems.size() + colsBuiltin - 1) / colsBuiltin));
    m_builtinGridW->setMinimumHeight(builtinRows * 78 + std::max(0, builtinRows - 1) * 6);

    while (QLayoutItem* it = m_customKeysGrid->takeAt(0)) {
        if (QWidget* w = it->widget()) { w->hide(); w->deleteLater(); }
        delete it;
    }

    m_lblCustomHdr->setVisible(!customItems.isEmpty());
    m_customGridW->setVisible(!customItems.isEmpty());
    const int colsCustom = 8;
    for (int idx = 0; idx < customItems.size(); ++idx) {
        AutoPresserKeyItem* kItem = customItems[idx];
        const bool hasMacro = hasMatchingSmartCombo(kItem->key);
        auto* tile = new KeyTileWidget(kItem, hasMacro, m_customGridW);
        connect(tile, &KeyTileWidget::changed, this, &AutoPresserPage::scheduleSaveWorkspace);
        connect(tile, &KeyTileWidget::testClicked, this, &AutoPresserPage::onTestSingleKey);
        connect(tile, &KeyTileWidget::removeClicked, this, &AutoPresserPage::onRemoveCustomKey);
        m_customKeysGrid->addWidget(tile, idx / colsCustom, idx % colsCustom);
    }
    if (!customItems.isEmpty()) {
        const int customRows = std::max(1, static_cast<int>((customItems.size() + colsCustom - 1) / colsCustom));
        m_customGridW->setMinimumHeight(customRows * 78 + std::max(0, customRows - 1) * 6);
    } else {
        m_customGridW->setMinimumHeight(0);
    }

    m_keysContainer->setUpdatesEnabled(true);
}

void AutoPresserPage::rebuildMousePointsList() {
    AutoPresserTab& tab = currentTab();
    m_mouseListW->setUpdatesEnabled(false);
    while (QLayoutItem* it = m_mouseVbox->takeAt(0)) {
        if (QWidget* w = it->widget()) { w->hide(); w->deleteLater(); }
        delete it;
    }

    for (int idx = 0; idx < tab.mousePoints.size(); ++idx) {
        auto* row = new MousePointRowWidget(idx, &tab.mousePoints[idx], m_pickingMouseIdx == idx, m_mouseListW);
        connect(row, &MousePointRowWidget::changed, this, &AutoPresserPage::scheduleSaveWorkspace);
        connect(row, &MousePointRowWidget::removed, this, &AutoPresserPage::onRemoveMousePoint);
        connect(row, &MousePointRowWidget::pickRequested, this, &AutoPresserPage::onPickMousePoint);
        connect(row, &MousePointRowWidget::testRequested, this, &AutoPresserPage::onTestSingleMousePoint);
        m_mouseVbox->addWidget(row);
    }
    m_mouseVbox->addStretch(1);
    m_mouseListW->setUpdatesEnabled(true);
}

void AutoPresserPage::onTestSingleMousePoint(int idx) {
    AutoPresserTab& tab = currentTab();
    if (idx < 0 || idx >= tab.mousePoints.size()) return;
    if (ensureTargetWindowAndFocus() == 0) return;
    const quintptr hwnd = AutoPresserEngine::resolveTargetHwnd(tab);
    const AutoPresserMousePoint& mp = tab.mousePoints[idx];

    QElapsedTimer timer;
    timer.start();
    QJsonObject metaM;
    const bool ok = AutoPresserEngine::dispatchMouseDetailed(mp, tab.mouseMode, hwnd, tab.runInBackground, true, false, &metaM);
    const int dtMs = std::max(1, static_cast<int>(timer.elapsed()));

    QJsonObject logEntry;
    logEntry["kind"] = "sent_mouse";
    logEntry["btn"] = mp.button;
    logEntry["x"] = mp.x;
    logEntry["y"] = mp.y;
    logEntry["client_x"] = metaM.value("client_x").toInt(mp.x);
    logEntry["client_y"] = metaM.value("client_y").toInt(mp.y);
    logEntry["child_cls"] = metaM.value("child_cls").toString("TopWindow");
    logEntry["dest_hwnd"] = metaM.value("dest_hwnd").toString(hwnd ? QStringLiteral("0x%1").arg(hwnd, 0, 16).toUpper() : "Foreground");
    logEntry["reason_code"] = metaM.value("reason_code").toString();
    logEntry["mode"] = tab.mouseMode;
    logEntry["target_app"] = tab.targetProcess.isEmpty() ? "Active Window" : tab.targetProcess;
    logEntry["is_bg"] = tab.runInBackground;
    logEntry["dt_ms"] = dtMs;
    logEntry["ok"] = ok;
    m_akp->appendTabLog(tab.tabId, logEntry, ok ? "#06B6D4" : "#EF4444");

    if (m_state) {
        emit m_state->toastRequested(
            QStringLiteral("%1 %2 (%3) -> (%4, %5) [%6]")
                .arg(ok ? "✓" : "✗", I18n::t("log_mouse_click_at"), mp.button.toUpper())
                .arg(mp.x).arg(mp.y).arg(tab.mouseMode),
            ok ? "ok" : "danger"
        );
    }
    if (m_subView == "log") rebuildActivityLog();
}

QString AutoPresserPage::renderLogHtml(const QJsonObject& entry) const {
    const QString kind = entry.value("kind").toString();
    if (kind == "started") {
        const QString modeLbl = I18n::t(entry.value("is_bg").toBool(true) ? "log_lbl_mode_bg" : "log_lbl_mode_fg");
        return QStringLiteral("<b style='color:#10B981;'>▶ %1</b> — %2: <b>%3</b> | Mouse: <b>%4</b> | %5 <b dir='ltr' style='color:#8B5CF6;'>%6</b> (%7)")
            .arg(I18n::t("akp_log_started"),
                 I18n::t("akp_sub_keys"),
                 I18n::fmtNum(QString::number(entry.value("keys_count").toInt())),
                 I18n::fmtNum(QString::number(entry.value("mouse_count").toInt())),
                 I18n::t("akp_lbl_method"),
                 entry.value("method").toString(),
                 modeLbl);
    }
    if (kind == "stopped") {
        return QStringLiteral("<b style='color:#F59E0B;'>⏹ %1</b>").arg(I18n::t("akp_log_stopped"));
    }
    if (kind == "no_keys_armed") {
        return QStringLiteral("<b style='color:#F59E0B;'>⚠ %1</b>").arg(I18n::t("akp_log_no_keys"));
    }
    if (kind == "sweep_header") {
        const QString modeLbl = I18n::t(entry.value("is_bg").toBool(true) ? "log_lbl_mode_bg" : "log_lbl_mode_fg");
        return QStringLiteral("<b style='color:#06B6D4;'>⚡ %1</b> <span dir='ltr'><b>%2</b> [%3]</span> (%4) — <b>%5</b> <span dir='ltr' style='color:#A78BFA;'><b>[%6]</b></span>")
            .arg(I18n::t("akp_log_sweep_header"),
                 entry.value("target").toString(),
                 entry.value("hwnd").toString(),
                 modeLbl,
                 I18n::t("log_lbl_action"),
                 entry.value("test_key").toString("F1"));
    }
    if (kind == "sweep_item") {
        const bool ok = entry.value("ok").toBool();
        const int idx = entry.value("idx").toInt(1);
        const int total = entry.value("total").toInt(22);
        const QString trK = entry.value("method_tr_key").toString();
        const QString mId = entry.value("method_id").toString();
        const QString mName = !trK.isEmpty() ? I18n::t(trK) : mId;
        const QString badgeCol = ok ? "#10B981" : "#EF4444";
        const QString badgeTxt = I18n::t(ok ? "akp_log_sweep_item_ok" : "akp_log_sweep_item_fail");
        const QString dtMs = I18n::fmtNum(QString::number(entry.value("dt_ms").toInt(1)));
        const QString targetApp = entry.value("target_app").toString("Active Window");
        const QString hwndStr = entry.value("dest_hwnd").toString(entry.value("hwnd").toString("Foreground"));
        const QString childCls = entry.value("child_cls").toString("TopWindow");
        const QString modeLbl = I18n::t(entry.value("is_bg").toBool(true) ? "log_lbl_mode_bg" : "log_lbl_mode_fg");
        const QString reasonStr = AutoPresserEngine::formatDiagnosticReason(entry);

        QString actStr;
        if (entry.value("sweep_type").toString() == "mouse") {
            const int sx = entry.value("screen_x").toInt();
            const int sy = entry.value("screen_y").toInt();
            const int cx = entry.value("client_x").toInt(sx);
            const int cy = entry.value("client_y").toInt(sy);
            actStr = QString::fromUtf8("🖱 %1 @ Screen(%2, %3) &rarr; Client(%4, %5)")
                .arg(entry.value("btn").toString("left").toUpper()).arg(sx).arg(sy).arg(cx).arg(cy);
        } else {
            actStr = QString::fromUtf8("⌨ [%1]").arg(entry.value("key").toString("F1"));
        }

        const QString line1 = QStringLiteral("<span dir='ltr' style='color:#06B6D4; font-weight:800;'>[#%1/%2]</span> <b style='color:%3;'>%4</b> — <b>%5</b> <span dir='ltr' style='color:#3B82F6; font-weight:bold;'>(%6 ms)</span>")
            .arg(idx, 2, 10, QChar('0')).arg(total).arg(badgeCol, badgeTxt, mName, dtMs);
        const QString line2 = QString::fromUtf8("<span style='color:#94A3B8; font-size:11px;'>🎯 <b>%1</b> <span dir='ltr'>%2 [%3 &rarr; %4]</span> (%5) &nbsp;|&nbsp; ⚡ <b>%6</b> <span dir='ltr' style='color:#A78BFA;'>%7</span> &nbsp;|&nbsp; 📌 <b>%8</b> <span style='color:%9;'>%10</span></span>")
            .arg(I18n::t("log_lbl_target"), targetApp, hwndStr, childCls, modeLbl, I18n::t("log_lbl_action"), actStr, I18n::t("log_lbl_details"), badgeCol, reasonStr);
        return QStringLiteral("<div>%1<br>%2</div>").arg(line1, line2);
    }
    if (kind == "sweep_summary") {
        const QString modeLbl = I18n::t(entry.value("is_bg").toBool(true) ? "log_lbl_mode_bg" : "log_lbl_mode_fg");
        QString msg = I18n::t("akp_log_sweep_summary");
        msg.replace("{ok}", I18n::fmtNum(QString::number(entry.value("ok_count").toInt())));
        msg.replace("{fail}", I18n::fmtNum(QString::number(entry.value("fail_count").toInt())));
        return QString::fromUtf8("<b style='color:#10B981;'>🏁 %1</b> — <span dir='ltr'><b>%2</b></span> (%3)")
            .arg(msg, entry.value("target").toString(), modeLbl);
    }
    if (kind == "triggered_smart_macro") {
        return QStringLiteral("<b style='color:#F59E0B;'>%1</b> <b dir='ltr'>[ %2 ] &rarr; %3</b> <span dir='ltr'>(%4 ms)</span>")
            .arg(I18n::t("akp_log_triggered_smart_macro"),
                 entry.value("key").toString(),
                 entry.value("combo_name").toString(),
                 I18n::fmtNum(QString::number(entry.value("interval_ms").toInt(1000))));
    }
    if (kind == "window_required_warn") {
        return QStringLiteral("<b style='color:#EF4444;'>%1</b>").arg(I18n::t("akp_err_must_select_window"));
    }
    if (kind == "sent_key") {
        const bool ok = entry.value("ok").toBool(true);
        const QString badgeCol = ok ? "#10B981" : "#EF4444";
        return QString::fromUtf8("⌨ %1 <b dir='ltr' style='color:#8B5CF6;'>[ %2 ]</b> <span dir='ltr'>(%3 ms | %4 ms)</span> — %5 <b dir='ltr' style='color:%6;'>%7</b>")
            .arg(I18n::t("akp_log_sent_key"),
                 entry.value("key").toString(),
                 I18n::fmtNum(QString::number(entry.value("interval_ms").toInt(1000))),
                 I18n::fmtNum(QString::number(entry.value("dt_ms").toInt(1))),
                 I18n::t("log_actual_engine"),
                 badgeCol,
                 entry.value("method").toString());
    }
    if (kind == "sent_mouse") {
        const bool ok = entry.value("ok").toBool(true);
        const QString badgeCol = ok ? "#10B981" : "#EF4444";
        const int sx = entry.value("x").toInt();
        const int sy = entry.value("y").toInt();
        const int cx = entry.value("client_x").toInt(sx);
        const int cy = entry.value("client_y").toInt(sy);
        const QString childCls = entry.value("child_cls").toString("TopWindow");
        const QString dtMs = I18n::fmtNum(QString::number(entry.value("dt_ms").toInt(1)));
        const QString reasonStr = !entry.value("reason_code").toString().isEmpty() ? AutoPresserEngine::formatDiagnosticReason(entry) : QString();
        const QString extraR = !reasonStr.isEmpty() ? QStringLiteral(" — <span style='color:%1;'>%2</span>").arg(badgeCol, reasonStr) : QString();
        return QString::fromUtf8("🖱 %1 <b dir='ltr' style='color:#06B6D4;'>(%2)</b> %3 <b dir='ltr'>Screen(%4, %5) &rarr; Client(%6, %7) [%8]</b> — <b dir='ltr' style='color:%9;'>%10</b> <span dir='ltr'>(%11 ms)</span>%12")
            .arg(I18n::t("log_mouse_click_at"),
                 entry.value("btn").toString("left").toUpper(),
                 I18n::t("log_at_coords"))
            .arg(sx).arg(sy).arg(cx).arg(cy)
            .arg(childCls, badgeCol, entry.value("mode").toString(), dtMs, extraR);
    }
    return entry.value("text").toString();
}

QString AutoPresserPage::computeLogSig(const QString& tabId) const {
    const auto logs = m_akp->getTabLogs(tabId);
    const QString topTs = !logs.isEmpty()
        ? QStringLiteral("%1_%2_%3").arg(logs.first().value("ts").toString(), logs.first().value("kind").toString(), logs.first().value("method_id").toString())
        : QString();
    return QStringLiteral("%1:%2:%3").arg(tabId).arg(logs.size()).arg(topTs);
}

void AutoPresserPage::rebuildActivityLog() {
    AutoPresserTab& tab = currentTab();
    m_lastLogSig = computeLogSig(tab.tabId);
    while (QLayoutItem* it = m_logVbox->takeAt(0)) {
        if (QWidget* w = it->widget()) { w->hide(); w->deleteLater(); }
        delete it;
    }

    const auto logs = m_akp->getTabLogs(tab.tabId);
    const int limit = std::min(80, static_cast<int>(logs.size()));
    for (int idx = 0; idx < limit; ++idx) {
        const QJsonObject& item = logs[idx];
        const QString accentHex = item.value("accent").toString("#8B5CF6");
        auto* card = new StepCard(idx, accentHex, m_logListW);
        auto* lay = new QHBoxLayout(card);
        lay->setContentsMargins(12, 5, 12, 5);
        lay->setSpacing(10);

        auto* lblTs = new QLabel(item.value("ts").toString(), card);
        lblTs->setFont(makeFont(11, QFont::Bold));
        lblTs->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(accentHex));

        auto* lblMsg = new QLabel(renderLogHtml(item), card);
        lblMsg->setTextFormat(Qt::RichText);
        lblMsg->setWordWrap(true);
        lblMsg->setFont(makeFont(12, QFont::DemiBold));
        lblMsg->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

        lay->addWidget(lblTs);
        lay->addWidget(lblMsg, 1);
        m_logVbox->addWidget(card);
    }
    m_logVbox->addStretch(1);
}

void AutoPresserPage::onSubViewChanged(const QString& viewKey) {
    m_subView = viewKey;
    if (m_keysScroll) m_keysScroll->setVisible(viewKey == "keys");
    if (m_mousePanel) m_mousePanel->setVisible(viewKey == "mouse");
    if (m_logPanel) m_logPanel->setVisible(viewKey == "log");
    if (viewKey == "log") rebuildActivityLog();
}

void AutoPresserPage::onTabNameEdited(const QString& txt) {
    AutoPresserTab& tab = currentTab();
    tab.title = txt.trimmed().isEmpty() ? QStringLiteral("Tab %1").arg(m_akp->activeTabIndex + 1) : txt.trimmed();
    m_akp->saveWorkspace();
    refreshTabsBar();
}

void AutoPresserPage::onAddTab() {
    m_akp->addNewTab(-1);
    scheduleSaveWorkspace();
    refreshTabsBar();
    loadCurrentTabIntoUi();
    QTimer::singleShot(25, this, [this]() {
        if (m_tabsScroll) m_tabsScroll->horizontalScrollBar()->setValue(m_tabsScroll->horizontalScrollBar()->maximum());
    });
}

void AutoPresserPage::onDupTab() {
    m_akp->addNewTab(m_akp->activeTabIndex);
    scheduleSaveWorkspace();
    refreshTabsBar();
    loadCurrentTabIntoUi();
    QTimer::singleShot(25, this, [this]() {
        if (m_tabsScroll) m_tabsScroll->horizontalScrollBar()->setValue(m_tabsScroll->horizontalScrollBar()->maximum());
    });
}

void AutoPresserPage::onCloseCurrentTab() {
    if (m_akp->tabs.size() <= 1) return;
    m_akp->closeTab(m_akp->activeTabIndex);
    scheduleSaveWorkspace();
    refreshTabsBar();
    loadCurrentTabIntoUi();
}

void AutoPresserPage::onBgToggled(bool val) {
    currentTab().runInBackground = val;
    m_akp->saveWorkspace();
}

void AutoPresserPage::onMethodChanged(const QString& methodKey) {
    currentTab().keyMethod = methodKey;
    m_akp->saveWorkspace();
}

void AutoPresserPage::onHoldChanged(int val) {
    currentTab().holdMs = std::max(10, val);
    m_akp->saveWorkspace();
}

quintptr AutoPresserPage::ensureTargetWindowAndFocus(int focusWindowMode) {
    AutoPresserTab& tab = currentTab();
    if (tab.targetProcess.trimmed().isEmpty()) {
        QJsonObject warn; warn["kind"] = "window_required_warn";
        m_akp->appendTabLog(tab.tabId, warn, "#EF4444");
        if (m_state) emit m_state->toastRequested(I18n::t("akp_err_must_select_window"), "danger");
        if (m_subView == "log") rebuildActivityLog();
        return 0;
    }

    const quintptr hwnd = AutoPresserEngine::resolveTargetHwnd(tab);
#ifdef _WIN32
    if (hwnd == 0) {
        if (m_state) emit m_state->toastRequested(I18n::t("akp_err_window_not_found"), "danger");
        return 0;
    }
#endif
    const bool shouldFocus = (focusWindowMode < 0) ? (!tab.runInBackground) : (focusWindowMode > 0);
    if (hwnd > 0 && shouldFocus) {
        Win32Input::focusAndBringHwndToFront(hwnd);
    }
    return hwnd > 0 ? hwnd : 1;
}

void AutoPresserPage::onSwitchToTargetWindow() {
    ensureTargetWindowAndFocus(1);
}

void AutoPresserPage::onTestActiveMethod() {
    AutoPresserTab& tab = currentTab();
    QString testKey = "F1";
    for (const auto& k : tab.keys) {
        if (k.enabled) { testKey = k.key; break; }
    }
    onTestSingleKey(testKey);
}

void AutoPresserPage::onSweepAllKeyMethods() {
    if (m_sweepRunning) return;
    AutoPresserTab& tab = currentTab();
    if (ensureTargetWindowAndFocus() == 0) return;

    QString testKey = "F1";
    for (const auto& k : tab.keys) {
        if (k.enabled) { testKey = k.key; break; }
    }
    m_sweepRunning = true;
    if (m_btnSweepAll) m_btnSweepAll->setBusy(true);
    if (m_subSeg) m_subSeg->setSelectedKey("log", true);
    onSubViewChanged("log");

    QPointer<AutoPresserPage> self(this);
    const int tabIdx = m_akp->activeTabIndex;
    std::thread([self, tabIdx, testKey]() {
        if (!self || !self->m_akp || tabIdx < 0 || tabIdx >= self->m_akp->tabs.size()) return;
        int okC = 0, failC = 0;
        self->m_akp->sweepAllKeyMethods(self->m_akp->tabs[tabIdx], testKey, okC, failC);
        QString summaryMsg = I18n::t("akp_log_sweep_summary");
        summaryMsg.replace("{ok}", I18n::fmtNum(QString::number(okC)));
        summaryMsg.replace("{fail}", I18n::fmtNum(QString::number(failC)));
        if (self) emit self->sweepDoneSignal("keys", summaryMsg, okC > 0 ? "ok" : "info");
    }).detach();
}

void AutoPresserPage::onSweepAllMouseModes() {
    if (m_sweepRunning) return;
    if (ensureTargetWindowAndFocus() == 0) return;

    m_sweepRunning = true;
    if (m_btnSweepMouse) m_btnSweepMouse->setBusy(true);
    if (m_subSeg) m_subSeg->setSelectedKey("log", true);
    onSubViewChanged("log");

    QPointer<AutoPresserPage> self(this);
    const int tabIdx = m_akp->activeTabIndex;
    std::thread([self, tabIdx]() {
        if (!self || !self->m_akp || tabIdx < 0 || tabIdx >= self->m_akp->tabs.size()) return;
        int okC = 0, failC = 0;
        self->m_akp->sweepAllMouseModes(self->m_akp->tabs[tabIdx], okC, failC);
        QString summaryMsg = I18n::t("akp_log_sweep_summary");
        summaryMsg.replace("{ok}", I18n::fmtNum(QString::number(okC)));
        summaryMsg.replace("{fail}", I18n::fmtNum(QString::number(failC)));
        if (self) emit self->sweepDoneSignal("mouse", summaryMsg, okC > 0 ? "ok" : "info");
    }).detach();
}

void AutoPresserPage::onSweepDoneMainThread(const QString& which, const QString& summaryMsg, const QString& toastKind) {
    m_sweepRunning = false;
    if (which == "keys") {
        if (m_btnSweepAll) m_btnSweepAll->setBusy(false);
    } else {
        if (m_btnSweepMouse) m_btnSweepMouse->setBusy(false);
    }
    if (m_subView == "log") rebuildActivityLog();
    if (!summaryMsg.isEmpty() && m_state) {
        emit m_state->toastRequested(summaryMsg, toastKind);
    }
}

void AutoPresserPage::onTestSingleKey(const QString& keyLabel) {
    AutoPresserTab& tab = currentTab();
    if (ensureTargetWindowAndFocus() == 0) return;
    const quintptr hwnd = AutoPresserEngine::resolveTargetHwnd(tab);

    for (const auto& kItem : tab.keys) {
        if (kItem.key.compare(keyLabel, Qt::CaseInsensitive) == 0 && kItem.useSmartCombo) {
            SmartCombo matched;
            if (hasMatchingSmartCombo(keyLabel, &matched) && m_macroEngine) {
                m_macroEngine->executeSmartCombo(matched);
                QJsonObject entry;
                entry["kind"] = "triggered_smart_macro";
                entry["key"] = keyLabel;
                entry["combo_name"] = matched.name.isEmpty() ? keyLabel : matched.name;
                entry["interval_ms"] = kItem.intervalMs;
                m_akp->appendTabLog(tab.tabId, entry, "#F59E0B");
                if (m_state) {
                    emit m_state->toastRequested(QStringLiteral("%1 [%2]").arg(I18n::t("akp_log_triggered_smart_macro"), keyLabel), "ok");
                }
                if (m_subView == "log") rebuildActivityLog();
                return;
            }
            break;
        }
    }

    QString outReason;
    const bool ok = AutoPresserEngine::dispatchKey(keyLabel, tab.keyMethod, tab.holdMs, hwnd, tab.runInBackground, true, false, &outReason);
    QJsonObject entry;
    entry["kind"] = "sent_key";
    entry["key"] = keyLabel;
    entry["interval_ms"] = tab.holdMs;
    entry["method"] = tab.keyMethod;
    entry["ok"] = ok;
    m_akp->appendTabLog(tab.tabId, entry, ok ? "#10B981" : "#EF4444");
    if (m_state) {
        emit m_state->toastRequested(QStringLiteral("✓ %1 -> %2").arg(keyLabel, tab.keyMethod), ok ? "ok" : "danger");
    }
    if (m_subView == "log") rebuildActivityLog();
}

void AutoPresserPage::onCustomKeyCaptured(const QString& keyStr) {
    const QString clean = keyStr.trimmed();
    if (clean.isEmpty()) return;
    AutoPresserTab& tab = currentTab();
    const bool hasSmart = hasMatchingSmartCombo(clean);
    bool found = false;
    for (auto& k : tab.keys) {
        if (k.key.compare(clean, Qt::CaseInsensitive) == 0) {
            k.enabled = true;
            if (hasSmart) k.useSmartCombo = true;
            found = true;
            break;
        }
    }
    if (!found) {
        AutoPresserKeyItem item;
        item.key = clean;
        item.enabled = true;
        item.intervalMs = 1000;
        item.isCustom = true;
        item.useSmartCombo = hasSmart;
        tab.keys.append(item);
    }
    m_akp->saveWorkspace();
    rebuildKeysGrid();
    QTimer::singleShot(40, this, [this]() {
        if (m_keysScroll) m_keysScroll->verticalScrollBar()->setValue(m_keysScroll->verticalScrollBar()->maximum());
    });
    if (m_state) {
        emit m_state->toastRequested(QStringLiteral("%1 [ %2 ]").arg(I18n::t("akp_toast_custom_key_added"), clean), "ok");
    }
}

void AutoPresserPage::onRemoveCustomKey(const QString& keyStr) {
    AutoPresserTab& tab = currentTab();
    for (int i = tab.keys.size() - 1; i >= 0; --i) {
        if (tab.keys[i].isCustom && tab.keys[i].key == keyStr) {
            tab.keys.removeAt(i);
        }
    }
    m_akp->saveWorkspace();
    rebuildKeysGrid();
}

void AutoPresserPage::setAllKeysEnabled(bool enabled) {
    AutoPresserTab& tab = currentTab();
    for (auto& k : tab.keys) k.enabled = enabled;
    m_akp->saveWorkspace();
    rebuildKeysGrid();
}

void AutoPresserPage::onMouseMasterToggled(bool val) {
    currentTab().mouseEnabled = val;
    m_akp->saveWorkspace();
}

void AutoPresserPage::onMouseModeChanged(const QString& modeKey) {
    currentTab().mouseMode = modeKey;
    m_akp->saveWorkspace();
}

void AutoPresserPage::onAddMousePoint() {
    AutoPresserTab& tab = currentTab();
    const QPoint pt = Win32Input::getCursorPos();
    AutoPresserMousePoint mp;
    mp.name = QStringLiteral("Click %1").arg(tab.mousePoints.size() + 1);
    mp.x = pt.x();
    mp.y = pt.y();
    mp.button = "left";
    mp.intervalMs = 1000;
    mp.enabled = true;
    tab.mousePoints.append(mp);
    tab.mouseEnabled = true;
    if (m_togMouseMaster) m_togMouseMaster->setChecked(true, false);
    m_akp->saveWorkspace();
    rebuildMousePointsList();
}

void AutoPresserPage::onRemoveMousePoint(int idx) {
    AutoPresserTab& tab = currentTab();
    if (idx >= 0 && idx < tab.mousePoints.size()) {
        tab.mousePoints.removeAt(idx);
        m_akp->saveWorkspace();
        rebuildMousePointsList();
    }
}

void AutoPresserPage::onPickMousePoint(int idx) {
    m_pickingMouseIdx = (m_pickingMouseIdx == idx) ? -1 : idx;
    rebuildMousePointsList();
}

void AutoPresserPage::onF8Captured(int x, int y) {
    if (m_subView != "mouse") return;
    AutoPresserTab& tab = currentTab();
    if (m_pickingMouseIdx >= 0 && m_pickingMouseIdx < tab.mousePoints.size()) {
        tab.mousePoints[m_pickingMouseIdx].x = x;
        tab.mousePoints[m_pickingMouseIdx].y = y;
        m_pickingMouseIdx = -1;
    } else {
        AutoPresserMousePoint mp;
        mp.name = QStringLiteral("Click %1").arg(tab.mousePoints.size() + 1);
        mp.x = x;
        mp.y = y;
        mp.button = "left";
        mp.intervalMs = 1000;
        mp.enabled = true;
        tab.mousePoints.append(mp);
        tab.mouseEnabled = true;
        if (m_togMouseMaster) m_togMouseMaster->setChecked(true, false);
    }
    m_akp->saveWorkspace();
    rebuildMousePointsList();
}

void AutoPresserPage::onToggleCurrentTab() {
    AutoPresserTab& tab = currentTab();
    const bool running = m_akp->toggleTab(tab);
    bool anyKeyArmed = false;
    for (const auto& k : tab.keys) { if (k.enabled) { anyKeyArmed = true; break; } }
    bool anyMouseArmed = false;
    if (tab.mouseEnabled) {
        for (const auto& m : tab.mousePoints) { if (m.enabled) { anyMouseArmed = true; break; } }
    }
    if (!running && !anyKeyArmed && !anyMouseArmed && m_state) {
        emit m_state->toastRequested(I18n::t("akp_log_no_keys"), "info");
    }
    syncStartButtonState();
    refreshTabsBar();
}

void AutoPresserPage::onStopAllTabs() {
    m_akp->stopAllTabs();
    syncStartButtonState();
    refreshTabsBar();
}

void AutoPresserPage::syncStartButtonState() {
    if (!m_btnStartTab) return;
    AutoPresserTab& tab = currentTab();
    const bool running = m_akp->isTabRunning(tab.tabId);
    m_btnStartTab->setText(I18n::t(running ? "akp_btn_stop_tab" : "akp_btn_start_tab"));
    m_btnStartTab->setVariant(running ? "danger" : "primary");
    m_btnStartTab->setIconName(running ? "stop" : "play");
}

void AutoPresserPage::tickUiStatus() {
    syncStartButtonState();
    if (m_subView == "log") {
        AutoPresserTab& tab = currentTab();
        if (computeLogSig(tab.tabId) != m_lastLogSig) {
            rebuildActivityLog();
        }
    }
}

void AutoPresserPage::onClearTabLog() {
    AutoPresserTab& tab = currentTab();
    m_akp->clearTabLogs(tab.tabId);
    rebuildActivityLog();
}

void AutoPresserPage::onSaveWorkspace() {
    const QString path = QFileDialog::getSaveFileName(this, I18n::t("akp_btn_save_workspace"), "V-AutoKeyPresser_Workspace.json", "JSON Workspace (*.json)");
    if (!path.isEmpty() && m_akp->saveWorkspace(path) && m_state) {
        emit m_state->toastRequested(I18n::t("toast_backup_exported"), "ok");
    }
}

void AutoPresserPage::onLoadWorkspace() {
    const QString path = QFileDialog::getOpenFileName(this, I18n::t("akp_btn_load_workspace"), QString(), "JSON Workspace (*.json)");
    if (!path.isEmpty() && m_akp->loadWorkspace(path)) {
        m_akp->saveWorkspace();
        refreshTabsBar();
        loadCurrentTabIntoUi();
        if (m_state) emit m_state->toastRequested(I18n::t("toast_backup_imported"), "ok");
    }
}

void AutoPresserPage::onSaveTabProfile() {
    AutoPresserTab& tab = currentTab();
    const QString path = QFileDialog::getSaveFileName(
        this,
        I18n::t("akp_btn_save_profile"),
        QStringLiteral("%1_Profile.json").arg(QString(tab.title).replace(' ', '_')),
        "JSON Tab Profile (*.json)"
    );
    if (!path.isEmpty()) {
        QFile f(path);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(tab.toJson()).toJson(QJsonDocument::Indented));
            if (m_state) emit m_state->toastRequested(I18n::t("toast_backup_exported"), "ok");
        }
    }
}

void AutoPresserPage::onLoadTabProfile() {
    const QString path = QFileDialog::getOpenFileName(this, I18n::t("akp_btn_load_profile"), QString(), "JSON Tab Profile (*.json)");
    if (!path.isEmpty()) {
        QFile f(path);
        if (f.open(QIODevice::ReadOnly)) {
            const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            if (doc.isObject() && m_akp->activeTabIndex >= 0 && m_akp->activeTabIndex < m_akp->tabs.size()) {
                m_akp->tabs[m_akp->activeTabIndex] = AutoPresserTab::fromJson(doc.object());
                m_akp->saveWorkspace();
                refreshTabsBar();
                loadCurrentTabIntoUi();
                if (m_state) emit m_state->toastRequested(I18n::t("toast_backup_imported"), "ok");
            }
        }
    }
}

} // namespace vmacro
