#include "Widgets.h"
#include "Icons.h"
#include "../core/AppState.h"
#include "../core/I18n.h"
#include "../core/Theme.h"
#include "../platform/Win32Input.h"

#include <QApplication>
#include <QEasingCurve>
#include <QEnterEvent>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QLabel>
#include <QLinearGradient>
#include <QMap>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPointer>
#include <QScreen>
#include <QSizePolicy>
#include <QStyle>
#include <QTextDocument>
#include <QTextOption>
#include <QVariant>
#include <algorithm>
#include <cmath>

namespace vmacro {

static QPointer<KeyCaptureButton> s_activeKeyCaptureWidget = nullptr;
static QFileIconProvider* s_iconProvider = nullptr;
static QMap<QString, QPixmap> s_appIconCache;
static QList<QPointer<QPropertyAnimation>> s_activeRowAnims;
static QPointer<QTimer> s_restoreTimer = nullptr;

QFont makeFont(int sizePx, QFont::Weight weight) {
    QFont f(I18n::langFontFamily());
    f.setPixelSize(sizePx);
    f.setWeight(weight);
    return f;
}

int alignFlag(bool preferStart, bool vcenter) {
    Qt::AlignmentFlag hFlag = preferStart
        ? (I18n::isRtl() ? Qt::AlignRight : Qt::AlignLeft)
        : (I18n::isRtl() ? Qt::AlignLeft : Qt::AlignRight);
    Qt::AlignmentFlag vFlag = vcenter ? Qt::AlignVCenter : Qt::AlignTop;
    return static_cast<int>(Qt::AlignAbsolute | hFlag | vFlag);
}

QString framedHorizontalScrollStyle(const QString& tintHex) {
    const QString accent = tintHex.isEmpty() ? Theme::instance().tokenHex("accent") : tintHex;
    QColor qc(accent);
    if (!qc.isValid()) qc = QColor("#8B5CF6");
    const int bgAlpha = Theme::instance().isDark() ? 36 : 24;
    const int borderAlpha = Theme::instance().isDark() ? 165 : 145;
    const QString bgRgba = QStringLiteral("rgba(%1, %2, %3, %4)").arg(qc.red()).arg(qc.green()).arg(qc.blue()).arg(bgAlpha);
    const QString borderRgba = QStringLiteral("rgba(%1, %2, %3, %4)").arg(qc.red()).arg(qc.green()).arg(qc.blue()).arg(borderAlpha);
    const QString thumbRgba = QStringLiteral("rgba(%1, %2, %3, 195)").arg(qc.red()).arg(qc.green()).arg(qc.blue());
    return QStringLiteral(R"(
        QScrollArea {
            background: %1;
            border: 1.5px solid %2;
            border-radius: 10px;
            padding: 1px 4px;
        }
        QScrollBar:horizontal {
            background: transparent;
            height: 6px;
            margin: 0px 4px 1px 4px;
            border-radius: 3px;
        }
        QScrollBar::handle:horizontal {
            background: %3;
            min-width: 28px;
            border-radius: 3px;
        }
        QScrollBar::handle:horizontal:hover {
            background: %4;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
            background: none;
        }
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: transparent;
        }
    )").arg(bgRgba, borderRgba, thumbRgba, accent);
}

QString modernScrollStyle() {
    const QString thumb = Theme::instance().tokenHex("accent");
    const QString hover = Theme::instance().tokenHex("accent_hi");
    return QStringLiteral(R"(
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: rgba(128, 128, 128, 28);
            width: 10px;
            margin: 3px 1px 3px 1px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical {
            background: %1;
            min-height: 28px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical:hover {
            background: %2;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
            background: none;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }
        QScrollBar:horizontal {
            background: transparent;
            height: 6px;
            margin: 1px 4px 1px 4px;
            border-radius: 3px;
        }
        QScrollBar::handle:horizontal {
            background: %1;
            min-width: 28px;
            border-radius: 3px;
        }
        QScrollBar::handle:horizontal:hover {
            background: %2;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
            background: none;
        }
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: transparent;
        }
    )").arg(thumb, hover);
}

QPixmap getExeIconPixmap(const QString& exePath, int size) {
    if (exePath.isEmpty() || !QFileInfo::exists(exePath)) return QPixmap();
    const QString cacheKey = QStringLiteral("%1:%2").arg(exePath).arg(size);
    auto it = s_appIconCache.constFind(cacheKey);
    if (it != s_appIconCache.cend()) return it.value();

    if (!s_iconProvider) s_iconProvider = new QFileIconProvider();
    QIcon icon = s_iconProvider->icon(QFileInfo(exePath));
    if (!icon.isNull()) {
        QPixmap pix = icon.pixmap(QSize(size, size));
        if (!pix.isNull()) {
            s_appIconCache.insert(cacheKey, pix);
            return pix;
        }
    }
    return QPixmap();
}

void refreshWidgetTreeTheme(QWidget* root) {
    if (!root) return;
    const auto& oldMap = Theme::instance().prevTokenMap();
    const auto& newMap = Theme::instance().currentTokenMap();

    static const QStringList kTokenKeys = {
        "bg0", "bg1", "bg2", "surface", "surface_hi", "border", "border_hi",
        "text", "text2", "text3", "accent", "accent2", "accent_hi", "ok", "danger", "warn"
    };

    struct ReplPair { QString oldHex; QString newHex; QString placeholder; };
    QList<ReplPair> repls;
    for (int i = 0; i < kTokenKeys.size(); ++i) {
        const QString& k = kTokenKeys[i];
        const QString oHx = oldMap.value(k);
        const QString nHx = newMap.value(k);
        if (!oHx.isEmpty() && !nHx.isEmpty() && oHx.compare(nHx, Qt::CaseInsensitive) != 0) {
            repls.append({oHx, nHx, QStringLiteral("__VMACRO_TK_%1__").arg(i)});
        }
    }

    const QString oldAcc = oldMap.value("accent");
    const QString newAcc = newMap.value("accent");

    root->setUpdatesEnabled(false);
    QList<QWidget*> allWidgets = root->findChildren<QWidget*>();
    allWidgets.prepend(root);
    for (QWidget* w : allWidgets) {
        if (auto* sa = qobject_cast<QScrollArea*>(w)) {
            if (sa->verticalScrollBarPolicy() != Qt::ScrollBarAlwaysOff) {
                sa->setStyleSheet(modernScrollStyle());
            } else {
                sa->setStyleSheet(framedHorizontalScrollStyle(newAcc));
            }
        } else {
            const QString ss = w->styleSheet();
            if (!ss.isEmpty() && !repls.isEmpty()) {
                QString updated = ss;
                for (const auto& rp : repls) {
                    updated.replace(rp.oldHex, rp.placeholder, Qt::CaseInsensitive);
                }
                for (const auto& rp : repls) {
                    updated.replace(rp.placeholder, rp.newHex);
                }
                if (updated != ss) {
                    w->setStyleSheet(updated);
                }
            }
            if (auto* btn = qobject_cast<Button*>(w)) {
                if (!oldAcc.isEmpty() && !newAcc.isEmpty() && btn->tintHex().compare(oldAcc, Qt::CaseInsensitive) == 0) {
                    btn->setTintHex(newAcc);
                }
            }
        }
        w->update();
    }
    root->setUpdatesEnabled(true);
}

// ============================================================================
// TintedScrollBox
// ============================================================================

TintedScrollBox::TintedScrollBox(const QString& tintHex, QWidget* parent)
    : QWidget(parent), m_tintHex(tintHex) {
    connect(&Theme::instance(), &Theme::themeChanged, this, [this]() { update(); });
}

TintedScrollBox::TintedScrollBox(QWidget* parent)
    : TintedScrollBox(QString(), parent) {}

void TintedScrollBox::setTintHex(const QString& tintHex) {
    m_tintHex = tintHex;
    update();
}

void TintedScrollBox::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    QColor qc = m_tintHex.isEmpty() ? Theme::instance().c("accent") : QColor(m_tintHex);
    if (!qc.isValid()) qc = Theme::instance().c("accent");
    QColor bg(qc.red(), qc.green(), qc.blue(), Theme::instance().isDark() ? 42 : 28);
    QColor border(qc.red(), qc.green(), qc.blue(), Theme::instance().isDark() ? 185 : 160);
    p.setBrush(bg);
    p.setPen(QPen(border, 1.6));
    p.drawRoundedRect(r, 10.0, 10.0);
}

// ============================================================================
// DragGripHandle
// ============================================================================

DragGripHandle::DragGripHandle(QWidget* parent)
    : QWidget(parent) {
    setFixedSize(22, 30);
    setCursor(Qt::OpenHandCursor);
    setToolTip(I18n::tl(
        QString::fromUtf8("اسحب بالماوس لنقل الخطوة فوراً لأي مكان"),
        QStringLiteral("Drag to move this step live")));
}

void DragGripHandle::enterEvent(QEnterEvent*) {
    m_hover = true;
    update();
}

void DragGripHandle::leaveEvent(QEvent*) {
    m_hover = false;
    update();
}

void DragGripHandle::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        setCursor(Qt::ClosedHandCursor);
        emit dragStarted();
        update();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void DragGripHandle::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        emit dragMoved(static_cast<int>(event->globalPosition().y()));
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void DragGripHandle::mouseReleaseEvent(QMouseEvent* event) {
    if (m_dragging) {
        m_dragging = false;
        setCursor(Qt::OpenHandCursor);
        emit dragFinished();
        update();
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void DragGripHandle::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r(rect());
    if (m_hover || m_dragging) {
        p.setBrush(Theme::instance().c("surface_hi", 160));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(r.adjusted(1.0, 2.0, -1.0, -2.0), 6.0, 6.0);
    }
    QColor col = (m_hover || m_dragging) ? Theme::instance().c("accent_hi") : Theme::instance().c("text3");
    Icons::draw(&p, "grip", r.adjusted(2.0, 5.0, -2.0, -5.0), col, 2.0);
}

// ============================================================================
// Button
// ============================================================================

Button::Button(const QString& text, const QString& variant, const QString& iconName, int height, QWidget* parent)
    : QWidget(parent), m_text(text), m_variant(variant), m_iconName(iconName) {
    setCursor(Qt::PointingHandCursor);
    QWidget::setFixedHeight(height);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_anim = new QPropertyAnimation(this, "hoverProgress", this);
    m_anim->setDuration(140);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);

    m_spinTimer = new QTimer(this);
    m_spinTimer->setInterval(24);
    connect(m_spinTimer, &QTimer::timeout, this, &Button::tickSpin);

    recalcMinWidth();
}

