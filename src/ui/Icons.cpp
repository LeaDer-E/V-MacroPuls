#include "Icons.h"
#include "../core/Theme.h"
#include <QHash>
#include <QLinearGradient>
#include <QPainterPath>
#include <QPolygonF>
#include <QRadialGradient>
#include <cmath>

namespace vmacro {

QPixmap Icons::logoPixmap(int size, double dpr) {
    const auto& th = Theme::instance();
    const QString cacheKey = QStringLiteral("%1|%2|%3|%4|%5")
                                 .arg(size)
                                 .arg(dpr, 0, 'f', 1)
                                 .arg(th.paletteId())
                                 .arg(th.isDark() ? 1 : 0)
                                 .arg(th.customAccent());
    static QHash<QString, QPixmap> cache;
    auto it = cache.constFind(cacheKey);
    if (it != cache.constEnd()) return it.value();

    int pxSize = qMax(16, static_cast<int>(std::round(size * dpr)));
    QPixmap pix(pxSize, pxSize);
    pix.fill(Qt::transparent);
    pix.setDevicePixelRatio(dpr);

    QPainter p(&pix);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    const double s = static_cast<double>(size);
    const double cx = s * 0.5, cy = s * 0.5;
    Q_UNUSED(cy);
    const bool showMicro = (size >= 40);

    QColor bg0("#0B0D14");
    QColor bg1("#121623");
    QColor accent = th.c(QStringLiteral("accent"));
    QColor accent2 = th.c(QStringLiteral("accent2"));
    QColor accentHi = th.c(QStringLiteral("accent_hi"));

    // 1. Hardware Keycap Squircle Base
    double pad = qMax(1.0, s * 0.04);
    QRectF badgeRect(pad, pad, s - 2 * pad, s - 2 * pad);
    double radius = s * 0.24;

    QLinearGradient bgGrad(badgeRect.topLeft(), badgeRect.bottomRight());
    bgGrad.setColorAt(0.0, bg1.lighter(125));
    bgGrad.setColorAt(0.55, bg0);
    bgGrad.setColorAt(1.0, QColor(7, 8, 14));

    p.setPen(Qt::NoPen);
    p.setBrush(bgGrad);
    p.drawRoundedRect(badgeRect, radius, radius);

    // 2. Internal Tech Glow
    QRadialGradient glow(QPointF(cx, s * 0.54), s * 0.44);
    QColor cGlow(accent);
    cGlow.setAlpha(showMicro ? 85 : 65);
    QColor cZero(accent2);
    cZero.setAlpha(0);
    glow.setColorAt(0.0, cGlow);
    glow.setColorAt(1.0, cZero);
    p.setBrush(glow);
    p.drawRoundedRect(badgeRect, radius, radius);

    // 3. Subtle Inner Keycap Bevel & Macro Pulse Wave
    if (showMicro) {
        QRectF innerKey = badgeRect.adjusted(s * 0.10, s * 0.10, -s * 0.10, -s * 0.10);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(accentHi.red(), accentHi.green(), accentHi.blue(), 42), qMax(1.0, s * 0.015)));
        p.drawRoundedRect(innerKey, radius * 0.72, radius * 0.72);

