#include "MainWindow.h"
#include "AboutDialog.h"
#include "Icons.h"
#include "OnboardingOverlay.h"
#include "SettingsPage.h"
#include "Widgets.h"
#include "pages/AutoPresserPage.h"
#include "pages/CombosPage.h"
#include "pages/RecorderPage.h"
#include "pages/TestPage.h"
#include "../core/AppState.h"
#include "../core/I18n.h"
#include "../core/Theme.h"
#include "../engine/MacroEngine.h"
#include "../platform/Win32Input.h"

#include <QAction>
#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLinearGradient>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QScreen>
#include <QWindow>
#include <algorithm>

namespace vmacro {

static const QString kAppName = QStringLiteral("V-MacroPlus");

// ============================================================================
// RecordingFloatingIndicator
// ============================================================================

RecordingFloatingIndicator::RecordingFloatingIndicator(AppState* state)
    : QWidget(nullptr, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
      m_state(state) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_ShowWithoutActivating, true);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    m_pulseTimer = new QTimer(this);
    m_pulseTimer->setInterval(520);
    connect(m_pulseTimer, &QTimer::timeout, this, &RecordingFloatingIndicator::togglePulse);

    if (m_state) {
        m_state->registerStopButtonRectProvider([this]() -> QRect {
            if (!isVisible()) return QRect();
            return geometry();
        });
    }
}

void RecordingFloatingIndicator::togglePulse() {
    m_pulseOn = !m_pulseOn;
    update();
}

void RecordingFloatingIndicator::showIndicator() {
    m_eventCount = m_state ? m_state->recordedEvents.size() : 0;
    recalcSizeAndPosition();
    m_pulseTimer->start();
    show();
    raise();
}

void RecordingFloatingIndicator::hideIndicator() {
    m_pulseTimer->stop();
    hide();
}

void RecordingFloatingIndicator::updateCount(int count) {
    m_eventCount = count;
    if (isVisible()) {
        recalcSizeAndPosition();
        update();
    }
}

QString RecordingFloatingIndicator::labelText() const {
    if (m_state && m_state->isPlaying && !m_state->isRecording) {
        const QString hkPlay = m_state->config.hotkeyPlaybackToggle.isEmpty() ? QStringLiteral("F10") : m_state->config.hotkeyPlaybackToggle;
        const QString hkStop = m_state->config.hotkeyPanicStop.isEmpty() ? QStringLiteral("Esc") : m_state->config.hotkeyPanicStop;
        const QString hkDisp = (hkPlay.compare(hkStop, Qt::CaseInsensitive) == 0)
            ? hkPlay
            : QStringLiteral("%1 / %2").arg(hkPlay, hkStop);
        return QStringLiteral("%1 (%2)").arg(I18n::t("btn_stop_play"), hkDisp);
    }
    const QString hk = (m_state && !m_state->config.hotkeyRecordToggle.isEmpty())
        ? m_state->config.hotkeyRecordToggle
        : QStringLiteral("F9");
    if (m_eventCount > 0) {
        return QStringLiteral("%1 (%2)  •  %3")
            .arg(I18n::t("btn_stop_rec"), hk, I18n::fmtNum(QString::number(m_eventCount)));
    }
    return QStringLiteral("%1 (%2)").arg(I18n::t("btn_stop_rec"), hk);
}

void RecordingFloatingIndicator::recalcSizeAndPosition() {
    QFont font = makeFont(13, QFont::ExtraBold);
    QFontMetrics fm(font);
    const QString txt = I18n::bidiText(labelText());
    const int w = std::max(188, fm.horizontalAdvance(txt) + 56);
    const int h = 46;
    setFixedSize(w, h);

    if (QScreen* scr = QApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        const int x = avail.right() - w - 18;
        const int y = avail.bottom() - h - 18;
        move(x, y);
    }
}

void RecordingFloatingIndicator::enterEvent(QEnterEvent*) {
    m_hovered = true;
    update();
}

void RecordingFloatingIndicator::leaveEvent(QEvent*) {
    m_hovered = false;
    update();
}

void RecordingFloatingIndicator::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit stopClicked();
    }
}