Button::Button(const QString& text, const QString& variant, const QString& iconName,
               const QString& tintHex, bool fixedCompact, QWidget* parent)
    : QWidget(parent), m_text(text), m_variant(variant), m_iconName(iconName),
      m_fixedCompact(fixedCompact), m_tintHex(tintHex) {
    setCursor(Qt::PointingHandCursor);
    QWidget::setFixedHeight(38);
    setSizePolicy(fixedCompact ? QSizePolicy::Expanding : QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_anim = new QPropertyAnimation(this, "hoverProgress", this);
    m_anim->setDuration(140);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);

    m_spinTimer = new QTimer(this);
    m_spinTimer->setInterval(24);
    connect(m_spinTimer, &QTimer::timeout, this, &Button::tickSpin);

    recalcMinWidth();
}

void Button::setFixedHeight(int h) {
    QWidget::setFixedHeight(h);
    recalcMinWidth();
}

int Button::naturalCompactWidth() const {
    const bool isSmall = height() <= 32;
    const int fSz = isSmall ? 12 : 13;
    QFont paintFont = makeFont(fSz, !m_tintHex.isEmpty() ? QFont::Bold : QFont::DemiBold);
    QFontMetrics fm(paintFont);
    const int tw = m_text.isEmpty() ? 0 : fm.horizontalAdvance(I18n::bidiText(m_text));
    const bool hasIcon = !m_iconName.isEmpty() || !m_appPixmap.isNull();
    const int iw = hasIcon ? (isSmall ? 18 : 24) : 0;
    const int pad = isSmall ? 28 : 34;
    return std::max(isSmall ? 60 : 74, tw + iw + pad);
}

void Button::recalcMinWidth() {
    if (m_fixedCompact) {
        setMinimumWidth(52);
        return;
    }
    setMinimumWidth(naturalCompactWidth());
}

void Button::setHoverProgress(double val) {
    m_hover = val;
    update();
}

void Button::setText(const QString& text) {
    m_text = text;
    recalcMinWidth();
    updateGeometry();
    update();
}

void Button::setVariant(const QString& variant) {
    m_variant = variant;
    update();
}

void Button::setIconName(const QString& iconName) {
    m_iconName = iconName;
    recalcMinWidth();
    updateGeometry();
    update();
}

void Button::setAppPixmap(const QPixmap& pix) {
    m_appPixmap = pix;
    recalcMinWidth();
    updateGeometry();
    update();
}

void Button::setTintHex(const QString& tintHex) {
    m_tintHex = tintHex;
    recalcMinWidth();
    update();
}

void Button::setFixedCompact(bool compact) {
    m_fixedCompact = compact;
    setSizePolicy(compact ? QSizePolicy::Expanding : QSizePolicy::Minimum, QSizePolicy::Fixed);
    recalcMinWidth();
    updateGeometry();
    update();
}

void Button::setBusy(bool busy) {
    m_busy = busy;
    if (busy) m_spinTimer->start();
    else m_spinTimer->stop();
    update();
}

void Button::tickSpin() {
    m_spinAngle = (m_spinAngle + 18) % 360;
    update();
}

QSize Button::sizeHint() const {
    if (m_fixedCompact) return QSize(110, height());
    return QSize(naturalCompactWidth(), height());
}

void Button::enterEvent(QEnterEvent* event) {
    m_anim->stop();
    m_anim->setEndValue(1.0);
    m_anim->start();
    QWidget::enterEvent(event);
}

void Button::leaveEvent(QEvent* event) {
    m_anim->stop();
    m_anim->setEndValue(0.0);
    m_anim->start();
    QWidget::leaveEvent(event);
}

void Button::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isEnabled() && !m_busy) {
        m_pressed = true;
        update();
    }
}

void Button::mouseReleaseEvent(QMouseEvent* event) {
    if (m_pressed) {
        m_pressed = false;
        update();
        if (rect().contains(event->pos()) && isEnabled() && !m_busy) {
            emit clicked();
        }
    }
}

void Button::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    const double radius = 11.0;
    QColor icColor;
    bool hasCustomIcColor = false;
    QColor fg;

    if (!m_tintHex.isEmpty()) {
        QColor tc(m_tintHex);
        if (!tc.isValid()) tc = Theme::instance().c("accent");
        int bgAlpha = 0;
        int borderAlpha = 0;
        double penW = 1.3;
        if (m_variant == "tinted_row") {
            bgAlpha = static_cast<int>(78 + 52 * m_hover + (m_pressed ? 25 : 0));
            borderAlpha = static_cast<int>(210 + 45 * m_hover);
            penW = 1.7;
            fg = Theme::instance().isDark() ? QColor("#FFFFFF") : tc.darker(145);
        } else {
            bgAlpha = static_cast<int>(38 + 45 * m_hover + (m_pressed ? 25 : 0));
            borderAlpha = static_cast<int>(125 + 85 * m_hover);
            penW = 1.3;
            fg = Theme::instance().isDark() ? tc.lighter(135) : tc.darker(125);
        }
        p.setBrush(QColor(tc.red(), tc.green(), tc.blue(), qBound(0, bgAlpha, 255)));
        p.setPen(QPen(QColor(tc.red(), tc.green(), tc.blue(), qBound(0, borderAlpha, 255)), penW));
        icColor = tc.lighter(125);
        hasCustomIcColor = true;
    } else if (m_variant == "primary") {
        QLinearGradient grad(r.topLeft(), r.bottomRight());
        QColor c1 = Theme::instance().c("accent").lighter(static_cast<int>(100 + 12 * m_hover));
        QColor c2 = Theme::instance().c("accent2").lighter(static_cast<int>(100 + 12 * m_hover));
        if (m_pressed) {
            c1 = c1.darker(115);
            c2 = c2.darker(115);
        }
        grad.setColorAt(0.0, c1);
        grad.setColorAt(1.0, c2);
        p.setBrush(grad);
        p.setPen(QPen(Theme::instance().c("accent_hi", static_cast<int>(90 + 70 * m_hover)), 1.0));
        fg = Theme::instance().c("on_accent");
    } else if (m_variant == "danger") {
        QColor base = Theme::instance().c("danger");
        QColor bg = base.lighter(static_cast<int>(100 + 12 * m_hover));
        if (m_pressed) bg = bg.darker(118);
        p.setBrush(bg);
        p.setPen(QPen(base.lighter(135), 1.0));
        fg = QColor("#FFFFFF");
    } else if (m_variant == "success") {
        QColor base = Theme::instance().c("ok");
        QColor bg = base.lighter(static_cast<int>(100 + 12 * m_hover));
        if (m_pressed) bg = bg.darker(118);
        p.setBrush(bg);
        p.setPen(QPen(base.lighter(135), 1.0));
        fg = QColor("#FFFFFF");
    } else if (m_variant == "ghost") {
        const int alpha = static_cast<int>(42 * m_hover + (m_pressed ? 25 : 0));
        p.setBrush(Theme::instance().c("surface_hi", alpha));
        p.setPen(QPen(Theme::instance().c(m_hover > 0.3 ? "border_hi" : "border"), 1.0));
        fg = Theme::instance().c("text");
    } else { // "glass" or "secondary"
        QColor base = Theme::instance().c(m_hover > 0.4 ? "surface_hi" : "surface");
        base.setAlpha(static_cast<int>(180 + 55 * m_hover));
        p.setBrush(base);
        p.setPen(QPen(Theme::instance().c(m_hover > 0.2 ? "border_hi" : "border"), 1.0));
        fg = Theme::instance().c("text");
    }

    if (!isEnabled()) p.setOpacity(0.45);
    p.drawRoundedRect(r, radius, radius);

    if (m_busy) {
        const double cx = r.center().x();
        const double cy = r.center().y();
        QRectF spinRect(cx - 9.0, cy - 9.0, 18.0, 18.0);
        QPen pen(fg, 2.3);
        pen.setCapStyle(Qt::RoundCap);
        p.setPen(pen);
        p.drawArc(spinRect, -m_spinAngle * 16, 250 * 16);
        return;
    }

    const bool isSmall = height() <= 32;
    QFont font = makeFont(isSmall ? 12 : 13, !m_tintHex.isEmpty() ? QFont::Bold : QFont::DemiBold);
    p.setFont(font);
    p.setPen(fg);
    QFontMetrics fm(font);

    const bool hasIcon = !m_iconName.isEmpty() || !m_appPixmap.isNull();
    if (hasIcon && !m_text.isEmpty()) {
        const double iconS = isSmall ? 14.0 : 16.0;
        const double padEdge = isSmall ? 8.0 : 10.0;
        QRectF ir;
        QRectF tr;
        if (I18n::isRtl()) {
            ir = QRectF(r.right() - padEdge - iconS, r.center().y() - iconS / 2.0, iconS, iconS);
            tr = QRectF(r.left() + 6.0, r.top(), std::max(10.0, r.width() - iconS - padEdge - 10.0), r.height());
        } else {
            ir = QRectF(r.left() + padEdge, r.center().y() - iconS / 2.0, iconS, iconS);
            tr = QRectF(r.left() + iconS + padEdge + 4.0, r.top(), std::max(10.0, r.width() - iconS - padEdge - 10.0), r.height());
        }

        if (!m_appPixmap.isNull()) {
            p.drawPixmap(static_cast<int>(ir.x()), static_cast<int>(ir.y()), static_cast<int>(iconS), static_cast<int>(iconS), m_appPixmap);
        } else {
            Icons::draw(&p, m_iconName, ir, hasCustomIcColor ? icColor : fg, 2.0);
        }

        const QString elided = fm.elidedText(m_text, Qt::ElideRight, static_cast<int>(tr.width()));
        const int align = (!m_appPixmap.isNull() || m_fixedCompact)
            ? alignFlag(true, true)
            : static_cast<int>(Qt::AlignAbsolute | Qt::AlignCenter);
        p.drawText(tr, align, I18n::bidiText(elided));
    } else if (hasIcon) {
        QRectF ir(r.center().x() - 9.0, r.center().y() - 9.0, 18.0, 18.0);
        if (!m_appPixmap.isNull()) {
            p.drawPixmap(static_cast<int>(ir.x()), static_cast<int>(ir.y()), 18, 18, m_appPixmap);
        } else {
            Icons::draw(&p, m_iconName, ir, hasCustomIcColor ? icColor : fg, 2.0);
        }
    } else {
        QRectF tr = r.adjusted(10.0, 0.0, -10.0, 0.0);
        const QString elided = fm.elidedText(m_text, Qt::ElideRight, static_cast<int>(tr.width()));
        const QString dispTxt = m_forceLtrText
            ? QStringLiteral("\u200E%1\u200E").arg(elided)
            : I18n::bidiText(elided);
        p.drawText(tr, static_cast<int>(Qt::AlignAbsolute | Qt::AlignCenter), dispTxt);
    }
}

// ============================================================================
// ResponsiveFlowBar
// ============================================================================

