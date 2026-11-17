#pragma once
#include <QObject>
#include <QColor>
#include <QMap>
#include <QString>
#include <QVector>

namespace vmacro {

struct PaletteInfo {
    QString id;
    QString arName;
    QString enName;
    QString accentHex;
    QString accent2Hex;
    QString name;
    QString accent;

    PaletteInfo(const QString& i = QString(), const QString& ar = QString(), const QString& en = QString(),
                const QString& a1 = QString(), const QString& a2 = QString())
        : id(i), arName(ar), enName(en), accentHex(a1), accent2Hex(a2), name(en), accent(a1) {}
};

struct ThemeTokens {
    QString bg = "#0B0D14";
    QString surface = "#161B2A";
    QString card = "#121623";
    QString cardHover = "#1E2538";
    QString sidebarBg = "#121623";
    QString border = "#262E45";
    QString text = "#F1F5F9";
    QString textMuted = "#94A3B8";
    QString textDim = "#64748B";
    QString accent = "#8B5CF6";
    QString accentHover = "#A78BFA";
    QString success = "#10B981";
    QString warning = "#F59E0B";
    QString danger = "#EF4444";
};

class Theme : public QObject {
    Q_OBJECT
public:
    static Theme& instance();

    bool isDark() const { return m_dark; }
    QString mode() const { return m_dark ? QStringLiteral("dark") : QStringLiteral("light"); }
    QString paletteId() const { return m_paletteId; }
    QString customAccent() const { return m_customAccent; }

    void setTheme(const QString& mode, const QString& paletteId = QString(), const QString& customAccent = QString());

    QColor c(const QString& token, int alpha = 255) const;
    QString tokenHex(const QString& token) const;
    const QVector<PaletteInfo>& palettes() const { return m_palettes; }
    const ThemeTokens& currentTokens() const { return m_structTokens; }

    // Static convenience helpers
    static const ThemeTokens& tokens() { return instance().currentTokens(); }
    static const QVector<PaletteInfo>& allPalettes() { return instance().palettes(); }
    static void apply(const QString& mode, const QString& paletteId = QString(), const QString& customAccent = QString()) {
        instance().setTheme(mode, paletteId, customAccent);
    }
    static QString rgba(const QString& hexColor, double alpha) {
        QColor col(hexColor);
        if (!col.isValid()) col = QColor("#8B5CF6");
        return QStringLiteral("rgba(%1, %2, %3, %4)")
            .arg(col.red())
            .arg(col.green())
            .arg(col.blue())
            .arg(qBound(0.0, alpha, 1.0), 0, 'f', 2);
    }

signals:
    void themeChanged();

private:
    Theme();
    void rebuildTokens();

    bool m_dark = true;
    QString m_paletteId = "cyber_violet";
    QString m_customAccent = "#8B5CF6";
    QVector<PaletteInfo> m_palettes;
    QMap<QString, QString> m_tokens;
    ThemeTokens m_structTokens;
};

} // namespace vmacro
