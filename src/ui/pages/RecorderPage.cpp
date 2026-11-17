#include "RecorderPage.h"
#include "../Widgets.h"
#include "../../core/AppState.h"
#include "../../core/I18n.h"
#include "../../core/Theme.h"
#include "../../engine/MacroEngine.h"
#include "../../platform/Win32Input.h"

#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QJsonDocument>
#include <QKeySequence>
#include <QMap>
#include <QMouseEvent>
#include <QShortcut>
#include <QSizePolicy>
#include <algorithm>
#include <cmath>

namespace vmacro {

static QString recCategoryColor(const QString& kind) {
    static const QMap<QString, QString> kColors = {
        {"key_tap", "#8B5CF6"},
        {"key_hold", "#8B5CF6"},
        {"key_down", "#7C3AED"},
        {"key_up", "#C084FC"},
        {"wait", "#F59E0B"},
        {"mouse_click", "#10B981"},
        {"mouse_down", "#059669"},
        {"mouse_up", "#34D399"},
        {"mouse_move", "#06B6D4"},
        {"mouse_scroll", "#EC4899"},
    };
    return kColors.value(kind, "#8B5CF6");
}

static QList<QPair<QString, QString>> recMouseButtonOptions() {
    return {
        {"left", I18n::t("mouse_btn_left")},
        {"right", I18n::t("mouse_btn_right")},
        {"middle", I18n::t("mouse_btn_middle")},
        {"x1", I18n::t("mouse_btn_x1")},
        {"x2", I18n::t("mouse_btn_x2")},
    };
}

static QList<QPair<QString, QString>> recScrollDirOptions() {
    return {
        {"down", I18n::t("scroll_down")},
        {"up", I18n::t("scroll_up")},
        {"left", I18n::t("scroll_left")},
        {"right", I18n::t("scroll_right")},
    };
}

// ============================================================================
// EditableEventRow
// ============================================================================

EditableEventRow::EditableEventRow(int index, MacroEvent* evPtr, AppState* state, QWidget* parent)
    : QWidget(parent), m_index(index), m_ev(evPtr), m_state(state) {
    const QString kind = m_ev ? m_ev->kind : QStringLiteral("key_tap");
    m_catHex = recCategoryColor(kind);

    m_card = new StepCard(m_index, m_catHex, this);
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(m_card);

    auto* lay = new QHBoxLayout(m_card);
    lay->setContentsMargins(10, 6, 10, 6);
    lay->setSpacing(6);

    m_grip = new DragGripHandle(m_card);
    connect(m_grip, &DragGripHandle::dragStarted, this, [this]() { m_card->setDragging(true); });
    connect(m_grip, &DragGripHandle::dragMoved, this, [this](int gy) { emit dragMoveRequested(this, gy); });
    connect(m_grip, &DragGripHandle::dragFinished, this, [this]() {
        m_card->setDragging(false);
        emit dragFinished();
    });
    lay->addWidget(m_grip);

    m_idxLbl = new QLabel(QStringLiteral("#%1").arg(I18n::fmtNum(QString::number(m_index + 1))), m_card);
    m_idxLbl->setFont(makeFont(12, QFont::ExtraBold));
    m_idxLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(m_catHex));
    lay->addWidget(m_idxLbl);

    const bool evEn = m_ev ? m_ev->enabled : true;
    m_card->setStepEnabled(evEn);

    m_btnEnableStep = new StepEnableToggleButton(evEn, m_card);
    connect(m_btnEnableStep, &StepEnableToggleButton::toggled, this, &EditableEventRow::onEvEnabledToggled);
    lay->addWidget(m_btnEnableStep);

    m_lblPaused = new QLabel(I18n::t("lbl_step_paused_badge"), m_card);
    m_lblPaused->setFont(makeFont(11, QFont::ExtraBold));
    m_lblPaused->setStyleSheet("color: #F59E0B; background: transparent;");
    m_lblPaused->setVisible(!evEn);
    lay->addWidget(m_lblPaused);

    QMap<QString, QString> kindLabels = {
        {"key_tap", I18n::t("step_row_key_tap")},
        {"key_hold", I18n::t("step_row_key_tap")},
        {"key_down", I18n::t("step_row_key_down")},
        {"key_up", I18n::t("step_row_key_up")},
        {"wait", I18n::t("step_row_wait")},
        {"mouse_click", I18n::t("step_row_mouse_click")},
        {"mouse_down", I18n::t("step_row_mouse_down")},
        {"mouse_up", I18n::t("step_row_mouse_up")},
        {"mouse_move", I18n::t("step_row_mouse_move")},
        {"mouse_scroll", I18n::t("lbl_scroll_dir")},
    };
    auto* kLbl = new QLabel(kindLabels.value(kind, kind), m_card);
    kLbl->setFont(makeFont(12, QFont::Bold));
    kLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    lay->addWidget(kLbl);