ResponsiveFlowBar::ResponsiveFlowBar(const QList<Button*>& buttons, int rowHeight, int hSpacing, int vSpacing, QWidget* parent)
    : QWidget(parent), m_buttons(buttons), m_rowH(rowHeight), m_hSpace(hSpacing), m_vSpace(vSpacing) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    for (Button* b : m_buttons) {
        if (b) {
            b->setParent(this);
            b->setFixedHeight(rowHeight);
        }
    }
    relayout(920);
}

QSize ResponsiveFlowBar::sizeHint() const {
    return QSize(600, height());
}

QSize ResponsiveFlowBar::minimumSizeHint() const {
    return QSize(240, height());
}

void ResponsiveFlowBar::forceRelayout() {
    m_lastAvailW = -1;
    relayout(std::max(200, width()));
}

void ResponsiveFlowBar::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    relayout(std::max(200, width()));
}

void ResponsiveFlowBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    relayout(std::max(200, width()));
}

void ResponsiveFlowBar::relayout(int availW) {
    if (m_buttons.isEmpty()) {
        setFixedHeight(m_rowH);
        return;
    }
    const int rtl = I18n::isRtl() ? 1 : 0;
    if (std::abs(availW - m_lastAvailW) < 3 && rtl == m_lastRtl) return;
    m_lastAvailW = availW;
    m_lastRtl = rtl;

    const int n = m_buttons.size();
    QList<int> natWidths;
    int totalNat = 0;
    for (Button* b : m_buttons) {
        int nw = std::max(54, b ? b->naturalCompactWidth() : 54);
        natWidths.append(nw);
        totalNat += nw;
    }
    const int totalGaps = m_hSpace * (n - 1);
    totalNat += totalGaps;

    if (availW >= totalNat) {
        const int extraEach = std::min(10, std::max(0, (availW - totalNat) / n));
        int xCursor = rtl ? availW : 0;
        for (int i = 0; i < n; ++i) {
            Button* b = m_buttons[i];
            const int bw = natWidths[i] + extraEach;
            if (rtl) {
                xCursor -= bw;
                if (b) b->setGeometry(xCursor, 0, bw, m_rowH);
                xCursor -= m_hSpace;
            } else {
                if (b) b->setGeometry(xCursor, 0, bw, m_rowH);
                xCursor += bw + m_hSpace;
            }
        }
        if (height() != m_rowH) {
            setFixedHeight(m_rowH);
            updateGeometry();
        }
        return;
    }

    QList<QList<QPair<Button*, int>>> rows;
    QList<QPair<Button*, int>> curRow;
    int curW = 0;

    for (int i = 0; i < n; ++i) {
        Button* b = m_buttons[i];
        const int bw = std::min(natWidths[i], availW);
        const int needed = curRow.isEmpty() ? bw : (curW + m_hSpace + bw);
        if (!curRow.isEmpty() && needed > availW) {
            rows.append(curRow);
            curRow.clear();
            curRow.append(qMakePair(b, bw));
            curW = bw;
        } else {
            curRow.append(qMakePair(b, bw));
            curW = needed;
        }
    }
    if (!curRow.isEmpty()) rows.append(curRow);

    for (int rIdx = 0; rIdx < rows.size(); ++rIdx) {
        const int y = rIdx * (m_rowH + m_vSpace);
        int xCursor = rtl ? availW : 0;
        for (const auto& item : rows[rIdx]) {
            Button* b = item.first;
            const int bw = item.second;
            if (rtl) {
                xCursor -= bw;
                if (b) b->setGeometry(xCursor, y, bw, m_rowH);
                xCursor -= m_hSpace;
            } else {
                if (b) b->setGeometry(xCursor, y, bw, m_rowH);
                xCursor += bw + m_hSpace;
            }
        }
    }

    const int totalH = rows.size() * m_rowH + std::max(0, static_cast<int>(rows.size()) - 1) * m_vSpace;
    if (height() != totalH) {
        setFixedHeight(totalH);
        updateGeometry();
    }
}

// ============================================================================
// IconButton
// ============================================================================

IconButton::IconButton(const QString& iconName, const QString& tooltip, int size, const QString& variant, QWidget* parent)
    : QWidget(parent), m_iconName(iconName), m_variant(variant) {
    setFixedSize(size, size);
    setCursor(Qt::PointingHandCursor);
    if (!tooltip.isEmpty()) setToolTip(tooltip);
    m_anim = new QPropertyAnimation(this, "hoverProgress", this);
    m_anim->setDuration(140);
}

IconButton::IconButton(const QString& iconName, int size, const QString& variant, const QString& tooltip, QWidget* parent)
    : IconButton(iconName, tooltip, size, variant, parent) {}

void IconButton::setHoverProgress(double val) {
    m_hover = val;
    update();
}

void IconButton::setIconName(const QString& name) {
    m_iconName = name;
    update();
}

void IconButton::setVariant(const QString& variant) {
    m_variant = variant;
    update();
}

void IconButton::enterEvent(QEnterEvent*) {
    m_anim->stop();
    m_anim->setEndValue(1.0);
    m_anim->start();
}

void IconButton::leaveEvent(QEvent*) {
    m_anim->stop();
    m_anim->setEndValue(0.0);
    m_anim->start();
}

void IconButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
    }
}

void IconButton::mouseReleaseEvent(QMouseEvent* event) {
    if (m_pressed) {
        m_pressed = false;
        update();
        if (rect().contains(event->pos())) {
            emit clicked();
        }
    }
}

void IconButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);

    QColor bg, fg, border;
    if (m_variant == "danger") {
        bg = Theme::instance().c("danger", static_cast<int>(35 + 165 * m_hover));
        fg = m_hover > 0.5 ? QColor("#FFFFFF") : Theme::instance().c("danger");
        border = Theme::instance().c("danger", 110);
    } else if (m_variant == "ok" || m_variant == "success") {
        bg = Theme::instance().c("ok", static_cast<int>(38 + 150 * m_hover));
        fg = m_hover > 0.5 ? QColor("#FFFFFF") : Theme::instance().c("ok");
        border = Theme::instance().c("ok", 130);
    } else if (m_variant == "ghost") {
        bg = Theme::instance().c("surface_hi", static_cast<int>(140 * m_hover));
        fg = Theme::instance().c(m_hover > 0.3 ? "text" : "text2");
        border = Qt::transparent;
    } else {
        bg = Theme::instance().c(m_hover > 0.3 ? "surface_hi" : "surface");
        fg = Theme::instance().c(m_hover > 0.5 ? "accent" : "text");
        border = Theme::instance().c(m_hover > 0.3 ? "border_hi" : "border");
    }

    p.setBrush(bg);
    p.setPen(QPen(border, 1.0));
    p.drawRoundedRect(r, 9.0, 9.0);

    QRectF ir(r.center().x() - 7.5, r.center().y() - 7.5, 15.0, 15.0);
    Icons::draw(&p, m_iconName, ir, fg, 1.9);
}

// ============================================================================
// ToggleSwitch
// ============================================================================

ToggleSwitch::ToggleSwitch(bool checked, QWidget* parent)
    : QWidget(parent), m_checked(checked), m_pos(checked ? 1.0 : 0.0) {
    setFixedSize(44, 24);
    setCursor(Qt::PointingHandCursor);
    m_anim = new QPropertyAnimation(this, "thumbPos", this);
    m_anim->setDuration(220);
    m_anim->setEasingCurve(QEasingCurve::OutBack);
}

void ToggleSwitch::setThumbPos(double val) {
    m_pos = val;
    update();
}

void ToggleSwitch::setChecked(bool checked, bool emitSignal) {
    if (m_checked == checked) return;
    m_checked = checked;
    m_anim->stop();
    m_anim->setEndValue(checked ? 1.0 : 0.0);
    m_anim->start();
    if (emitSignal) emit toggled(m_checked);
}

void ToggleSwitch::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        if (rect().contains(event->pos())) {
            setChecked(!m_checked, true);
        }
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void ToggleSwitch::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);

    QColor offC = Theme::instance().c("bg2");
    QColor onC = Theme::instance().c("accent");
    const double tVal = qBound(0.0, m_pos, 1.0);
    QColor bg(
        static_cast<int>(offC.red() + (onC.red() - offC.red()) * tVal),
        static_cast<int>(offC.green() + (onC.green() - offC.green()) * tVal),
        static_cast<int>(offC.blue() + (onC.blue() - offC.blue()) * tVal)
    );
    p.setBrush(bg);
    p.setPen(QPen(Theme::instance().c(m_checked ? "border_hi" : "border"), 1.0));
    p.drawRoundedRect(r, r.height() / 2.0, r.height() / 2.0);

    const double thumbD = r.height() - 6.0;
    const double travel = r.width() - thumbD - 6.0;
    const double effectivePos = I18n::isRtl() ? (1.0 - m_pos) : m_pos;
    const double tx = r.left() + 3.0 + travel * effectivePos;
    const double ty = r.top() + 3.0;

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#FFFFFF"));
    p.drawEllipse(QRectF(tx, ty, thumbD, thumbD));
}

// ============================================================================
// SegmentedControl
// ============================================================================

SegmentedControl::SegmentedControl(const QList<QPair<QString, QString>>& items, const QString& selectedKey, QWidget* parent)
    : QWidget(parent), m_items(items) {
    for (int idx = 0; idx < items.size(); ++idx) {
        if (items[idx].first == selectedKey) {
            m_selectedIdx = idx;
            break;
        }
    }
    m_thumbIdx = static_cast<double>(m_selectedIdx);
    setFixedHeight(36);
    setCursor(Qt::PointingHandCursor);
    m_anim = new QPropertyAnimation(this, "thumbIndex", this);
    m_anim->setDuration(200);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
    recalcMinWidth();
}

void SegmentedControl::recalcMinWidth() {
    if (m_items.isEmpty()) {
        setMinimumWidth(165);
        return;
    }
    QFontMetrics fm(makeFont(12, QFont::DemiBold));
    int maxTw = 60;
    for (const auto& pair : m_items) {
        maxTw = std::max(maxTw, fm.horizontalAdvance(I18n::bidiText(pair.second)));
    }
    const int segW = std::max(108, maxTw + 38);
    setMinimumWidth(std::max(210, static_cast<int>(m_items.size()) * segW + 14));
}

void SegmentedControl::setThumbIndex(double val) {
    m_thumbIdx = val;
    update();
}

void SegmentedControl::setItems(const QList<QPair<QString, QString>>& items, const QString& selectedKey) {
    m_items = items;
    m_selectedIdx = 0;
    for (int idx = 0; idx < items.size(); ++idx) {
        if (items[idx].first == selectedKey) {
            m_selectedIdx = idx;
            break;
        }
    }
    m_thumbIdx = static_cast<double>(m_selectedIdx);
    recalcMinWidth();
    updateGeometry();
    update();
}

QString SegmentedControl::currentKey() const {
    if (m_items.isEmpty()) return QString();
    return m_items[qBound(0, m_selectedIdx, static_cast<int>(m_items.size()) - 1)].first;
}