void RecordingFloatingIndicator::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    QRectF r = QRectF(rect()).adjusted(2.0, 2.0, -2.0, -2.0);
    const bool isDk = Theme::instance().isDark();
    QColor bgCol = isDk ? QColor(18, 20, 32, 245) : QColor(255, 255, 255, 248);
    QColor borderCol = m_hovered ? Theme::instance().c("danger") : Theme::instance().c("accent");
    if (m_hovered) bgCol = Theme::instance().c("danger");

    p.setBrush(bgCol);
    p.setPen(QPen(borderCol, 2.0));
    p.drawRoundedRect(r, 11.0, 11.0);

    const double dotX = r.left() + 18.0;
    const double dotY = r.center().y();
    QColor dotCol = m_hovered ? QColor("#FFFFFF") : Theme::instance().c("danger");
    if (m_pulseOn) {
        QColor glowCol(dotCol.red(), dotCol.green(), dotCol.blue(), 80);
        p.setBrush(glowCol);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(dotX, dotY), 8.0, 8.0);
    }

    p.setBrush(dotCol);
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPointF(dotX, dotY), 4.8, 4.8);

    p.setFont(makeFont(13, QFont::ExtraBold));
    QColor fgCol = m_hovered ? QColor("#FFFFFF") : (isDk ? QColor("#F8FAFC") : QColor("#0F172A"));
    p.setPen(fgCol);
    QRectF textRect(r.left() + 32.0, r.top(), r.width() - 40.0, r.height());
    p.drawText(textRect, static_cast<int>(Qt::AlignVCenter | Qt::AlignCenter), I18n::bidiText(labelText()));
}

// ============================================================================
// HeaderStatusAndAdminPill
// ============================================================================

HeaderStatusAndAdminPill::HeaderStatusAndAdminPill(AppState* state, QWidget* parent)
    : QWidget(parent), m_state(state), m_isAdmin(Win32Input::isRunningAsAdmin()) {
    if (!m_isAdmin) {
        setCursor(Qt::PointingHandCursor);
        setToolTip(I18n::t("btn_relaunch_admin"));
    }
    setFixedHeight(32);
    recalcWidth();
}

void HeaderStatusAndAdminPill::recalcWidth() {
    QFontMetrics fm(makeFont(11, QFont::Bold));
    const QString admTxt = I18n::t(m_isAdmin ? "badge_admin" : "badge_user");
    const int wAdm = fm.horizontalAdvance(I18n::bidiText(admTxt)) + 38;
    setFixedWidth(std::max(106, wAdm));
}

void HeaderStatusAndAdminPill::refresh() {
    m_isAdmin = Win32Input::isRunningAsAdmin();
    recalcWidth();
    update();
}

