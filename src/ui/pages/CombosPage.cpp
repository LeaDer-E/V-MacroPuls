#include "CombosPage.h"
#include "../Icons.h"
#include "../Widgets.h"
#include "../../core/AppState.h"
#include "../../core/I18n.h"
#include "../../core/Theme.h"
#include "../../engine/MacroEngine.h"
#include "../../engine/PythonScriptRunner.h"
#include "../../platform/Win32Input.h"

#include <QDateTime>
#include <QFileDialog>
#include <QKeySequence>
#include <QMap>
#include <QMouseEvent>
#include <QShortcut>
#include <QScrollBar>
#include <QSizePolicy>
#include <algorithm>

namespace vmacro {

static QString stepCategoryColor(const QString& st) {
    static const QMap<QString, QString> kColors = {
        {"key_tap", "#8B5CF6"},
        {"key_hold", "#8B5CF6"},
        {"key_down", "#7C3AED"},
        {"key_up", "#C084FC"},
        {"wait", "#F59E0B"},
        {"mouse_click", "#10B981"},
        {"mouse_down", "#059669"},
        {"mouse_up", "#34D399"},
        {"mouse_move", "#14B8A6"},
        {"mouse_scroll", "#EC4899"},
        {"open_folder", "#3B82F6"},
        {"powershell", "#06B6D4"},
        {"cmd", "#F43F5E"},
        {"type_text", "#06B6D4"},
        {"python_script", "#F59E0B"},
    };
    return kColors.value(st, "#8B5CF6");
}

static QList<QPair<QString, QString>> mouseButtonOptions() {
    return {
        {"left", I18n::t("mouse_btn_left")},
        {"right", I18n::t("mouse_btn_right")},
        {"middle", I18n::t("mouse_btn_middle")},
        {"x1", I18n::t("mouse_btn_x1")},
        {"x2", I18n::t("mouse_btn_x2")},
    };
}

static QList<QPair<QString, QString>> scrollDirOptions() {
    return {
        {"down", I18n::t("scroll_down")},
        {"up", I18n::t("scroll_up")},
        {"left", I18n::t("scroll_left")},
        {"right", I18n::t("scroll_right")},
    };
}

static QList<SearchComboItem> runningAppsComboItems() {
    QList<SearchComboItem> items;
    items.append(SearchComboItem(QString(), I18n::t("app_all")));
    for (const auto& app : Win32Input::listRunningApps()) {
        if (!app.procName.isEmpty()) {
            const QString lbl = app.title.isEmpty()
                ? app.procName
                : QStringLiteral("%1 — %2").arg(app.procName, app.title);
            items.append(SearchComboItem(app.procName, lbl, app.exePath));
        }
    }
    return items;
}

// ============================================================================
// ComboStepRowWidget
// ============================================================================

ComboStepRowWidget::ComboStepRowWidget(int index, ComboStep* stepPtr, AppState* state, QWidget* parent)
    : QWidget(parent), m_index(index), m_step(stepPtr), m_state(state) {
    const QString st = m_step ? m_step->stepType : QStringLiteral("key_tap");
    m_catHex = stepCategoryColor(st);

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

    m_stepNum = new QLabel(QStringLiteral("#%1").arg(I18n::fmtNum(QString::number(m_index + 1))), m_card);
    m_stepNum->setFont(makeFont(12, QFont::ExtraBold));
    m_stepNum->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(m_catHex));
    lay->addWidget(m_stepNum);

    const bool stepEn = m_step ? m_step->enabled : true;
    m_card->setStepEnabled(stepEn);

    m_btnEnableStep = new StepEnableToggleButton(stepEn, m_card);
    connect(m_btnEnableStep, &StepEnableToggleButton::toggled, this, &ComboStepRowWidget::onStepEnabledToggled);
    lay->addWidget(m_btnEnableStep);

    m_lblPaused = new QLabel(I18n::t("lbl_step_paused_badge"), m_card);
    m_lblPaused->setFont(makeFont(11, QFont::ExtraBold));
    m_lblPaused->setStyleSheet("color: #F59E0B; background: transparent;");
    m_lblPaused->setVisible(!stepEn);
    lay->addWidget(m_lblPaused);