void SegmentedControl::setSelectedKey(const QString& key, bool emitSignal) {
    for (int idx = 0; idx < m_items.size(); ++idx) {
        if (m_items[idx].first == key) {
            m_selectedIdx = idx;
            m_anim->stop();
            m_anim->setEndValue(static_cast<double>(idx));
            m_anim->start();
            if (emitSignal) emit changed(key);
            break;
        }
    }
}

void SegmentedControl::mouseReleaseEvent(QMouseEvent* event) {
    if (m_items.isEmpty() || event->button() != Qt::LeftButton) return;
    const int n = m_items.size();
    const double segW = static_cast<double>(width()) / n;
    int clickedVisual = static_cast<int>(event->position().x() / segW);
    clickedVisual = qBound(0, clickedVisual, n - 1);
    const int logicalIdx = I18n::isRtl() ? (n - 1 - clickedVisual) : clickedVisual;
    if (logicalIdx != m_selectedIdx) {
        m_selectedIdx = logicalIdx;
        m_anim->stop();
        m_anim->setEndValue(static_cast<double>(logicalIdx));
        m_anim->start();
        emit changed(m_items[logicalIdx].first);
    }
}

void SegmentedControl::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    p.setBrush(Theme::instance().c("bg1"));
    p.setPen(QPen(Theme::instance().c("border"), 1.0));
    p.drawRoundedRect(r, 11.0, 11.0);

    const int n = m_items.size();
    if (n == 0) return;

    const double segW = (r.width() - 6.0) / n;
    const double visualThumb = I18n::isRtl() ? (n - 1 - m_thumbIdx) : m_thumbIdx;
    QRectF thumbRect(r.left() + 3.0 + visualThumb * segW, r.top() + 3.0, segW, r.height() - 6.0);

    QLinearGradient grad(thumbRect.topLeft(), thumbRect.bottomRight());
    grad.setColorAt(0.0, Theme::instance().c("accent"));
    grad.setColorAt(1.0, Theme::instance().c("accent2"));
    p.setBrush(grad);
    p.setPen(QPen(Theme::instance().c("accent_hi", 110), 1.0));
    p.drawRoundedRect(thumbRect, 8.0, 8.0);

    QFont segFont = makeFont(12, QFont::DemiBold);
    p.setFont(segFont);
    QFontMetrics fm(segFont);
    for (int idx = 0; idx < n; ++idx) {
        const int visI = I18n::isRtl() ? (n - 1 - idx) : idx;
        QRectF sr(r.left() + 3.0 + visI * segW, r.top(), segW, r.height());
        QRectF innerSr = sr.adjusted(4.0, 0.0, -4.0, 0.0);
        const bool active = std::abs(idx - m_thumbIdx) < 0.45;
        p.setPen(Theme::instance().c(active ? "on_accent" : "text2"));
        const QString elided = fm.elidedText(m_items[idx].second, Qt::ElideRight, std::max(20, static_cast<int>(innerSr.width())));
        p.drawText(innerSr, static_cast<int>(Qt::AlignAbsolute | Qt::AlignCenter), I18n::bidiText(elided));
    }
}

// ============================================================================
// NumberInput
// ============================================================================

NumberInput::NumberInput(double value, double minVal, double maxVal, double step, const QString& suffix, int width, QWidget* parent)
    : QWidget(parent), m_val(value), m_min(minVal), m_max(maxVal), m_step(step), m_decimals(0), m_suffix(suffix) {
    setFixedSize(width, 34);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    m_inlineEdit = new QLineEdit(this);
    m_inlineEdit->hide();
    m_inlineEdit->setAlignment(Qt::AlignCenter);
    connect(m_inlineEdit, &QLineEdit::returnPressed, this, &NumberInput::commitInlineEdit);
    connect(m_inlineEdit, &QLineEdit::editingFinished, this, &NumberInput::commitInlineEdit);
}

NumberInput::NumberInput(double value, double minVal, double maxVal, double step, int decimals, const QString& suffix, QWidget* parent)
    : NumberInput(value, minVal, maxVal, step, suffix, 130, parent) {
    m_decimals = decimals;
}

void NumberInput::setValue(double val, bool emitSignal) {
    double clamped = qBound(m_min, val, m_max);
    if (m_decimals == 0) {
        clamped = static_cast<double>(qRound64(clamped));
    } else {
        const double factor = std::pow(10.0, m_decimals);
        clamped = std::round(clamped * factor) / factor;
    }
    if (std::abs(clamped - m_val) > 1e-6) {
        m_val = clamped;
        update();
        if (emitSignal) {
            emit valueChanged(static_cast<int>(qRound(m_val)));
            emit doubleValueChanged(m_val);
        }
    }
}

void NumberInput::setSuffix(const QString& suffix) {
    m_suffix = suffix;
    update();
}

void NumberInput::startInlineEdit() {
    emit focused();
    const QString rawStr = (m_decimals == 0)
        ? QString::number(qRound64(m_val))
        : QString::number(m_val, 'f', m_decimals);
    m_inlineEdit->setGeometry(26, 2, width() - 52, height() - 4);
    m_inlineEdit->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            font-family: '%4';
            font-size: 13px;
            font-weight: bold;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("accent_hi"),
            Theme::instance().tokenHex("accent"),
            I18n::langFontFamily()));
    m_inlineEdit->setText(rawStr);
    m_inlineEdit->show();
    m_inlineEdit->setFocus();
    m_inlineEdit->selectAll();
}

void NumberInput::commitInlineEdit() {
    if (!m_inlineEdit->isVisible()) return;
    QString txt = m_inlineEdit->text().trimmed();
    const QString arDigits = QString::fromUtf8("٠١٢٣٤٥٦٧٨٩");
    const QString faDigits = QString::fromUtf8("۰۱۲۳۴۵۶۷۸۹");
    for (int i = 0; i < 10; ++i) {
        txt.replace(arDigits[i], QChar('0' + i));
        txt.replace(faDigits[i], QChar('0' + i));
    }
    m_inlineEdit->hide();
    bool ok = false;
    const double v = txt.toDouble(&ok);
    if (ok) {
        setValue(v, true);
    } else {
        update();
    }
}

void NumberInput::mouseMoveEvent(QMouseEvent* event) {
    const double x = event->position().x();
    if (x < 26.0) m_hoverPart = 1;
    else if (x > width() - 26.0) m_hoverPart = 2;
    else m_hoverPart = 3;
    update();
}

void NumberInput::leaveEvent(QEvent*) {
    m_hoverPart = 0;
    update();
}

void NumberInput::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;
    const double x = event->position().x();
    if (x < 26.0) setValue(m_val - m_step, true);
    else if (x > width() - 26.0) setValue(m_val + m_step, true);
    else startInlineEdit();
}

void NumberInput::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    p.setBrush(Theme::instance().c("bg0", 210));
    p.setPen(QPen(Theme::instance().c(m_hoverPart == 3 ? "border_hi" : "border"), 1.0));
    p.drawRoundedRect(r, 9.0, 9.0);

    QRectF leftR(r.left() + 2.0, r.top() + 2.0, 23.0, r.height() - 4.0);
    QRectF rightR(r.right() - 25.0, r.top() + 2.0, 23.0, r.height() - 4.0);

    if (m_hoverPart == 1) {
        p.setBrush(Theme::instance().c("surface_hi"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(leftR, 6.0, 6.0);
    } else if (m_hoverPart == 2) {
        p.setBrush(Theme::instance().c("surface_hi"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rightR, 6.0, 6.0);
    }

    Icons::draw(&p, "minimize", leftR.adjusted(4.0, 4.0, -4.0, -4.0), Theme::instance().c("text2"));
    Icons::draw(&p, "plus", rightR.adjusted(4.0, 4.0, -4.0, -4.0), Theme::instance().c("accent"));

    const QString rawNum = (m_decimals == 0)
        ? QString::number(qRound64(m_val))
        : QString::number(m_val, 'f', m_decimals);
    QString disp = I18n::fmtNum(rawNum);
    if (!m_suffix.isEmpty()) disp = QStringLiteral("%1 %2").arg(disp, m_suffix);

    p.setFont(makeFont(12, QFont::Bold));
    p.setPen(Theme::instance().c("text"));
    QRectF midR(leftR.right(), r.top(), r.width() - 50.0, r.height());
    p.drawText(midR, static_cast<int>(Qt::AlignAbsolute | Qt::AlignCenter), disp);
}

// ============================================================================
// SearchPopup & SearchCombo
// ============================================================================

SearchPopup::SearchPopup(QWidget* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint) {
    setAttribute(Qt::WA_TranslucentBackground, true);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(10, 10, 10, 10);
    lay->setSpacing(6);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(I18n::t("search_placeholder"));
    m_searchEdit->setFixedHeight(32);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SearchPopup::onFilter);
    lay->addWidget(m_searchEdit);

    m_scroll = new QScrollArea(this);
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setStyleSheet(modernScrollStyle());

    m_listContainer = new QWidget();
    m_listContainer->setStyleSheet("background: transparent;");
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 2, 0);
    m_listLayout->setSpacing(4);
    m_scroll->setWidget(m_listContainer);
    lay->addWidget(m_scroll);
}