void HeaderStatusAndAdminPill::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && !m_isAdmin) {
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void HeaderStatusAndAdminPill::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && !m_isAdmin) {
        event->accept();
        if (rect().contains(event->pos())) {
            const auto ans = QMessageBox::question(
                window(),
                I18n::t("dialog_admin_title"),
                I18n::t("dialog_admin_body"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );
            if (ans == QMessageBox::Yes) {
                Win32Input::relaunchAsAdmin();
            }
        }
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void HeaderStatusAndAdminPill::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    QFont font = makeFont(11, QFont::Bold);
    p.setFont(font);

    const QString admTxt = I18n::t(m_isAdmin ? "badge_admin" : "badge_user");
    QRectF admR(0.0, 2.0, static_cast<double>(width()), 28.0);
    QColor admCol = Theme::instance().c(m_isAdmin ? "ok" : "text3");
    p.setBrush(QColor(admCol.red(), admCol.green(), admCol.blue(), 32));
    p.setPen(QPen(QColor(admCol.red(), admCol.green(), admCol.blue(), 115), 1.0));
    p.drawRoundedRect(admR, 9.0, 9.0);

    Icons::draw(&p, "shield", QRectF(admR.left() + 8.0, admR.center().y() - 7.0, 14.0, 14.0), admCol, 2.0);
    p.setPen(admCol);
    p.drawText(admR.adjusted(24.0, 0.0, -6.0, 0.0), static_cast<int>(Qt::AlignVCenter | Qt::AlignCenter), I18n::bidiText(admTxt));
}

// ============================================================================
// CenterTabsBar
// ============================================================================

CenterTabsBar::CenterTabsBar(QWidget* parent)
    : QWidget(parent) {
    setFixedSize(620, 36);
    setMouseTracking(true);
    m_tabs = {
        {"combos", "bolt", "nav_combos"},
        {"autopresser", "keyboard", "nav_autopresser"},
        {"recorder", "recorder", "nav_recorder"},
        {"test", "test", "nav_test"},
        {"settings", "settings", "nav_settings"},
        {"about", "about", "nav_about"},
    };
}

void CenterTabsBar::setActiveTab(const QString& tabId) {
    m_activeTab = tabId;
    update();
}

QList<QPair<QString, QRectF>> CenterTabsBar::tabRects() const {
    const double w = static_cast<double>(width()) - 6.0;
    const double segW = w / m_tabs.size();
    QList<TabMeta> ordered = m_tabs;
    if (I18n::isRtl()) {
        std::reverse(ordered.begin(), ordered.end());
    }
    QList<QPair<QString, QRectF>> res;
    for (int i = 0; i < ordered.size(); ++i) {
        res.append(qMakePair(ordered[i].id, QRectF(3.0 + i * segW, 2.5, segW, 31.0)));
    }
    return res;
}

void CenterTabsBar::mouseMoveEvent(QMouseEvent* event) {
    const QPointF pos = event->position();
    int hit = -1;
    const auto rects = tabRects();
    for (int i = 0; i < rects.size(); ++i) {
        if (rects[i].second.contains(pos)) {
            hit = i;
            break;
        }
    }
    if (hit != m_hoverTab) {
        m_hoverTab = hit;
        setCursor(hit >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void CenterTabsBar::leaveEvent(QEvent*) {
    m_hoverTab = -1;
    update();
}

void CenterTabsBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        const QPointF pos = event->position();
        for (const auto& pair : tabRects()) {
            if (pair.second.contains(pos)) {
                if (pair.first == "about") {
                    emit aboutClicked();
                } else {
                    m_activeTab = pair.first;
                    update();
                    emit tabSelected(pair.first);
                }
                return;
            }
        }
    }
}

void CenterTabsBar::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    QRectF barR = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    p.setBrush(Theme::instance().c("bg0", 175));
    p.setPen(QPen(Theme::instance().c("border"), 1.0));
    p.drawRoundedRect(barR, 11.0, 11.0);

    QMap<QString, QString> tabTrKey;
    for (const auto& tm : m_tabs) tabTrKey.insert(tm.id, tm.trKey);

    const auto rects = tabRects();
    for (int i = 0; i < rects.size(); ++i) {
        const QString tid = rects[i].first;
        const QRectF tr = rects[i].second;
        const QString trKey = tabTrKey.value(tid);
        const bool active = (tid == m_activeTab);
        const bool hovered = (i == m_hoverTab);

        QColor fg;
        if (active) {
            QLinearGradient grad(tr.topLeft(), tr.bottomRight());
            grad.setColorAt(0.0, Theme::instance().c("accent"));
            grad.setColorAt(1.0, Theme::instance().c("accent2"));
            p.setBrush(grad);
            p.setPen(QPen(Theme::instance().c("accent_hi", 120), 1.0));
            p.drawRoundedRect(tr.adjusted(1.0, 0.5, -1.0, -0.5), 8.5, 8.5);
            fg = Theme::instance().c("on_accent");
        } else if (hovered) {
            p.setBrush(Theme::instance().c("surface_hi", 140));
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(tr.adjusted(1.0, 0.5, -1.0, -0.5), 8.5, 8.5);
            fg = Theme::instance().c("text");
        } else {
            fg = Theme::instance().c("text2");
        }

        p.setFont(makeFont(12, active ? QFont::Bold : QFont::DemiBold));
        p.setPen(fg);
        p.drawText(tr, static_cast<int>(Qt::AlignAbsolute | Qt::AlignCenter), I18n::bidiText(I18n::t(trKey)));
    }
}

// ============================================================================
// TopHeaderBar
// ============================================================================

TopHeaderBar::TopHeaderBar(AppState* state, QWidget* parent)
    : QWidget(parent), m_state(state) {
    setFixedHeight(52);
    setLayoutDirection(Qt::LeftToRight);

    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(24, 6, 20, 4);
    lay->setSpacing(6);

    lay->addSpacing(148);
    lay->addStretch(1);

    m_tabsBar = new CenterTabsBar(this);
    connect(m_tabsBar, &CenterTabsBar::tabSelected, this, &TopHeaderBar::tabSelected);
    connect(m_tabsBar, &CenterTabsBar::aboutClicked, this, &TopHeaderBar::aboutClicked);
    lay->addWidget(m_tabsBar);

    lay->addStretch(1);

    m_statusPill = new HeaderStatusAndAdminPill(m_state, this);
    lay->addWidget(m_statusPill);

    m_btnLang = new IconButton("globe", I18n::t("lbl_language"), 30, "ghost", this);
    connect(m_btnLang, &IconButton::clicked, this, &TopHeaderBar::openLanguageDropPanel);

    m_langPopup = new SearchPopup(this);
    connect(m_langPopup, &SearchPopup::itemSelected, this, [this](const QString& code, const QString&) {
        if (m_state) m_state->setLanguage(code);
    });

    m_btnAbout = new IconButton("about", I18n::t("nav_about"), 30, "ghost", this);
    connect(m_btnAbout, &IconButton::clicked, this, &TopHeaderBar::aboutClicked);

    m_btnMin = new IconButton("minimize", QString(), 30, "ghost", this);
    connect(m_btnMin, &IconButton::clicked, this, &TopHeaderBar::minimizeClicked);

    m_btnMax = new IconButton("maximize", QString(), 30, "ghost", this);
    connect(m_btnMax, &IconButton::clicked, this, &TopHeaderBar::maximizeClicked);

    m_btnClose = new IconButton("close", QString(), 30, "danger", this);
    connect(m_btnClose, &IconButton::clicked, this, &TopHeaderBar::closeClicked);

    lay->addWidget(m_btnLang);
    lay->addWidget(m_btnAbout);
    lay->addWidget(m_btnMin);
    lay->addWidget(m_btnMax);
    lay->addWidget(m_btnClose);
}

void TopHeaderBar::openLanguageDropPanel() {
    QList<SearchComboItem> items;
    for (const auto& info : I18n::supportedLanguages()) {
        items.append(SearchComboItem(info.code, info.name));
    }
    m_langPopup->populate(items);
    m_langPopup->setFixedSize(230, 290);
    const QPoint gpos = m_btnLang->mapToGlobal(QPoint(-140, m_btnLang->height() + 6));
    m_langPopup->move(gpos);
    m_langPopup->show();
    m_langPopup->searchEdit()->setFocus();
}

void TopHeaderBar::setActiveTab(const QString& tabId) {
    m_tabsBar->setActiveTab(tabId);
}

void TopHeaderBar::refreshStatus() {
    m_btnLang->setToolTip(I18n::t("lbl_language"));
    m_btnAbout->setToolTip(I18n::t("nav_about"));
    m_langPopup->searchEdit()->setPlaceholderText(I18n::t("search_placeholder"));
    m_statusPill->refresh();
    m_tabsBar->update();
    update();
}

void TopHeaderBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (childAt(event->position().toPoint()) == nullptr) {
            if (QWindow* win = window()->windowHandle()) {
                win->startSystemMove();
            }
        }
    }
    QWidget::mousePressEvent(event);
}

void TopHeaderBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit maximizeClicked();
    }
}

void TopHeaderBar::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    QRectF hdrR(16.0, 5.0, static_cast<double>(width()) - 32.0, static_cast<double>(height()) - 7.0);
    p.setBrush(Theme::instance().c("surface", 165));
    p.setPen(QPen(Theme::instance().c("border"), 1.0));
    p.drawRoundedRect(hdrR, 14.0, 14.0);

    QPixmap pix = Icons::logoPixmap(28, 2.0);
    p.drawPixmap(26, static_cast<int>(hdrR.center().y() - 14.0), pix);
    p.setFont(makeFont(15, QFont::ExtraBold));
    p.setPen(Theme::instance().c("text"));
    p.drawText(QRectF(62.0, hdrR.top(), 130.0, hdrR.height()), static_cast<int>(Qt::AlignVCenter | Qt::AlignLeft), kAppName);
}

// ============================================================================
// CornerResizeGrip
// ============================================================================

CornerResizeGrip::CornerResizeGrip(MainWindow* mainWin, bool isLeft)
    : QWidget(mainWin), m_mainWin(mainWin), m_isLeft(isLeft) {
    setFixedSize(22, 22);
    setCursor(isLeft ? Qt::SizeBDiagCursor : Qt::SizeFDiagCursor);
}

void CornerResizeGrip::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_mainWin && !m_mainWin->isMaximized()) {
        const Qt::Edges edges = m_isLeft ? (Qt::BottomEdge | Qt::LeftEdge) : (Qt::BottomEdge | Qt::RightEdge);
        if (QWindow* wh = m_mainWin->windowHandle()) {
            if (wh->startSystemResize(edges)) {
                event->accept();
                return;
            }
        }
        m_dragging = true;
        m_dragStartGpos = event->globalPosition().toPoint();
        m_dragStartGeom = m_mainWin->geometry();
        event->accept();
    }
}

void CornerResizeGrip::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging && m_mainWin) {
        const QPoint gpos = event->globalPosition().toPoint();
        m_mainWin->applyManualResizeDelta(
            gpos - m_dragStartGpos,
            m_dragStartGeom,
            m_isLeft ? (Qt::BottomEdge | Qt::LeftEdge) : (Qt::BottomEdge | Qt::RightEdge)
        );
        event->accept();
    }
}

void CornerResizeGrip::mouseReleaseEvent(QMouseEvent* event) {
    if (m_dragging) {
        m_dragging = false;
        event->accept();
    }
}

void CornerResizeGrip::paintEvent(QPaintEvent*) {
    if (!m_mainWin || m_mainWin->isMaximized()) return;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Theme::instance().c("accent_hi", 150), 1.6);
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);
    if (m_isLeft) {
        p.drawLine(QPointF(4.0, 14.0), QPointF(8.0, 18.0));
        p.drawLine(QPointF(4.0, 9.0), QPointF(13.0, 18.0));
    } else {
        p.drawLine(QPointF(18.0, 14.0), QPointF(14.0, 18.0));
        p.drawLine(QPointF(18.0, 9.0), QPointF(9.0, 18.0));
    }
}

// ============================================================================
// MainWindow
// ============================================================================

