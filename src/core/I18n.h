#pragma once
#include <QMap>
#include <QString>
#include <QVector>

namespace vmacro {

struct LangInfo {
    QString code;
    QString label;
    bool rtl;
    QString name;
    QString nativeName;
    LangInfo(const QString& c = QString(), const QString& l = QString(), bool r = false)
        : code(c), label(l), rtl(r), name(l), nativeName(l) {}
};

class I18n {
public:
    static I18n& instance();

    QString currentLanguage() const { return m_lang; }
    void setLanguage(const QString& code);
    bool currentRtl() const;
    QString fontFamily() const;

    QString translate(const QString& key) const;
    QString translateDual(const QString& arText, const QString& enText) const;
    QString formatNumberStr(const QString& raw) const;
    QString formatNumberInt(int val) const;
    QString formatNumberDouble(double val, int decimals = 0) const;

    const QVector<LangInfo>& languagesList() const { return m_langs; }

    // Unified static & instance-compatible helpers
    static QString language() { return instance().currentLanguage(); }
    static void applyLanguage(const QString& code) { instance().setLanguage(code); }
    static bool isRtl() { return instance().currentRtl(); }
    static bool currentIsRtl() { return instance().currentRtl(); }
    static QString langFontFamily() { return instance().fontFamily(); }

    static QString t(const QString& key) { return instance().translate(key); }
    static QString tr(const QString& key) { return instance().translate(key); }
    static QString tl(const QString& arText, const QString& enText) { return instance().translateDual(arText, enText); }

    static QString fmtNum(const QString& raw) { return instance().formatNumberStr(raw); }
    static QString fmtNum(int val) { return instance().formatNumberInt(val); }
    static QString fmtNum(double val, int decimals = 0) { return instance().formatNumberDouble(val, decimals); }

    static QString bidiText(const QString& text) {
        if (text.isEmpty()) return QString();
        return (isRtl() ? QStringLiteral("\u200F") : QStringLiteral("\u200E")) + text;
    }
    static QString ltrValue(const QString& val) {
        return QStringLiteral("\u200E%1\u200E").arg(val);
    }

    static const QVector<LangInfo>& supportedLanguages() { return instance().languagesList(); }
    static const QVector<LangInfo>& allLanguages() { return instance().languagesList(); }

private:
    I18n();
    QString m_lang = "ar";
    QVector<LangInfo> m_langs;
    QMap<QString, QMap<QString, QString>> m_translations;
};

inline QString trKey(const QString& key) { return I18n::t(key); }
inline QString trDual(const QString& ar, const QString& en) { return I18n::tl(ar, en); }

} // namespace vmacro
