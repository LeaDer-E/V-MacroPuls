#include "AboutDialog.h"
#include "Icons.h"
#include "Widgets.h"
#include "../core/I18n.h"
#include "../core/Theme.h"
#include <QClipboard>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QRadialGradient>
#include <QUrl>
#include <QVBoxLayout>

namespace vmacro {

namespace {

class DiamondDivider : public QWidget {
public:
    explicit DiamondDivider(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedHeight(16);
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        double w = width(), h = height();
        double cx = w * 0.5, cy = h * 0.5;
        QColor acc = Theme::instance().c(QStringLiteral("accent"));
        QLinearGradient lineGrad(QPointF(w * 0.08, cy), QPointF(w * 0.92, cy));
        lineGrad.setColorAt(0.0, QColor(acc.red(), acc.green(), acc.blue(), 0));
        lineGrad.setColorAt(0.5, QColor(acc.red(), acc.green(), acc.blue(), 110));
        lineGrad.setColorAt(1.0, QColor(acc.red(), acc.green(), acc.blue(), 0));
        p.setPen(QPen(lineGrad, 1.1));
        p.drawLine(QPointF(w * 0.08, cy), QPointF(cx - 12, cy));
        p.drawLine(QPointF(cx + 12, cy), QPointF(w * 0.92, cy));

        QPolygonF diamond({
            QPointF(cx, cy - 4.2),
            QPointF(cx + 4.2, cy),
            QPointF(cx, cy + 4.2),
            QPointF(cx - 4.2, cy),
        });
        p.setPen(Qt::NoPen);
        p.setBrush(acc);
        p.drawPolygon(diamond);
    }
};

class GradientIconBadge : public QWidget {
public:
    explicit GradientIconBadge(const QString& iconName, QWidget* parent = nullptr)
        : QWidget(parent), m_icon(iconName) {
        setFixedSize(38, 38);
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QRectF r = QRectF(rect()).adjusted(1, 1, -1, -1);
        QLinearGradient grad(r.topLeft(), r.bottomRight());
        grad.setColorAt(0.0, Theme::instance().c(QStringLiteral("accent_hi")));
        grad.setColorAt(1.0, Theme::instance().c(QStringLiteral("accent")));
        p.setPen(Qt::NoPen);
        p.setBrush(grad);
        p.drawRoundedRect(r, 11.0, 11.0);
        Icons::draw(&p, m_icon, r.adjusted(9, 9, -9, -9), QColor("#FFFFFF"), 2.1);
    }
private:
    QString m_icon;
};

class ContactPillCard : public QWidget {
public:
    ContactPillCard(const QString& iconName, const QString& valText, bool showMailto, QWidget* parent = nullptr)
        : QWidget(parent), m_val(valText) {
        setFixedHeight(50);
        setLayoutDirection(Qt::LeftToRight);
        auto* lay = new QHBoxLayout(this);
        lay->setContentsMargins(10, 5, 12, 5);
        lay->setSpacing(12);

        lay->addWidget(new GradientIconBadge(iconName, this));
        m_lbl = new QLabel(valText, this);
        m_lbl->setStyleSheet(QStringLiteral("font-size: 14px; font-weight: 700; color: %1; background: transparent;")
                                 .arg(Theme::instance().tokenHex(QStringLiteral("text"))));
        lay->addWidget(m_lbl, 1);

        if (showMailto) {
            auto* btnMail = new IconButton(QStringLiteral("mail"), valText, 32, QStringLiteral("ghost"), this);
            connect(btnMail, &IconButton::clicked, this, [valText]() {
                QDesktopServices::openUrl(QUrl(QStringLiteral("mailto:") + valText));
            });
            lay->addWidget(btnMail);
        }

        m_btnCopy = new IconButton(QStringLiteral("copy"), I18n::tr(QStringLiteral("btn_copy")), 32, QStringLiteral("ghost"), this);
        connect(m_btnCopy, &IconButton::clicked, this, [valText]() {
            if (auto* cb = QGuiApplication::clipboard()) cb->setText(valText);
        });
        lay->addWidget(m_btnCopy);
    }