void SearchPopup::populate(const QList<SearchComboItem>& items) {
    m_items = items;
    m_scroll->setStyleSheet(modernScrollStyle());
    m_searchEdit->setPlaceholderText(I18n::t("search_placeholder"));
    m_searchEdit->setStyleSheet(QStringLiteral(R"(
        QLineEdit {
            background: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 8px;
            padding: 4px 10px;
            font-family: '%4';
            font-size: 13px;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            Theme::instance().tokenHex("border_hi"),
            I18n::langFontFamily()));
    m_searchEdit->clear();
    rebuildRows(m_items);
}

void SearchPopup::onFilter(const QString& q) {
    const QString qLow = q.trimmed().toLower();
    if (qLow.isEmpty()) {
        rebuildRows(m_items);
    } else {
        QList<SearchComboItem> filtered;
        for (const auto& row : m_items) {
            if (row.key.toLower().contains(qLow) || row.label.toLower().contains(qLow)) {
                filtered.append(row);
            }
        }
        rebuildRows(filtered);
    }
}

void SearchPopup::rebuildRows(const QList<SearchComboItem>& rows) {
    while (QLayoutItem* it = m_listLayout->takeAt(0)) {
        if (QWidget* w = it->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete it;
    }

    for (const auto& row : rows) {
        auto* btn = new Button(
            row.label,
            row.tintHex.isEmpty() ? "ghost" : "tinted_row",
            QString(),
            row.tintHex,
            true,
            m_listContainer
        );
        if (!row.pixmap.isNull()) btn->setAppPixmap(row.pixmap);
        btn->setFixedHeight(36);
        const QString k = row.key;
        const QString lbl = row.label;
        connect(btn, &Button::clicked, this, [this, k, lbl]() { pick(k, lbl); });
        m_listLayout->addWidget(btn);
    }
    m_listLayout->addStretch(1);
}

void SearchPopup::pick(const QString& k, const QString& lbl) {
    hide();
    emit itemSelected(k, lbl);
}

void SearchPopup::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(2.0, 2.0, -2.0, -2.0);
    p.setBrush(Theme::instance().c("surface"));
    p.setPen(QPen(Theme::instance().c("border_hi"), 1.2));
    p.drawRoundedRect(r, 14.0, 14.0);
}

SearchCombo::SearchCombo(QWidget* parent)
    : QWidget(parent) {
    setFixedHeight(34);
    setMinimumWidth(135);
    setCursor(Qt::PointingHandCursor);
    m_popup = new SearchPopup(this);
    connect(m_popup, &SearchPopup::itemSelected, this, &SearchCombo::onSelected);
}

SearchCombo::SearchCombo(const QList<QPair<QString, QString>>& items, const QString& selectedKey, QWidget* parent)
    : SearchCombo(parent) {
    setItems(items, selectedKey);
}


void SearchCombo::setItems(const QList<QPair<QString, QString>>& items, const QString& selectedKey) {
    QList<SearchComboItem> rich;
    rich.reserve(items.size());
    for (const auto& pair : items) {
        rich.append(SearchComboItem(pair.first, pair.second));
    }
    setRichItems(rich, selectedKey);
}

void SearchCombo::setRichItems(const QList<SearchComboItem>& items, const QString& selectedKey) {
    m_items = items;
    bool foundSelected = false;
    bool foundCurrent = false;
    for (const auto& row : m_items) {
        if (!selectedKey.isEmpty() && row.key == selectedKey) foundSelected = true;
        if (!m_selectedKey.isEmpty() && row.key == m_selectedKey) foundCurrent = true;
    }
    if (foundSelected) {
        m_selectedKey = selectedKey;
    } else if (!m_items.isEmpty() && !foundCurrent) {
        m_selectedKey = m_items.first().key;
    }
    update();
}

QString SearchCombo::currentLabel() const {
    for (const auto& row : m_items) {
        if (row.key == m_selectedKey) return row.label;
    }
    return m_selectedKey;
}

QPixmap SearchCombo::currentPixmap() const {
    for (const auto& row : m_items) {
        if (row.key == m_selectedKey) return row.pixmap;
    }
    return QPixmap();
}

QString SearchCombo::currentTintHex() const {
    for (const auto& row : m_items) {
        if (row.key == m_selectedKey) return row.tintHex;
    }
    return QString();
}

void SearchCombo::setSelectedKey(const QString& key, bool emitSignal) {
    m_selectedKey = key;
    update();
    if (emitSignal) emit currentChanged(key);
}

void SearchCombo::onSelected(const QString& key, const QString&) {
    m_selectedKey = key;
    update();
    emit currentChanged(key);
}

void SearchCombo::enterEvent(QEnterEvent*) {
    m_hover = true;
    update();
}

void SearchCombo::leaveEvent(QEvent*) {
    m_hover = false;
    update();
}

void SearchCombo::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_popup->populate(m_items);
        QFontMetrics fm(makeFont(12, QFont::DemiBold));
        int longestPx = 240;
        for (const auto& row : m_items) {
            longestPx = std::max(longestPx, fm.horizontalAdvance(row.label) + (!row.pixmap.isNull() ? 92 : 64));
        }
        const int desiredW = std::max({280, width(), longestPx});
        int popupW = std::min(desiredW, 780);
        int popupH = 280;
        int x = 0, y = 0;

        if (QScreen* scr = screen()) {
            const QRect avail = scr->availableGeometry();
            popupW = std::min(desiredW, std::max(280, avail.width() - 32));
            popupH = std::min({320, std::max(200, static_cast<int>(m_items.size()) * 40 + 68), avail.height() - 40});
            QPoint gpos = mapToGlobal(QPoint(I18n::isRtl() ? (width() - popupW) : 0, height() + 4));
            x = qBound(avail.left() + 8, gpos.x(), avail.right() - popupW - 8);
            y = gpos.y();
            if (y + popupH > avail.bottom() - 8) {
                y = std::max(avail.top() + 8, mapToGlobal(QPoint(0, -popupH - 4)).y());
            }
        } else {
            QPoint gpos = mapToGlobal(QPoint(0, height() + 4));
            x = gpos.x();
            y = gpos.y();
        }
        m_popup->setFixedSize(popupW, popupH);
        m_popup->move(x, y);
        m_popup->show();
        m_popup->searchEdit()->setFocus();
    }
}

void SearchCombo::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    const QString selTint = currentTintHex();
    if (!selTint.isEmpty()) {
        QColor tc(selTint);
        tc.setAlpha(m_hover ? 68 : 45);
        p.setBrush(tc);
        p.setPen(QPen(QColor(selTint), 1.5));
    } else {
        p.setBrush(Theme::instance().c(m_hover ? "surface_hi" : "bg0", 210));
        p.setPen(QPen(Theme::instance().c(m_hover ? "border_hi" : "border"), 1.0));
    }
    p.drawRoundedRect(r, 9.0, 9.0);

    QPixmap pix = currentPixmap();
    QFont font = makeFont(12, QFont::DemiBold);
    p.setFont(font);
    p.setPen(Theme::instance().c("text"));
    QFontMetrics fm(font);

    QRectF chevR, tr;
    if (I18n::isRtl()) {
        chevR = QRectF(r.left() + 8.0, r.center().y() - 7.0, 14.0, 14.0);
        if (!pix.isNull()) {
            p.drawPixmap(static_cast<int>(r.right() - 28.0), static_cast<int>(r.center().y() - 9.0), 18, 18, pix);
            tr = QRectF(r.left() + 26.0, r.top(), r.width() - 58.0, r.height());
        } else {
            tr = QRectF(r.left() + 26.0, r.top(), r.width() - 36.0, r.height());
        }
    } else {
        chevR = QRectF(r.right() - 22.0, r.center().y() - 7.0, 14.0, 14.0);
        if (!pix.isNull()) {
            p.drawPixmap(static_cast<int>(r.left() + 10.0), static_cast<int>(r.center().y() - 9.0), 18, 18, pix);
            tr = QRectF(r.left() + 34.0, r.top(), r.width() - 58.0, r.height());
        } else {
            tr = QRectF(r.left() + 10.0, r.top(), r.width() - 36.0, r.height());
        }
    }

    const QString elided = fm.elidedText(currentLabel(), Qt::ElideRight, static_cast<int>(tr.width()));
    p.drawText(tr, alignFlag(true, true), I18n::bidiText(elided));
    Icons::draw(&p, "chevron_down", chevR, Theme::instance().c("text2"), 2.0);
}

// ============================================================================
// KeyCaptureButton
// ============================================================================

KeyCaptureButton::KeyCaptureButton(AppState* state, const QString& initialKey, bool allowChord, QWidget* parent)
    : KeyCaptureButton(initialKey, 0, false, {}, allowChord, state, false, parent) {}

KeyCaptureButton::KeyCaptureButton(const QString& currentKey, int scanCode, bool isExtended,
                                   const QList<QPair<int, bool>>& chord, bool allowChord,
                                   AppState* state, bool largeHero, QWidget* parent)
    : QWidget(parent), m_state(state), m_largeHero(largeHero), m_allowChord(allowChord) {
    if (!chord.isEmpty()) {
        for (const auto& pair : chord) {
            int sc = pair.first <= 0 ? 0x1B7 : pair.first;
            bool ext = pair.first <= 0 ? true : pair.second;
            m_chord.append(qMakePair(sc, ext));
        }
    } else if (scanCode > 0) {
        m_chord.append(qMakePair(scanCode, isExtended));
    } else if (!currentKey.isEmpty()) {
        QString clean = currentKey;
        clean.replace("Key_0", "Calculator");
        for (const QString& pLbl : clean.split('+')) {
            const QString trimmed = pLbl.trimmed();
            if (!trimmed.isEmpty()) {
                auto scExt = Win32Input::scancodeForLabel(trimmed);
                if (scExt.first > 0) m_chord.append(scExt);
            }
        }
    }

    m_keyStr = !m_chord.isEmpty() ? formatChordLabel(m_chord) : QString(currentKey).replace("Key_0", "Calculator");

    setFixedHeight(largeHero ? 44 : 34);
    setMinimumWidth(largeHero ? 200 : 108);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);

    if (m_state) {
        connect(m_state, &AppState::hookKeyEvent, this, &KeyCaptureButton::onHookKeyEvent);
    }
}

void KeyCaptureButton::setLargeHero(bool largeHero) {
    m_largeHero = largeHero;
    setFixedHeight(largeHero ? 44 : 34);
    setMinimumWidth(largeHero ? 200 : 108);
    update();
}

QString KeyCaptureButton::formatChordLabel(const QList<QPair<int, bool>>& chord) {
    if (chord.isEmpty()) return QString();
    QList<QPair<int, bool>> mods;
    QList<QPair<int, bool>> normals;
    for (const auto& c : chord) {
        if (Win32Input::isModifierScancode(c.first)) mods.append(c);
        else normals.append(c);
    }
    QStringList parts;
    for (const auto& c : mods) parts.append(Win32Input::labelForScancode(c.first, c.second));
    for (const auto& c : normals) parts.append(Win32Input::labelForScancode(c.first, c.second));
    return parts.join(" + ");
}

int KeyCaptureButton::scanCode() const {
    return m_chord.isEmpty() ? 0 : m_chord.last().first;
}

bool KeyCaptureButton::isExtended() const {
    return m_chord.isEmpty() ? false : m_chord.last().second;
}

void KeyCaptureButton::clearKey() {
    m_chord.clear();
    m_pendingChord.clear();
    m_keyStr.clear();
    m_listening = false;
    if (m_state) m_state->isCapturingKey = false;
    update();
    emit keyCaptured(QString(), 0, false);
    emit keyStringCaptured(QString());
    emit chordCaptured({}, QString());
}

void KeyCaptureButton::setChordData(const QList<QPair<int, bool>>& chord, const QString& label) {
    QList<QPair<int, bool>> cleaned;
    for (const auto& pair : chord) {
        int sVal = pair.first <= 0 ? 0x1B7 : pair.first;
        bool eVal = pair.first <= 0 ? true : pair.second;
        if (!cleaned.contains(qMakePair(sVal, eVal))) {
            cleaned.append(qMakePair(sVal, eVal));
        }
    }
    m_chord = cleaned;
    m_keyStr = !m_chord.isEmpty() ? formatChordLabel(m_chord) : QString(label).replace("Key_0", "Calculator");
    m_listening = false;
    update();
}