    if (kind == "key_tap" || kind == "key_hold" || kind == "key_down" || kind == "key_up") {
        m_keyBtn = new KeyCaptureButton(
            m_ev ? m_ev->key : "Space",
            m_ev ? m_ev->scanCode : 0,
            m_ev ? m_ev->isExtended : false,
            {},
            false,
            m_state,
            false,
            m_card
        );
        m_keyBtn->setFixedSize(112, 32);
        connect(m_keyBtn, &KeyCaptureButton::scanKeyCaptured, this, &EditableEventRow::onKeyEdited);
        lay->addWidget(m_keyBtn);

        if (kind == "key_tap" || kind == "key_hold") {
            auto* lblHold = new QLabel(I18n::t("lbl_hold_duration"), m_card);
            lblHold->setFont(makeFont(11, QFont::Bold));
            lblHold->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
            const int defaultHold = m_ev ? m_ev->holdMs : 0;
            m_numHold = new NumberInput(defaultHold, 0, 60000, 100, "ms", 108, m_card);
            m_numHold->setFixedSize(108, 32);
            connect(m_numHold, &NumberInput::valueChanged, this, [this](int v) {
                if (m_ev) m_ev->holdMs = v;
            });
            lay->addWidget(lblHold);
            lay->addWidget(m_numHold);
        }
    } else if (kind == "mouse_scroll") {
        QString sDir = m_ev ? m_ev->scrollDir : QString();
        if (sDir.isEmpty() && m_ev) {
            if (m_ev->dy != 0) sDir = m_ev->dy > 0 ? "up" : "down";
            else if (m_ev->dx != 0) sDir = m_ev->dx > 0 ? "right" : "left";
            else sDir = "down";
            m_ev->scrollDir = sDir;
        }
        m_scrollDirCombo = new SearchCombo(recScrollDirOptions(), sDir.isEmpty() ? "down" : sDir, m_card);
        m_scrollDirCombo->setFixedSize(118, 32);
        connect(m_scrollDirCombo, &SearchCombo::currentChanged, this, [this](const QString& d) {
            if (m_ev) m_ev->scrollDir = d;
        });
        lay->addWidget(m_scrollDirCombo);

        auto* lblAmt = new QLabel(I18n::t("lbl_scroll_amount"), m_card);
        lblAmt->setFont(makeFont(12, QFont::Bold));
        lblAmt->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
        int amtVal = m_ev ? m_ev->scrollAmount : 3;
        if (amtVal <= 0 && m_ev) {
            amtVal = std::max(1, std::abs(m_ev->dy) > 0 ? std::abs(m_ev->dy) : (std::abs(m_ev->dx) > 0 ? std::abs(m_ev->dx) : 3));
            m_ev->scrollAmount = amtVal;
        }
        m_numScrollAmt = new NumberInput(amtVal, 1, 100, 1, QString(), 98, m_card);
        m_numScrollAmt->setFixedSize(98, 32);
        connect(m_numScrollAmt, &NumberInput::valueChanged, this, [this](int v) {
            if (m_ev) m_ev->scrollAmount = std::max(1, v);
        });
        lay->addWidget(lblAmt);
        lay->addWidget(m_numScrollAmt);
    } else if (kind.contains("mouse")) {
        if (kind == "mouse_click" || kind == "mouse_down" || kind == "mouse_up") {
            m_btnCombo = new SearchCombo(recMouseButtonOptions(), m_ev ? m_ev->button : "left", m_card);
            m_btnCombo->setFixedSize(122, 32);
            connect(m_btnCombo, &SearchCombo::currentChanged, this, [this](const QString& b) {
                if (m_ev) m_ev->button = b;
            });
            lay->addWidget(m_btnCombo);
        }

        auto* xLbl = new QLabel("X:", m_card);
        xLbl->setFont(makeFont(11, QFont::Bold));
        xLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
        m_numX = new NumberInput(m_ev ? m_ev->x : 500, -32768, 32768, 10, QString(), 88, m_card);
        m_numX->setFixedSize(88, 32);
        connect(m_numX, &NumberInput::valueChanged, this, [this](int v) {
            if (m_ev) m_ev->x = v;
        });

        auto* yLbl = new QLabel("Y:", m_card);
        yLbl->setFont(makeFont(11, QFont::Bold));
        yLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
        m_numY = new NumberInput(m_ev ? m_ev->y : 500, -32768, 32768, 10, QString(), 88, m_card);
        m_numY->setFixedSize(88, 32);
        connect(m_numY, &NumberInput::valueChanged, this, [this](int v) {
            if (m_ev) m_ev->y = v;
        });

        m_btnPickXy = new Button(I18n::t("btn_pick_live_xy"), "glass", "crosshair", "#10B981", false, m_card);
        m_btnPickXy->setFixedHeight(30);
        connect(m_btnPickXy, &Button::clicked, this, &EditableEventRow::toggleLiveXyPicker);

        m_lblLiveXy = new QLabel(m_card);
        m_lblLiveXy->setFont(makeFont(11, QFont::Bold));
        m_lblLiveXy->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("ok")));

        lay->addWidget(xLbl);
        lay->addWidget(m_numX);
        lay->addWidget(yLbl);
        lay->addWidget(m_numY);
        lay->addWidget(m_btnPickXy);
        lay->addWidget(m_lblLiveXy);

        if (kind == "mouse_click") {
            auto* lblHold = new QLabel(I18n::t("lbl_hold_duration"), m_card);
            lblHold->setFont(makeFont(11, QFont::Bold));
            lblHold->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
            m_numHold = new NumberInput(m_ev ? m_ev->holdMs : 0, 0, 60000, 100, "ms", 102, m_card);
            m_numHold->setFixedSize(102, 32);
            connect(m_numHold, &NumberInput::valueChanged, this, [this](int v) {
                if (m_ev) m_ev->holdMs = v;
            });
            lay->addWidget(lblHold);
            lay->addWidget(m_numHold);
        }

        m_xyTimer = new QTimer(this);
        m_xyTimer->setInterval(50);
        connect(m_xyTimer, &QTimer::timeout, this, &EditableEventRow::tickLiveXy);
        connect(m_numX, &NumberInput::focused, this, &EditableEventRow::startLiveXyPreview);
        connect(m_numY, &NumberInput::focused, this, &EditableEventRow::startLiveXyPreview);
        if (m_state) {
            connect(m_state, &AppState::cursorPickHotkeyPressed, this, &EditableEventRow::onF8Picked);
        }
    }

    lay->addStretch(1);

    auto* dLbl = new QLabel(I18n::t("lbl_pre_delay"), m_card);
    dLbl->setFont(makeFont(11, QFont::Medium));
    dLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    lay->addWidget(dLbl);

    m_numDelay = new NumberInput(m_ev ? m_ev->delayMs : 40, 1, 600000, 25, "ms", 112, m_card);
    m_numDelay->setFixedSize(112, 32);
    connect(m_numDelay, &NumberInput::valueChanged, this, [this](int v) {
        if (m_ev) m_ev->delayMs = v;
    });
    lay->addWidget(m_numDelay);

    m_btnNote = new StepNoteIconButton(m_ev ? m_ev->note : QString(), m_card);
    connect(m_btnNote, &StepNoteIconButton::noteChanged, this, [this](const QString& txt) {
        if (m_ev) m_ev->note = txt;
    });
    lay->addWidget(m_btnNote);

    auto* btnDel = new IconButton("trash", I18n::t("btn_delete_rec"), 28, "danger", m_card);
    connect(btnDel, &IconButton::clicked, this, [this]() { emit deleteClicked(m_index); });
    lay->addWidget(btnDel);
}

void EditableEventRow::setStepIndex(int newIdx) {
    m_index = newIdx;
    m_idxLbl->setText(QStringLiteral("#%1").arg(I18n::fmtNum(QString::number(newIdx + 1))));
    m_card->setIndex(newIdx);
}

void EditableEventRow::onEvEnabledToggled(bool enabled) {
    if (m_ev) m_ev->enabled = enabled;
    m_card->setStepEnabled(enabled);
    m_lblPaused->setVisible(!enabled);
}

void EditableEventRow::onKeyEdited(int sc, bool ext, const QString& lbl) {
    if (!m_ev) return;
    m_ev->key = lbl;
    m_ev->scanCode = sc;
    m_ev->isExtended = ext;
}