MainWindow::MainWindow(AppState* state, MacroEngine* engine, QWidget* parent)
    : QMainWindow(parent), m_state(state), m_engine(engine) {
    setWindowTitle(kAppName);
    setWindowIcon(Icons::logoIcon(64));
    setMinimumSize(1180, 700);
    resize(1240, 760);
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    if (m_state && m_state->config.alwaysOnTop) {
        setWindowFlag(Qt::WindowStaysOnTopHint, true);
    }

    m_central = new QWidget(this);
    m_central->setMouseTracking(true);
    setCentralWidget(m_central);

    m_rootLayout = new QVBoxLayout(m_central);
    m_rootLayout->setContentsMargins(OUTER_MARGIN, OUTER_MARGIN, OUTER_MARGIN, OUTER_MARGIN);
    m_rootLayout->setSpacing(0);

    m_headerBar = new TopHeaderBar(m_state, m_central);
    connect(m_headerBar, &TopHeaderBar::tabSelected, this, &MainWindow::navigateTo);
    connect(m_headerBar, &TopHeaderBar::aboutClicked, this, &MainWindow::showAbout);
    connect(m_headerBar, &TopHeaderBar::minimizeClicked, this, &QWidget::showMinimized);
    connect(m_headerBar, &TopHeaderBar::maximizeClicked, this, &MainWindow::toggleMaximize);
    connect(m_headerBar, &TopHeaderBar::closeClicked, this, &MainWindow::onCloseClicked);
    m_rootLayout->addWidget(m_headerBar);

    m_pageStack = new SlideStack(m_central);
    m_rootLayout->addWidget(m_pageStack, 1);

    m_toast = new ToastOverlay(m_central);
    m_aboutOverlay = new AboutDialog(m_central);
    m_onboarding = new OnboardingOverlay(m_state, m_central);
    m_onboarding->hide();

    m_gripBr = new CornerResizeGrip(this, false);
    m_gripBl = new CornerResizeGrip(this, true);
    repositionCornerGrips();

    m_recFloatingPill = new RecordingFloatingIndicator(m_state);
    connect(m_recFloatingPill, &RecordingFloatingIndicator::stopClicked, this, &MainWindow::onFloatingStopClicked);

    rebuildPages();
    setupTray();

    if (QApplication* appInst = qApp) {
        appInst->installEventFilter(this);
    }

    connect(&Theme::instance(), &Theme::themeChanged, this, &MainWindow::onThemeChanged);
    if (m_state) {
        connect(m_state, &AppState::languageChanged, this, [this](const QString&) { onLanguageChanged(); });
        connect(m_state, &AppState::navigationRequested, this, &MainWindow::navigateTo);
        connect(m_state, &AppState::recordingStateChanged, this, &MainWindow::onRecordingStateChanged);
        connect(m_state, &AppState::recordingCountChanged, m_recFloatingPill, &RecordingFloatingIndicator::updateCount);
        connect(m_state, &AppState::playbackStateChanged, this, &MainWindow::onPlaybackStateChanged);
        connect(m_state, &AppState::combosMasterChanged, this, [this](bool) { refreshStatusSurfaces(); });
        connect(m_state, &AppState::toastRequested, this, [this](const QString& msg, const QString& kind) {
            if (m_toast) m_toast->showToast(msg, kind);
        });
    }
}

MainWindow::~MainWindow() {
    if (m_recFloatingPill) {
        m_recFloatingPill->hideIndicator();
        delete m_recFloatingPill;
        m_recFloatingPill = nullptr;
    }
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    repositionCornerGrips();
    if (m_state && !m_state->config.firstRunCompleted) {
        QTimer::singleShot(0, this, &MainWindow::presentOnboardingFull);
    }
}

void MainWindow::presentOnboardingFull() {
    if (m_state && !m_state->config.firstRunCompleted && m_onboarding) {
        m_onboarding->setGeometry(m_central->rect());
        m_onboarding->refreshTexts();
        m_onboarding->show();
        m_onboarding->raise();
    }
}

void MainWindow::repositionCornerGrips() {
    const int m = isMaximized() ? 0 : OUTER_MARGIN;
    if (m_gripBr) {
        m_gripBr->move(width() - m - 24, height() - m - 24);
        m_gripBr->raise();
    }
    if (m_gripBl) {
        m_gripBl->move(m + 2, height() - m - 24);
        m_gripBl->raise();
    }
}

void MainWindow::onRecordingStateChanged(bool active) {
    refreshStatusSurfaces();
    const bool showPlayPill = m_state && m_state->config.showFloatingPlaybackPill;
    if (active || (m_state && m_state->isPlaying && showPlayPill)) {
        m_recFloatingPill->showIndicator();
    } else {
        m_recFloatingPill->hideIndicator();
    }
}

void MainWindow::onPlaybackStateChanged(bool active) {
    refreshStatusSurfaces();
    const bool showPlayPill = m_state && m_state->config.showFloatingPlaybackPill;
    if ((active && showPlayPill) || (m_state && m_state->isRecording)) {
        m_recFloatingPill->showIndicator();
    } else {
        m_recFloatingPill->hideIndicator();
    }
}

void MainWindow::onFloatingStopClicked() {
    if (!m_engine || !m_state) return;
    if (m_state->isRecording) {
        m_engine->stopRecording(true);
    } else if (m_state->isPlaying) {
        m_engine->stopAll();
    }
}