void KeyCaptureButton::setKeyData(const QString& keyStr, int scanCode, bool isExtended, const QList<QPair<int, bool>>& chord) {
    QString cleanStr = keyStr;
    cleanStr.replace("Key_0", "Calculator");
    if (!chord.isEmpty()) {
        setChordData(chord, cleanStr);
        return;
    }
    if (cleanStr.isEmpty() && scanCode <= 0) {
        m_chord.clear();
        m_keyStr.clear();
    } else if (cleanStr.contains('+')) {
        QList<QPair<int, bool>> parsed;
        for (const QString& pLbl : cleanStr.split('+')) {
            auto scExt = Win32Input::scancodeForLabel(pLbl.trimmed());
            if (scExt.first > 0 && !parsed.contains(scExt)) {
                parsed.append(scExt);
            }
        }
        m_chord = parsed;
        m_keyStr = formatChordLabel(m_chord);
    } else {
        auto scExt = (scanCode > 0) ? qMakePair(scanCode, isExtended) : Win32Input::scancodeForLabel(cleanStr);
        m_chord.clear();
        if (scExt.first > 0) {
            m_chord.append(scExt);
            m_keyStr = Win32Input::labelForScancode(scExt.first, scExt.second);
        } else {
            m_keyStr = cleanStr;
        }
    }
    m_listening = false;
    update();
}

void KeyCaptureButton::enterEvent(QEnterEvent*) {
    m_hover = true;
    update();
}

void KeyCaptureButton::leaveEvent(QEvent*) {
    m_hover = false;
    update();
}

void KeyCaptureButton::startCapture() {
    m_listening = true;
    s_activeKeyCaptureWidget = this;
    m_pendingChord.clear();
    m_heldCount = 0;
    if (m_state) m_state->isCapturingKey = true;
    setFocus();
    update();
}

void KeyCaptureButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (!m_listening) {
            startCapture();
        } else {
            m_listening = false;
            if (s_activeKeyCaptureWidget == this) s_activeKeyCaptureWidget = nullptr;
            if (m_state) m_state->isCapturingKey = false;
            update();
        }
    }
}

void KeyCaptureButton::finalizeCapture() {
    if (m_pendingChord.isEmpty()) return;
    s_activeKeyCaptureWidget = nullptr;
    m_listening = false;
    if (m_state) m_state->isCapturingKey = false;

    m_chord = m_pendingChord;
    m_pendingChord.clear();
    m_heldCount = 0;
    m_keyStr = formatChordLabel(m_chord);
    update();

    const int sc = m_chord.last().first;
    const bool ext = m_chord.last().second;
    emit keyCaptured(m_keyStr, sc, ext);
    emit keyStringCaptured(m_keyStr);
    emit scanKeyCaptured(sc, ext, m_keyStr);
    emit chordCaptured(m_chord, m_keyStr);
}

void KeyCaptureButton::onHookKeyEvent(int scanCode, bool isExtended, bool isDown, const QString&) {
    if (!m_listening || s_activeKeyCaptureWidget != this) return;
    if (scanCode <= 0) return;
    const auto kid = qMakePair(scanCode, isExtended);
    if (isDown) {
        if (!m_pendingChord.contains(kid) && m_pendingChord.size() < (m_allowChord ? 3 : 1)) {
            m_pendingChord.append(kid);
            m_heldCount += 1;
            update();
            if (!m_allowChord || m_pendingChord.size() >= 3) {
                finalizeCapture();
            }
        }
    } else {
        if (!m_pendingChord.isEmpty()) {
            finalizeCapture();
        }
    }
}

void KeyCaptureButton::keyPressEvent(QKeyEvent* event) {
    if (!m_listening) {
        QWidget::keyPressEvent(event);
        return;
    }
    const int hwSc = static_cast<int>(event->nativeScanCode());
    int sc = 0;
    bool ext = false;
    QString lbl;
    if ((hwSc & 0xFF) > 0) {
        sc = hwSc & 0xFF;
        ext = (hwSc & 0x100) != 0;
        lbl = Win32Input::labelForScancode(sc, ext);
    } else {
        const QString txt = event->text().trimmed().toUpper();
        lbl = txt.isEmpty() ? QStringLiteral("Space") : txt;
        auto scExt = Win32Input::scancodeForLabel(lbl);
        sc = scExt.first;
        ext = scExt.second;
    }
    onHookKeyEvent(sc, ext, true, lbl);
    event->accept();
}

void KeyCaptureButton::keyReleaseEvent(QKeyEvent* event) {
    if (!m_listening) {
        QWidget::keyReleaseEvent(event);
        return;
    }
    if (!m_pendingChord.isEmpty()) {
        finalizeCapture();
    }
    event->accept();
}

void KeyCaptureButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
    const double rad = m_largeHero ? 12.0 : 9.0;

    QString label;
    QColor fg;
    if (m_listening) {
        p.setBrush(Theme::instance().c("accent", 55));
        p.setPen(QPen(Theme::instance().c("accent_hi"), 2.0));
        if (!m_pendingChord.isEmpty()) {
            label = QStringLiteral("[ %1 + ... ]").arg(formatChordLabel(m_pendingChord));
        } else {
            label = m_allowChord
                ? I18n::tl(QString::fromUtf8("اضغط زراراً أو 2 أو 3 معاً الآن..."), QStringLiteral("Press 1, 2, or 3 keys together now..."))
                : I18n::tl(QString::fromUtf8("اضغط الزرار الآن..."), QStringLiteral("Press key now..."));
        }
        fg = Theme::instance().c("accent_hi");
    } else if (m_keyStr.isEmpty()) {
        p.setBrush(Theme::instance().c(m_hover ? "surface_hi" : "bg0", 210));
        p.setPen(QPen(Theme::instance().c(m_hover ? "accent_hi" : "border_hi"), 1.4, Qt::DashLine));
        label = m_largeHero
            ? I18n::tl(QString::fromUtf8("بدون زرار (تشغيل يدوي) — اضغط هنا لتحديد اختصار"),
                       QStringLiteral("No Key (Manual Only) — Click to bind shortcut"))
            : I18n::tl(QString::fromUtf8("تحديد اختصار"), QStringLiteral("Bind Key"));
        fg = Theme::instance().c("text2");
    } else {
        p.setBrush(Theme::instance().c(m_hover ? "surface_hi" : "bg0", 220));
        p.setPen(QPen(Theme::instance().c(m_hover ? "border_hi" : "border"), 1.2));
        if (m_largeHero) {
            label = I18n::tl(
                QString::fromUtf8("الاختصار المحدد:  [ %1 ]  (اضغط لتغييره)").arg(m_keyStr),
                QStringLiteral("Bound Shortcut:  [ %1 ]  (Click to change)").arg(m_keyStr));
        } else {
            label = QStringLiteral("[ %1 ]").arg(m_keyStr);
        }
        fg = Theme::instance().c("text");
    }

    p.drawRoundedRect(r, rad, rad);
    p.setFont(makeFont(m_largeHero ? 14 : 12, m_largeHero ? QFont::ExtraBold : QFont::Bold));
    p.setPen(fg);
    p.drawText(r, static_cast<int>(Qt::AlignAbsolute | Qt::AlignCenter), I18n::bidiText(label));
}

// ============================================================================
// StepCard
// ============================================================================

StepCard::StepCard(int index, const QString& accentHex, QWidget* parent)
    : QWidget(parent), m_index(index), m_accentHex(accentHex) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

StepCard::StepCard(const QString& tintHex, bool enabled, QWidget* parent)
    : QWidget(parent), m_index(0), m_accentHex(tintHex), m_stepEnabled(enabled) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void StepCard::setIndex(int newIndex) {
    if (m_index != newIndex) {
        m_index = newIndex;
        update();
    }
}

void StepCard::setDragging(bool dragging) {
    if (m_dragging != dragging) {
        m_dragging = dragging;
        update();
    }
}

void StepCard::setStepEnabled(bool enabled) {
    if (m_stepEnabled != enabled) {
        m_stepEnabled = enabled;
        update();
    }
}

void StepCard::setAccentHex(const QString& accentHex) {
    m_accentHex = accentHex;
    update();
}

void StepCard::setTint(const QString& tintHex, bool enabled) {
    m_accentHex = tintHex;
    m_stepEnabled = enabled;
    update();
}

void StepCard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    const double radius = 13.0;

    const bool isOddStep = (m_index % 2) == 0;
    QColor stripeC(m_accentHex);
    if (!stripeC.isValid()) stripeC = Theme::instance().c("accent");

    QColor tint;
    QColor borderC;
    double borderW = 1.1;

    const bool isDk = Theme::instance().isDark();
    if (m_dragging) {
        QColor base = Theme::instance().c("surface_hi");
        tint = QColor(
            static_cast<int>(base.red() * 0.76 + stripeC.red() * 0.24),
            static_cast<int>(base.green() * 0.76 + stripeC.green() * 0.24),
            static_cast<int>(base.blue() * 0.76 + stripeC.blue() * 0.24),
            248
        );
        borderC = stripeC.lighter(isDk ? 125 : 105);
        borderW = 2.2;
    } else if (isOddStep) {
        QColor base = isDk ? Theme::instance().c("surface") : QColor("#FFFFFF");
        const double mix = isDk ? 0.11 : 0.08;
        tint = QColor(
            static_cast<int>(base.red() * (1.0 - mix) + stripeC.red() * mix),
            static_cast<int>(base.green() * (1.0 - mix) + stripeC.green() * mix),
            static_cast<int>(base.blue() * (1.0 - mix) + stripeC.blue() * mix),
            isDk ? 230 : 248
        );
        borderC = QColor(stripeC.red(), stripeC.green(), stripeC.blue(), isDk ? 90 : 105);
        borderW = 1.1;
    } else {
        QColor base = isDk ? Theme::instance().c("bg2") : Theme::instance().c("surface_hi");
        const double mix = isDk ? 0.07 : 0.05;
        tint = QColor(
            static_cast<int>(base.red() * (1.0 - mix) + stripeC.red() * mix),
            static_cast<int>(base.green() * (1.0 - mix) + stripeC.green() * mix),
            static_cast<int>(base.blue() * (1.0 - mix) + stripeC.blue() * mix),
            isDk ? 225 : 245
        );
        borderC = QColor(stripeC.red(), stripeC.green(), stripeC.blue(), isDk ? 70 : 85);
        borderW = 1.1;
    }

    if (!m_stepEnabled) {
        p.setOpacity(0.56);
        p.setBrush(tint);
        p.setPen(QPen(QColor("#F59E0B"), 1.3, Qt::DashLine));
    } else {
        p.setBrush(tint);
        p.setPen(QPen(borderC, borderW));
    }
    p.drawRoundedRect(r, radius, radius);

    const double barW = 4.5;
    QRectF barR = I18n::isRtl()
        ? QRectF(r.right() - barW - 2.0, r.top() + 6.0, barW, r.height() - 12.0)
        : QRectF(r.left() + 2.0, r.top() + 6.0, barW, r.height() - 12.0);
    p.setPen(Qt::NoPen);
    p.setBrush(!m_stepEnabled ? QColor("#94A3B8") : stripeC);
    p.drawRoundedRect(barR, 2.2, 2.2);
}