void EditableEventRow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_rowDragging = true;
        setCursor(Qt::ClosedHandCursor);
        m_card->setDragging(true);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void EditableEventRow::mouseMoveEvent(QMouseEvent* event) {
    if (m_rowDragging) {
        emit dragMoveRequested(this, static_cast<int>(event->globalPosition().y()));
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void EditableEventRow::mouseReleaseEvent(QMouseEvent* event) {
    if (m_rowDragging) {
        m_rowDragging = false;
        setCursor(Qt::ArrowCursor);
        m_card->setDragging(false);
        emit dragFinished();
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void EditableEventRow::startLiveXyPreview() {
    m_pickingLiveXy = true;
    if (m_xyTimer && !m_xyTimer->isActive()) m_xyTimer->start();
}

void EditableEventRow::toggleLiveXyPicker() {
    m_pickingLiveXy = !m_pickingLiveXy;
    if (m_pickingLiveXy) {
        if (m_xyTimer) m_xyTimer->start();
        if (m_btnPickXy) m_btnPickXy->setText(I18n::t("btn_press_f8"));
    } else {
        if (m_xyTimer) m_xyTimer->stop();
        if (m_btnPickXy) m_btnPickXy->setText(I18n::t("btn_pick_live_xy"));
        if (m_lblLiveXy) m_lblLiveXy->clear();
    }
}

void EditableEventRow::tickLiveXy() {
    const QPoint pt = Win32Input::getCursorPos();
    if (m_lblLiveXy) {
        m_lblLiveXy->setText(I18n::ltrValue(QStringLiteral("(%1, %2)").arg(pt.x()).arg(pt.y())));
    }
}

void EditableEventRow::onF8Picked(int cx, int cy) {
    if (!m_pickingLiveXy) return;
    if (m_numX) m_numX->setValue(cx, true);
    if (m_numY) m_numY->setValue(cy, true);
    m_pickingLiveXy = false;
    if (m_xyTimer) m_xyTimer->stop();
    if (m_btnPickXy) m_btnPickXy->setText(I18n::t("btn_pick_live_xy"));
    if (m_lblLiveXy) m_lblLiveXy->setText(I18n::ltrValue(QStringLiteral("✓ (%1,%2)").arg(cx).arg(cy)));
}

// ============================================================================
// RecorderPage
// ============================================================================

RecorderPage::RecorderPage(AppState* state, MacroEngine* engine, QWidget* parent)
    : QWidget(parent), m_state(state), m_engine(engine) {
    m_sidebarCollapsed = m_state ? m_state->recorderSidebarCollapsed : false;
    buildUi();

    auto* scUndo = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(scUndo, &QShortcut::activated, this, &RecorderPage::undoSteps);

    auto* scRedo = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(scRedo, &QShortcut::activated, this, &RecorderPage::redoSteps);

    connectSignals();
    if (m_state) {
        m_state->registerStopButtonRectProvider([this]() { return getStopBtnScreenRect(); });
    }
    refreshTimeline();
    refreshSavedLibrary();
}

QRect RecorderPage::getStopBtnScreenRect() const {
    if (!m_btnRec || !m_btnRec->isVisible()) return QRect();
    const QPoint tlPt = m_btnRec->mapToGlobal(QPoint(0, 0));
    return QRect(tlPt.x(), tlPt.y(), m_btnRec->width(), m_btnRec->height());
}

void RecorderPage::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 8, 16, 12);
    root->setSpacing(10);

    // Top Card
    auto* topCard = new GlassCard(true, this);
    auto* tLay = new QHBoxLayout(topCard);
    tLay->setContentsMargins(18, 10, 18, 10);
    tLay->setSpacing(10);

    auto* tCol = new QVBoxLayout();
    tCol->setSpacing(2);
    auto* title = new QLabel(I18n::t("rec_title"), topCard);
    title->setFont(makeFont(16, QFont::ExtraBold));
    title->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    auto* sub = new QLabel(I18n::t("rec_subtitle"), topCard);
    sub->setFont(makeFont(12, QFont::Medium));
    sub->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    tCol->addWidget(title);
    tCol->addWidget(sub);
    tLay->addLayout(tCol, 1);

    const QString hkRec = (m_state && !m_state->config.hotkeyRecordToggle.isEmpty()) ? m_state->config.hotkeyRecordToggle : QStringLiteral("F9");
    m_btnRec = new Button(QStringLiteral("%1 (%2)").arg(I18n::t("btn_start_rec"), hkRec), "danger", "recorder", 38, topCard);
    connect(m_btnRec, &Button::clicked, this, [this]() {
        if (m_engine) m_engine->toggleRecording(true);
    });

    const QString hkPlay = (m_state && !m_state->config.hotkeyPlaybackToggle.isEmpty()) ? m_state->config.hotkeyPlaybackToggle : QStringLiteral("F10");
    m_btnPlay = new Button(QStringLiteral("%1 (%2)").arg(I18n::t("btn_play_macro"), hkPlay), "primary", "play", 38, topCard);
    connect(m_btnPlay, &Button::clicked, this, &RecorderPage::onPlayClicked);

    m_btnToCombo = new Button(I18n::t("btn_convert_to_combo"), "glass", "bolt", "#8B5CF6", false, topCard);
    connect(m_btnToCombo, &Button::clicked, this, &RecorderPage::convertCurrentToCombo);

    m_btnClear = new Button(I18n::t("btn_clear_events"), "ghost", "trash", 38, topCard);
    connect(m_btnClear, &Button::clicked, this, &RecorderPage::clearEvents);

    m_btnToggleSidebar = new Button(
        I18n::t(m_sidebarCollapsed ? "btn_show_macros_sidebar" : "btn_hide_macros_sidebar"),
        "glass",
        "dashboard",
        Theme::instance().tokenHex("accent"),
        false,
        topCard
    );
    connect(m_btnToggleSidebar, &Button::clicked, this, &RecorderPage::toggleRecSidebar);

    tLay->addWidget(m_btnRec);
    tLay->addWidget(m_btnPlay);
    tLay->addWidget(m_btnToCombo);
    tLay->addWidget(m_btnClear);
    tLay->addWidget(m_btnToggleSidebar);
    root->addWidget(topCard);

    auto* body = new QHBoxLayout();
    body->setSpacing(12);

    // Left Library Card
    m_libCard = new GlassCard(false, this);
    m_libCard->setFixedWidth(315);
    m_libCard->setVisible(!m_sidebarCollapsed);
    auto* lLay = new QVBoxLayout(m_libCard);
    lLay->setContentsMargins(12, 12, 12, 12);
    lLay->setSpacing(8);

    auto* libHdr = new QLabel(I18n::t("saved_recordings_header"), m_libCard);
    libHdr->setFont(makeFont(14, QFont::ExtraBold));
    libHdr->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    lLay->addWidget(libHdr);

    auto* saveRow = new QHBoxLayout();
    saveRow->setSpacing(6);
    m_nameEdit = new QLineEdit(m_state ? m_state->currentMacroName : "W -> Wait 1s -> Space", m_libCard);
    m_nameEdit->setPlaceholderText(I18n::t("ph_macro_name"));
    m_nameEdit->setFixedHeight(34);
    m_nameEdit->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 9px;
            padding: 4px 10px;
            font-family: '%4';
            font-size: 13px;
            font-weight: 600;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border_hi"),
            I18n::langFontFamily()));

    auto* btnSave = new Button(I18n::t("btn_save_macro"), "primary", "save", QString(), true, m_libCard);
    btnSave->setFixedHeight(34);
    connect(btnSave, &Button::clicked, this, [this]() {
        if (m_engine && m_nameEdit) {
            m_engine->saveCurrentMacroToLibrary(m_nameEdit->text());
        }
    });
    saveRow->addWidget(m_nameEdit, 1);
    saveRow->addWidget(btnSave, 1);
    lLay->addLayout(saveRow);

    auto* libScroll = new QScrollArea(m_libCard);
    libScroll->setWidgetResizable(true);
    libScroll->setFrameShape(QFrame::NoFrame);
    libScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    libScroll->setStyleSheet(modernScrollStyle());

    m_libContainer = new QWidget();
    m_libContainer->setStyleSheet("background: transparent;");
    m_libVbox = new QVBoxLayout(m_libContainer);
    m_libVbox->setContentsMargins(0, 0, 4, 0);
    m_libVbox->setSpacing(8);
    libScroll->setWidget(m_libContainer);
    lLay->addWidget(libScroll, 1);

    auto* impRow = new QHBoxLayout();
    impRow->setSpacing(6);
    auto* btnExpAll = new Button(I18n::t("btn_export_all"), "glass", "save", QString(), true, m_libCard);
    btnExpAll->setFixedHeight(32);
    connect(btnExpAll, &Button::clicked, this, &RecorderPage::exportFullBackup);

    auto* btnImpAll = new Button(I18n::t("btn_import_all"), "ghost", "folder", QString(), true, m_libCard);
    btnImpAll->setFixedHeight(32);
    connect(btnImpAll, &Button::clicked, this, &RecorderPage::importJsonOrBackup);

    m_btnUndoLib = new IconButton("chevron_up", I18n::t("btn_undo_delete"), 32, "ghost", m_libCard);
    connect(m_btnUndoLib, &IconButton::clicked, this, &RecorderPage::undoDeleteMacro);
    m_btnUndoLib->hide();

    impRow->addWidget(btnExpAll, 1);
    impRow->addWidget(btnImpAll, 1);
    impRow->addWidget(m_btnUndoLib);
    lLay->addLayout(impRow);

    body->addWidget(m_libCard);

    // Right Timeline Card
    auto* tlCard = new GlassCard(false, this);
    auto* rLay = new QVBoxLayout(tlCard);
    rLay->setContentsMargins(16, 10, 16, 10);
    rLay->setSpacing(7);

    // Row 1: Capture Filters + Repeat Speed
    auto* fltBar = new QHBoxLayout();
    fltBar->setSpacing(8);

    auto* lblKb = new QLabel(I18n::t("lbl_rec_keyboard"), tlCard);
    lblKb->setFont(makeFont(12, QFont::Bold));
    lblKb->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    auto* togKb = new ToggleSwitch(m_state ? m_state->config.recordKeyboard : true, tlCard);
    connect(togKb, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("record_keyboard", v);
    });

    auto* lblMc = new QLabel(I18n::t("lbl_rec_clicks"), tlCard);
    lblMc->setFont(makeFont(12, QFont::Bold));
    lblMc->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    auto* togMc = new ToggleSwitch(m_state ? m_state->config.recordMouseClicks : true, tlCard);
    connect(togMc, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("record_mouse_clicks", v);
    });

    auto* lblMs = new QLabel(I18n::t("lbl_rec_scroll"), tlCard);
    lblMs->setFont(makeFont(12, QFont::Bold));
    lblMs->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    auto* togMs = new ToggleSwitch(m_state ? m_state->config.recordMouseScroll : true, tlCard);
    connect(togMs, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("record_mouse_scroll", v);
    });

    auto* lblMm = new QLabel(I18n::t("lbl_rec_moves"), tlCard);
    lblMm->setFont(makeFont(12, QFont::Bold));
    lblMm->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    auto* togMm = new ToggleSwitch(m_state ? m_state->config.recordMouseMoves : false, tlCard);
    connect(togMm, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("record_mouse_moves", v);
    });

    auto* lblSpd = new QLabel(I18n::t("lbl_repeat_speed"), tlCard);
    lblSpd->setFont(makeFont(12, QFont::ExtraBold));
    lblSpd->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    m_numSpeed = new NumberInput(m_state ? m_state->config.defaultPlaybackSpeed : 1.0, 0.25, 10.0, 0.25, 2, "x", tlCard);
    m_numSpeed->setFixedSize(118, 32);
    connect(m_numSpeed, &NumberInput::doubleValueChanged, this, [this](double v) {
        if (m_state) m_state->updateSetting("default_playback_speed", v);
    });

    fltBar->addWidget(lblKb);
    fltBar->addWidget(togKb);
    fltBar->addWidget(lblMc);
    fltBar->addWidget(togMc);
    fltBar->addWidget(lblMs);
    fltBar->addWidget(togMs);
    fltBar->addWidget(lblMm);
    fltBar->addWidget(togMm);
    fltBar->addStretch(1);
    fltBar->addWidget(lblSpd);
    fltBar->addWidget(m_numSpeed);
    rLay->addLayout(fltBar);

    // Row 2: Repeat Settings Panel (Fixed vertical size policy so hiding steps NEVER stretches it!)
    auto* repPanel = new GlassCard(false, tlCard);
    repPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* rpLay = new QVBoxLayout(repPanel);
    rpLay->setContentsMargins(14, 8, 14, 8);
    rpLay->setSpacing(6);

    auto* modeRow = new QHBoxLayout();
    modeRow->setSpacing(10);

    m_repModeSeg = new SegmentedControl(
        {
            {"once", I18n::t("rec_repeat_mode_once")},
            {"count", I18n::t("rec_repeat_mode_count")},
            {"duration", I18n::t("rec_repeat_mode_duration")},
        },
        m_state ? m_state->config.repeatMode : "once",
        repPanel
    );
    m_repModeSeg->setFixedHeight(34);
    connect(m_repModeSeg, &SegmentedControl::changed, this, &RecorderPage::onRepeatModeChanged);
    modeRow->addWidget(m_repModeSeg, 1);

    m_numRepeatCount = new NumberInput(m_state ? m_state->config.defaultRepeatCount : 5, 0, 99999, 1, I18n::t("unit_times"), 130, repPanel);
    m_numRepeatCount->setFixedSize(130, 32);
    connect(m_numRepeatCount, &NumberInput::valueChanged, this, [this](int v) {
        if (m_state) m_state->updateSetting("default_repeat_count", v);
    });
    modeRow->addWidget(m_numRepeatCount);

    m_numDurationVal = new NumberInput(m_state ? m_state->config.repeatDurationValue : 10, 1, 9999, 1, QString(), 104, repPanel);
    m_numDurationVal->setFixedSize(104, 32);
    connect(m_numDurationVal, &NumberInput::valueChanged, this, [this](int v) {
        if (m_state) m_state->updateSetting("repeat_duration_value", v);
    });

    m_comboDurationUnit = new SearchCombo(
        {
            {"seconds", I18n::t("unit_seconds")},
            {"minutes", I18n::t("unit_minutes")},
            {"hours", I18n::t("unit_hours")},
        },
        m_state ? m_state->config.repeatDurationUnit : "minutes",
        repPanel
    );
    m_comboDurationUnit->setFixedSize(124, 32);
    connect(m_comboDurationUnit, &SearchCombo::currentChanged, this, [this](const QString& u) {
        if (m_state) m_state->updateSetting("repeat_duration_unit", u);
    });

    modeRow->addWidget(m_numDurationVal);
    modeRow->addWidget(m_comboDurationUnit);
    rpLay->addLayout(modeRow);

    auto* optsGrid = new QGridLayout();
    optsGrid->setContentsMargins(0, 4, 0, 2);
    optsGrid->setHorizontalSpacing(18);
    optsGrid->setVerticalSpacing(8);

    // Cell (0, 0)
    auto* c00 = new QHBoxLayout();
    c00->setSpacing(8);
    m_togDelayBetween = new ToggleSwitch(m_state ? m_state->config.repeatDelayEnabled : false, repPanel);
    connect(m_togDelayBetween, &ToggleSwitch::toggled, this, &RecorderPage::onDelayBetweenToggled);
    auto* lblSetDelay = new QLabel(I18n::t("lbl_set_delay_between"), repPanel);
    lblSetDelay->setFont(makeFont(12, QFont::Bold));
    lblSetDelay->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    m_numDelayBetween = new NumberInput(m_state ? m_state->config.repeatDelayMs : 1000, 10, 3600000, 250, "ms", 124, repPanel);
    m_numDelayBetween->setFixedSize(124, 30);
    connect(m_numDelayBetween, &NumberInput::valueChanged, this, [this](int v) {
        if (m_state) m_state->updateSetting("repeat_delay_ms", v);
    });
    c00->addWidget(m_togDelayBetween);
    c00->addWidget(lblSetDelay);
    c00->addWidget(m_numDelayBetween);
    c00->addStretch(1);

    // Cell (0, 1)
    auto* c01 = new QHBoxLayout();
    c01->setSpacing(8);
    m_togDelayRandom = new ToggleSwitch(m_state ? m_state->config.repeatDelayRandom : false, repPanel);
    connect(m_togDelayRandom, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("repeat_delay_random", v);
    });
    auto* lblRand = new QLabel(I18n::t("lbl_delay_random"), repPanel);
    lblRand->setFont(makeFont(12, QFont::DemiBold));
    lblRand->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    c01->addWidget(m_togDelayRandom);
    c01->addWidget(lblRand);
    c01->addStretch(1);

    // Cell (1, 0)
    auto* c10 = new QHBoxLayout();
    c10->setSpacing(8);
    m_togNoActivate = new ToggleSwitch(m_state ? m_state->config.doNotActivateOnStop : true, repPanel);
    connect(m_togNoActivate, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("do_not_activate_on_stop", v);
    });
    auto* lblNoAct = new QLabel(I18n::t("lbl_do_not_activate"), repPanel);
    lblNoAct->setFont(makeFont(12, QFont::Bold));
    lblNoAct->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    c10->addWidget(m_togNoActivate);
    c10->addWidget(lblNoAct);
    c10->addStretch(1);

    // Cell (1, 1)
    auto* c11 = new QHBoxLayout();
    c11->setSpacing(8);
    m_togShutdown = new ToggleSwitch(m_state ? m_state->config.shutdownOnFinish : false, repPanel);
    connect(m_togShutdown, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("shutdown_on_finish", v);
    });
    auto* lblShut = new QLabel(I18n::t("lbl_shutdown_when_finished"), repPanel);
    lblShut->setFont(makeFont(12, QFont::Bold));
    lblShut->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("danger")));
    c11->addWidget(m_togShutdown);
    c11->addWidget(lblShut);
    c11->addStretch(1);

    // Cell (2, 0)
    auto* c20 = new QHBoxLayout();
    c20->setSpacing(8);
    auto* lblPdelay = new QLabel(I18n::t("lbl_playback_start_delay"), repPanel);
    lblPdelay->setFont(makeFont(12, QFont::Bold));
    lblPdelay->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    m_numPlayStartDelay = new NumberInput(m_state ? m_state->config.playbackStartDelayMs : 0, 0, 60000, 500, "ms", 124, repPanel);
    m_numPlayStartDelay->setFixedSize(124, 30);
    connect(m_numPlayStartDelay, &NumberInput::valueChanged, this, [this](int v) {
        if (m_state) m_state->updateSetting("playback_start_delay_ms", v);
    });
    c20->addWidget(lblPdelay);
    c20->addWidget(m_numPlayStartDelay);
    c20->addStretch(1);

    // Cell (2, 1)
    auto* c21 = new QHBoxLayout();
    c21->setSpacing(8);
    m_togFloatingPill = new ToggleSwitch(m_state ? m_state->config.showFloatingPlaybackPill : false, repPanel);
    connect(m_togFloatingPill, &ToggleSwitch::toggled, this, [this](bool v) {
        if (m_state) m_state->updateSetting("show_floating_playback_pill", v);
    });
    auto* lblFpill = new QLabel(I18n::t("lbl_show_floating_pill"), repPanel);
    lblFpill->setFont(makeFont(12, QFont::Bold));
    lblFpill->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    c21->addWidget(m_togFloatingPill);
    c21->addWidget(lblFpill);
    c21->addStretch(1);

    optsGrid->addLayout(c00, 0, 0);
    optsGrid->addLayout(c01, 0, 1);
    optsGrid->addLayout(c10, 1, 0);
    optsGrid->addLayout(c11, 1, 1);
    optsGrid->addLayout(c20, 2, 0);
    optsGrid->addLayout(c21, 2, 1);
    optsGrid->setColumnStretch(0, 1);
    optsGrid->setColumnStretch(1, 1);
    rpLay->addLayout(optsGrid);

    rLay->addWidget(repPanel);
    syncRepeatModeInputs();

    // Live Recording Banner
    m_lblLiveRecBanner = new QLabel(tlCard);
    m_lblLiveRecBanner->setFont(makeFont(15, QFont::ExtraBold));
    m_lblLiveRecBanner->setAlignment(Qt::AlignCenter);
    m_lblLiveRecBanner->setStyleSheet(QStringLiteral("color: %1; background: %2; border-radius: 12px; padding: 24px;")
        .arg(Theme::instance().tokenHex("danger"), Theme::instance().tokenHex("bg1")));
    m_lblLiveRecBanner->hide();
    rLay->addWidget(m_lblLiveRecBanner);

    // Steps Hidden Summary Holder (takes stretch=1 when steps are hidden so top panels NEVER stretch!)
    m_stepsHiddenHolder = new QWidget(tlCard);
    m_stepsHiddenHolder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stepsHiddenHolder->setStyleSheet("background: transparent;");
    auto* shOuter = new QVBoxLayout(m_stepsHiddenHolder);
    shOuter->setContentsMargins(0, 0, 0, 0);
    shOuter->setSpacing(0);

    m_stepsHiddenCard = new GlassCard(false, m_stepsHiddenHolder);
    m_stepsHiddenCard->setFixedHeight(62);
    auto* shLay = new QHBoxLayout(m_stepsHiddenCard);
    shLay->setContentsMargins(16, 10, 16, 10);
    shLay->setSpacing(12);

    m_lblStepsHidden = new QLabel(I18n::t("lbl_steps_hidden_hint"), m_stepsHiddenCard);
    m_lblStepsHidden->setWordWrap(true);
    m_lblStepsHidden->setFont(makeFont(12, QFont::DemiBold));
    m_lblStepsHidden->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));

    m_btnOpenStepsEditor = new Button(I18n::t("btn_edit_steps"), "primary", "bolt", 34, m_stepsHiddenCard);
    connect(m_btnOpenStepsEditor, &Button::clicked, this, [this]() { setStepsEditorVisible(true); });

    shLay->addWidget(m_lblStepsHidden, 1);
    shLay->addWidget(m_btnOpenStepsEditor);
    shOuter->addWidget(m_stepsHiddenCard);
    shOuter->addStretch(1);
    rLay->addWidget(m_stepsHiddenHolder, 1);

    // Edit & History Toolbar (shown only when editing steps)
    m_recEditToolbar = new GlassCard(true, tlCard);
    m_recEditToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* retLay = new QHBoxLayout(m_recEditToolbar);
    retLay->setContentsMargins(12, 6, 12, 6);
    retLay->setSpacing(6);

    m_lblEditingRecBadge = new QLabel(m_recEditToolbar);
    m_lblEditingRecBadge->setFont(makeFont(12, QFont::ExtraBold));
    m_lblEditingRecBadge->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_btnRecUndo = new Button(I18n::t("btn_undo"), "ghost", "chevron_up", 30, m_recEditToolbar);
    connect(m_btnRecUndo, &Button::clicked, this, &RecorderPage::undoSteps);

    m_btnRecRedo = new Button(I18n::t("btn_redo"), "ghost", "chevron_down", 30, m_recEditToolbar);
    connect(m_btnRecRedo, &Button::clicked, this, &RecorderPage::redoSteps);

    m_btnRecSaveCurr = new Button(I18n::t("btn_save_changes"), "primary", "save", 30, m_recEditToolbar);
    connect(m_btnRecSaveCurr, &Button::clicked, this, &RecorderPage::saveEditsToCurrentMacro);

    m_btnRecSaveNew = new Button(I18n::t("btn_save_as_new"), "glass", "plus", "#10B981", false, m_recEditToolbar);
    m_btnRecSaveNew->setFixedHeight(30);
    connect(m_btnRecSaveNew, &Button::clicked, this, &RecorderPage::saveEditsAsNewMacro);

    m_btnRecHideSteps = new Button(I18n::t("btn_hide_steps"), "ghost", "close", 30, m_recEditToolbar);
    connect(m_btnRecHideSteps, &Button::clicked, this, [this]() { setStepsEditorVisible(false); });

    retLay->addWidget(m_lblEditingRecBadge, 1);
    retLay->addWidget(m_btnRecUndo);
    retLay->addWidget(m_btnRecRedo);
    retLay->addWidget(m_btnRecSaveCurr);
    retLay->addWidget(m_btnRecSaveNew);
    retLay->addWidget(m_btnRecHideSteps);
    rLay->addWidget(m_recEditToolbar);

    // Timeline ScrollArea
    m_scroll = new QScrollArea(tlCard);
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scroll->setStyleSheet(modernScrollStyle());

    m_tlContainer = new QWidget();
    m_tlContainer->setStyleSheet("background: transparent;");
    m_tlVbox = new QVBoxLayout(m_tlContainer);
    m_tlVbox->setContentsMargins(0, 0, 6, 0);
    m_tlVbox->setSpacing(6);
    m_scroll->setWidget(m_tlContainer);
    rLay->addWidget(m_scroll, 1);

    // Bottom Responsive Adder Bar (4 step types)
    auto* bAddK = new Button(I18n::t("step_add_key_tap"), "glass", "keyboard", "#8B5CF6", false, tlCard);
    connect(bAddK, &Button::clicked, this, [this]() { insertStep("key_tap"); });

    auto* bAddW = new Button(I18n::t("step_add_wait"), "glass", "clock", "#F59E0B", false, tlCard);
    connect(bAddW, &Button::clicked, this, [this]() { insertStep("wait"); });

    auto* bAddC = new Button(I18n::t("step_add_mouse_click"), "glass", "mouse", "#10B981", false, tlCard);
    connect(bAddC, &Button::clicked, this, [this]() { insertStep("mouse_click"); });

    auto* bAddS = new Button(I18n::t("step_add_mouse_scroll"), "glass", "mouse", "#EC4899", false, tlCard);
    connect(bAddS, &Button::clicked, this, [this]() { insertStep("mouse_scroll"); });

    m_recAdderBar = new ResponsiveFlowBar({bAddK, bAddW, bAddC, bAddS}, 32, 6, 6, tlCard);
    rLay->addWidget(m_recAdderBar);

    setStepsEditorVisible(false);

    body->addWidget(tlCard, 1);
    root->addLayout(body, 1);
}