void MainWindow::rebuildPages() {
    const QStringList validIds = {"combos", "autopresser", "recorder", "test", "settings"};
    const QString activeId = (m_state && validIds.contains(m_state->currentPageId))
        ? m_state->currentPageId
        : QStringLiteral("combos");

    setUpdatesEnabled(false);
    while (m_pageStack->count() > 0) {
        QWidget* w = m_pageStack->widget(0);
        w->hide();
        m_pageStack->removeWidget(w);
        w->deleteLater();
    }
    m_pageMap.clear();

    m_central->setLayoutDirection(I18n::isRtl() ? Qt::RightToLeft : Qt::LeftToRight);
    m_headerBar->setLayoutDirection(Qt::LeftToRight);

    auto* pageCombos = new CombosPage(m_state, m_engine, m_pageStack);
    auto* pageAuto = new AutoPresserPage(m_state, m_engine, m_pageStack);
    auto* pageRec = new RecorderPage(m_state, m_engine, m_pageStack);
    auto* pageTest = new TestPage(m_state, m_engine, m_pageStack);
    auto* pageSet = new SettingsPage(m_state, m_engine, m_pageStack);

    QList<QPair<QString, QWidget*>> pages = {
        {"combos", pageCombos},
        {"autopresser", pageAuto},
        {"recorder", pageRec},
        {"test", pageTest},
        {"settings", pageSet},
    };
    for (int idx = 0; idx < pages.size(); ++idx) {
        m_pageStack->addWidget(pages[idx].second);
        m_pageMap.insert(pages[idx].first, idx);
    }

    m_pageStack->setCurrentIndex(m_pageMap.value(activeId, 0));
    m_headerBar->setActiveTab(activeId);
    repositionCornerGrips();
    setUpdatesEnabled(true);
}

void MainWindow::onThemeChanged() {
    QIcon icon = Icons::logoIcon(64);
    setWindowIcon(icon);
    if (m_tray) {
        m_tray->setIcon(icon);
        rebuildTrayMenu();
    }
    refreshWidgetTreeTheme(m_central);
    if (m_aboutOverlay) m_aboutOverlay->refreshStyles();
    if (m_onboarding) m_onboarding->refreshTexts();
    if (m_headerBar) m_headerBar->refreshStatus();
    if (m_recFloatingPill && m_recFloatingPill->isVisible()) {
        m_recFloatingPill->update();
    }
    update();
}

void MainWindow::onLanguageChanged() {
    QIcon icon = Icons::logoIcon(64);
    setWindowIcon(icon);
    if (m_tray) {
        m_tray->setIcon(icon);
        rebuildTrayMenu();
    }
    rebuildPages();
    if (m_aboutOverlay) m_aboutOverlay->refreshStyles();
    if (m_onboarding) {
        m_onboarding->refreshTexts();
        if (m_onboarding->isVisible()) {
            m_onboarding->setGeometry(m_central->rect());
            m_onboarding->raise();
        }
    }
    if (m_headerBar) m_headerBar->refreshStatus();
    if (m_recFloatingPill && m_recFloatingPill->isVisible()) {
        m_recFloatingPill->recalcSizeAndPosition();
        m_recFloatingPill->update();
    }
    update();
}

void MainWindow::navigateTo(const QString& pageId) {
    if (pageId == "about") {
        showAbout();
        return;
    }
    if (m_pageMap.contains(pageId)) {
        if (m_state) m_state->currentPageId = pageId;
        m_headerBar->setActiveTab(pageId);
        m_pageStack->setCurrentIndexSmooth(m_pageMap.value(pageId));
    }
}

void MainWindow::showAbout() {
    if (m_aboutOverlay) m_aboutOverlay->showModal();
}

void MainWindow::toggleMaximize() {
    if (isMaximized()) {
        showNormal();
        m_rootLayout->setContentsMargins(OUTER_MARGIN, OUTER_MARGIN, OUTER_MARGIN, OUTER_MARGIN);
    } else {
        m_rootLayout->setContentsMargins(0, 0, 0, 0);
        showMaximized();
    }
    repositionCornerGrips();
    update();
}

void MainWindow::onCloseClicked() {
    if (m_state && m_state->config.minimizeToTray && m_tray && QSystemTrayIcon::isSystemTrayAvailable()) {
        hide();
    } else {
        quitApplication();
    }
}

void MainWindow::quitApplication() {
    if (m_recFloatingPill) m_recFloatingPill->hideIndicator();
    if (m_engine) m_engine->shutdown();
    if (m_tray) m_tray->hide();
    QApplication::quit();
}

void MainWindow::setupTray() {
    m_tray = new QSystemTrayIcon(Icons::logoIcon(64), this);
    m_tray->setToolTip(kAppName);
    m_trayMenu = new QMenu(this);
    rebuildTrayMenu();
    m_tray->setContextMenu(m_trayMenu);
    connect(m_tray, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        m_tray->show();
    }
}