    if (st == "wait") {
        auto* lbl = new QLabel(I18n::t("step_row_wait"), m_card);
        lbl->setFont(makeFont(12, QFont::Bold));
        lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
        lay->addWidget(lbl);

        m_numMs = new NumberInput(m_step ? m_step->delayMs : 1000, 1, 600000, 100, "ms", 134, m_card);
        m_numMs->setFixedSize(134, 32);
        connect(m_numMs, &NumberInput::valueChanged, this, [this](int ms) {
            if (m_step) m_step->delayMs = ms;
            emit changed();
        });
        lay->addWidget(m_numMs);
    } else if (st == "mouse_click" || st == "mouse_down" || st == "mouse_up" || st == "mouse_move") {
        QString lblTxt = I18n::t("step_row_mouse_click");
        if (st == "mouse_down") lblTxt = I18n::t("step_row_mouse_down");
        else if (st == "mouse_up") lblTxt = I18n::t("step_row_mouse_up");
        else if (st == "mouse_move") lblTxt = I18n::t("step_row_mouse_move");

        auto* lbl = new QLabel(lblTxt, m_card);
        lbl->setFont(makeFont(12, QFont::Bold));
        lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
        lay->addWidget(lbl);

        if (st != "mouse_move") {
            m_btnCombo = new SearchCombo(mouseButtonOptions(), m_step ? m_step->value : "left", m_card);
            m_btnCombo->setFixedSize(124, 32);
            connect(m_btnCombo, &SearchCombo::currentChanged, this, [this](const QString& btn) {
                if (m_step) m_step->value = btn;
                emit changed();
            });
            lay->addWidget(m_btnCombo);
        }

        auto* lblX = new QLabel("X:", m_card);
        lblX->setFont(makeFont(11, QFont::Bold));
        lblX->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
        m_numX = new NumberInput(m_step ? m_step->x : 500, -32768, 32768, 10, QString(), 88, m_card);
        m_numX->setFixedSize(88, 32);
        connect(m_numX, &NumberInput::valueChanged, this, [this](int xv) {
            if (m_step) { m_step->x = xv; m_step->useCoords = true; }
            emit changed();
        });

        auto* lblY = new QLabel("Y:", m_card);
        lblY->setFont(makeFont(11, QFont::Bold));
        lblY->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
        m_numY = new NumberInput(m_step ? m_step->y : 500, -32768, 32768, 10, QString(), 88, m_card);
        m_numY->setFixedSize(88, 32);
        connect(m_numY, &NumberInput::valueChanged, this, [this](int yv) {
            if (m_step) { m_step->y = yv; m_step->useCoords = true; }
            emit changed();
        });

        m_btnPickXy = new Button(I18n::t("btn_pick_live_xy"), "glass", "crosshair", "#10B981", false, m_card);
        m_btnPickXy->setFixedHeight(30);
        connect(m_btnPickXy, &Button::clicked, this, &ComboStepRowWidget::toggleLiveXyPicker);

        m_lblLiveXy = new QLabel(m_card);
        m_lblLiveXy->setFont(makeFont(11, QFont::Bold));
        m_lblLiveXy->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("ok")));

        lay->addWidget(lblX);
        lay->addWidget(m_numX);
        lay->addWidget(lblY);
        lay->addWidget(m_numY);
        lay->addWidget(m_btnPickXy);
        lay->addWidget(m_lblLiveXy);

        if (st == "mouse_click") {
            auto* lblHold = new QLabel(I18n::t("lbl_hold_duration"), m_card);
            lblHold->setFont(makeFont(11, QFont::Bold));
            lblHold->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
            m_numHold = new NumberInput(m_step ? m_step->holdMs : 0, 0, 60000, 100, "ms", 102, m_card);
            m_numHold->setFixedSize(102, 32);
            connect(m_numHold, &NumberInput::valueChanged, this, [this](int ms) {
                if (m_step) m_step->holdMs = ms;
                emit changed();
            });
            lay->addWidget(lblHold);
            lay->addWidget(m_numHold);
        }

        m_xyTimer = new QTimer(this);
        m_xyTimer->setInterval(50);
        connect(m_xyTimer, &QTimer::timeout, this, &ComboStepRowWidget::tickLiveXy);
        connect(m_numX, &NumberInput::focused, this, &ComboStepRowWidget::startLiveXyPreview);
        connect(m_numY, &NumberInput::focused, this, &ComboStepRowWidget::startLiveXyPreview);
        if (m_state) {
            connect(m_state, &AppState::cursorPickHotkeyPressed, this, &ComboStepRowWidget::onF8Picked);
        }
    } else if (st == "mouse_scroll") {
        auto* lbl = new QLabel(I18n::t("lbl_scroll_dir"), m_card);
        lbl->setFont(makeFont(12, QFont::ExtraBold));
        lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(m_catHex));
        lay->addWidget(lbl);

        m_scrollDirCombo = new SearchCombo(scrollDirOptions(), m_step ? m_step->value : "down", m_card);
        m_scrollDirCombo->setFixedSize(118, 32);
        connect(m_scrollDirCombo, &SearchCombo::currentChanged, this, [this](const QString& dir) {
            if (m_step) m_step->value = dir;
            emit changed();
        });
        lay->addWidget(m_scrollDirCombo);

        auto* lblAmt = new QLabel(I18n::t("lbl_scroll_amount"), m_card);
        lblAmt->setFont(makeFont(12, QFont::Bold));
        lblAmt->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
        m_numScrollAmt = new NumberInput(m_step ? m_step->scrollAmount : 3, 1, 100, 1, QString(), 98, m_card);
        m_numScrollAmt->setFixedSize(98, 32);
        connect(m_numScrollAmt, &NumberInput::valueChanged, this, [this](int val) {
            if (m_step) m_step->scrollAmount = std::max(1, val);
            emit changed();
        });
        lay->addWidget(lblAmt);
        lay->addWidget(m_numScrollAmt);
    } else if (st == "open_folder") {
        auto* lbl = new QLabel(I18n::t("lbl_open_folder_step"), m_card);
        lbl->setFont(makeFont(12, QFont::ExtraBold));
        lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(m_catHex));
        lay->addWidget(lbl);

        m_folderEdit = new QLineEdit(m_step ? m_step->value : "C:\\", m_card);
        m_folderEdit->setPlaceholderText("C:\\Users\\...");
        m_folderEdit->setFixedHeight(32);
        m_folderEdit->setStyleSheet(QStringLiteral(R"(
            QLineEdit {
                background: %1;
                color: %2;
                border: 1px solid %3;
                border-radius: 8px;
                padding: 2px 10px;
                font-family: 'Consolas', '%4';
                font-size: 13px;
            }
        )").arg(Theme::instance().tokenHex("bg0"),
                Theme::instance().tokenHex("text"),
                Theme::instance().tokenHex("border_hi"),
                I18n::langFontFamily()));
        connect(m_folderEdit, &QLineEdit::textChanged, this, [this](const QString& txt) {
            if (m_step) m_step->value = txt;
            emit changed();
        });
        lay->addWidget(m_folderEdit, 1);

        auto* btnBrowseDir = new Button(I18n::t("btn_browse_folder"), "glass", "folder", m_catHex, false, m_card);
        btnBrowseDir->setFixedHeight(32);
        connect(btnBrowseDir, &Button::clicked, this, &ComboStepRowWidget::browseFolder);
        lay->addWidget(btnBrowseDir);

        auto* btnBrowseApp = new Button(I18n::t("btn_browse_app_file"), "glass", "terminal", m_catHex, false, m_card);
        btnBrowseApp->setFixedHeight(32);
        connect(btnBrowseApp, &Button::clicked, this, &ComboStepRowWidget::browseProgramOrFile);
        lay->addWidget(btnBrowseApp);
    } else if (st == "type_text") {
        auto* lbl = new QLabel(I18n::t("step_row_type_text"), m_card);
        lbl->setFont(makeFont(12, QFont::ExtraBold));
        lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(m_catHex));
        lay->addWidget(lbl);

        m_cmdEdit = new QLineEdit(m_step ? m_step->value : QString(), m_card);
        m_cmdEdit->setPlaceholderText(I18n::t("ph_type_text"));
        m_cmdEdit->setFixedHeight(32);
        m_cmdEdit->setStyleSheet(QStringLiteral(R"(
            QLineEdit {
                background: %1;
                color: %2;
                border: 1px solid %3;
                border-radius: 8px;
                padding: 2px 10px;
                font-family: '%4';
                font-size: 13px;
            }
        )").arg(Theme::instance().tokenHex("bg0"),
                Theme::instance().tokenHex("text"),
                Theme::instance().tokenHex("border_hi"),
                I18n::langFontFamily()));
        connect(m_cmdEdit, &QLineEdit::textChanged, this, [this](const QString& txt) {
            if (m_step) m_step->value = txt;
            emit changed();
        });
        lay->addWidget(m_cmdEdit, 1);
    } else if (st == "python_script") {
        auto* pyCol = new QVBoxLayout();
        pyCol->setSpacing(4);

        auto* pyTopRow = new QHBoxLayout();
        pyTopRow->setSpacing(6);
        auto* lbl = new QLabel(I18n::t("step_row_python_script"), m_card);
        lbl->setFont(makeFont(11, QFont::ExtraBold));
        lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(m_catHex));

        m_pyPresetCombo = new SearchCombo(PythonScriptRunner::dropdownItems(), "preset:human_3d_keys", m_card);
        m_pyPresetCombo->setMinimumWidth(210);
        m_pyPresetCombo->setFixedHeight(28);
        connect(m_pyPresetCombo, &SearchCombo::currentChanged, this, &ComboStepRowWidget::onStepPyPresetSelected);

        m_pyScriptNameEdit = new QLineEdit(m_card);
        m_pyScriptNameEdit->setPlaceholderText(I18n::t("ph_py_script_name"));
        m_pyScriptNameEdit->setFixedWidth(135);
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

        auto* btnSavePy = new Button(I18n::t("btn_save_py_script"), "glass", "save", m_catHex, false, m_card);
        btnSavePy->setFixedHeight(28);
        connect(btnSavePy, &Button::clicked, this, &ComboStepRowWidget::onStepSavePyScript);

        auto* btnDelPy = new IconButton("trash", I18n::t("tip_del_py_script"), 28, "danger", m_card);
        connect(btnDelPy, &IconButton::clicked, this, &ComboStepRowWidget::onStepDeletePyScript);

        pyTopRow->addWidget(lbl);
        pyTopRow->addWidget(m_pyPresetCombo, 1);
        pyTopRow->addWidget(m_pyScriptNameEdit);
        pyTopRow->addWidget(btnSavePy);
        pyTopRow->addWidget(btnDelPy);
        pyCol->addLayout(pyTopRow);

        m_pyEdit = new QPlainTextEdit(m_step ? m_step->value : QString(), m_card);
        m_pyEdit->setLayoutDirection(Qt::LeftToRight);
        m_pyEdit->setPlaceholderText(I18n::t("ph_python_script"));
        m_pyEdit->setFixedHeight(84);
        m_pyEdit->setStyleSheet(QStringLiteral(R"(
            QPlainTextEdit {
                background: %1;
                color: %2;
                border: 1px solid %3;
                border-radius: 8px;
                padding: 4px 8px;
                font-family: 'Consolas', 'Courier New', monospace;
                font-size: 12px;
            }
        )").arg(Theme::instance().tokenHex("bg0"),
                Theme::instance().tokenHex("text"),
                Theme::instance().tokenHex("border_hi")));
        connect(m_pyEdit, &QPlainTextEdit::textChanged, this, [this]() {
            if (m_step) m_step->value = m_pyEdit->toPlainText();
            emit changed();
        });
        pyCol->addWidget(m_pyEdit);
        lay->addLayout(pyCol, 1);

        if (m_state) {
            connect(m_state, &AppState::pythonScriptsUpdated, this, [this]() { refreshStepPyPresetsCombo(); });
        }
    } else if (st == "powershell" || st == "cmd") {
        const QString tag = (st == "powershell") ? I18n::t("step_row_ps") : I18n::t("step_row_cmd");
        auto* lbl = new QLabel(tag, m_card);
        lbl->setFont(makeFont(12, QFont::ExtraBold));
        lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(m_catHex));
        lay->addWidget(lbl);

        m_cmdEdit = new QLineEdit(m_step ? m_step->value : QString(), m_card);
        m_cmdEdit->setPlaceholderText(st == "powershell" ? "Get-Process | ..." : "ipconfig /flushdns ...");
        m_cmdEdit->setFixedHeight(32);
        m_cmdEdit->setStyleSheet(QStringLiteral(R"(
            QLineEdit {
                background: %1;
                color: %2;
                border: 1px solid %3;
                border-radius: 8px;
                padding: 2px 10px;
                font-family: 'Consolas', '%4';
                font-size: 13px;
            }
        )").arg(Theme::instance().tokenHex("bg0"),
                Theme::instance().tokenHex("text"),
                Theme::instance().tokenHex("border_hi"),
                I18n::langFontFamily()));
        connect(m_cmdEdit, &QLineEdit::textChanged, this, [this](const QString& txt) {
            if (m_step) m_step->value = txt;
            emit changed();
        });
        lay->addWidget(m_cmdEdit, 1);
    } else {
        QString keyLbl = I18n::t("step_row_key_tap");
        if (st == "key_down") keyLbl = I18n::t("step_row_key_down");
        else if (st == "key_up") keyLbl = I18n::t("step_row_key_up");

        auto* lbl = new QLabel(keyLbl, m_card);
        lbl->setFont(makeFont(12, QFont::Bold));
        lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
        lay->addWidget(lbl);

        m_keyBtn = new KeyCaptureButton(
            m_step ? m_step->value : "Space",
            m_step ? m_step->scanCode : 0,
            m_step ? m_step->isExtended : false,
            {},
            st == "key_tap" || st == "key_hold",
            m_state,
            false,
            m_card
        );
        m_keyBtn->setFixedHeight(32);
        connect(m_keyBtn, &KeyCaptureButton::chordCaptured, this, &ComboStepRowWidget::onStepChordCaptured);
        lay->addWidget(m_keyBtn);

        if (st == "key_tap" || st == "key_hold") {
            auto* lblHold = new QLabel(I18n::t("lbl_hold_duration"), m_card);
            lblHold->setFont(makeFont(11, QFont::Bold));
            lblHold->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
            const int defaultHold = m_step ? m_step->holdMs : 0;
            m_numHold = new NumberInput(defaultHold, 0, 60000, 100, "ms", 108, m_card);
            m_numHold->setFixedSize(108, 32);
            connect(m_numHold, &NumberInput::valueChanged, this, [this](int ms) {
                if (m_step) m_step->holdMs = ms;
                emit changed();
            });
            lay->addWidget(lblHold);
            lay->addWidget(m_numHold);
        }
    }

    if (st != "powershell" && st != "cmd" && st != "open_folder" && st != "type_text" && st != "python_script") {
        lay->addStretch(1);
    }

    if (st != "wait") {
        auto* lblD = new QLabel(I18n::t("lbl_pre_delay"), m_card);
        lblD->setFont(makeFont(11, QFont::Medium));
        lblD->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
        m_numDelay = new NumberInput(m_step ? m_step->delayMs : 20, 1, 600000, 25, "ms", 108, m_card);
        m_numDelay->setFixedSize(108, 32);
        connect(m_numDelay, &NumberInput::valueChanged, this, [this](int ms) {
            if (m_step) m_step->delayMs = ms;
            emit changed();
        });
        lay->addWidget(lblD);
        lay->addWidget(m_numDelay);
    }

    m_btnNote = new StepNoteIconButton(m_step ? m_step->note : QString(), m_card);
    connect(m_btnNote, &StepNoteIconButton::noteChanged, this, &ComboStepRowWidget::onStepNoteChanged);
    lay->addWidget(m_btnNote);

    auto* bTestOne = new IconButton("play", I18n::tl(QString::fromUtf8("تجربة هذه الخطوة وحدها مرة واحدة"), QStringLiteral("Test this step once")), 28, "ok", m_card);
    connect(bTestOne, &IconButton::clicked, this, [this]() { emit testSingleStep(m_index); });

    auto* bUp = new IconButton("chevron_up", QString(), 28, "ghost", m_card);
    connect(bUp, &IconButton::clicked, this, [this]() { emit moveUp(m_index); });

    auto* bDn = new IconButton("chevron_down", QString(), 28, "ghost", m_card);
    connect(bDn, &IconButton::clicked, this, [this]() { emit moveDown(m_index); });

    auto* bDel = new IconButton("trash", I18n::t("btn_delete"), 28, "danger", m_card);
    connect(bDel, &IconButton::clicked, this, [this]() { emit removed(m_index); });

    lay->addWidget(bTestOne);
    lay->addWidget(bUp);
    lay->addWidget(bDn);
    lay->addWidget(bDel);
}