    void refreshStyles() {
        m_lbl->setStyleSheet(QStringLiteral("font-size: 14px; font-weight: 700; color: %1; background: transparent;")
                                 .arg(Theme::instance().tokenHex(QStringLiteral("text"))));
        m_btnCopy->setToolTip(I18n::tr(QStringLiteral("btn_copy")));
        m_btnCopy->applyTheme();
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QRectF r = QRectF(rect()).adjusted(1, 1, -1, -1);
        p.setBrush(Theme::instance().c(QStringLiteral("surface_hi"), 210));
        p.setPen(QPen(Theme::instance().c(QStringLiteral("border_hi")), 1.1));
        p.drawRoundedRect(r, 14.0, 14.0);
    }
private:
    QString m_val;
    QLabel* m_lbl = nullptr;
    IconButton* m_btnCopy = nullptr;
};

class AboutCardCanvas : public QWidget {
public:
    explicit AboutCardCanvas(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(506, 636);
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        QRectF r = QRectF(rect()).adjusted(1.5, 1.5, -1.5, -1.5);
        double radius = 24.0;

        QPainterPath path;
        path.addRoundedRect(r, radius, radius);
        p.setClipPath(path);

        const auto& th = Theme::instance();
        bool dark = th.isDark();
        QColor bg0 = th.c(QStringLiteral("bg0"));
        QColor bg1 = th.c(QStringLiteral("bg1"));
        QColor accHi = th.c(QStringLiteral("accent_hi"));

        QLinearGradient bgGrad(r.topLeft(), r.bottomLeft());
        if (dark) {
            bgGrad.setColorAt(0.0, bg1);
            bgGrad.setColorAt(0.28, bg0);
            bgGrad.setColorAt(1.0, bg0.darker(115));
        } else {
            bgGrad.setColorAt(0.0, QColor("#FFFFFF"));
            bgGrad.setColorAt(0.35, bg0);
            bgGrad.setColorAt(1.0, bg1);
        }
        p.fillPath(path, bgGrad);

        // Hero Cover Banner (top 102px)
        double bannerH = 102.0;
        QRectF bannerRect(r.left(), r.top(), r.width(), bannerH);
        QLinearGradient bGrad(bannerRect.topLeft(), bannerRect.bottomRight());
        bGrad.setColorAt(0.0, QColor("#0B0719"));
        bGrad.setColorAt(0.35, QColor("#1D1040"));
        bGrad.setColorAt(0.68, QColor("#2D145E"));
        bGrad.setColorAt(1.0, QColor("#0E172E"));
        p.fillRect(bannerRect, bGrad);

        double cxBanner = width() * 0.5;
        QRadialGradient aura(QPointF(cxBanner, 68.0), 135.0);
        aura.setColorAt(0.0, QColor(accHi.red(), accHi.green(), accHi.blue(), 115));
        aura.setColorAt(0.55, QColor(6, 182, 212, 48));
        aura.setColorAt(1.0, QColor(11, 7, 25, 0));
        p.fillRect(bannerRect, aura);

        p.setPen(QPen(QColor(139, 92, 246, 40), 1.0));
        for (int gx = 36; gx < width(); gx += 48) {
            p.drawLine(QPointF(gx, 0), QPointF(gx - 18, bannerH));
        }

        QPainterPath wave1;
        wave1.moveTo(0, 88);
        wave1.cubicTo(110, 62, 165, 98, cxBanner - 40, 76);
        wave1.lineTo(cxBanner + 40, 76);
        wave1.cubicTo(width() - 165, 98, width() - 110, 62, width(), 88);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor("#06B6D4"), 1.8));
        p.drawPath(wave1);

        p.setPen(QPen(QColor(accHi.red(), accHi.green(), accHi.blue(), 170), 1.4));
        p.drawLine(QPointF(r.left(), bannerH), QPointF(r.right(), bannerH));

        // Left Wing Keycaps
        struct KeySpec { double x, y, w, h; const char* hex; };
        const KeySpec specs[] = {
            {22.0, 16.0, 34.0, 28.0, "#8B5CF6"},
            {62.0, 16.0, 34.0, 28.0, "#06B6D4"},
            {102.0, 16.0, 34.0, 28.0, "#10B981"},
            {36.0, 50.0, 86.0, 22.0, "#A855F7"},
        };
        for (const auto& k : specs) {
            QColor kc(k.hex);
            QRectF kr(k.x, k.y, k.w, k.h);
            p.setBrush(QColor(kc.red(), kc.green(), kc.blue(), 34));
            p.setPen(QPen(QColor(kc.red(), kc.green(), kc.blue(), 155), 1.2));
            p.drawRoundedRect(kr, 6.5, 6.5);
            QRectF inKr = kr.adjusted(4.0, 3.5, -4.0, -5.5);
            p.setBrush(QColor(kc.red(), kc.green(), kc.blue(), 22));
            p.setPen(QPen(QColor(kc.red(), kc.green(), kc.blue(), 95), 0.9));
            p.drawRoundedRect(inKr, 4.0, 4.0);
        }