void RecorderPage::connectSignals() {
    if (!m_state) return;
    connect(m_state, &AppState::eventsUpdated, this, &RecorderPage::refreshTimeline);
    connect(m_state, &AppState::recordingCountChanged, this, &RecorderPage::onLiveRecordingCount);
    connect(m_state, &AppState::macrosLibraryUpdated, this, &RecorderPage::refreshSavedLibrary);
    connect(m_state, &AppState::recordingStateChanged, this, [this](bool) { syncTransportButtons(); });
    connect(m_state, &AppState::playbackStateChanged, this, [this](bool) { syncTransportButtons(); });
    connect(m_state, &AppState::configChanged, this, &RecorderPage::syncTransportButtons);
}

void RecorderPage::toggleRecSidebar() {
    m_sidebarCollapsed = !m_sidebarCollapsed;
    if (m_state) m_state->recorderSidebarCollapsed = m_sidebarCollapsed;
    if (m_libCard) m_libCard->setVisible(!m_sidebarCollapsed);
    if (m_btnToggleSidebar) {
        m_btnToggleSidebar->setText(I18n::t(m_sidebarCollapsed ? "btn_show_macros_sidebar" : "btn_hide_macros_sidebar"));
    }
}

void RecorderPage::onRepeatModeChanged(const QString& mode) {
    if (m_state) m_state->updateSetting("repeat_mode", mode);
    syncRepeatModeInputs();
}