void ComboStepRowWidget::setStepIndex(int newIdx) {
    m_index = newIdx;
    m_stepNum->setText(QStringLiteral("#%1").arg(I18n::fmtNum(QString::number(newIdx + 1))));
    m_card->setIndex(newIdx);
}

void ComboStepRowWidget::onStepEnabledToggled(bool enabled) {
    if (m_step) m_step->enabled = enabled;
    m_card->setStepEnabled(enabled);
    m_lblPaused->setVisible(!enabled);
    emit changed();
}

void ComboStepRowWidget::onStepNoteChanged(const QString& noteTxt) {
    if (m_step) m_step->note = noteTxt;
    emit changed();
}

void ComboStepRowWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_rowDragging = true;
        setCursor(Qt::ClosedHandCursor);
        m_card->setDragging(true);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void ComboStepRowWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_rowDragging) {
        emit dragMoveRequested(this, static_cast<int>(event->globalPosition().y()));
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void ComboStepRowWidget::mouseReleaseEvent(QMouseEvent* event) {
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

void ComboStepRowWidget::browseFolder() {
    const QString cur = (m_step && !m_step->value.isEmpty()) ? m_step->value : QStringLiteral("C:\\");
    const QString picked = QFileDialog::getExistingDirectory(this, I18n::t("lbl_open_folder_step"), cur);
    if (!picked.isEmpty() && m_folderEdit) {
        m_folderEdit->setText(picked);
        if (m_step) m_step->value = picked;
        emit changed();
    }
}

void ComboStepRowWidget::browseProgramOrFile() {
    const QString cur = (m_step && !m_step->value.isEmpty()) ? m_step->value : QStringLiteral("C:\\");
    const QString picked = QFileDialog::getOpenFileName(
        this,
        I18n::t("btn_browse_app_file"),
        cur,
        "Programs & Files (*.exe *.bat *.cmd *.lnk *.py *.*);;All Files (*.*)"
    );
    if (!picked.isEmpty() && m_folderEdit) {
        m_folderEdit->setText(picked);
        if (m_step) m_step->value = picked;
        emit changed();
    }
}

void ComboStepRowWidget::refreshStepPyPresetsCombo(const QString& selectKey) {
    if (!m_pyPresetCombo) return;
    const QString cur = selectKey.isEmpty() ? m_pyPresetCombo->currentKey() : selectKey;
    m_pyPresetCombo->setItems(PythonScriptRunner::dropdownItems(), cur);
}

void ComboStepRowWidget::onStepPyPresetSelected(const QString& scriptKey) {
    const QString code = PythonScriptRunner::codeByKey(scriptKey);
    if (!code.isEmpty() && m_pyEdit) {
        m_pyEdit->setPlainText(code);
        if (m_step) m_step->value = code;
        emit changed();
    }
    const QString savedNm = PythonScriptRunner::nameByKey(scriptKey);
    if (!savedNm.isEmpty() && m_pyScriptNameEdit) {
        m_pyScriptNameEdit->setText(savedNm);
    }
}

void ComboStepRowWidget::onStepSavePyScript() {
    if (!m_pyEdit || !m_pyPresetCombo || !m_pyScriptNameEdit) return;
    const QString nm = m_pyScriptNameEdit->text().trimmed();
    const QString code = m_pyEdit->toPlainText();
    const QString curK = m_pyPresetCombo->currentKey();
    const auto saved = PythonScriptRunner::upsertUserScript(nm, code, nm.isEmpty() ? curK : QString());
    m_pyScriptNameEdit->setText(saved.second);
    refreshStepPyPresetsCombo(saved.first);
    if (m_state) {
        emit m_state->pythonScriptsUpdated();
        emit m_state->toastRequested(QStringLiteral("%1 «%2»").arg(I18n::t("toast_py_script_saved"), saved.second), "ok");
    }
}

void ComboStepRowWidget::onStepDeletePyScript() {
    if (!m_pyPresetCombo) return;
    const QString curK = m_pyPresetCombo->currentKey();
    if (PythonScriptRunner::deleteUserScript(curK)) {
        if (m_pyScriptNameEdit) m_pyScriptNameEdit->clear();
        refreshStepPyPresetsCombo("preset:human_3d_keys");
        if (m_state) {
            emit m_state->pythonScriptsUpdated();
            emit m_state->toastRequested(I18n::t("toast_py_script_deleted"), "info");
        }
    }
}

void ComboStepRowWidget::onStepChordCaptured(const QList<QPair<int, bool>>& chord, const QString& lbl) {
    if (lbl.isEmpty() || !m_step) return;
    m_step->value = lbl;
    if (!chord.isEmpty()) {
        m_step->scanCode = chord.last().first;
        m_step->isExtended = chord.last().second;
    }
    emit changed();
}

void ComboStepRowWidget::startLiveXyPreview() {
    m_pickingLiveXy = true;
    if (m_xyTimer && !m_xyTimer->isActive()) m_xyTimer->start();
}

void ComboStepRowWidget::toggleLiveXyPicker() {
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

void ComboStepRowWidget::tickLiveXy() {
    const QPoint pt = Win32Input::getCursorPos();
    if (m_lblLiveXy) {
        m_lblLiveXy->setText(I18n::ltrValue(QStringLiteral("(%1, %2)").arg(pt.x()).arg(pt.y())));
    }
}

void ComboStepRowWidget::applyPickedCoords(int cx, int cy) {
    if (m_step) {
        m_step->x = cx;
        m_step->y = cy;
    }
    if (m_numX) m_numX->setValue(cx, true);
    if (m_numY) m_numY->setValue(cy, true);
    m_pickingLiveXy = false;
    if (m_xyTimer) m_xyTimer->stop();
    if (m_btnPickXy) m_btnPickXy->setText(I18n::t("btn_pick_live_xy"));
    if (m_lblLiveXy) m_lblLiveXy->setText(I18n::ltrValue(QStringLiteral("✓ (%1,%2)").arg(cx).arg(cy)));
    emit changed();
}

void ComboStepRowWidget::onF8Picked(int cx, int cy) {
    if (!m_pickingLiveXy) return;
    applyPickedCoords(cx, cy);
    if (m_state) {
        emit m_state->toastRequested(
            I18n::tl(QString::fromUtf8("✓ تم التقاط إحداثيات الماوس: (%1, %2)").arg(cx).arg(cy),
                     QStringLiteral("✓ Picked mouse coordinates: (%1, %2)").arg(cx).arg(cy)),
            QStringLiteral("ok")
        );
    }
}

// ============================================================================
// CombosPage
// ============================================================================

CombosPage::CombosPage(AppState* state, MacroEngine* engine, QWidget* parent)
    : QWidget(parent), m_state(state), m_engine(engine) {
    m_sidebarCollapsed = m_state ? m_state->combosSidebarCollapsed : false;
    buildUi();
    connectSignals();

    auto* scUndo = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(scUndo, &QShortcut::activated, this, &CombosPage::undoComboSteps);

    auto* scRedo = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(scRedo, &QShortcut::activated, this, &CombosPage::redoComboSteps);

    refreshLeftList();
    loadSelectedIntoEditor();
}

void CombosPage::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 8, 16, 12);
    root->setSpacing(10);

    // Top Card
    auto* topCard = new GlassCard(true, this);
    auto* tLay = new QHBoxLayout(topCard);
    tLay->setContentsMargins(18, 10, 18, 10);
    tLay->setSpacing(12);

    auto* tCol = new QVBoxLayout();
    tCol->setSpacing(2);
    auto* title = new QLabel(I18n::t("combo_title"), topCard);
    title->setWordWrap(true);
    title->setMinimumWidth(80);
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    title->setFont(makeFont(15, QFont::ExtraBold));
    title->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    auto* sub = new QLabel(I18n::t("combo_subtitle"), topCard);
    sub->setWordWrap(true);
    sub->setMinimumWidth(80);
    sub->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sub->setFont(makeFont(11, QFont::Medium));
    sub->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    tCol->addWidget(title);
    tCol->addWidget(sub);
    tLay->addLayout(tCol, 1);

    auto* swLbl = new QLabel(I18n::t("combo_master_switch"), topCard);
    swLbl->setFont(makeFont(13, QFont::Bold));
    swLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    m_masterTog = new ToggleSwitch(m_state ? m_state->combosActive : true, topCard);
    connect(m_masterTog, &ToggleSwitch::toggled, this, [this](bool) {
        if (m_engine) m_engine->toggleCombosMaster();
    });

    m_btnToggleSidebar = new Button(
        I18n::t(m_sidebarCollapsed ? "btn_show_macros_sidebar" : "btn_hide_macros_sidebar"),
        "glass",
        "dashboard",
        Theme::instance().tokenHex("accent"),
        false,
        topCard
    );
    m_btnToggleSidebar->setFixedHeight(32);
    connect(m_btnToggleSidebar, &Button::clicked, this, &CombosPage::toggleMacrosSidebar);

    tLay->addWidget(swLbl);
    tLay->addWidget(m_masterTog);
    tLay->addSpacing(6);
    tLay->addWidget(m_btnToggleSidebar);
    root->addWidget(topCard);

    auto* body = new QHBoxLayout();
    body->setSpacing(12);

    // Left Sidebar Card
    m_leftCard = new GlassCard(false, this);
    m_leftCard->setFixedWidth(LEFT_PANEL_WIDTH);
    auto* lLay = new QVBoxLayout(m_leftCard);
    lLay->setContentsMargins(10, 12, 10, 12);
    lLay->setSpacing(8);

    auto* topBtns = new QHBoxLayout();
    topBtns->setSpacing(6);
    m_btnNew = new Button(I18n::t("btn_new_smart_key"), "primary", "plus", QString(), true, m_leftCard);
    connect(m_btnNew, &Button::clicked, this, &CombosPage::createNewBlankCombo);

    m_btnUndoDelete = new IconButton("chevron_up", I18n::t("btn_undo_delete"), 34, "ghost", m_leftCard);
    connect(m_btnUndoDelete, &IconButton::clicked, this, &CombosPage::undoLastDelete);
    m_btnUndoDelete->hide();

    topBtns->addWidget(m_btnNew, 1);
    topBtns->addWidget(m_btnUndoDelete);
    lLay->addLayout(topBtns);

    auto* listScroll = new QScrollArea(m_leftCard);
    listScroll->setWidgetResizable(true);
    listScroll->setFrameShape(QFrame::NoFrame);
    listScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listScroll->setStyleSheet(modernScrollStyle());

    m_listContainer = new QWidget();
    m_listContainer->setStyleSheet("background: transparent;");
    m_listVbox = new QVBoxLayout(m_listContainer);
    m_listVbox->setContentsMargins(0, 0, 2, 0);
    m_listVbox->setSpacing(6);
    listScroll->setWidget(m_listContainer);
    lLay->addWidget(listScroll, 1);

    auto* bkRow = new QHBoxLayout();
    bkRow->setSpacing(6);
    auto* btnExpAll = new Button(I18n::t("btn_export_all"), "glass", "save", QString(), true, m_leftCard);
    btnExpAll->setFixedHeight(32);
    connect(btnExpAll, &Button::clicked, this, &CombosPage::exportFullBackup);

    auto* btnImpAll = new Button(I18n::t("btn_import_all"), "ghost", "folder", QString(), true, m_leftCard);
    btnImpAll->setFixedHeight(32);
    connect(btnImpAll, &Button::clicked, this, &CombosPage::importFullBackup);

    bkRow->addWidget(btnExpAll, 1);
    bkRow->addWidget(btnImpAll, 1);
    lLay->addLayout(bkRow);

    m_leftCard->setVisible(!m_sidebarCollapsed);
    body->addWidget(m_leftCard);

    // Empty State Card
    m_emptyStateCard = new GlassCard(true, this);
    auto* emLay = new QVBoxLayout(m_emptyStateCard);
    emLay->setContentsMargins(40, 40, 40, 40);
    emLay->setAlignment(Qt::AlignCenter);
    emLay->setSpacing(16);

    auto* emLogo = new QLabel(m_emptyStateCard);
    emLogo->setPixmap(Icons::logoPixmap(72, 2.0));
    emLogo->setAlignment(Qt::AlignCenter);
    emLay->addWidget(emLogo);

    auto* emTitle = new QLabel(I18n::t("empty_macros_title"), m_emptyStateCard);
    emTitle->setFont(makeFont(20, QFont::ExtraBold));
    emTitle->setAlignment(Qt::AlignCenter);
    emTitle->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    emLay->addWidget(emTitle);

    auto* emSub = new QLabel(I18n::t("empty_macros_subtitle"), m_emptyStateCard);
    emSub->setFont(makeFont(13, QFont::Medium));
    emSub->setWordWrap(true);
    emSub->setAlignment(Qt::AlignCenter);
    emSub->setFixedWidth(440);
    emSub->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    emLay->addWidget(emSub, 0, Qt::AlignCenter);

    emLay->addSpacing(8);
    m_btnEmptyNew = new Button(I18n::t("btn_new_smart_key"), "primary", "plus", 46, m_emptyStateCard);
    m_btnEmptyNew->setFixedSize(220, 46);
    connect(m_btnEmptyNew, &Button::clicked, this, &CombosPage::createNewBlankCombo);
    emLay->addWidget(m_btnEmptyNew, 0, Qt::AlignCenter);

    m_emptyStateCard->hide();
    body->addWidget(m_emptyStateCard, 1);

    // Right Editor Card
    m_rightCard = new GlassCard(false, this);
    auto* rLay = new QVBoxLayout(m_rightCard);
    rLay->setContentsMargins(16, 10, 16, 10);
    rLay->setSpacing(7);

    // STEP ① Header
    auto* s1HdrRow = new QHBoxLayout();
    auto* s1Lbl = new QLabel(I18n::t("step1_header"), m_rightCard);
    s1Lbl->setFont(makeFont(13, QFont::ExtraBold));
    s1Lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    s1HdrRow->addWidget(s1Lbl, 1);

    m_btnNoKey = new Button(I18n::t("btn_clear_hotkey"), "ghost", QString(), "#64748B", false, m_rightCard);
    m_btnNoKey->setFixedHeight(28);
    connect(m_btnNoKey, &Button::clicked, this, &CombosPage::clearTriggerForManualMode);
    s1HdrRow->addWidget(m_btnNoKey);
    rLay->addLayout(s1HdrRow);

    // Key & Name Row
    auto* keyAndNameRow = new QHBoxLayout();
    keyAndNameRow->setSpacing(10);

    m_nameEdit = new QLineEdit(m_rightCard);
    m_nameEdit->setPlaceholderText(I18n::t("ph_macro_title_opt"));
    m_nameEdit->setFixedHeight(40);
    m_nameEdit->setFixedWidth(185);
    m_nameEdit->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 11px;
            padding: 4px 12px;
            font-family: '%4';
            font-size: 13px;
            font-weight: bold;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border_hi"),
            I18n::langFontFamily()));
    connect(m_nameEdit, &QLineEdit::textEdited, this, &CombosPage::onCustomNameEdited);

    m_heroKeyBtn = new KeyCaptureButton(QString(), 0, false, {}, true, m_state, true, m_rightCard);
    m_heroKeyBtn->setFixedHeight(40);
    connect(m_heroKeyBtn, &KeyCaptureButton::chordCaptured, this, &CombosPage::onTriggerChordCaptured);

    keyAndNameRow->addWidget(m_nameEdit);
    keyAndNameRow->addWidget(m_heroKeyBtn, 1);
    rLay->addLayout(keyAndNameRow);

    // Row 2: Target App + Execution Mode + Re-Trigger Policy / Stop Hotkey
    auto* targetRow = new QHBoxLayout();
    targetRow->setSpacing(8);

    auto* appLbl = new QLabel(I18n::t("lbl_target_app"), m_rightCard);
    appLbl->setFont(makeFont(12, QFont::Bold));
    appLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    m_appCombo = new SearchCombo(QList<QPair<QString, QString>>{}, QString(), m_rightCard);
    m_appCombo->setRichItems(runningAppsComboItems());
    m_appCombo->setMinimumWidth(165);
    connect(m_appCombo, &SearchCombo::currentChanged, this, &CombosPage::onTargetAppChanged);

    auto* btnRefreshApps = new IconButton("search", QString(), 34, "glass", m_rightCard);
    connect(btnRefreshApps, &IconButton::clicked, this, &CombosPage::refreshRunningAppsDropdown);

    auto* modeLbl = new QLabel(I18n::t("lbl_repeat_mode"), m_rightCard);
    modeLbl->setFont(makeFont(12, QFont::Bold));
    modeLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    m_modeSeg = new SearchCombo(
        {
            {"once", I18n::t("mode_once")},
            {"repeat_hold", I18n::t("mode_repeat_hold")},
            {"toggle_loop", I18n::t("mode_toggle_loop")},
        },
        "once",
        m_rightCard
    );
    m_modeSeg->setMinimumWidth(145);
    connect(m_modeSeg, &SearchCombo::currentChanged, this, &CombosPage::onModeChanged);

    m_retrigLbl = new QLabel(I18n::t("lbl_retrigger_policy"), m_rightCard);
    m_retrigLbl->setFont(makeFont(12, QFont::Bold));
    m_retrigLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));

    m_retriggerSeg = new SearchCombo(
        {
            {"ignore", I18n::t("retrigger_ignore")},
            {"restart", I18n::t("retrigger_restart")},
            {"queue", I18n::t("retrigger_queue")},
        },
        "ignore",
        m_rightCard
    );
    m_retriggerSeg->setMinimumWidth(145);
    connect(m_retriggerSeg, &SearchCombo::currentChanged, this, &CombosPage::onRetriggerPolicyChanged);

    m_lblStopHk = new QLabel(I18n::t("lbl_combo_stop_hotkey"), m_rightCard);
    m_lblStopHk->setFont(makeFont(12, QFont::ExtraBold));
    m_lblStopHk->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("danger")));

    m_stopHkBtn = new KeyCaptureButton(QString(), 0, false, {}, true, m_state, false, m_rightCard);
    m_stopHkBtn->setFixedHeight(32);
    m_stopHkBtn->setMinimumWidth(140);
    connect(m_stopHkBtn, &KeyCaptureButton::chordCaptured, this, &CombosPage::onStopHotkeyCaptured);

    targetRow->addWidget(appLbl);
    targetRow->addWidget(m_appCombo, 2);
    targetRow->addWidget(btnRefreshApps);
    targetRow->addSpacing(4);
    targetRow->addWidget(modeLbl);
    targetRow->addWidget(m_modeSeg, 1);
    targetRow->addSpacing(4);
    targetRow->addWidget(m_retrigLbl);
    targetRow->addWidget(m_retriggerSeg, 1);
    targetRow->addWidget(m_lblStopHk);
    targetRow->addWidget(m_stopHkBtn, 1);
    rLay->addLayout(targetRow);

    // Row 2b: Multi-Application Exclusion Bar
    auto* exclRow = new QHBoxLayout();
    exclRow->setSpacing(8);

    auto* exclLbl = new QLabel(I18n::t("lbl_excluded_apps"), m_rightCard);
    exclLbl->setFont(makeFont(12, QFont::ExtraBold));
    exclLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("danger")));

    m_excludeAppCombo = new SearchCombo(QList<QPair<QString, QString>>{}, QString(), m_rightCard);
    m_excludeAppCombo->setMinimumWidth(240);
    m_excludeAppCombo->setFixedHeight(32);

    m_btnAddExclApp = new IconButton("plus", I18n::t("btn_add_excluded_app"), 32, "danger", m_rightCard);
    connect(m_btnAddExclApp, &IconButton::clicked, this, &CombosPage::onAddExcludedApp);

    m_excludedScrollBox = new TintedScrollBox("#EF4444", m_rightCard);
    m_excludedScrollBox->setFixedHeight(40);
    auto* exBoxLay = new QHBoxLayout(m_excludedScrollBox);
    exBoxLay->setContentsMargins(6, 2, 6, 2);
    exBoxLay->setSpacing(0);

    m_excludedChipsScroll = new QScrollArea(m_excludedScrollBox);
    m_excludedChipsScroll->setWidgetResizable(true);
    m_excludedChipsScroll->setFrameShape(QFrame::NoFrame);
    m_excludedChipsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_excludedChipsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_excludedChipsScroll->setStyleSheet(modernScrollStyle());

    m_excludedChipsW = new QWidget();
    m_excludedChipsW->setStyleSheet("background: transparent;");
    m_excludedChipsLay = new QHBoxLayout(m_excludedChipsW);
    m_excludedChipsLay->setContentsMargins(4, 1, 4, 1);
    m_excludedChipsLay->setSpacing(6);
    m_excludedChipsScroll->setWidget(m_excludedChipsW);
    exBoxLay->addWidget(m_excludedChipsScroll);

    m_btnClearExclApps = new IconButton("trash", I18n::t("btn_clear_excluded_apps"), 32, "ghost", m_rightCard);
    connect(m_btnClearExclApps, &IconButton::clicked, this, &CombosPage::onClearExcludedApps);
    m_btnClearExclApps->hide();

    exclRow->addWidget(exclLbl);
    exclRow->addWidget(m_excludeAppCombo, 2);
    exclRow->addWidget(m_btnAddExclApp);
    exclRow->addWidget(m_excludedScrollBox, 2);
    exclRow->addWidget(m_btnClearExclApps);
    rLay->addLayout(exclRow);

    // Steps Hidden Summary Holder (takes stretch=1 when steps are hidden so top panels NEVER stretch!)
    m_comboStepsHiddenHolder = new QWidget(m_rightCard);
    m_comboStepsHiddenHolder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_comboStepsHiddenHolder->setStyleSheet("background: transparent;");
    auto* cshOuter = new QVBoxLayout(m_comboStepsHiddenHolder);
    cshOuter->setContentsMargins(0, 0, 0, 0);
    cshOuter->setSpacing(0);

    m_comboStepsHiddenCard = new GlassCard(false, m_comboStepsHiddenHolder);
    m_comboStepsHiddenCard->setFixedHeight(62);
    auto* cshLay = new QHBoxLayout(m_comboStepsHiddenCard);
    cshLay->setContentsMargins(16, 10, 16, 10);
    cshLay->setSpacing(12);

    m_lblComboStepsHidden = new QLabel(I18n::t("lbl_steps_hidden_hint"), m_comboStepsHiddenCard);
    m_lblComboStepsHidden->setWordWrap(true);
    m_lblComboStepsHidden->setFont(makeFont(12, QFont::DemiBold));
    m_lblComboStepsHidden->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));

    m_btnComboOpenSteps = new Button(I18n::t("btn_edit_steps"), "primary", "bolt", 34, m_comboStepsHiddenCard);
    connect(m_btnComboOpenSteps, &Button::clicked, this, [this]() { setComboStepsVisible(true); });

    cshLay->addWidget(m_lblComboStepsHidden, 1);
    cshLay->addWidget(m_btnComboOpenSteps);
    cshOuter->addWidget(m_comboStepsHiddenCard);
    cshOuter->addStretch(1);
    rLay->addWidget(m_comboStepsHiddenHolder, 1);

    // Edit & History Toolbar (shown when editing steps)
    m_comboEditToolbar = new GlassCard(true, m_rightCard);
    m_comboEditToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* cetLay = new QHBoxLayout(m_comboEditToolbar);
    cetLay->setContentsMargins(12, 6, 12, 6);
    cetLay->setSpacing(6);

    m_s2Lbl = new QLabel(I18n::t("step2_header"), m_comboEditToolbar);
    m_s2Lbl->setWordWrap(true);
    m_s2Lbl->setMinimumWidth(60);
    m_s2Lbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_s2Lbl->setFont(makeFont(12, QFont::ExtraBold));
    m_s2Lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_btnComboUndo = new Button(I18n::t("btn_undo"), "ghost", "chevron_up", 30, m_comboEditToolbar);
    connect(m_btnComboUndo, &Button::clicked, this, &CombosPage::undoComboSteps);

    m_btnComboRedo = new Button(I18n::t("btn_redo"), "ghost", "chevron_down", 30, m_comboEditToolbar);
    connect(m_btnComboRedo, &Button::clicked, this, &CombosPage::redoComboSteps);

    m_btnComboSaveCurr = new Button(I18n::t("btn_save_changes"), "primary", "save", 30, m_comboEditToolbar);
    connect(m_btnComboSaveCurr, &Button::clicked, this, &CombosPage::saveComboChangesNow);

    m_btnComboSaveNew = new Button(I18n::t("btn_save_as_new"), "glass", "plus", "#10B981", false, m_comboEditToolbar);
    m_btnComboSaveNew->setFixedHeight(30);
    connect(m_btnComboSaveNew, &Button::clicked, this, &CombosPage::saveComboAsNew);

    m_btnComboHideSteps = new Button(I18n::t("btn_hide_steps"), "ghost", "close", 30, m_comboEditToolbar);
    connect(m_btnComboHideSteps, &Button::clicked, this, [this]() { setComboStepsVisible(false); });

    cetLay->addWidget(m_s2Lbl, 1);
    cetLay->addWidget(m_btnComboUndo);
    cetLay->addWidget(m_btnComboRedo);
    cetLay->addWidget(m_btnComboSaveCurr);
    cetLay->addWidget(m_btnComboSaveNew);
    cetLay->addWidget(m_btnComboHideSteps);
    rLay->addWidget(m_comboEditToolbar);

    // 9 Tinted Step Adder Buttons inside ResponsiveFlowBar
    auto* bAddKey = new Button(I18n::t("step_add_key_tap"), "glass", "keyboard", stepCategoryColor("key_tap"), false, m_rightCard);
    connect(bAddKey, &Button::clicked, this, [this]() { addStep("key_tap", "W", 20, 0); });

    auto* bAddWait = new Button(I18n::t("step_add_wait"), "glass", "clock", stepCategoryColor("wait"), false, m_rightCard);
    connect(bAddWait, &Button::clicked, this, [this]() { addStep("wait", "Wait", 1000); });

    auto* bAddClick = new Button(I18n::t("step_add_mouse_click"), "glass", "mouse", stepCategoryColor("mouse_click"), false, m_rightCard);
    connect(bAddClick, &Button::clicked, this, [this]() { addStep("mouse_click", "left", 25); });

    auto* bAddScroll = new Button(I18n::t("step_add_mouse_scroll"), "glass", "mouse", stepCategoryColor("mouse_scroll"), false, m_rightCard);
    connect(bAddScroll, &Button::clicked, this, [this]() { addStep("mouse_scroll", "down", 25, 0, 3); });

    auto* bAddTypeText = new Button(I18n::t("step_add_type_text"), "glass", "keyboard", stepCategoryColor("type_text"), false, m_rightCard);
    connect(bAddTypeText, &Button::clicked, this, [this]() { addStep("type_text", "Hello V-MacroPlus", 20); });

    auto* bAddPy = new Button(I18n::t("step_add_python_script"), "glass", "bolt", stepCategoryColor("python_script"), false, m_rightCard);
    connect(bAddPy, &Button::clicked, this, [this]() {
        addStep("python_script", "for i in range(3):\n    print(f\"Step {i+1}\")\n    press(\"Enter\")\n    wait(200)", 20);
    });

    auto* bAddFolder = new Button(I18n::t("step_add_folder"), "glass", "folder", stepCategoryColor("open_folder"), false, m_rightCard);
    connect(bAddFolder, &Button::clicked, this, [this]() { addStep("open_folder", "C:\\", 20); });

    auto* bAddPs = new Button(I18n::t("step_add_ps"), "glass", "terminal", stepCategoryColor("powershell"), false, m_rightCard);
    connect(bAddPs, &Button::clicked, this, [this]() { addStep("powershell", "Write-Host 'Hello'", 20); });

    auto* bAddCmd = new Button(I18n::t("step_add_cmd"), "glass", "terminal", stepCategoryColor("cmd"), false, m_rightCard);
    connect(bAddCmd, &Button::clicked, this, [this]() { addStep("cmd", "echo Hello", 20); });

    m_stepAdderBar = new ResponsiveFlowBar(
        {bAddKey, bAddWait, bAddClick, bAddScroll, bAddTypeText, bAddPy, bAddFolder, bAddPs, bAddCmd},
        32, 6, 6, m_rightCard
    );
    rLay->addWidget(m_stepAdderBar);

    // Scrollable Steps Area
    m_stepsScroll = new QScrollArea(m_rightCard);
    m_stepsScroll->setWidgetResizable(true);
    m_stepsScroll->setFrameShape(QFrame::NoFrame);
    m_stepsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_stepsScroll->setStyleSheet(modernScrollStyle());

    m_stepsContainer = new QWidget();
    m_stepsContainer->setStyleSheet("background: transparent;");
    m_stepsVbox = new QVBoxLayout(m_stepsContainer);
    m_stepsVbox->setContentsMargins(0, 0, 6, 0);
    m_stepsVbox->setSpacing(6);
    m_stepsScroll->setWidget(m_stepsContainer);
    connect(m_stepsScroll->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int val) {
        if (m_stepsScroll && val >= m_stepsScroll->verticalScrollBar()->maximum() - 260) {
            loadMoreStepRows();
        }
    });
    rLay->addWidget(m_stepsScroll, 1);

    setComboStepsVisible(false);

    // STEP ③ Footer Bar
    auto* s3Bar = new QHBoxLayout();
    s3Bar->setSpacing(10);

    auto* s3Lbl = new QLabel(I18n::t("step3_header"), m_rightCard);
    s3Lbl->setFont(makeFont(13, QFont::ExtraBold));
    s3Lbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_comboEnableTog = new ToggleSwitch(false, m_rightCard);
    connect(m_comboEnableTog, &ToggleSwitch::toggled, this, &CombosPage::onCurrentEnabledToggled);

    auto* afterLbl = new QLabel(I18n::t("lbl_run_after"), m_rightCard);
    afterLbl->setFont(makeFont(12, QFont::Bold));
    afterLbl->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));

    m_numStartDelay = new NumberInput(1000, 0, 60000, 500, "ms", 132, m_rightCard);
    connect(m_numStartDelay, &NumberInput::valueChanged, this, &CombosPage::onStartDelayChanged);

    m_btnDelCombo = new Button(I18n::t("btn_delete"), "danger", "trash", 38, m_rightCard);
    connect(m_btnDelCombo, &Button::clicked, this, &CombosPage::deleteSelectedCombo);

    s3Bar->addWidget(s3Lbl);
    s3Bar->addWidget(m_comboEnableTog);
    s3Bar->addStretch(1);
    s3Bar->addWidget(afterLbl);
    s3Bar->addWidget(m_numStartDelay);
    s3Bar->addWidget(m_btnDelCombo);
    rLay->addLayout(s3Bar);

    body->addWidget(m_rightCard, 1);
    root->addLayout(body, 1);
}