// ============================================================================
// GlassCard
// ============================================================================

GlassCard::GlassCard(bool accentTint, QWidget* parent)
    : QWidget(parent), m_accentTint(accentTint) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

GlassCard::GlassCard(const QString& variant, QWidget* parent)
    : GlassCard(variant == "accent" || variant == "header", parent) {}

void GlassCard::setAccentTint(bool accentTint) {
    m_accentTint = accentTint;
    update();
}

void GlassCard::setVariant(const QString& variant) {
    m_accentTint = (variant == "accent" || variant == "header");
    update();
}

void GlassCard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    const double radius = 18.0;

    p.setBrush(Theme::instance().c("glass"));
    p.setPen(QPen(Theme::instance().c(m_accentTint ? "border_hi" : "border"), 1.0));
    p.drawRoundedRect(r, radius, radius);

    if (m_accentTint) {
        QLinearGradient grad(r.topLeft(), QPointF(r.left(), r.top() + 55.0));
        grad.setColorAt(0.0, Theme::instance().c("accent", 28));
        grad.setColorAt(1.0, Theme::instance().c("accent", 0));
        p.setBrush(grad);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(r, radius, radius);
    }
}

// ============================================================================
// ToastOverlay & SlideStack
// ============================================================================

ToastOverlay::ToastOverlay(QWidget* parent)
    : QWidget(parent) {
    setFixedHeight(42);
    hide();
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &QWidget::hide);
}

void ToastOverlay::showToast(const QString& message, const QString& kind, int durationMs) {
    m_msg = message;
    m_kind = kind;
    QFontMetrics fm(makeFont(13, QFont::Bold));
    const int w = qBound(240, fm.horizontalAdvance(message) + 74, 600);
    setFixedWidth(w);
    if (QWidget* pw = parentWidget()) {
        move((pw->width() - w) / 2, pw->height() - 62);
    }
    show();
    raise();
    update();
    m_timer->start(durationMs);
}

void ToastOverlay::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);

    p.setBrush(Theme::instance().c("surface_hi"));
    QColor borderC = (m_kind == "ok" || m_kind == "success") ? Theme::instance().c("ok")
        : (m_kind == "danger" || m_kind == "error") ? Theme::instance().c("danger")
        : Theme::instance().c("accent_hi");
    p.setPen(QPen(borderC, 1.3));
    p.drawRoundedRect(r, 18.0, 18.0);

    const QString iconName = (m_kind == "ok" || m_kind == "success") ? QStringLiteral("check") : QStringLiteral("info");
    QRectF ir(r.left() + 14.0, r.center().y() - 8.0, 16.0, 16.0);
    Icons::draw(&p, iconName, ir, borderC, 2.2);

    p.setFont(makeFont(13, QFont::Bold));
    p.setPen(Theme::instance().c("text"));
    QRectF tr(r.left() + 36.0, r.top(), r.width() - 48.0, r.height());
    p.drawText(tr, static_cast<int>(Qt::AlignAbsolute | Qt::AlignCenter), I18n::bidiText(m_msg));
}

SlideStack::SlideStack(QWidget* parent)
    : QStackedWidget(parent) {}

void SlideStack::setCurrentIndexSmooth(int idx) {
    if (idx == currentIndex() || idx < 0 || idx >= count()) return;
    setCurrentIndex(idx);
}

// ============================================================================
// StepEnableToggleButton
// ============================================================================

StepEnableToggleButton::StepEnableToggleButton(bool enabled, QWidget* parent)
    : QWidget(parent), m_enabled(enabled) {
    setFixedSize(28, 28);
    setCursor(Qt::PointingHandCursor);
    setToolTip(I18n::t("tip_step_enable_toggle"));
}

void StepEnableToggleButton::setStepEnabled(bool enabled, bool emitSignal) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        update();
        if (emitSignal) {
            emit toggled(m_enabled);
            emit stepToggled(m_enabled);
        }
    }
}

void StepEnableToggleButton::enterEvent(QEnterEvent*) {
    m_hover = true;
    setToolTip(I18n::t("tip_step_enable_toggle"));
    update();
}

void StepEnableToggleButton::leaveEvent(QEvent*) {
    m_hover = false;
    update();
}

void StepEnableToggleButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_enabled = !m_enabled;
        update();
        emit toggled(m_enabled);
        emit stepToggled(m_enabled);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void StepEnableToggleButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
    QColor col, bg;
    QString icName;
    if (m_enabled) {
        col = QColor("#10B981");
        bg = QColor(16, 185, 129, m_hover ? 55 : 34);
        icName = QStringLiteral("check");
    } else {
        col = QColor("#F59E0B");
        bg = QColor(245, 158, 11, m_hover ? 68 : 42);
        icName = QStringLiteral("pause");
    }
    p.setBrush(bg);
    p.setPen(QPen(col, 1.5));
    p.drawRoundedRect(r, 7.5, 7.5);
    QRectF ir = QRectF(rect()).adjusted(6.5, 6.5, -6.5, -6.5);
    Icons::draw(&p, icName, ir, col, 2.1);
}

// ============================================================================
// StepNoteHoverCard, StepNoteEditorPopup & StepNoteIconButton
// ============================================================================

StepNoteHoverCard::StepNoteHoverCard(QWidget* parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::NoDropShadowWindowHint) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_ShowWithoutActivating, true);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(14, 10, 14, 10);
    lay->setSpacing(4);

    m_lblTitle = new QLabel(this);
    m_lblTitle->setFont(makeFont(11, QFont::ExtraBold));
    m_lblTitle->setStyleSheet("color: #F59E0B; background: transparent;");

    m_lblBody = new QLabel(this);
    m_lblBody->setWordWrap(true);
    m_lblBody->setFont(makeFont(12, QFont::DemiBold));
    m_lblBody->setMaximumWidth(320);
    m_lblBody->setMinimumWidth(180);

    lay->addWidget(m_lblTitle);
    lay->addWidget(m_lblBody);
}

void StepNoteHoverCard::showNoteNear(QWidget* anchorWidget, const QString& noteText) {
    const QString clean = noteText.trimmed();
    if (clean.isEmpty() || !anchorWidget) {
        hide();
        return;
    }
    m_noteText = clean;
    bool hasArabic = false;
    for (QChar ch : clean) {
        if ((ch.unicode() >= 0x0600 && ch.unicode() <= 0x06FF) ||
            (ch.unicode() >= 0x0750 && ch.unicode() <= 0x08FF)) {
            hasArabic = true;
            break;
        }
    }
    const bool useRtl = hasArabic || I18n::isRtl();
    setLayoutDirection(useRtl ? Qt::RightToLeft : Qt::LeftToRight);
    const Qt::Alignment alignH = useRtl ? Qt::AlignRight : Qt::AlignLeft;

    m_lblTitle->setFont(makeFont(12, QFont::ExtraBold));
    m_lblTitle->setAlignment(alignH | Qt::AlignVCenter);
    m_lblTitle->setText(I18n::bidiText(QString::fromUtf8("📝 %1").arg(I18n::t("lbl_step_note_title"))));

    m_lblBody->setFont(makeFont(13, QFont::Bold));
    m_lblBody->setAlignment(alignH | Qt::AlignTop);
    m_lblBody->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    m_lblBody->setText(hasArabic ? I18n::bidiText(clean) : clean);
    adjustSize();

    const QPoint gPos = anchorWidget->mapToGlobal(QPoint(0, 0));
    const int w = qBound(220, sizeHint().width() + 20, 370);
    const int h = std::max(60, sizeHint().height() + 8);
    resize(w, h);

    const int x = gPos.x() + (anchorWidget->width() / 2) - (w / 2);
    int y = gPos.y() - h - 8;
    if (y < 20) y = gPos.y() + anchorWidget->height() + 8;
    move(std::max(12, x), std::max(12, y));
    show();
    raise();
}

void StepNoteHoverCard::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
    p.setBrush(Theme::instance().c("surface_hi", 248));
    p.setPen(QPen(QColor("#F59E0B"), 1.6));
    p.drawRoundedRect(r, 11.0, 11.0);
}

StepNoteEditorPopup::StepNoteEditorPopup(const QString& initialNote, QWidget* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    setFixedSize(310, 148);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(12, 10, 12, 10);
    lay->setSpacing(6);

    auto* hdr = new QHBoxLayout();
    hdr->setSpacing(6);
    auto* lbl = new QLabel(QString::fromUtf8("📝 %1").arg(I18n::t("lbl_step_note_title")), this);
    lbl->setFont(makeFont(12, QFont::ExtraBold));
    lbl->setStyleSheet("color: #F59E0B; background: transparent;");
    hdr->addWidget(lbl, 1);
    lay->addLayout(hdr);

    setLayoutDirection(I18n::isRtl() ? Qt::RightToLeft : Qt::LeftToRight);
    lbl->setAlignment((I18n::isRtl() ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter);

    m_edit = new QPlainTextEdit(initialNote, this);
    m_edit->setPlaceholderText(I18n::t("ph_step_note_input"));
    m_edit->setFixedHeight(68);
    syncNoteEditDirection();
    connect(m_edit, &QPlainTextEdit::textChanged, this, &StepNoteEditorPopup::syncNoteEditDirection);
    m_edit->setStyleSheet(QStringLiteral(R"(
        QPlainTextEdit {
            background: %1;
            color: %2;
            border: 1.2px solid #F59E0B;
            border-radius: 8px;
            padding: 4px 8px;
            font-family: '%3';
            font-size: 12px;
            font-weight: 600;
        }
    )").arg(Theme::instance().tokenHex("bg0"),
            Theme::instance().tokenHex("text"),
            I18n::langFontFamily()));
    lay->addWidget(m_edit);

    auto* btns = new QHBoxLayout();
    btns->setSpacing(6);
    m_btnSave = new Button(I18n::t("btn_save_note"), "primary", "check", "#F59E0B", false, this);
    m_btnSave->setFixedHeight(28);
    connect(m_btnSave, &Button::clicked, this, &StepNoteEditorPopup::onSave);

    m_btnClear = new Button(I18n::t("btn_clear_note"), "ghost", "trash", 28, this);
    connect(m_btnClear, &Button::clicked, this, &StepNoteEditorPopup::onClear);

    btns->addWidget(m_btnSave, 1);
    btns->addWidget(m_btnClear);
    lay->addLayout(btns);
}