void RecorderPage::onDelayBetweenToggled(bool val) {
    if (m_state) m_state->updateSetting("repeat_delay_enabled", val);
    if (m_numDelayBetween) m_numDelayBetween->setEnabled(val);
    if (m_togDelayRandom) m_togDelayRandom->setEnabled(val);
}

void RecorderPage::syncRepeatModeInputs() {
    const QString mode = m_state ? m_state->config.repeatMode : QStringLiteral("once");
    if (m_numRepeatCount) m_numRepeatCount->setVisible(mode == "count");
    if (m_numDurationVal) m_numDurationVal->setVisible(mode == "duration");
    if (m_comboDurationUnit) m_comboDurationUnit->setVisible(mode == "duration");
    const bool delayEn = m_state && m_state->config.repeatDelayEnabled;
    if (m_numDelayBetween) m_numDelayBetween->setEnabled(delayEn);
    if (m_togDelayRandom) m_togDelayRandom->setEnabled(delayEn);
}

void RecorderPage::onLiveRecordingCount(int count) {
    const QString hk = (m_state && !m_state->config.hotkeyRecordToggle.isEmpty()) ? m_state->config.hotkeyRecordToggle : QStringLiteral("F9");
    if (m_lblLiveRecBanner) {
        m_lblLiveRecBanner->setText(
            I18n::tl(
                QString::fromUtf8("● جاري التسجيل الحي للماوس والكيبورد... (%1 خطوة — اضغط %2 أو الزر العائم أسفل الشاشة للإيقاف)")
                    .arg(I18n::fmtNum(QString::number(count)), hk),
                QStringLiteral("● Live Mouse & Keyboard Recording... (%1 events — Press %2 or click the floating Stop button)")
                    .arg(count).arg(hk)
            )
        );
    }
}