        // Left signal traces
        p.setPen(QPen(QColor(6, 182, 212, 130), 1.4));
        p.drawLine(QPointF(136, 30), QPointF(178, 30));
        p.drawLine(QPointF(178, 30), QPointF(198, 52));
        p.drawLine(QPointF(122, 61), QPointF(195, 61));
        p.setBrush(QColor("#06B6D4"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(178, 30), 2.5, 2.5);
        p.drawEllipse(QPointF(195, 61), 2.5, 2.5);

        // Right Wing Target Ripples + Vector Cursor
        double targetCx = width() - 92.0, targetCy = 48.0;
        struct Ripple { double rad; int alph; const char* hex; };
        const Ripple ripples[] = {{28.0, 75, "#8B5CF6"}, {19.0, 120, "#06B6D4"}, {10.0, 175, "#10B981"}};
        for (const auto& rp : ripples) {
            QColor rc(rp.hex);
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor(rc.red(), rc.green(), rc.blue(), rp.alph), 1.4));
            p.drawEllipse(QPointF(targetCx, targetCy), rp.rad, rp.rad);
        }
        p.setPen(QPen(QColor(16, 185, 129, 170), 1.2));
        p.drawLine(QPointF(targetCx - 34, targetCy), QPointF(targetCx - 22, targetCy));
        p.drawLine(QPointF(targetCx + 22, targetCy), QPointF(targetCx + 34, targetCy));
        p.drawLine(QPointF(targetCx, targetCy - 34), QPointF(targetCx, targetCy - 22));
        p.drawLine(QPointF(targetCx, targetCy + 22), QPointF(targetCx, targetCy + 34));

        QPolygonF cursorPoly({
            QPointF(targetCx + 2, targetCy + 2),
            QPointF(targetCx + 2, targetCy + 22),
            QPointF(targetCx + 8, targetCy + 17),
            QPointF(targetCx + 13, targetCy + 25),
            QPointF(targetCx + 16, targetCy + 23),
            QPointF(targetCx + 11, targetCy + 15),
            QPointF(targetCx + 18, targetCy + 14),
        });
        p.setBrush(QColor("#F8FAFC"));
        p.setPen(QPen(QColor("#8B5CF6"), 1.2));
        p.drawPolygon(cursorPoly);

        p.setPen(QPen(QColor(168, 85, 247, 130), 1.4));
        p.drawLine(QPointF(cxBanner + 42, 52), QPointF(targetCx - 38, 52));
        p.setBrush(QColor("#A855F7"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(targetCx - 38, 52), 2.5, 2.5);

        // Center 72px V+ Logo badge at y=36
        int logoS = 72;
        int lx = (width() - logoS) / 2;
        int ly = 36;
        QRectF haloR(lx - 5, ly - 5, logoS + 10, logoS + 10);
        p.setBrush(QColor(15, 10, 30, 210));
        p.setPen(QPen(QColor("#8B5CF6"), 2.0));
        p.drawRoundedRect(haloR, 20.0, 20.0);
        p.drawPixmap(lx, ly, Icons::logoPixmap(logoS, 2.0));

        p.setClipping(false);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(th.c(QStringLiteral("border_hi")), 1.5));
        p.drawRoundedRect(r, radius, radius);
    }
};

} // namespace

AboutDialog::AboutDialog(QWidget* parent) : QWidget(parent) {
    hide();
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setAlignment(Qt::AlignCenter);

    m_card = new AboutCardCanvas(this);
    auto* cLay = new QVBoxLayout(m_card);
    cLay->setContentsMargins(32, 14, 32, 18);
    cLay->setSpacing(6);

    auto* topBar = new QHBoxLayout();
    topBar->setDirection(QBoxLayout::LeftToRight);
    topBar->addStretch(1);
    m_closeBtn = new IconButton(QStringLiteral("close"), I18n::tr(QStringLiteral("btn_close")), 34, QStringLiteral("secondary"), m_card);
    connect(m_closeBtn, &IconButton::clicked, this, &QWidget::hide);
    topBar->addWidget(m_closeBtn);
    cLay->addLayout(topBar);

    cLay->addSpacing(76);

    m_lblAppName = new QLabel(QStringLiteral("V-MacroPlus"), m_card);
    m_lblAppName->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblAppName);

    m_lblSubVer = new QLabel(m_card);
    m_lblSubVer->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblSubVer);

    cLay->addSpacing(4);

    m_lblDesc = new QLabel(m_card);
    m_lblDesc->setWordWrap(true);
    m_lblDesc->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblDesc);

    cLay->addWidget(new DiamondDivider(m_card));

    m_lblTech = new QLabel(m_card);
    m_lblTech->setWordWrap(true);
    m_lblTech->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblTech);

    m_lblBadge = new QLabel(m_card);
    m_lblBadge->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblBadge);

    cLay->addWidget(new DiamondDivider(m_card));

    m_lblDevHdr = new QLabel(m_card);
    m_lblDevHdr->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblDevHdr);

    m_lblDevName = new QLabel(QStringLiteral("Eslam Mustafa"), m_card);
    m_lblDevName->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblDevName);

    cLay->addSpacing(4);

    m_pillEmail = new ContactPillCard(QStringLiteral("mail"), QStringLiteral("eslaam.mustafa@gmail.com"), true, m_card);
    cLay->addWidget(m_pillEmail);

    m_pillPhone = new ContactPillCard(QStringLiteral("phone"), QStringLiteral("+20 128 344 7065"), false, m_card);
    cLay->addWidget(m_pillPhone);

    cLay->addSpacing(4);

    m_lblArchFooter = new QLabel(m_card);
    m_lblArchFooter->setWordWrap(true);
    m_lblArchFooter->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblArchFooter);

    m_lblAssisted = new QLabel(m_card);
    m_lblAssisted->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblAssisted);

    m_lblCare = new QLabel(m_card);
    m_lblCare->setAlignment(Qt::AlignCenter);
    cLay->addWidget(m_lblCare);

    root->addWidget(m_card);
    refreshStyles();
}