void StepNoteEditorPopup::syncNoteEditDirection() {
    const QString txt = m_edit->toPlainText();
    bool hasAr = false;
    for (QChar ch : txt) {
        if ((ch.unicode() >= 0x0600 && ch.unicode() <= 0x06FF) ||
            (ch.unicode() >= 0x0750 && ch.unicode() <= 0x08FF)) {
            hasAr = true;
            break;
        }
    }
    const bool useRtl = !txt.trimmed().isEmpty() ? hasAr : I18n::isRtl();
    m_edit->setLayoutDirection(useRtl ? Qt::RightToLeft : Qt::LeftToRight);
    QTextOption opt = m_edit->document()->defaultTextOption();
    opt.setTextDirection(useRtl ? Qt::RightToLeft : Qt::LeftToRight);
    opt.setAlignment(useRtl ? Qt::AlignRight : Qt::AlignLeft);
    m_edit->document()->setDefaultTextOption(opt);
}

void StepNoteEditorPopup::onSave() {
    emit noteSaved(m_edit->toPlainText().trimmed());
    close();
}

void StepNoteEditorPopup::onClear() {
    m_edit->clear();
    emit noteSaved(QString());
    close();
}

void StepNoteEditorPopup::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
    p.setBrush(Theme::instance().c("surface_hi", 252));
    p.setPen(QPen(QColor("#F59E0B"), 1.6));
    p.drawRoundedRect(r, 12.0, 12.0);
}

StepNoteIconButton::StepNoteIconButton(const QString& noteText, QWidget* parent)
    : QWidget(parent), m_note(noteText.trimmed()) {
    setFixedSize(28, 28);
    setCursor(Qt::PointingHandCursor);
}

StepNoteIconButton::~StepNoteIconButton() {
    if (m_hoverCard) {
        m_hoverCard->hide();
        m_hoverCard->deleteLater();
    }
    if (m_editorPopup) {
        m_editorPopup->hide();
        m_editorPopup->deleteLater();
    }
}

void StepNoteIconButton::setNoteText(const QString& noteText, bool emitSignal) {
    const QString clean = noteText.trimmed();
    if (m_note != clean) {
        m_note = clean;
        update();
        if (emitSignal) emit noteChanged(m_note);
    }
}

void StepNoteIconButton::enterEvent(QEnterEvent*) {
    m_hover = true;
    update();
    if (!m_note.isEmpty()) {
        setToolTip(QString());
        showHoverCardNow();
    } else {
        setToolTip(I18n::t("tip_step_note_btn"));
    }
}

void StepNoteIconButton::leaveEvent(QEvent*) {
    m_hover = false;
    if (m_hoverCard) m_hoverCard->hide();
    update();
}

void StepNoteIconButton::showHoverCardNow() {
    if (!m_hover || m_note.isEmpty()) return;
    if (!m_hoverCard) m_hoverCard = new StepNoteHoverCard(nullptr);
    m_hoverCard->showNoteNear(this, m_note);
}

void StepNoteIconButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_hoverCard) m_hoverCard->hide();
        openEditorPopup();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void StepNoteIconButton::openEditorPopup() {
    if (m_editorPopup) {
        m_editorPopup->hide();
        m_editorPopup->deleteLater();
    }
    m_editorPopup = new StepNoteEditorPopup(m_note, nullptr);
    connect(m_editorPopup, &StepNoteEditorPopup::noteSaved, this, [this](const QString& txt) {
        setNoteText(txt, true);
    });
    const QPoint gPos = mapToGlobal(QPoint(0, height() + 4));
    const int x = std::max(12, gPos.x() - m_editorPopup->width() + width());
    const int y = gPos.y();
    m_editorPopup->move(x, y);
    m_editorPopup->show();
    m_editorPopup->editor()->setFocus();
}

void StepNoteIconButton::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
    const bool hasNote = !m_note.isEmpty();
    QColor col, bg, borderC;
    double penW = 1.1;
    if (hasNote) {
        col = QColor("#F59E0B");
        bg = QColor(245, 158, 11, m_hover ? 64 : 38);
        borderC = QColor("#F59E0B");
        penW = 1.5;
    } else {
        col = Theme::instance().c(m_hover ? "accent_hi" : "text3");
        bg = Theme::instance().c("surface_hi", m_hover ? 190 : 110);
        borderC = Theme::instance().c(m_hover ? "border_hi" : "border");
        penW = 1.1;
    }
    p.setBrush(bg);
    p.setPen(QPen(borderC, penW));
    p.drawRoundedRect(r, 7.5, 7.5);

    QRectF ir = QRectF(rect()).adjusted(6.2, 6.2, -6.2, -6.2);
    Icons::draw(&p, "note", ir, col, 1.9);

    if (hasNote) {
        p.setBrush(QColor("#F59E0B"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QRectF(r.right() - 5.5, r.top() + 1.5, 4.2, 4.2));
    }
}

// ============================================================================
// FlowLayout
// ============================================================================

FlowLayout::FlowLayout(QWidget* parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing) {
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout() {
    QLayoutItem* item;
    while ((item = takeAt(0))) delete item;
}

void FlowLayout::addItem(QLayoutItem* item) { m_itemList.append(item); }
int FlowLayout::horizontalSpacing() const { return m_hSpace >= 0 ? m_hSpace : 6; }
int FlowLayout::verticalSpacing() const { return m_vSpace >= 0 ? m_vSpace : 6; }
Qt::Orientations FlowLayout::expandingDirections() const { return {}; }
bool FlowLayout::hasHeightForWidth() const { return true; }
int FlowLayout::heightForWidth(int width) const { return doLayout(QRect(0, 0, width, 0), true); }
int FlowLayout::count() const { return m_itemList.size(); }
QLayoutItem* FlowLayout::itemAt(int index) const { return m_itemList.value(index); }
QLayoutItem* FlowLayout::takeAt(int index) {
    if (index >= 0 && index < m_itemList.size()) return m_itemList.takeAt(index);
    return nullptr;
}
QSize FlowLayout::minimumSize() const {
    QSize size;
    for (const QLayoutItem* item : m_itemList) size = size.expandedTo(item->minimumSize());
    const QMargins m = contentsMargins();
    size += QSize(m.left() + m.right(), m.top() + m.bottom());
    return size;
}
QSize FlowLayout::sizeHint() const { return minimumSize(); }
void FlowLayout::setGeometry(const QRect& rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

int FlowLayout::doLayout(const QRect& rect, bool testOnly) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (QLayoutItem* item : m_itemList) {
        int spaceX = horizontalSpacing();
        int spaceY = verticalSpacing();
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }
        if (!testOnly) item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        x = nextX;
        lineHeight = std::max(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

// ============================================================================
// Smooth Step Reordering
// ============================================================================

int getRowLogicalY(QWidget* w) {
    if (!w) return 0;
    QVariant prop = w->property("_target_slot_y");
    if (prop.isValid()) return prop.toInt();
    return w->y();
}

bool smoothReorderVBoxWidget(QVBoxLayout* vbox, QList<QWidget*>& rowWidgets, int fromIdx, int toIdx, int durationMs) {
    if (!vbox || fromIdx == toIdx || fromIdx < 0 || fromIdx >= rowWidgets.size() || toIdx < 0 || toIdx >= rowWidgets.size()) {
        return false;
    }

    QMap<QWidget*, QPoint> currentVisualPos;
    for (QWidget* w : rowWidgets) {
        if (w) currentVisualPos.insert(w, w->pos());
    }

    for (const auto& anim : s_activeRowAnims) {
        if (anim) anim->stop();
    }
    s_activeRowAnims.clear();

    if (s_restoreTimer) {
        s_restoreTimer->stop();
    }

    const QMargins margins = vbox->contentsMargins();
    const int spacing = std::max(0, vbox->spacing());
    int baseX = margins.left();
    for (QWidget* w : rowWidgets) {
        if (w && w->x() > 0) {
            baseX = w->x();
            break;
        }
    }
    int topY = margins.top();
    if (!rowWidgets.isEmpty()) {
        int minKnownY = getRowLogicalY(rowWidgets.first());
        for (QWidget* w : rowWidgets) {
            if (w) minKnownY = std::min(minKnownY, getRowLogicalY(w));
        }
        if (minKnownY >= 0) topY = minKnownY;
    }

    QWidget* movedWidget = rowWidgets.takeAt(fromIdx);
    rowWidgets.insert(toIdx, movedWidget);

    vbox->setEnabled(false);
    vbox->removeWidget(movedWidget);
    vbox->insertWidget(toIdx, movedWidget);

    QMap<QWidget*, QPoint> targetPositions;
    int cursorY = topY;
    for (QWidget* w : rowWidgets) {
        if (!w) continue;
        int h = w->height() > 10 ? w->height() : w->sizeHint().height();
        h = std::max(38, h);
        targetPositions.insert(w, QPoint(baseX, cursorY));
        w->setProperty("_target_slot_y", cursorY);
        cursorY += h + spacing;
    }

    if (movedWidget) movedWidget->raise();

    for (QWidget* w : rowWidgets) {
        if (!w) continue;
        const QPoint startPt = currentVisualPos.value(w, targetPositions.value(w));
        const QPoint endPt = targetPositions.value(w);
        if (startPt == endPt) {
            w->move(endPt);
            continue;
        }
        w->move(startPt);
        auto* anim = new QPropertyAnimation(w, "pos", w);
        anim->setDuration(durationMs);
        anim->setStartValue(startPt);
        anim->setEndValue(endPt);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        s_activeRowAnims.append(anim);
    }

    QWidget* parentW = !rowWidgets.isEmpty() ? rowWidgets.first()->parentWidget() : nullptr;
    s_restoreTimer = new QTimer(parentW);
    s_restoreTimer->setSingleShot(true);
    QList<QPointer<QWidget>> weakRows;
    for (QWidget* w : rowWidgets) weakRows.append(w);

    QObject::connect(s_restoreTimer, &QTimer::timeout, parentW, [vbox, weakRows]() {
        s_activeRowAnims.clear();
        vbox->setEnabled(true);
        vbox->invalidate();
        vbox->activate();
        for (const auto& rw : weakRows) {
            if (rw) rw->setProperty("_target_slot_y", rw->y());
        }
    });
    s_restoreTimer->start(durationMs + 20);
    return true;
}

void smoothReorderVBoxWidget(QVBoxLayout* vbox, QWidget* container, int fromIdx, int toIdx, std::function<void()> onDone) {
    if (!vbox || !container) {
        if (onDone) onDone();
        return;
    }
    QList<QWidget*> rows;
    for (int i = 0; i < vbox->count(); ++i) {
        if (QWidget* w = vbox->itemAt(i)->widget()) {
            rows.append(w);
        }
    }
    if (smoothReorderVBoxWidget(vbox, rows, fromIdx, toIdx, 240)) {
        if (onDone) QTimer::singleShot(265, container, onDone);
    } else if (onDone) {
        onDone();
    }
}

} // namespace vmacro