void RecorderPage::syncTransportButtons() {
    if (!m_state) return;
    const QString hkRec = m_state->config.hotkeyRecordToggle.isEmpty() ? QStringLiteral("F9") : m_state->config.hotkeyRecordToggle;
    const QString hkPlay = m_state->config.hotkeyPlaybackToggle.isEmpty() ? QStringLiteral("F10") : m_state->config.hotkeyPlaybackToggle;
    const QString hkStop = m_state->config.hotkeyPanicStop.isEmpty() ? QStringLiteral("Esc") : m_state->config.hotkeyPanicStop;
    const QString stopComboLbl = (hkPlay.compare(hkStop, Qt::CaseInsensitive) == 0)
        ? hkPlay
        : QStringLiteral("%1 / %2").arg(hkPlay, hkStop);

    if (m_state->isRecording) {
        if (m_btnRec) {
            m_btnRec->setText(QStringLiteral("%1 (%2)").arg(I18n::t("btn_stop_rec"), hkRec));
            m_btnRec->setIconName("stop");
        }
        if (m_stepsHiddenHolder) m_stepsHiddenHolder->hide();
        if (m_recEditToolbar) m_recEditToolbar->hide();
        if (m_scroll) m_scroll->hide();
        if (m_recAdderBar) m_recAdderBar->hide();
        if (m_lblLiveRecBanner) m_lblLiveRecBanner->show();
        onLiveRecordingCount(m_state->recordedEvents.size());
    } else {
        if (m_btnRec) {
            m_btnRec->setText(QStringLiteral("%1 (%2)").arg(I18n::t("btn_start_rec"), hkRec));
            m_btnRec->setIconName("recorder");
        }
        if (m_lblLiveRecBanner) m_lblLiveRecBanner->hide();
        setStepsEditorVisible(m_stepsEditorVisible);
    }

    if (m_btnPlay) {
        if (m_state->isPlaying) {
            m_btnPlay->setText(QStringLiteral("%1 (%2)").arg(I18n::t("btn_stop_play"), stopComboLbl));
            m_btnPlay->setVariant("danger");
            m_btnPlay->setIconName("stop");
        } else {
            m_btnPlay->setText(QStringLiteral("%1 (%2)").arg(I18n::t("btn_play_macro"), hkPlay));
            m_btnPlay->setVariant("primary");
            m_btnPlay->setIconName("play");
        }
    }
    refreshSavedLibrary();
}