        QPainterPath pulse;
        pulse.moveTo(s * 0.14, s * 0.54);
        pulse.lineTo(s * 0.26, s * 0.54);
        pulse.lineTo(s * 0.31, s * 0.43);
        pulse.lineTo(s * 0.36, s * 0.62);
        pulse.lineTo(s * 0.41, s * 0.54);
        pulse.moveTo(s * 0.62, s * 0.54);
        pulse.lineTo(s * 0.67, s * 0.44);
        pulse.lineTo(s * 0.72, s * 0.61);
        pulse.lineTo(s * 0.77, s * 0.54);
        pulse.lineTo(s * 0.86, s * 0.54);
        QPen pulsePen(QColor(accentHi.red(), accentHi.green(), accentHi.blue(), 95), qMax(1.1, s * 0.020), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pulsePen);
        p.drawPath(pulse);
    }

    // 4. Bold 3D Metallic 'V' Blade Integrated with an Upward Precision Cursor
    QPainterPath leftWing;
    leftWing.moveTo(s * 0.19, s * 0.22);
    leftWing.lineTo(s * 0.36, s * 0.22);
    leftWing.lineTo(cx, s * 0.60);
    leftWing.lineTo(cx, s * 0.83);
    leftWing.closeSubpath();

    QLinearGradient leftGrad(QPointF(s * 0.19, s * 0.22), QPointF(cx, s * 0.83));
    leftGrad.setColorAt(0.0, QColor("#FFFFFF"));
    leftGrad.setColorAt(0.25, accentHi);
    leftGrad.setColorAt(0.75, accent);
    leftGrad.setColorAt(1.0, accent2);

    p.setPen(Qt::NoPen);
    p.setBrush(leftGrad);
    p.drawPath(leftWing);

    QPainterPath rightWing;
    rightWing.moveTo(cx, s * 0.60);
    rightWing.lineTo(s * 0.64, s * 0.26);
    rightWing.lineTo(s * 0.57, s * 0.26);
    rightWing.lineTo(s * 0.77, s * 0.16);
    rightWing.lineTo(s * 0.83, s * 0.36);
    rightWing.lineTo(s * 0.76, s * 0.31);
    rightWing.lineTo(cx, s * 0.83);
    rightWing.closeSubpath();

    QLinearGradient rightGrad(QPointF(s * 0.80, s * 0.16), QPointF(cx, s * 0.83));
    rightGrad.setColorAt(0.0, QColor("#FFFFFF"));
    rightGrad.setColorAt(0.28, accentHi);
    rightGrad.setColorAt(0.65, accent);
    rightGrad.setColorAt(1.0, accent2.darker(145));

    p.setBrush(rightGrad);
    p.drawPath(rightWing);

    QPen ridgePen(QColor(255, 255, 255, 200), qMax(1.1, s * 0.018), Qt::SolidLine, Qt::RoundCap);
    p.setPen(ridgePen);
    p.drawLine(QPointF(s * 0.27, s * 0.23), QPointF(cx, s * 0.82));
    p.drawLine(QPointF(s * 0.77, s * 0.17), QPointF(cx, s * 0.82));

    if (size >= 26) {
        QPen plusPen(QColor(255, 255, 255, 235), qMax(1.3, s * 0.026), Qt::SolidLine, Qt::RoundCap);
        p.setPen(plusPen);
        double pxC = s * 0.46, pyC = s * 0.33, arm = s * 0.052;
        p.drawLine(QPointF(pxC - arm, pyC), QPointF(pxC + arm, pyC));
        p.drawLine(QPointF(pxC, pyC - arm), QPointF(pxC, pyC + arm));
    }

    // 5. Outer Metallic Keycap Rim
    QLinearGradient rimGrad(badgeRect.topLeft(), badgeRect.bottomRight());
    rimGrad.setColorAt(0.0, QColor(accentHi.red(), accentHi.green(), accentHi.blue(), 190));
    rimGrad.setColorAt(0.5, QColor(accent.red(), accent.green(), accent.blue(), 100));
    rimGrad.setColorAt(1.0, QColor(accent2.red(), accent2.green(), accent2.blue(), 160));

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(rimGrad, qMax(1.2, s * 0.026)));
    p.drawRoundedRect(badgeRect.adjusted(0.6, 0.6, -0.6, -0.6), radius, radius);

    p.end();
    cache.insert(cacheKey, pix);
    return pix;
}

QIcon Icons::logoIcon(int size) {
    return QIcon(logoPixmap(size, 2.0));
}