void AboutDialog::refreshStyles() {
    const auto& th = Theme::instance();
    QString txtCol = th.tokenHex(QStringLiteral("text"));
    QString txt2Col = th.tokenHex(QStringLiteral("text2"));
    QString txt3Col = th.tokenHex(QStringLiteral("text3"));
    QString accCol = th.tokenHex(th.isDark() ? QStringLiteral("accent_hi") : QStringLiteral("accent"));
    QString okCol = th.tokenHex(QStringLiteral("ok"));

    m_lblAppName->setStyleSheet(QStringLiteral("font-size: 24px; font-weight: 800; color: %1; background: transparent;").arg(txtCol));
    m_lblSubVer->setText(QStringLiteral("%1  ·  %2 3.9.0")
                             .arg(I18n::tr(QStringLiteral("about_subtitle_caps")), I18n::tr(QStringLiteral("about_version_prefix"))));
    m_lblSubVer->setStyleSheet(QStringLiteral("font-size: 12px; font-weight: 700; color: %1; background: transparent;").arg(accCol));

    m_lblDesc->setText(I18n::tr(QStringLiteral("about_desc_body")));
    m_lblDesc->setStyleSheet(QStringLiteral("font-size: 13px; font-weight: 500; color: %1; background: transparent;").arg(txt2Col));

    m_lblTech->setText(I18n::tr(QStringLiteral("about_tech_highlights")));
    m_lblTech->setStyleSheet(QStringLiteral("font-size: 13px; font-weight: 700; color: %1; background: transparent;").arg(accCol));

    m_lblBadge->setText(I18n::tr(QStringLiteral("about_tech_badge")));
    m_lblBadge->setStyleSheet(QStringLiteral("font-size: 12px; font-weight: 800; color: %1; background: transparent;").arg(okCol));

    m_lblDevHdr->setText(I18n::tr(QStringLiteral("about_dev_header")));
    m_lblDevHdr->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 700; color: %1; background: transparent;").arg(txt3Col));

    m_lblDevName->setStyleSheet(QStringLiteral("font-size: 19px; font-weight: 800; color: %1; background: transparent;").arg(txtCol));

    static_cast<ContactPillCard*>(m_pillEmail)->refreshStyles();
    static_cast<ContactPillCard*>(m_pillPhone)->refreshStyles();

    m_lblArchFooter->setText(I18n::tr(QStringLiteral("about_arch_footer")));
    m_lblArchFooter->setStyleSheet(QStringLiteral("font-size: 12px; font-weight: 600; color: %1; background: transparent;").arg(txt2Col));

    m_lblAssisted->setText(I18n::tr(QStringLiteral("about_assisted_by")));
    m_lblAssisted->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 500; color: %1; background: transparent;").arg(txt3Col));

    m_lblCare->setText(I18n::tr(QStringLiteral("about_made_with_care")));
    m_lblCare->setStyleSheet(QStringLiteral("font-size: 11px; font-weight: 600; color: %1; background: transparent;").arg(accCol));

    m_closeBtn->setToolTip(I18n::tr(QStringLiteral("btn_close")));
    m_closeBtn->applyTheme();
    m_card->update();
    update();
}

void AboutDialog::showModal() {
    if (parentWidget()) {
        setGeometry(parentWidget()->rect());
    }
    refreshStyles();
    show();
    raise();
}

void AboutDialog::mousePressEvent(QMouseEvent* e) {
    if (m_card && !m_card->geometry().contains(e->pos())) {
        hide();
    }
}

void AboutDialog::paintEvent(QPaintEvent*) {
    QPainter p(this);
    int backdropAlpha = Theme::instance().isDark() ? 180 : 120;
    p.fillRect(rect(), QColor(5, 7, 14, backdropAlpha));
}

} // namespace vmacro