void RecorderPage::onPlayClicked() {
    if (!m_engine || !m_state) return;
    if (m_state->isPlaying) {
        m_engine->stopAll();
    } else {
        m_engine->togglePlayback();
    }
}

void RecorderPage::convertCurrentToCombo() {
    if (!m_state || !m_engine) return;
    if (m_state->recordedEvents.isEmpty()) {
        emit m_state->toastRequested(I18n::tl(QString::fromUtf8("سجّل أو أضف خطوات أولاً"), QStringLiteral("Add steps first")), "danger");
        return;
    }
    const QString nm = (m_nameEdit && !m_nameEdit->text().trimmed().isEmpty()) ? m_nameEdit->text().trimmed() : QStringLiteral("Macro");
    m_engine->convertRecordingToSmartCombo(nm, m_state->recordedEvents);
}

void RecorderPage::pushUndoSnapshot() {
    if (!m_state) return;
    m_undoStack.append(m_state->recordedEvents);
    if (m_undoStack.size() > 40) m_undoStack.removeFirst();
    m_redoStack.clear();
    syncUndoRedoButtons();
}

void RecorderPage::syncUndoRedoButtons() {
    if (m_btnRecUndo) m_btnRecUndo->setEnabled(!m_undoStack.isEmpty());
    if (m_btnRecRedo) m_btnRecRedo->setEnabled(!m_redoStack.isEmpty());
}

void RecorderPage::undoSteps() {
    if (!m_state) return;
    if (m_undoStack.isEmpty()) {
        if (m_hasDeletedMacro) undoDeleteMacro();
        return;
    }
    m_redoStack.append(m_state->recordedEvents);
    m_state->recordedEvents = m_undoStack.takeLast();
    refreshTimeline();
    syncUndoRedoButtons();
}

void RecorderPage::redoSteps() {
    if (!m_state || m_redoStack.isEmpty()) return;
    m_undoStack.append(m_state->recordedEvents);
    m_state->recordedEvents = m_redoStack.takeLast();
    refreshTimeline();
    syncUndoRedoButtons();
}

void RecorderPage::setStepsEditorVisible(bool visible) {
    m_stepsEditorVisible = visible;
    if (m_state && m_state->isRecording) return;
    if (m_stepsHiddenHolder) m_stepsHiddenHolder->setVisible(!visible);
    if (m_recEditToolbar) m_recEditToolbar->setVisible(visible);
    if (m_scroll) m_scroll->setVisible(visible);
    if (m_recAdderBar) m_recAdderBar->setVisible(visible);

    const int cnt = m_state ? m_state->recordedEvents.size() : 0;
    if (!visible) {
        if (m_lblStepsHidden) {
            m_lblStepsHidden->setText(
                QStringLiteral("%1 (%2 %3)")
                    .arg(I18n::t("lbl_steps_hidden_hint"), I18n::fmtNum(QString::number(cnt)), I18n::t("unit_steps"))
            );
        }
    } else {
        const QString nm = (m_nameEdit && !m_nameEdit->text().trimmed().isEmpty())
            ? m_nameEdit->text().trimmed()
            : I18n::tl(QString::fromUtf8("التسجيل الحالي"), QStringLiteral("Current Recording"));
        if (m_lblEditingRecBadge) {
            m_lblEditingRecBadge->setText(
                I18n::tl(QString::fromUtf8("✏️ تعديل: «%1» (%2 خطوة)").arg(nm, I18n::fmtNum(QString::number(cnt))),
                         QStringLiteral("✏️ Editing: '%1' (%2 steps)").arg(nm).arg(cnt))
            );
        }
        if (m_btnRecSaveCurr) m_btnRecSaveCurr->setVisible(!m_editingMacroId.isEmpty());
        syncUndoRedoButtons();
    }
}

void RecorderPage::saveEditsToCurrentMacro() {
    if (!m_engine || !m_state) return;
    const QString nm = (m_nameEdit && !m_nameEdit->text().trimmed().isEmpty()) ? m_nameEdit->text().trimmed() : QStringLiteral("Macro");
    if (!m_editingMacroId.isEmpty() && m_engine->updateSavedMacro(m_editingMacroId, nm, m_state->recordedEvents)) {
        return;
    }
    m_engine->saveCurrentMacroToLibrary(nm);
}

void RecorderPage::saveEditsAsNewMacro() {
    if (!m_engine) return;
    const QString nm = (m_nameEdit && !m_nameEdit->text().trimmed().isEmpty()) ? m_nameEdit->text().trimmed() : QStringLiteral("Macro");
    m_engine->saveCurrentMacroToLibrary(QStringLiteral("%1 (Copy)").arg(nm));
}

void RecorderPage::clearEvents() {
    if (!m_state) return;
    if (!m_state->recordedEvents.isEmpty()) pushUndoSnapshot();
    m_state->recordedEvents.clear();
    refreshTimeline();
}

void RecorderPage::insertStep(const QString& kind) {
    if (!m_state) return;
    pushUndoSnapshot();
    const auto spSc = Win32Input::scancodeForLabel("Space");
    const QPoint pt = kind.contains("mouse") ? Win32Input::getCursorPos() : QPoint(500, 500);
    MacroEvent ev;
    ev.kind = kind;
    if (kind == "key_tap") {
        ev.delayMs = 50; ev.holdMs = 0; ev.key = "Space"; ev.scanCode = spSc.first; ev.isExtended = spSc.second;
    } else if (kind == "wait") {
        ev.delayMs = 1000; ev.key = "Wait";
    } else if (kind == "mouse_scroll") {
        ev.delayMs = 50; ev.x = pt.x(); ev.y = pt.y(); ev.scrollDir = "down"; ev.scrollAmount = 3; ev.dy = -3;
    } else {
        ev.delayMs = 50; ev.holdMs = 0; ev.x = pt.x(); ev.y = pt.y(); ev.button = "left";
    }
    m_state->recordedEvents.append(ev);
    refreshTimeline();
}

void RecorderPage::deleteEventAt(int idx) {
    if (!m_state || idx < 0 || idx >= m_state->recordedEvents.size()) return;
    pushUndoSnapshot();
    m_state->recordedEvents.removeAt(idx);
    refreshTimeline();
}

