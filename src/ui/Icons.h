#pragma once
#include <QColor>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QString>

namespace vmacro {

class Icons {
public:
    static QIcon icon(const QString& name, const QColor& color, int size = 18) { return get(name, color.name(), size); }
public:
    static QPixmap logoPixmap(int size = 64, double dpr = 2.0);
    static QIcon logoIcon(int size = 64);

    static void draw(QPainter* p, const QString& name, const QRectF& rect, const QColor& color = QColor(), double strokeWidth = 1.9);
    static QPixmap pixmap(const QString& name, const QString& colorHex = "#FFFFFF", int size = 18);
    static QIcon get(const QString& name, const QString& colorHex = "#FFFFFF", int size = 18);
};

} // namespace vmacro