void MainWindow::rebuildTrayMenu() {
    if (!m_trayMenu) return;
    m_trayMenu->clear();
    m_trayMenu->setStyleSheet(QStringLiteral(R"(
        QMenu {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 12px;
            padding: 6px;
            font-size: 13px;
        }
        QMenu::item {
            padding: 7px 16px;
            border-radius: 8px;
        }
        QMenu::item:selected {
            background-color: %4;
            color: %5;
        }
    )").arg(Theme::instance().tokenHex("bg1"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border_hi"),
            Theme::instance().tokenHex("surface_hi"),
            Theme::instance().tokenHex("accent_hi")));

    const bool combosOn = m_state && m_state->combosActive;
    const QString stLabel = QStringLiteral("%1 — %2").arg(kAppName, I18n::t(combosOn ? "status_combos_active" : "status_idle"));
    QAction* actStatus = m_trayMenu->addAction(Icons::logoIcon(20), stLabel);
    actStatus->setEnabled(false);
    m_trayMenu->addSeparator();

    QAction* actShow = m_trayMenu->addAction(Icons::icon("bolt", Theme::instance().c("accent_hi")), I18n::t("tray_show"));
    connect(actShow, &QAction::triggered, this, &MainWindow::restoreFromTray);

    const QString comboSt = I18n::t(combosOn ? "state_on" : "state_off");
    QAction* actCombos = m_trayMenu->addAction(
        Icons::icon("bolt", Theme::instance().c(combosOn ? "ok" : "text3")),
        QStringLiteral("%1%2").arg(I18n::t("tray_toggle_combos"), comboSt)
    );
    connect(actCombos, &QAction::triggered, this, [this]() {
        if (m_engine) m_engine->toggleCombosMaster();
    });

    QAction* actRec = m_trayMenu->addAction(Icons::icon("recorder", Theme::instance().c("danger")), I18n::t("tray_quick_rec"));
    connect(actRec, &QAction::triggered, this, [this]() {
        if (m_engine) m_engine->toggleRecording(true);
    });

    QAction* actPlay = m_trayMenu->addAction(Icons::icon("play", Theme::instance().c("ok")), I18n::t("tray_quick_play"));
    connect(actPlay, &QAction::triggered, this, [this]() {
        if (m_engine) m_engine->togglePlayback();
    });

    QAction* actAbout = m_trayMenu->addAction(Icons::icon("about", Theme::instance().c("text")), I18n::t("nav_about"));
    connect(actAbout, &QAction::triggered, this, &MainWindow::showAboutFromTray);

    m_trayMenu->addSeparator();
    QAction* actQuit = m_trayMenu->addAction(Icons::icon("close", Theme::instance().c("danger")), I18n::t("tray_quit"));
    connect(actQuit, &QAction::triggered, this, &MainWindow::quitApplication);
}

void MainWindow::showAboutFromTray() {
    restoreFromTray();
    showAbout();
}

void MainWindow::restoreFromTray() {
    showNormal();
    raise();
    activateWindow();
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        restoreFromTray();
    }
}

void MainWindow::refreshStatusSurfaces() {
    if (m_headerBar) m_headerBar->refreshStatus();
    rebuildTrayMenu();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    repositionCornerGrips();
    if (m_aboutOverlay && m_aboutOverlay->isVisible()) {
        m_aboutOverlay->setGeometry(m_central->rect());
    }
    if (m_onboarding && m_onboarding->isVisible()) {
        m_onboarding->setGeometry(m_central->rect());
    }
}

Qt::Edges MainWindow::hitTestEdges(const QPoint& pos) const {
    if (isMaximized()) return {};
    const int b = OUTER_MARGIN + RESIZE_BORDER;
    const int w = width();
    const int h = height();
    Qt::Edges edges;
    if (pos.x() <= b) edges |= Qt::LeftEdge;
    else if (pos.x() >= w - b) edges |= Qt::RightEdge;
    if (pos.y() <= b) edges |= Qt::TopEdge;
    else if (pos.y() >= h - b) edges |= Qt::BottomEdge;
    return edges;
}

bool MainWindow::cursorForEdges(Qt::Edges edges, Qt::CursorShape& outShape) {
    if (!edges) return false;
    const bool left = edges.testFlag(Qt::LeftEdge);
    const bool right = edges.testFlag(Qt::RightEdge);
    const bool top = edges.testFlag(Qt::TopEdge);
    const bool bottom = edges.testFlag(Qt::BottomEdge);
    if ((left && top) || (right && bottom)) { outShape = Qt::SizeFDiagCursor; return true; }
    if ((right && top) || (left && bottom)) { outShape = Qt::SizeBDiagCursor; return true; }
    if (left || right) { outShape = Qt::SizeHorCursor; return true; }
    if (top || bottom) { outShape = Qt::SizeVerCursor; return true; }
    return false;
}