void RecorderPage::refreshTimeline() {
    if (!m_state || m_state->isRecording || !m_tlVbox) return;
    m_eventRows.clear();
    while (QLayoutItem* it = m_tlVbox->takeAt(0)) {
        if (QWidget* w = it->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete it;
    }

    if (m_state->recordedEvents.isEmpty()) {
        auto* empty = new QLabel(I18n::t("timeline_empty"), m_tlContainer);
        empty->setFont(makeFont(13, QFont::DemiBold));
        empty->setAlignment(Qt::AlignCenter);
        empty->setWordWrap(true);
        empty->setStyleSheet(QStringLiteral("color: %1; padding: 36px;").arg(Theme::instance().tokenHex("text3")));
        m_tlVbox->addWidget(empty);
        if (!m_stepsEditorVisible) setStepsEditorVisible(false);
        return;
    }

    const int limit = std::min(250, static_cast<int>(m_state->recordedEvents.size()));
    for (int idx = 0; idx < limit; ++idx) {
        auto* row = new EditableEventRow(idx, &m_state->recordedEvents[idx], m_state, m_tlContainer);
        connect(row, &EditableEventRow::deleteClicked, this, &RecorderPage::deleteEventAt);
        connect(row, &EditableEventRow::dragMoveRequested, this, &RecorderPage::onEventDragMoved);
        m_tlVbox->addWidget(row);
        m_eventRows.append(row);
    }
    m_tlVbox->addStretch(1);
    setStepsEditorVisible(m_stepsEditorVisible);
}

void RecorderPage::onEventDragMoved(EditableEventRow* draggedRow, int globalY) {
    if (!m_state || !draggedRow || !m_eventRows.contains(draggedRow)) return;
    const int curIdx = m_eventRows.indexOf(draggedRow);
    const int localY = m_tlContainer->mapFromGlobal(QPoint(0, globalY)).y();

    int targetIdx = curIdx;
    for (int i = 0; i < m_eventRows.size(); ++i) {
        EditableEventRow* rw = m_eventRows[i];
        const int ry = getRowLogicalY(rw);
        const int rh = std::max(24, rw->height());
        if (i < curIdx && localY < (ry + rh * 0.55)) {
            targetIdx = i;
            break;
        } else if (i > curIdx && localY > (ry + rh * 0.45)) {
            targetIdx = i;
        }
    }

    if (targetIdx != curIdx && targetIdx >= 0 && targetIdx < m_state->recordedEvents.size()) {
        QList<QWidget*> widgetList;
        for (auto* r : m_eventRows) widgetList.append(r);
        if (smoothReorderVBoxWidget(m_tlVbox, widgetList, curIdx, targetIdx)) {
            pushUndoSnapshot();
            EditableEventRow* movedRow = m_eventRows.takeAt(curIdx);
            m_eventRows.insert(targetIdx, movedRow);
            MacroEvent movedEv = m_state->recordedEvents.takeAt(curIdx);
            m_state->recordedEvents.insert(targetIdx, movedEv);
            for (int i = 0; i < m_eventRows.size(); ++i) {
                m_eventRows[i]->setStepIndex(i);
            }
        }
    }
}

void RecorderPage::refreshSavedLibrary() {
    if (!m_libVbox || !m_state) return;
    while (QLayoutItem* it = m_libVbox->takeAt(0)) {
        if (QWidget* w = it->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete it;
    }

    const bool playing = m_state->isPlaying;
    for (int idx = 0; idx < m_state->savedMacros.size(); ++idx) {
        const SavedMacro m = m_state->savedMacros[idx];
        auto* card = new StepCard(idx, "#8B5CF6", m_libContainer);
        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(12, 8, 10, 8);
        cl->setSpacing(6);

        auto* nm = new QLabel(
            QStringLiteral("%1 (%2 %3)")
                .arg(m.name.isEmpty() ? "Macro" : m.name,
                     I18n::fmtNum(QString::number(m.events.size())),
                     I18n::t("unit_steps")),
            card
        );
        nm->setWordWrap(true);
        nm->setFont(makeFont(13, QFont::ExtraBold));
        nm->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
        cl->addWidget(nm);

        const QString midStr = m.id;
        auto* hkRow = new QHBoxLayout();
        hkRow->setSpacing(5);

        auto* hkBtn = new KeyCaptureButton(m_state, m.hotkey, true, card);
        hkBtn->setFixedHeight(30);
        connect(hkBtn, &KeyCaptureButton::keyCaptured, this, [this, midStr](const QString& kStr, int, bool) {
            if (m_engine) m_engine->updateSavedMacroHotkey(midStr, kStr);
        });

        auto* bCmb = new IconButton("bolt", I18n::t("btn_convert_to_combo"), 30, "glass", card);
        connect(bCmb, &IconButton::clicked, this, [this, m]() {
            if (m_engine) m_engine->convertRecordingToSmartCombo(m.name.isEmpty() ? "Macro" : m.name, m.events);
        });

        auto* bExp = new IconButton("save", I18n::t("btn_export_all"), 30, "ghost", card);
        connect(bExp, &IconButton::clicked, this, [this, m]() { exportJson(m); });

        auto* bDel = new IconButton("trash", I18n::t("btn_delete_rec"), 30, "danger", card);
        connect(bDel, &IconButton::clicked, this, [this, m]() { deleteSavedWithUndo(m); });

        hkRow->addWidget(hkBtn, 1);
        hkRow->addWidget(bCmb);
        hkRow->addWidget(bExp);
        hkRow->addWidget(bDel);
        cl->addLayout(hkRow);

        auto* btns = new QHBoxLayout();
        btns->setSpacing(6);
        auto* bPlay = new Button(
            I18n::t(playing ? "btn_stop_play" : "btn_play_macro"),
            playing ? "danger" : "primary",
            playing ? "stop" : "play",
            QString(),
            true,
            card
        );
        bPlay->setFixedHeight(32);
        connect(bPlay, &Button::clicked, this, [this, m]() {
            if (!m_engine || !m_state) return;
            if (m_state->isPlaying) {
                m_engine->stopAll();
            } else {
                m_engine->startPlayback(m.events, m_state->config.defaultPlaybackSpeed, m_state->config.defaultRepeatCount);
            }
        });

        auto* bEdit = new Button(I18n::t("btn_load_to_editor"), "glass", QString(), QString(), true, card);
        bEdit->setFixedHeight(32);
        connect(bEdit, &Button::clicked, this, [this, m]() { loadSavedIntoTimeline(m); });

        btns->addWidget(bPlay, 1);
        btns->addWidget(bEdit, 1);
        cl->addLayout(btns);

        m_libVbox->addWidget(card);
    }
    m_libVbox->addStretch(1);
}

void RecorderPage::loadSavedIntoTimeline(const SavedMacro& m) {
    if (!m_state) return;
    m_editingMacroId = m.id;
    m_undoStack.clear();
    m_redoStack.clear();
    if (m_nameEdit) m_nameEdit->setText(m.name.isEmpty() ? "Macro" : m.name);
    m_state->recordedEvents = m.events;
    setStepsEditorVisible(true);
    refreshTimeline();
}

void RecorderPage::deleteSavedWithUndo(const SavedMacro& m) {
    m_lastDeletedMacro = m;
    m_hasDeletedMacro = true;
    if (m_btnUndoLib) m_btnUndoLib->show();
    if (m_engine) m_engine->deleteSavedMacro(m.id);
}

void RecorderPage::undoDeleteMacro() {
    if (m_hasDeletedMacro && m_state && m_engine) {
        m_state->recordedEvents = m_lastDeletedMacro.events;
        m_engine->saveCurrentMacroToLibrary(
            m_lastDeletedMacro.name.isEmpty() ? "Macro" : m_lastDeletedMacro.name,
            m_lastDeletedMacro.hotkey
        );
        m_hasDeletedMacro = false;
        if (m_btnUndoLib) m_btnUndoLib->hide();
    }
}

void RecorderPage::exportJson(const SavedMacro& m) {
    const QString path = QFileDialog::getSaveFileName(this, I18n::t("btn_export_all"), QStringLiteral("%1.json").arg(m.name.isEmpty() ? "macro" : m.name), "JSON (*.json)");
    if (!path.isEmpty()) {
        QFile f(path);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(QJsonDocument(m.toJson()).toJson(QJsonDocument::Indented));
        }
    }
}

void RecorderPage::exportFullBackup() {
    if (!m_engine) return;
    const QString path = QFileDialog::getSaveFileName(this, I18n::t("btn_export_all"), "V-MacroPlus_Full_Backup.json", "JSON Backup (*.json)");
    if (!path.isEmpty()) m_engine->exportAllBackup(path);
}

void RecorderPage::importJsonOrBackup() {
    if (!m_engine) return;
    const QString path = QFileDialog::getOpenFileName(this, I18n::t("btn_import_all"), QString(), "JSON (*.json)");
    if (!path.isEmpty()) m_engine->importAllBackup(path);
}

} // namespace vmacro