void CombosPage::connectSignals() {
    if (!m_state) return;
    connect(m_state, &AppState::combosListUpdated, this, &CombosPage::onCombosUpdated);
    connect(m_state, &AppState::combosMasterChanged, this, [this](bool v) {
        if (m_masterTog) m_masterTog->setChecked(v, false);
    });
    connect(m_state, &AppState::cursorPickHotkeyPressed, this, &CombosPage::onPageF8Captured);
}

SmartCombo* CombosPage::currentCombo() {
    if (m_state && m_selectedIdx >= 0 && m_selectedIdx < m_state->smartCombos.size()) {
        return &m_state->smartCombos[m_selectedIdx];
    }
    return nullptr;
}

void CombosPage::autoSave() {
    if (m_engine) m_engine->saveCombos();
    refreshLeftList();
}

void CombosPage::toggleMacrosSidebar() {
    m_sidebarCollapsed = !m_sidebarCollapsed;
    if (m_state) m_state->combosSidebarCollapsed = m_sidebarCollapsed;
    if (m_leftCard) m_leftCard->setVisible(!m_sidebarCollapsed);
    if (m_btnToggleSidebar) {
        m_btnToggleSidebar->setText(I18n::t(m_sidebarCollapsed ? "btn_show_macros_sidebar" : "btn_hide_macros_sidebar"));
    }
}