void MainWindow::applyManualResizeDelta(const QPoint& delta, const QRect& startGeom, Qt::Edges edges) {
    const int minW = minimumWidth();
    const int minH = minimumHeight();
    int x = startGeom.x();
    int y = startGeom.y();
    int w = startGeom.width();
    int h = startGeom.height();

    if (edges.testFlag(Qt::RightEdge)) {
        w = std::max(minW, startGeom.width() + delta.x());
    } else if (edges.testFlag(Qt::LeftEdge)) {
        const int newW = std::max(minW, startGeom.width() - delta.x());
        x = startGeom.right() - newW + 1;
        w = newW;
    }

    if (edges.testFlag(Qt::BottomEdge)) {
        h = std::max(minH, startGeom.height() + delta.y());
    } else if (edges.testFlag(Qt::TopEdge)) {
        const int newH = std::max(minH, startGeom.height() - delta.y());
        y = startGeom.bottom() - newH + 1;
        h = newH;
    }

    setGeometry(x, y, w, h);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    const QEvent::Type etype = event->type();
    if (etype != QEvent::MouseMove && etype != QEvent::MouseButtonPress && etype != QEvent::MouseButtonRelease) {
        return false;
    }
    if (!isVisible() || isMaximized()) return false;
    if (!m_manualResizing && !m_edgeCursorOverride) {
        if (watched != this && watched != m_central && watched != m_headerBar) {
            return false;
        }
    }

    auto* me = static_cast<QMouseEvent*>(event);
    if (etype == QEvent::MouseMove) {
        const QPoint gpos = me->globalPosition().toPoint();
        if (m_manualResizing) {
            applyManualResizeDelta(gpos - m_resizeStartGpos, m_resizeStartGeom, m_resizeEdges);
            return true;
        }
        const QPoint localPos = mapFromGlobal(gpos);
        if (rect().contains(localPos)) {
            const Qt::Edges edges = hitTestEdges(localPos);
            Qt::CursorShape cShape = Qt::ArrowCursor;
            if (cursorForEdges(edges, cShape)) {
                if (!m_edgeCursorOverride) {
                    QApplication::setOverrideCursor(cShape);
                    m_edgeCursorOverride = true;
                } else {
                    QApplication::changeOverrideCursor(cShape);
                }
            } else if (m_edgeCursorOverride) {
                QApplication::restoreOverrideCursor();
                m_edgeCursorOverride = false;
            }
        } else if (m_edgeCursorOverride) {
            QApplication::restoreOverrideCursor();
            m_edgeCursorOverride = false;
        }
    } else if (etype == QEvent::MouseButtonPress) {
        if (me->button() == Qt::LeftButton) {
            const QPoint gpos = me->globalPosition().toPoint();
            const QPoint localPos = mapFromGlobal(gpos);
            if (rect().contains(localPos)) {
                const Qt::Edges edges = hitTestEdges(localPos);
                if (edges) {
                    if (QWindow* wh = windowHandle()) {
                        if (wh->startSystemResize(edges)) return true;
                    }
                    m_manualResizing = true;
                    m_resizeEdges = edges;
                    m_resizeStartGpos = gpos;
                    m_resizeStartGeom = geometry();
                    return true;
                }
            }
        }
    } else if (etype == QEvent::MouseButtonRelease) {
        if (m_manualResizing && me->button() == Qt::LeftButton) {
            m_manualResizing = false;
            m_resizeEdges = {};
            if (m_edgeCursorOverride) {
                QApplication::restoreOverrideCursor();
                m_edgeCursorOverride = false;
            }
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    if (isMaximized()) {
        p.fillRect(rect(), Theme::instance().c("bg0"));
        return;
    }

    p.fillRect(rect(), QColor(0, 0, 0, 3));

    const double m = static_cast<double>(OUTER_MARGIN);
    QRectF shellRect(m, m, width() - 2.0 * m, height() - 2.0 * m);
    const double radius = 18.0;

    for (const auto& pair : {qMakePair(2.0, 32), qMakePair(5.0, 14)}) {
        const double spread = pair.first;
        const int alpha = pair.second;
        QRectF sr = shellRect.adjusted(-spread, -spread + 1.5, spread, spread + 1.5);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(0, 0, 0, alpha), 2.5));
        p.drawRoundedRect(sr, radius + spread * 0.4, radius + spread * 0.4);
    }

    QLinearGradient grad(shellRect.topLeft(), shellRect.bottomRight());
    grad.setColorAt(0.0, Theme::instance().c("bg0"));
    grad.setColorAt(1.0, Theme::instance().c("bg1"));
    p.setBrush(grad);
    p.setPen(QPen(Theme::instance().c("border_hi"), 1.1));
    p.drawRoundedRect(shellRect, radius, radius);
}

} // namespace vmacro