void Icons::draw(QPainter* p, const QString& name, const QRectF& rect, const QColor& color, double strokeWidth) {
    if (!p) return;
    QColor c = color.isValid() ? color : Theme::instance().c(QStringLiteral("text"));
    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);

    double scale = qMin(rect.width() / 24.0, rect.height() / 24.0);
    double ox = rect.x() + (rect.width() - 24.0 * scale) * 0.5;
    double oy = rect.y() + (rect.height() - 24.0 * scale) * 0.5;
    p->translate(ox, oy);
    p->scale(scale, scale);

    QPen pen(c, strokeWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);

    if (name == QLatin1String("grip")) {
        p->setPen(Qt::NoPen);
        p->setBrush(c);
        for (double gx : {8.5, 15.5}) {
            for (double gy : {6.0, 12.0, 18.0}) {
                p->drawEllipse(QPointF(gx, gy), 1.6, 1.6);
            }
        }
    } else if (name == QLatin1String("pause")) {
        p->setPen(Qt::NoPen);
        p->setBrush(c);
        p->drawRoundedRect(QRectF(6.5, 5.0, 3.6, 14.0), 1.4, 1.4);
        p->drawRoundedRect(QRectF(13.9, 5.0, 3.6, 14.0), 1.4, 1.4);
    } else if (name == QLatin1String("note") || name == QLatin1String("edit")) {
        p->drawRoundedRect(QRectF(4.5, 3.5, 15.0, 17.0), 2.6, 2.6);
        p->drawLine(QPointF(8.0, 8.5), QPointF(16.0, 8.5));
        p->drawLine(QPointF(8.0, 12.5), QPointF(16.0, 12.5));
        p->drawLine(QPointF(8.0, 16.5), QPointF(13.0, 16.5));
    } else if (name == QLatin1String("test") || name == QLatin1String("flask") || name == QLatin1String("lab")) {
        QPainterPath path;
        path.moveTo(9.0, 3.5);
        path.lineTo(15.0, 3.5);
        path.moveTo(10.2, 3.5);
        path.lineTo(10.2, 8.8);
        path.lineTo(5.0, 18.2);
        path.cubicTo(4.4, 19.5, 5.2, 20.5, 6.8, 20.5);
        path.lineTo(17.2, 20.5);
        path.cubicTo(18.8, 20.5, 19.6, 19.5, 19.0, 18.2);
        path.lineTo(13.8, 8.8);
        path.lineTo(13.8, 3.5);
        p->drawPath(path);
        p->drawLine(QPointF(7.5, 14.5), QPointF(16.5, 14.5));
    } else if (name == QLatin1String("dashboard") || name == QLatin1String("sidebar")) {
        p->drawRoundedRect(QRectF(3, 3, 8, 8), 2.2, 2.2);
        p->drawRoundedRect(QRectF(13, 3, 8, 5), 2.0, 2.0);
        p->drawRoundedRect(QRectF(13, 10, 8, 11), 2.2, 2.2);
        p->drawRoundedRect(QRectF(3, 13, 8, 8), 2.2, 2.2);
    } else if (name == QLatin1String("recorder") || name == QLatin1String("record")) {
        p->drawEllipse(QPointF(12, 12), 8.5, 8.5);
        p->setBrush(c);
        p->setPen(Qt::NoPen);
        p->drawEllipse(QPointF(12, 12), 4.2, 4.2);
    } else if (name == QLatin1String("combos") || name == QLatin1String("bolt")) {
        QPainterPath path;
        path.moveTo(13.5, 2.5);
        path.lineTo(5.5, 13.2);
        path.lineTo(11.8, 13.2);
        path.lineTo(10.5, 21.5);
        path.lineTo(18.5, 10.8);
        path.lineTo(12.2, 10.8);
        path.closeSubpath();
        p->drawPath(path);
    } else if (name == QLatin1String("terminal") || name == QLatin1String("cmd") || name == QLatin1String("powershell")) {
        p->drawRoundedRect(QRectF(2.5, 4.0, 19.0, 16.0), 2.5, 2.5);
        p->drawLine(QPointF(6.5, 9.0), QPointF(10.5, 12.0));
        p->drawLine(QPointF(10.5, 12.0), QPointF(6.5, 15.0));
        p->drawLine(QPointF(12.5, 15.0), QPointF(17.5, 15.0));
    } else if (name == QLatin1String("crosshair")) {
        p->drawEllipse(QPointF(12, 12), 7.0, 7.0);
        p->drawLine(QPointF(12, 2.5), QPointF(12, 6.5));
        p->drawLine(QPointF(12, 17.5), QPointF(12, 21.5));
        p->drawLine(QPointF(2.5, 12), QPointF(6.5, 12));
        p->drawLine(QPointF(17.5, 12), QPointF(21.5, 12));
    } else if (name == QLatin1String("shield")) {
        QPainterPath path;
        path.moveTo(12, 2.8);
        path.lineTo(19.5, 6.0);
        path.cubicTo(19.5, 13.0, 16.5, 18.2, 12, 21.2);
        path.cubicTo(7.5, 18.2, 4.5, 13.0, 4.5, 6.0);
        path.closeSubpath();
        p->drawPath(path);
    } else if (name == QLatin1String("mail")) {
        p->drawRoundedRect(QRectF(3.0, 5.5, 18.0, 13.0), 2.2, 2.2);
        p->drawLine(QPointF(4.0, 6.5), QPointF(12.0, 12.5));
        p->drawLine(QPointF(20.0, 6.5), QPointF(12.0, 12.5));
    } else if (name == QLatin1String("phone")) {
        p->drawRoundedRect(QRectF(6.5, 3.0, 11.0, 18.0), 2.5, 2.5);
        p->drawLine(QPointF(10.5, 18.0), QPointF(13.5, 18.0));
    } else if (name == QLatin1String("library") || name == QLatin1String("folder")) {
        p->drawRoundedRect(QRectF(3, 6, 18, 14), 2.5, 2.5);
        p->drawLine(QPointF(3, 10), QPointF(21, 10));
        p->drawLine(QPointF(8, 14.5), QPointF(16, 14.5));
    } else if (name == QLatin1String("themes") || name == QLatin1String("palette")) {
        p->drawEllipse(QPointF(12, 12), 9.0, 9.0);
        p->setBrush(c);
        p->setPen(Qt::NoPen);
        p->drawEllipse(QPointF(8.5, 9.5), 1.4, 1.4);
        p->drawEllipse(QPointF(12.0, 7.5), 1.4, 1.4);
        p->drawEllipse(QPointF(15.5, 9.5), 1.4, 1.4);
        p->drawEllipse(QPointF(15.0, 14.0), 1.4, 1.4);
    } else if (name == QLatin1String("settings")) {
        p->drawEllipse(QPointF(12, 12), 3.4, 3.4);
        for (int i = 0; i < 8; ++i) {
            double ang = i * (3.14159265358979323846 / 4.0);
            double x1 = 12 + std::cos(ang) * 6.2, y1 = 12 + std::sin(ang) * 6.2;
            double x2 = 12 + std::cos(ang) * 9.0, y2 = 12 + std::sin(ang) * 9.0;
            p->drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    } else if (name == QLatin1String("about") || name == QLatin1String("info")) {
        p->drawEllipse(QPointF(12, 12), 9.0, 9.0);
        p->drawLine(QPointF(12, 11.2), QPointF(12, 16.5));
        p->setBrush(c);
        p->setPen(Qt::NoPen);
        p->drawEllipse(QPointF(12, 7.8), 1.2, 1.2);
    } else if (name == QLatin1String("play")) {
        QPolygonF poly({QPointF(7.5, 5.0), QPointF(18.5, 12.0), QPointF(7.5, 19.0)});
        p->setBrush(c);
        p->drawPolygon(poly);
    } else if (name == QLatin1String("stop")) {
        p->setBrush(c);
        p->drawRoundedRect(QRectF(6.5, 6.5, 11.0, 11.0), 2.2, 2.2);
    } else if (name == QLatin1String("plus") || name == QLatin1String("add")) {
        p->drawLine(QPointF(12, 5.5), QPointF(12, 18.5));
        p->drawLine(QPointF(5.5, 12), QPointF(18.5, 12));
    } else if (name == QLatin1String("trash") || name == QLatin1String("delete")) {
        p->drawLine(QPointF(4.5, 7), QPointF(19.5, 7));
        p->drawRoundedRect(QRectF(6.5, 7, 11, 13), 2.0, 2.0);
        p->drawLine(QPointF(9.5, 4.5), QPointF(14.5, 4.5));
        p->drawLine(QPointF(10, 11), QPointF(10, 16.5));
        p->drawLine(QPointF(14, 11), QPointF(14, 16.5));
    } else if (name == QLatin1String("save")) {
        p->drawRoundedRect(QRectF(4, 4, 16, 16), 2.5, 2.5);
        p->drawRoundedRect(QRectF(8, 4, 8, 6), 1.2, 1.2);
        p->drawRoundedRect(QRectF(7, 13, 10, 7), 1.5, 1.5);
    } else if (name == QLatin1String("keyboard") || name == QLatin1String("key")) {
        p->drawRoundedRect(QRectF(2.5, 6.0, 19.0, 12.0), 2.5, 2.5);
        p->drawLine(QPointF(6.5, 10.0), QPointF(8.0, 10.0));
        p->drawLine(QPointF(11.0, 10.0), QPointF(13.0, 10.0));
        p->drawLine(QPointF(16.0, 10.0), QPointF(17.5, 10.0));
        p->drawLine(QPointF(7.5, 14.2), QPointF(16.5, 14.2));
    } else if (name == QLatin1String("mouse")) {
        p->drawRoundedRect(QRectF(6.5, 3.5, 11.0, 17.0), 5.5, 5.5);
        p->drawLine(QPointF(12.0, 7.0), QPointF(12.0, 10.5));
    } else if (name == QLatin1String("clock") || name == QLatin1String("wait")) {
        p->drawEllipse(QPointF(12, 12), 8.5, 8.5);
        p->drawLine(QPointF(12, 7.2), QPointF(12, 12));
        p->drawLine(QPointF(12, 12), QPointF(15.5, 14.2));
    } else if (name == QLatin1String("check")) {
        p->drawLine(QPointF(5.5, 12.5), QPointF(10.0, 17.0));
        p->drawLine(QPointF(10.0, 17.0), QPointF(18.8, 7.2));
    } else if (name == QLatin1String("copy")) {
        p->drawRoundedRect(QRectF(8.5, 8.5, 11.0, 11.0), 2.0, 2.0);
        p->drawRoundedRect(QRectF(4.5, 4.5, 11.0, 11.0), 2.0, 2.0);
    } else if (name == QLatin1String("search")) {
        p->drawEllipse(QPointF(10.5, 10.5), 6.2, 6.2);
        p->drawLine(QPointF(15.2, 15.2), QPointF(19.5, 19.5));
    } else if (name == QLatin1String("chevron_down") || name == QLatin1String("redo")) {
        p->drawLine(QPointF(6.5, 9.5), QPointF(12.0, 15.0));
        p->drawLine(QPointF(12.0, 15.0), QPointF(17.5, 9.5));
    } else if (name == QLatin1String("chevron_up") || name == QLatin1String("undo")) {
        p->drawLine(QPointF(6.5, 14.5), QPointF(12.0, 9.0));
        p->drawLine(QPointF(12.0, 9.0), QPointF(17.5, 14.5));
    } else if (name == QLatin1String("minimize")) {
        p->drawLine(QPointF(6.0, 12.0), QPointF(18.0, 12.0));
    } else if (name == QLatin1String("maximize")) {
        p->drawRoundedRect(QRectF(6.0, 6.0, 12.0, 12.0), 2.0, 2.0);
    } else if (name == QLatin1String("close")) {
        p->drawLine(QPointF(6.8, 6.8), QPointF(17.2, 17.2));
        p->drawLine(QPointF(17.2, 6.8), QPointF(6.8, 17.2));
    } else if (name == QLatin1String("globe") || name == QLatin1String("lang")) {
        p->drawEllipse(QPointF(12, 12), 8.5, 8.5);
        p->drawLine(QPointF(3.5, 12), QPointF(20.5, 12));
        p->drawEllipse(QPointF(12, 12), 4.2, 8.5);
    } else if (name == QLatin1String("refresh")) {
        p->drawArc(QRectF(4, 4, 16, 16), 30 * 16, 290 * 16);
        p->drawLine(QPointF(17, 4), QPointF(20, 8));
        p->drawLine(QPointF(20, 8), QPointF(16, 9));
    } else {
        p->drawEllipse(QPointF(12, 12), 6.0, 6.0);
    }

    p->restore();
}

QPixmap Icons::pixmap(const QString& name, const QString& colorHex, int size) {
    double dpr = 2.0;
    int px = static_cast<int>(size * dpr);
    QPixmap pm(px, px);
    pm.fill(Qt::transparent);
    pm.setDevicePixelRatio(dpr);
    QPainter p(&pm);
    QColor col(colorHex.isEmpty() ? QStringLiteral("#FFFFFF") : colorHex);
    draw(&p, name, QRectF(2, 2, size - 4, size - 4), col, 2.0);
    p.end();
    return pm;
}

QIcon Icons::get(const QString& name, const QString& colorHex, int size) {
    return QIcon(pixmap(name, colorHex, size));
}

} // namespace vmacro