void CombosPage::exportFullBackup() {
    if (!m_engine) return;
    const QString path = QFileDialog::getSaveFileName(this, I18n::t("btn_export_all"), "V-MacroPlus_Full_Backup.json", "JSON Backup (*.json)");
    if (!path.isEmpty()) m_engine->exportAllBackup(path);
}

void CombosPage::importFullBackup() {
    if (!m_engine) return;
    const QString path = QFileDialog::getOpenFileName(this, I18n::t("btn_import_all"), QString(), "JSON Backup (*.json)");
    if (!path.isEmpty()) m_engine->importAllBackup(path);
}

void CombosPage::onCombosUpdated() {
    refreshLeftList();
    loadSelectedIntoEditor();
}

void CombosPage::refreshLeftList() {
    if (!m_listVbox || !m_state) return;
    while (QLayoutItem* it = m_listVbox->takeAt(0)) {
        if (QWidget* w = it->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete it;
    }

    const bool isAr = I18n::isRtl();
    for (int idx = 0; idx < m_state->smartCombos.size(); ++idx) {
        const SmartCombo& scObj = m_state->smartCombos[idx];
        const QString shortTitle = scObj.sidebarLabel(isAr);

        auto* rowW = new QWidget(m_listContainer);
        auto* rl = new QHBoxLayout(rowW);
        rl->setContentsMargins(0, 1, 0, 1);
        rl->setSpacing(6);

        auto* tog = new ToggleSwitch(scObj.enabled, rowW);
        connect(tog, &ToggleSwitch::toggled, this, [this, idx](bool v) { toggleListItem(idx, v); });

        auto* btn = new Button(shortTitle, idx == m_selectedIdx ? "primary" : "ghost", QString(), QString(), true, rowW);
        btn->setFixedHeight(34);
        connect(btn, &Button::clicked, this, [this, idx]() { selectCombo(idx); });

        auto* btnRunSidebar = new IconButton(
            "play",
            I18n::tl(QString::fromUtf8("تشغيل «%1» مرة واحدة الآن").arg(shortTitle),
                     QStringLiteral("Run '%1' once now").arg(shortTitle)),
            32,
            "ok",
            rowW
        );
        connect(btnRunSidebar, &IconButton::clicked, this, [this, idx]() { runComboFromSidebar(idx); });

        rl->addWidget(tog);
        rl->addWidget(btn, 1);
        rl->addWidget(btnRunSidebar);
        m_listVbox->addWidget(rowW);
    }
    m_listVbox->addStretch(1);
}

void CombosPage::runComboFromSidebar(int idx) {
    if (!m_state || !m_engine || idx < 0 || idx >= m_state->smartCombos.size()) return;
    const SmartCombo& c = m_state->smartCombos[idx];
    if (c.steps.isEmpty()) return;
    if ((c.mode == "repeat_hold" || c.mode == "toggle_loop") && c.stopHotkey.trimmed().isEmpty()) {
        selectCombo(idx);
        emit m_state->toastRequested(I18n::t("err_combo_stop_hotkey_required"), "danger");
        return;
    }
    const int delay = c.startDelayMs;
    const QString title = c.sidebarLabel(I18n::isRtl());
    emit m_state->toastRequested(
        I18n::tl(QString::fromUtf8("سيتم تشغيل «%1» مرة واحدة بعد %2 ms...").arg(title).arg(delay),
                 QStringLiteral("Running '%1' once in %2 ms...").arg(title).arg(delay)),
        "ok"
    );
    m_engine->executeSmartCombo(c, {}, delay);
}

void CombosPage::pushComboUndo() {
    SmartCombo* c = currentCombo();
    if (!c) return;
    m_undoStepsStack.append(c->steps);
    if (m_undoStepsStack.size() > 40) m_undoStepsStack.removeFirst();
    m_redoStepsStack.clear();
    syncComboUndoRedo();
}

void CombosPage::syncComboUndoRedo() {
    if (m_btnComboUndo) m_btnComboUndo->setEnabled(!m_undoStepsStack.isEmpty());
    if (m_btnComboRedo) m_btnComboRedo->setEnabled(!m_redoStepsStack.isEmpty());
}

void CombosPage::undoComboSteps() {
    SmartCombo* c = currentCombo();
    if (m_undoStepsStack.isEmpty()) {
        if (m_hasDeletedCombo) undoLastDelete();
        return;
    }
    if (c) {
        m_redoStepsStack.append(c->steps);
        c->steps = m_undoStepsStack.takeLast();
        rebuildStepsUi();
        autoSave();
        syncComboUndoRedo();
    }
}

void CombosPage::redoComboSteps() {
    SmartCombo* c = currentCombo();
    if (m_redoStepsStack.isEmpty() || !c) return;
    m_undoStepsStack.append(c->steps);
    c->steps = m_redoStepsStack.takeLast();
    rebuildStepsUi();
    autoSave();
    syncComboUndoRedo();
}

void CombosPage::setComboStepsVisible(bool visible) {
    m_stepsEditorVisible = visible;
    if (m_comboStepsHiddenHolder) m_comboStepsHiddenHolder->setVisible(!visible);
    if (m_comboEditToolbar) m_comboEditToolbar->setVisible(visible);
    if (m_stepAdderBar) m_stepAdderBar->setVisible(visible);
    if (m_stepsScroll) m_stepsScroll->setVisible(visible);
    SmartCombo* c = currentCombo();
    const int cnt = c ? c->steps.size() : 0;
    if (!visible && m_lblComboStepsHidden) {
        m_lblComboStepsHidden->setText(
            QStringLiteral("%1 (%2 %3)")
                .arg(I18n::t("lbl_steps_hidden_hint"), I18n::fmtNum(QString::number(cnt)), I18n::t("unit_steps"))
        );
    } else {
        syncComboUndoRedo();
    }
}

void CombosPage::saveComboChangesNow() {
    autoSave();
    if (m_state) emit m_state->toastRequested(I18n::t("toast_changes_saved"), "ok");
}

void CombosPage::saveComboAsNew() {
    SmartCombo* c = currentCombo();
    if (!c || !m_state) return;
    SmartCombo dup = *c;
    dup.id = QStringLiteral("combo_%1").arg(QDateTime::currentMSecsSinceEpoch());
    dup.name = QStringLiteral("%1 (Copy)").arg(c->name.isEmpty() ? "Macro" : c->name);
    dup.triggerKey.clear();
    dup.triggerScan = 0;
    dup.triggerChord.clear();
    dup.enabled = false;
    m_state->smartCombos.prepend(dup);
    m_selectedIdx = 0;
    autoSave();
    loadSelectedIntoEditor();
    emit m_state->toastRequested(I18n::t("toast_changes_saved"), "ok");
}

void CombosPage::selectCombo(int idx) {
    m_selectedIdx = idx;
    m_undoStepsStack.clear();
    m_redoStepsStack.clear();
    m_stepsEditorVisible = false;
    refreshLeftList();
    loadSelectedIntoEditor();
}

void CombosPage::toggleListItem(int idx, bool enabled) {
    if (!m_state || idx < 0 || idx >= m_state->smartCombos.size()) return;
    SmartCombo& c = m_state->smartCombos[idx];
    if (enabled && c.triggerKey.isEmpty()) {
        emit m_state->toastRequested(
            I18n::tl(QString::fromUtf8("هذا الماكرو يدوي بدون زرار — اضغط على زر ▶ الأخضر بجانبه لتشغيله!"),
                     QStringLiteral("Manual macro — click the green ▶ button next to it to run!")),
            "info"
        );
        selectCombo(idx);
        refreshLeftList();
        return;
    }
    if (enabled && (c.mode == "repeat_hold" || c.mode == "toggle_loop") && c.stopHotkey.trimmed().isEmpty()) {
        emit m_state->toastRequested(I18n::t("err_combo_stop_hotkey_required"), "danger");
        selectCombo(idx);
        refreshLeftList();
        return;
    }
    c.enabled = enabled;
    if (idx == m_selectedIdx && m_comboEnableTog) {
        m_comboEnableTog->setChecked(enabled, false);
    }
    if (m_engine) m_engine->saveCombos();
}

void CombosPage::loadSelectedIntoEditor() {
    SmartCombo* c = currentCombo();
    if (!c) {
        if (m_rightCard) m_rightCard->hide();
        if (m_emptyStateCard) m_emptyStateCard->show();
        return;
    }

    if (m_emptyStateCard) m_emptyStateCard->hide();
    if (m_rightCard) m_rightCard->show();

    if (m_nameEdit) m_nameEdit->setText(QString(c->name).replace("Key_0", "Calculator"));
    if (m_heroKeyBtn) m_heroKeyBtn->setKeyData(c->triggerKey, c->triggerScan, c->triggerExtended, c->triggerChord);

    const QString curMode = c->mode.isEmpty() ? QStringLiteral("once") : c->mode;
    const QString stopHk = c->stopHotkey.trimmed();
    if ((curMode == "repeat_hold" || curMode == "toggle_loop") && stopHk.isEmpty() && c->enabled) {
        c->enabled = false;
    }
    if (m_comboEnableTog) m_comboEnableTog->setChecked(c->enabled, false);
    if (m_modeSeg) m_modeSeg->setSelectedKey(curMode, false);
    if (m_retriggerSeg) m_retriggerSeg->setSelectedKey(c->retriggerPolicy.isEmpty() ? "ignore" : c->retriggerPolicy, false);
    if (m_stopHkBtn) m_stopHkBtn->setKeyData(stopHk, 0, false, {});
    syncModeStopHotkeyUi(curMode);
    if (m_numStartDelay) m_numStartDelay->setValue(c->startDelayMs, false);
    refreshRunningAppsDropdown();
    rebuildStepsUi();
    setComboStepsVisible(m_stepsEditorVisible);
}

void CombosPage::rebuildStepsUi() {
    m_stepRows.clear();
    m_renderedStepsCount = 0;
    if (!m_stepsVbox) return;
    while (QLayoutItem* it = m_stepsVbox->takeAt(0)) {
        if (QWidget* w = it->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete it;
    }

    SmartCombo* c = currentCombo();
    if (!c) return;

    m_stepsVbox->addStretch(1);
    loadMoreStepRows();
}

void CombosPage::loadMoreStepRows() {
    SmartCombo* c = currentCombo();
    if (!c || !m_stepsVbox) return;
    const int total = c->steps.size();
    if (m_renderedStepsCount >= total) return;

    if (m_stepsVbox->count() > 0) {
        QLayoutItem* lastIt = m_stepsVbox->itemAt(m_stepsVbox->count() - 1);
        if (lastIt && !lastIt->widget()) {
            delete m_stepsVbox->takeAt(m_stepsVbox->count() - 1);
        }
    }

    const int batchEnd = std::min(total, m_renderedStepsCount + 150);
    m_stepsContainer->setUpdatesEnabled(false);
    for (int idx = m_renderedStepsCount; idx < batchEnd; ++idx) {
        auto* row = new ComboStepRowWidget(idx, &c->steps[idx], m_state, m_stepsContainer);
        connect(row, &ComboStepRowWidget::changed, this, &CombosPage::autoSave);
        connect(row, &ComboStepRowWidget::removed, this, &CombosPage::removeStepAt);
        connect(row, &ComboStepRowWidget::moveUp, this, &CombosPage::moveStepUp);
        connect(row, &ComboStepRowWidget::moveDown, this, &CombosPage::moveStepDown);
        connect(row, &ComboStepRowWidget::testSingleStep, this, &CombosPage::testSingleStepAt);
        connect(row, &ComboStepRowWidget::dragMoveRequested, this, &CombosPage::onStepDragMoved);
        connect(row, &ComboStepRowWidget::dragFinished, this, &CombosPage::autoSave);
        m_stepsVbox->addWidget(row);
        m_stepRows.append(row);
    }
    m_renderedStepsCount = batchEnd;
    m_stepsVbox->addStretch(1);
    m_stepsContainer->setUpdatesEnabled(true);
}

void CombosPage::onPageF8Captured(int cx, int cy) {
    if (!m_state || m_state->isRecording || m_state->currentPageId != "combos") return;
    for (auto* row : m_stepRows) {
        if (row && row->isPickingLiveXy()) return;
    }
    for (int i = m_stepRows.size() - 1; i >= 0; --i) {
        if (m_stepRows[i] && m_stepRows[i]->isMouseRow()) {
            m_stepRows[i]->applyPickedCoords(cx, cy);
            emit m_state->toastRequested(
                I18n::tl(QString::fromUtf8("✓ تم التقاط إحداثيات الماوس: (%1, %2)").arg(cx).arg(cy),
                         QStringLiteral("✓ Picked mouse coordinates: (%1, %2)").arg(cx).arg(cy)),
                QStringLiteral("ok")
            );
            return;
        }
    }
    SmartCombo* c = currentCombo();
    if (!c) return;
    pushComboUndo();
    ComboStep step;
    step.stepType = "mouse_click";
    step.value = "left";
    step.delayMs = 25;
    step.holdMs = 0;
    step.x = cx;
    step.y = cy;
    step.useCoords = true;
    c->steps.append(step);
    setComboStepsVisible(true);
    rebuildStepsUi();
    autoSave();
    emit m_state->toastRequested(
        I18n::tl(QString::fromUtf8("✓ تمت إضافة كليك ماوس عند الإحداثيات: (%1, %2)").arg(cx).arg(cy),
                 QStringLiteral("✓ Added Mouse Click at: (%1, %2)").arg(cx).arg(cy)),
        QStringLiteral("ok")
    );
}

void CombosPage::onStepDragMoved(ComboStepRowWidget* draggedRow, int globalY) {
    SmartCombo* c = currentCombo();
    if (!c || !draggedRow || !m_stepRows.contains(draggedRow)) return;

    const int curIdx = m_stepRows.indexOf(draggedRow);
    const int localY = m_stepsContainer->mapFromGlobal(QPoint(0, globalY)).y();

    int targetIdx = curIdx;
    for (int i = 0; i < m_stepRows.size(); ++i) {
        ComboStepRowWidget* rw = m_stepRows[i];
        const int ry = getRowLogicalY(rw);
        const int rh = std::max(24, rw->height());
        if (i < curIdx && localY < (ry + rh * 0.55)) {
            targetIdx = i;
            break;
        } else if (i > curIdx && localY > (ry + rh * 0.45)) {
            targetIdx = i;
        }
    }

    if (targetIdx != curIdx && targetIdx >= 0 && targetIdx < c->steps.size()) {
        QList<QWidget*> widgetList;
        for (auto* r : m_stepRows) widgetList.append(r);
        if (smoothReorderVBoxWidget(m_stepsVbox, widgetList, curIdx, targetIdx)) {
            pushComboUndo();
            ComboStepRowWidget* movedRow = m_stepRows.takeAt(curIdx);
            m_stepRows.insert(targetIdx, movedRow);
            ComboStep movedStep = c->steps.takeAt(curIdx);
            c->steps.insert(targetIdx, movedStep);
            for (int i = 0; i < m_stepRows.size(); ++i) {
                m_stepRows[i]->setStepIndex(i);
            }
        }
    }
}

void CombosPage::onCustomNameEdited(const QString& txt) {
    if (SmartCombo* c = currentCombo()) {
        c->name = txt.trimmed();
        autoSave();
    }
}

void CombosPage::onTriggerChordCaptured(const QList<QPair<int, bool>>& chord, const QString& lbl) {
    SmartCombo* c = currentCombo();
    if (!c) return;
    c->triggerKey = lbl;
    c->triggerChord = chord;
    if (!chord.isEmpty()) {
        c->triggerScan = chord.last().first;
        c->triggerExtended = chord.last().second;
        const bool isRep = (c->mode == "repeat_hold" || c->mode == "toggle_loop");
        const bool hasStop = !c->stopHotkey.trimmed().isEmpty();
        const bool canEn = (!isRep) || hasStop;
        c->enabled = canEn;
        if (m_comboEnableTog) m_comboEnableTog->setChecked(canEn, false);
        if (isRep && !hasStop && m_state) {
            emit m_state->toastRequested(I18n::t("err_combo_stop_hotkey_required"), "danger");
        }
        if (c->name.isEmpty()) {
            c->name = lbl;
            if (m_nameEdit) m_nameEdit->setText(lbl);
        }
    } else {
        c->triggerScan = 0;
        c->triggerExtended = false;
        c->enabled = false;
        if (m_comboEnableTog) m_comboEnableTog->setChecked(false, false);
    }
    autoSave();
}

void CombosPage::clearTriggerForManualMode() {
    if (m_heroKeyBtn) m_heroKeyBtn->clearKey();
    if (SmartCombo* c = currentCombo()) {
        c->triggerKey.clear();
        c->triggerScan = 0;
        c->triggerExtended = false;
        c->triggerChord.clear();
        c->enabled = false;
        if (c->name.isEmpty()) {
            c->name = I18n::tl(QString::fromUtf8("ماكرو يدوي"), QStringLiteral("Manual Macro"));
            if (m_nameEdit) m_nameEdit->setText(c->name);
        }
        if (m_comboEnableTog) m_comboEnableTog->setChecked(false, false);
        autoSave();
    }
}

void CombosPage::addStep(const QString& stepType, const QString& value, int delayMs, int holdMs, int scrollAmount) {
    SmartCombo* c = currentCombo();
    if (!c) return;
    pushComboUndo();
    QPair<int, bool> scExt = (stepType == "key_tap" || stepType == "key_hold" || stepType == "key_down" || stepType == "key_up")
        ? Win32Input::scancodeForLabel(value)
        : qMakePair(0, false);
    QPoint pt = (stepType == "mouse_click" || stepType == "mouse_down" || stepType == "mouse_up" || stepType == "mouse_move")
        ? Win32Input::getCursorPos()
        : QPoint(500, 500);

    ComboStep step;
    step.stepType = stepType;
    step.value = value;
    step.scanCode = scExt.first;
    step.isExtended = scExt.second;
    step.delayMs = delayMs;
    step.holdMs = holdMs;
    step.scrollAmount = scrollAmount;
    step.x = pt.x();
    step.y = pt.y();
    step.useCoords = true;
    c->steps.append(step);
    rebuildStepsUi();
    autoSave();
}

void CombosPage::removeStepAt(int idx) {
    SmartCombo* c = currentCombo();
    if (!c || idx < 0 || idx >= c->steps.size()) return;
    pushComboUndo();
    c->steps.removeAt(idx);
    rebuildStepsUi();
    autoSave();
}

void CombosPage::moveStepUp(int idx) {
    SmartCombo* c = currentCombo();
    if (!c || idx <= 0 || idx >= c->steps.size()) return;
    pushComboUndo();
    c->steps.swapItemsAt(idx - 1, idx);
    QList<QWidget*> widgetList;
    for (auto* r : m_stepRows) widgetList.append(r);
    if (smoothReorderVBoxWidget(m_stepsVbox, widgetList, idx, idx - 1)) {
        m_stepRows.swapItemsAt(idx - 1, idx);
        for (int i = 0; i < m_stepRows.size(); ++i) m_stepRows[i]->setStepIndex(i);
    } else {
        rebuildStepsUi();
    }
    autoSave();
}

void CombosPage::moveStepDown(int idx) {
    SmartCombo* c = currentCombo();
    if (!c || idx < 0 || idx >= c->steps.size() - 1) return;
    pushComboUndo();
    c->steps.swapItemsAt(idx + 1, idx);
    QList<QWidget*> widgetList;
    for (auto* r : m_stepRows) widgetList.append(r);
    if (smoothReorderVBoxWidget(m_stepsVbox, widgetList, idx, idx + 1)) {
        m_stepRows.swapItemsAt(idx + 1, idx);
        for (int i = 0; i < m_stepRows.size(); ++i) m_stepRows[i]->setStepIndex(i);
    } else {
        rebuildStepsUi();
    }
    autoSave();
}

void CombosPage::testSingleStepAt(int idx) {
    SmartCombo* c = currentCombo();
    if (!c || !m_engine || idx < 0 || idx >= c->steps.size()) return;
    const int delay = m_numStartDelay ? m_numStartDelay->value() : 1000;
    if (m_state) {
        emit m_state->toastRequested(
            I18n::tl(QString::fromUtf8("سيتم تنفيذ الخطوة #%1 بعد %2 ms...").arg(idx + 1).arg(delay),
                     QStringLiteral("Running step #%1 in %2 ms...").arg(idx + 1).arg(delay)),
            "info"
        );
    }
    m_engine->executeSingleStepAfter(c->steps[idx], delay);
}

void CombosPage::onStartDelayChanged(int ms) {
    if (SmartCombo* c = currentCombo()) {
        c->startDelayMs = ms;
        autoSave();
    }
}

void CombosPage::createNewBlankCombo() {
    if (!m_state) return;
    const auto wSc = Win32Input::scancodeForLabel("W");
    const auto spSc = Win32Input::scancodeForLabel("Space");

    SmartCombo newC;
    newC.id = QStringLiteral("combo_%1").arg(QDateTime::currentMSecsSinceEpoch());
    newC.name = QStringLiteral("Macro %1").arg(m_state->smartCombos.size() + 1);
    newC.mode = "once";
    newC.startDelayMs = 1000;
    newC.enabled = false;

    ComboStep s1; s1.stepType = "key_tap"; s1.value = "W"; s1.scanCode = wSc.first; s1.isExtended = wSc.second; s1.delayMs = 20;
    ComboStep s2; s2.stepType = "wait"; s2.value = "Wait"; s2.delayMs = 1000;
    ComboStep s3; s3.stepType = "key_tap"; s3.value = "Space"; s3.scanCode = spSc.first; s3.isExtended = spSc.second; s3.delayMs = 20;
    newC.steps = {s1, s2, s3};

    m_state->smartCombos.prepend(newC);
    m_selectedIdx = 0;
    loadSelectedIntoEditor();
    autoSave();
}

void CombosPage::deleteSelectedCombo() {
    if (!m_state || m_selectedIdx < 0 || m_selectedIdx >= m_state->smartCombos.size()) return;
    m_lastDeletedCombo = m_state->smartCombos.takeAt(m_selectedIdx);
    m_hasDeletedCombo = true;
    if (m_btnUndoDelete) m_btnUndoDelete->show();
    m_selectedIdx = std::max(0, std::min(static_cast<int>(m_state->smartCombos.size()) - 1, m_selectedIdx));
    if (m_engine) m_engine->saveCombos();
    refreshLeftList();
    loadSelectedIntoEditor();
}

void CombosPage::undoLastDelete() {
    if (m_hasDeletedCombo && m_state) {
        m_state->smartCombos.insert(qBound(0, m_selectedIdx, static_cast<int>(m_state->smartCombos.size())), m_lastDeletedCombo);
        m_hasDeletedCombo = false;
        if (m_btnUndoDelete) m_btnUndoDelete->hide();
        if (m_engine) m_engine->saveCombos();
        refreshLeftList();
        loadSelectedIntoEditor();
    }
}

void CombosPage::refreshRunningAppsDropdown() {
    SmartCombo* c = currentCombo();
    const QString curApp = c ? c->targetProcess : QString();
    QList<SearchComboItem> apps = runningAppsComboItems();
    bool foundCur = false;
    for (const auto& item : apps) {
        if (item.key == curApp) { foundCur = true; break; }
    }
    if (!curApp.isEmpty() && !foundCur) {
        apps.append(SearchComboItem(curApp, curApp));
    }
    if (m_appCombo) m_appCombo->setRichItems(apps, curApp);

    QList<SearchComboItem> onlySpecific;
    for (const auto& item : apps) {
        if (!item.key.isEmpty()) onlySpecific.append(item);
    }
    if (m_excludeAppCombo) {
        if (onlySpecific.isEmpty()) {
            m_excludeAppCombo->setRichItems({SearchComboItem(QString(), I18n::t("lbl_no_excluded_apps"))}, QString());
        } else {
            m_excludeAppCombo->setRichItems(onlySpecific, onlySpecific.first().key);
        }
    }
    rebuildExcludedAppsChips();
}

void CombosPage::rebuildExcludedAppsChips() {
    if (!m_excludedChipsLay) return;
    while (QLayoutItem* it = m_excludedChipsLay->takeAt(0)) {
        if (QWidget* w = it->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete it;
    }

    SmartCombo* c = currentCombo();
    QStringList exclList;
    if (c) {
        for (const QString& x : c->excludedProcesses) {
            if (!x.trimmed().isEmpty()) exclList.append(x.trimmed());
        }
    }
    if (m_btnClearExclApps) m_btnClearExclApps->setVisible(exclList.size() >= 2);

    if (exclList.isEmpty()) {
        auto* lblNone = new QLabel(I18n::t("lbl_no_excluded_apps"), m_excludedChipsW);
        lblNone->setFont(makeFont(11, QFont::Medium));
        lblNone->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
        m_excludedChipsLay->addWidget(lblNone);
        m_excludedChipsLay->addStretch(1);
        return;
    }

    for (const QString& exeName : exclList) {
        auto* chipBtn = new Button(QString::fromUtf8("🚫 %1  ×").arg(exeName), "glass", QString(), "#EF4444", false, m_excludedChipsW);
        chipBtn->setFixedHeight(26);
        chipBtn->setFixedWidth(chipBtn->naturalCompactWidth() + 6);
        connect(chipBtn, &Button::clicked, this, [this, exeName]() { onRemoveExcludedApp(exeName); });
        m_excludedChipsLay->addWidget(chipBtn);
    }
    m_excludedChipsLay->addStretch(1);
}

void CombosPage::onAddExcludedApp() {
    SmartCombo* c = currentCombo();
    if (!c || !m_excludeAppCombo) return;
    const QString exe = m_excludeAppCombo->currentKey().trimmed();
    if (exe.isEmpty()) return;
    bool exists = false;
    for (const QString& x : c->excludedProcesses) {
        if (x.compare(exe, Qt::CaseInsensitive) == 0) { exists = true; break; }
    }
    if (!exists) {
        c->excludedProcesses.append(exe);
        rebuildExcludedAppsChips();
        autoSave();
    }
}

void CombosPage::onRemoveExcludedApp(const QString& exeName) {
    SmartCombo* c = currentCombo();
    if (!c) return;
    QStringList updated;
    for (const QString& x : c->excludedProcesses) {
        if (x.compare(exeName, Qt::CaseInsensitive) != 0) updated.append(x);
    }
    c->excludedProcesses = updated;
    rebuildExcludedAppsChips();
    autoSave();
}

void CombosPage::onClearExcludedApps() {
    SmartCombo* c = currentCombo();
    if (!c) return;
    c->excludedProcesses.clear();
    rebuildExcludedAppsChips();
    autoSave();
}

void CombosPage::onTargetAppChanged(const QString& exeName) {
    if (SmartCombo* c = currentCombo()) {
        c->targetProcess = exeName;
        autoSave();
    }
}

void CombosPage::syncModeStopHotkeyUi(const QString& modeKey) {
    const bool isRep = (modeKey == "repeat_hold" || modeKey == "toggle_loop");
    if (m_lblStopHk) m_lblStopHk->setVisible(isRep);
    if (m_stopHkBtn) m_stopHkBtn->setVisible(isRep);
    if (m_retrigLbl) m_retrigLbl->setVisible(!isRep);
    if (m_retriggerSeg) m_retriggerSeg->setVisible(!isRep);
}

void CombosPage::onStopHotkeyCaptured(const QList<QPair<int, bool>>&, const QString& lbl) {
    SmartCombo* c = currentCombo();
    if (!c) return;
    const QString clean = lbl.trimmed();
    c->stopHotkey = clean;
    if (!clean.isEmpty() && (c->mode == "repeat_hold" || c->mode == "toggle_loop") && !c->triggerKey.isEmpty()) {
        c->enabled = true;
        if (m_comboEnableTog) m_comboEnableTog->setChecked(true, false);
    } else if (clean.isEmpty() && (c->mode == "repeat_hold" || c->mode == "toggle_loop")) {
        c->enabled = false;
        if (m_comboEnableTog) m_comboEnableTog->setChecked(false, false);
    }
    autoSave();
}

void CombosPage::onModeChanged(const QString& modeKey) {
    SmartCombo* c = currentCombo();
    if (!c) return;
    c->mode = modeKey;
    syncModeStopHotkeyUi(modeKey);
    if ((modeKey == "repeat_hold" || modeKey == "toggle_loop") && c->stopHotkey.trimmed().isEmpty()) {
        c->enabled = false;
        if (m_comboEnableTog) m_comboEnableTog->setChecked(false, false);
        if (m_state) emit m_state->toastRequested(I18n::t("err_combo_stop_hotkey_required"), "danger");
    }
    autoSave();
}

void CombosPage::onRetriggerPolicyChanged(const QString& policy) {
    if (SmartCombo* c = currentCombo()) {
        c->retriggerPolicy = policy;
        if (m_engine) m_engine->saveCombos();
    }
}

void CombosPage::onCurrentEnabledToggled(bool val) {
    SmartCombo* c = currentCombo();
    if (!c) return;
    if (val && c->triggerKey.isEmpty()) {
        if (m_comboEnableTog) m_comboEnableTog->setChecked(false, false);
        if (m_state) {
            emit m_state->toastRequested(
                I18n::tl(QString::fromUtf8("هذا الماكرو يدوي بدون زرار — اضغط على زر ▶ الأخضر بجانبه في القائمة لتشغيله!"),
                         QStringLiteral("Manual macro — click the green ▶ button next to it in the sidebar!")),
                "info"
            );
        }
        return;
    }
    if (val && (c->mode == "repeat_hold" || c->mode == "toggle_loop") && c->stopHotkey.trimmed().isEmpty()) {
        if (m_comboEnableTog) m_comboEnableTog->setChecked(false, false);
        if (m_state) emit m_state->toastRequested(I18n::t("err_combo_stop_hotkey_required"), "danger");
        return;
    }
    c->enabled = val;
    autoSave();
}

} // namespace vmacro
