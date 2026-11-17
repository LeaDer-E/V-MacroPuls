#include "Theme.h"
#include <algorithm>
#include <cmath>

namespace vmacro {

static void rgbToHls(double r, double g, double b, double& h, double& l, double& s) {
    const double maxC = std::max({r, g, b});
    const double minC = std::min({r, g, b});
    l = (minC + maxC) / 2.0;
    if (std::abs(minC - maxC) < 1e-7) {
        h = 0.0;
        s = 0.0;
        return;
    }
    if (l <= 0.5) {
        s = (maxC - minC) / (maxC + minC);
    } else {
        s = (maxC - minC) / (2.0 - maxC - minC);
    }
    const double rc = (maxC - r) / (maxC - minC);
    const double gc = (maxC - g) / (maxC - minC);
    const double bc = (maxC - b) / (maxC - minC);
    if (r == maxC) h = bc - gc;
    else if (g == maxC) h = 2.0 + rc - bc;
    else h = 4.0 + gc - rc;
    h = std::fmod((h / 6.0), 1.0);
    if (h < 0.0) h += 1.0;
    h *= 360.0;
}

static double hlsValue(double n1, double n2, double hue) {
    hue = std::fmod(hue, 360.0);
    if (hue < 0.0) hue += 360.0;
    if (hue < 60.0) return n1 + (n2 - n1) * hue / 60.0;
    if (hue < 180.0) return n2;
    if (hue < 240.0) return n1 + (n2 - n1) * (240.0 - hue) / 60.0;
    return n1;
}

static QString hslToHex(double h, double s, double l) {
    double hDeg = std::fmod(h, 360.0);
    if (hDeg < 0.0) hDeg += 360.0;
    const double sClamped = qBound(0.0, s, 1.0);
    const double lClamped = qBound(0.0, l, 1.0);
    if (sClamped == 0.0) {
        return QColor::fromRgbF(lClamped, lClamped, lClamped).name();
    }
    const double m2 = (lClamped <= 0.5)
        ? (lClamped * (1.0 + sClamped))
        : (lClamped + sClamped - lClamped * sClamped);
    const double m1 = 2.0 * lClamped - m2;
    const double r = hlsValue(m1, m2, hDeg + 120.0);
    const double g = hlsValue(m1, m2, hDeg);
    const double b = hlsValue(m1, m2, hDeg - 120.0);
    return QColor::fromRgbF(qBound(0.0, r, 1.0), qBound(0.0, g, 1.0), qBound(0.0, b, 1.0)).name();
}

static QString rgbaHexStr(const QString& hexStr, int alpha) {
    QColor c(hexStr);
    c.setAlpha(qBound(0, alpha, 255));
    return c.name(QColor::HexArgb);
}

Theme& Theme::instance() {
    static Theme inst;
    return inst;
}

Theme::Theme() {
    m_palettes.append(PaletteInfo{QStringLiteral("cyber_violet"), QString::fromUtf8("بنفسجي سيبراني"), QStringLiteral("Cyber Violet"), QStringLiteral("#8B5CF6"), QStringLiteral("#8B5CF6")});
    m_palettes.append(PaletteInfo{QStringLiteral("arc_indigo"), QString::fromUtf8("نيلي آرك"), QStringLiteral("Arc Indigo"), QStringLiteral("#6366F1"), QStringLiteral("#6366F1")});
    m_palettes.append(PaletteInfo{QStringLiteral("linear_blue"), QString::fromUtf8("أزرق لينير"), QStringLiteral("Linear Sapphire"), QStringLiteral("#3B82F6"), QStringLiteral("#3B82F6")});
    m_palettes.append(PaletteInfo{QStringLiteral("neon_cyan"), QString::fromUtf8("سماوي نيون"), QStringLiteral("Neon Cyan"), QStringLiteral("#06B6D4"), QStringLiteral("#06B6D4")});
    m_palettes.append(PaletteInfo{QStringLiteral("emerald_pulse"), QString::fromUtf8("زمردي نابض"), QStringLiteral("Emerald Pulse"), QStringLiteral("#10B981"), QStringLiteral("#10B981")});
    m_palettes.append(PaletteInfo{QStringLiteral("matrix_mint"), QString::fromUtf8("نعناعي رقمي"), QStringLiteral("Matrix Mint"), QStringLiteral("#14B8A6"), QStringLiteral("#14B8A6")});
    m_palettes.append(PaletteInfo{QStringLiteral("amber_forge"), QString::fromUtf8("كهرمان متوهج"), QStringLiteral("Amber Forge"), QStringLiteral("#F59E0B"), QStringLiteral("#F59E0B")});
    m_palettes.append(PaletteInfo{QStringLiteral("crimson_laser"), QString::fromUtf8("قرمزي ليزر"), QStringLiteral("Crimson Laser"), QStringLiteral("#F43F5E"), QStringLiteral("#F43F5E")});
    m_palettes.append(PaletteInfo{QStringLiteral("royal_magenta"), QString::fromUtf8("أرجواني ملكي"), QStringLiteral("Royal Magenta"), QStringLiteral("#D946EF"), QStringLiteral("#D946EF")});
    m_palettes.append(PaletteInfo{QStringLiteral("sunset_coral"), QString::fromUtf8("مرجاني الغروب"), QStringLiteral("Sunset Coral"), QStringLiteral("#FB7185"), QStringLiteral("#FB7185")});
    m_palettes.append(PaletteInfo{QStringLiteral("electric_lime"), QString::fromUtf8("ليموني كهربائي"), QStringLiteral("Electric Lime"), QStringLiteral("#84CC16"), QStringLiteral("#84CC16")});
    m_palettes.append(PaletteInfo{QStringLiteral("oceanic_teal"), QString::fromUtf8("فيروزي محيطي"), QStringLiteral("Oceanic Teal"), QStringLiteral("#0EA5E9"), QStringLiteral("#0EA5E9")});
    m_palettes.append(PaletteInfo{QStringLiteral("obsidian_gold"), QString::fromUtf8("ذهبي الأوبسيديان"), QStringLiteral("Obsidian Gold"), QStringLiteral("#EAB308"), QStringLiteral("#EAB308")});
    m_palettes.append(PaletteInfo{QStringLiteral("titanium_ice"), QString::fromUtf8("تيتانيوم جليدي"), QStringLiteral("Titanium Ice"), QStringLiteral("#38BDF8"), QStringLiteral("#38BDF8")});
    m_palettes.append(PaletteInfo{QStringLiteral("plasma_pink"), QString::fromUtf8("وردي البلازما"), QStringLiteral("Plasma Pink"), QStringLiteral("#EC4899"), QStringLiteral("#EC4899")});
    m_palettes.append(PaletteInfo{QStringLiteral("volcanic_orange"), QString::fromUtf8("برتقالي بركاني"), QStringLiteral("Volcanic Orange"), QStringLiteral("#F97316"), QStringLiteral("#F97316")});
    m_palettes.append(PaletteInfo{QStringLiteral("aurora_green"), QString::fromUtf8("أخضر الشفق"), QStringLiteral("Aurora Green"), QStringLiteral("#22C55E"), QStringLiteral("#22C55E")});
    m_palettes.append(PaletteInfo{QStringLiteral("amethyst_dream"), QString::fromUtf8("حلم الجمشت"), QStringLiteral("Amethyst Dream"), QStringLiteral("#A855F7"), QStringLiteral("#A855F7")});
    m_palettes.append(PaletteInfo{QStringLiteral("cobalt_storm"), QString::fromUtf8("عاصفة الكوبالت"), QStringLiteral("Cobalt Storm"), QStringLiteral("#2563EB"), QStringLiteral("#2563EB")});
    m_palettes.append(PaletteInfo{QStringLiteral("ruby_flare"), QString::fromUtf8("وهج الياقوت"), QStringLiteral("Ruby Flare"), QStringLiteral("#E11D48"), QStringLiteral("#E11D48")});
    m_palettes.append(PaletteInfo{QStringLiteral("jade_horizon"), QString::fromUtf8("أفق اليشم"), QStringLiteral("Jade Horizon"), QStringLiteral("#0D9488"), QStringLiteral("#0D9488")});
    m_palettes.append(PaletteInfo{QStringLiteral("copper_spark"), QString::fromUtf8("شرارة النحاس"), QStringLiteral("Copper Spark"), QStringLiteral("#EA580C"), QStringLiteral("#EA580C")});
    m_palettes.append(PaletteInfo{QStringLiteral("nebula_orchid"), QString::fromUtf8("أوركيد السديم"), QStringLiteral("Nebula Orchid"), QStringLiteral("#C084FC"), QStringLiteral("#C084FC")});
    m_palettes.append(PaletteInfo{QStringLiteral("glacier_azure"), QString::fromUtf8("أزور جليدي"), QStringLiteral("Glacier Azure"), QStringLiteral("#0284C7"), QStringLiteral("#0284C7")});
    m_palettes.append(PaletteInfo{QStringLiteral("forest_moss"), QString::fromUtf8("أخضر الغابات"), QStringLiteral("Forest Moss"), QStringLiteral("#16A34A"), QStringLiteral("#16A34A")});
    m_palettes.append(PaletteInfo{QStringLiteral("solar_citrine"), QString::fromUtf8("سترين شمسي"), QStringLiteral("Solar Citrine"), QStringLiteral("#CA8A04"), QStringLiteral("#CA8A04")});
    m_palettes.append(PaletteInfo{QStringLiteral("velvet_plum"), QString::fromUtf8("برقوقي مخملي"), QStringLiteral("Velvet Plum"), QStringLiteral("#9333EA"), QStringLiteral("#9333EA")});
    m_palettes.append(PaletteInfo{QStringLiteral("arctic_slate"), QString::fromUtf8("رمادي قطبي"), QStringLiteral("Arctic Slate"), QStringLiteral("#64748B"), QStringLiteral("#64748B")});
    m_palettes.append(PaletteInfo{QStringLiteral("crimson_ember"), QString::fromUtf8("جمر قرمزي"), QStringLiteral("Crimson Ember"), QStringLiteral("#DC2626"), QStringLiteral("#DC2626")});
    m_palettes.append(PaletteInfo{QStringLiteral("hyper_lavender"), QString::fromUtf8("لافندر فائق"), QStringLiteral("Hyper Lavender"), QStringLiteral("#818CF8"), QStringLiteral("#818CF8")});
    m_palettes.append(PaletteInfo{QStringLiteral("cyber_peach"), QString::fromUtf8("خوخي مستقبلي"), QStringLiteral("Cyber Peach"), QStringLiteral("#FB923C"), QStringLiteral("#FB923C")});
    m_palettes.append(PaletteInfo{QStringLiteral("quantum_aqua"), QString::fromUtf8("أكوا كوانتم"), QStringLiteral("Quantum Aqua"), QStringLiteral("#2DD4BF"), QStringLiteral("#2DD4BF")});
    m_palettes.append(PaletteInfo{QStringLiteral("midnight_berry"), QString::fromUtf8("توتي منتصف الليل"), QStringLiteral("Midnight Berry"), QStringLiteral("#DB2777"), QStringLiteral("#DB2777")});
    m_palettes.append(PaletteInfo{QStringLiteral("spring_pistachio"), QString::fromUtf8("فستقي ربيعي"), QStringLiteral("Spring Pistachio"), QStringLiteral("#65A30D"), QStringLiteral("#65A30D")});
    m_palettes.append(PaletteInfo{QStringLiteral("deep_cerulean"), QString::fromUtf8("سيروليان عميق"), QStringLiteral("Deep Cerulean"), QStringLiteral("#0369A1"), QStringLiteral("#0369A1")});
    m_palettes.append(PaletteInfo{QStringLiteral("bronze_flame"), QString::fromUtf8("لهب البرونز"), QStringLiteral("Bronze Flame"), QStringLiteral("#D97706"), QStringLiteral("#D97706")});
    rebuildTokens();
}

void Theme::setTheme(const QString& mode, const QString& paletteId, const QString& customAccent) {
    if (!mode.isEmpty()) m_dark = (mode != "light");
    if (!paletteId.isEmpty()) m_paletteId = paletteId;
    if (!customAccent.isEmpty()) m_customAccent = customAccent;
    rebuildTokens();
    emit themeChanged();
}

void Theme::rebuildTokens() {
    QString rawAccentHex = m_customAccent;
    if (m_paletteId != "custom") {
        for (const auto& p : m_palettes) {
            if (p.id == m_paletteId) {
                rawAccentHex = p.accentHex;
                break;
            }
        }
    }
    QColor c(rawAccentHex);
    if (!c.isValid()) c = QColor("#8B5CF6");

    double h = 0.0, l = 0.0, s = 0.0;
    rgbToHls(c.redF(), c.greenF(), c.blueF(), h, l, s);
    const double sAcc = qBound(0.62, s, 0.95);
    const double lAcc = qBound(0.48, l, 0.64);

    const QString accent = hslToHex(h, sAcc, lAcc);
    const QString accent2 = hslToHex(std::fmod(h + 18.0, 360.0), std::min(0.95, sAcc * 0.92), std::max(0.36, lAcc - 0.12));
    const QString accentHi = hslToHex(std::fmod(h - 8.0 + 360.0, 360.0), std::min(1.0, sAcc * 1.05), std::min(0.78, lAcc + 0.16));

    QColor acQ(accent);
    const double luma = 0.299 * acQ.redF() + 0.587 * acQ.greenF() + 0.114 * acQ.blueF();
    const QString onAccent = (luma > 0.62) ? QStringLiteral("#090B10") : QStringLiteral("#FFFFFF");

    if (m_dark) {
        const QString darkBg0 = hslToHex(h, 0.26, 0.055);
        const QString darkBg1 = hslToHex(h, 0.24, 0.082);
        const QString darkBg2 = hslToHex(h, 0.22, 0.115);
        const QString darkSurf = hslToHex(h, 0.20, 0.145);
        const QString darkSurfHi = hslToHex(h, 0.22, 0.195);

        m_tokens["bg0"] = darkBg0;
        m_tokens["bg1"] = darkBg1;
        m_tokens["bg2"] = darkBg2;
        m_tokens["surface"] = darkSurf;
        m_tokens["surface_hi"] = darkSurfHi;
        m_tokens["border"] = rgbaHexStr(hslToHex(h, 0.30, 0.72), 34);
        m_tokens["border_hi"] = rgbaHexStr(accentHi, 96);
        m_tokens["text"] = "#F4F6FB";
        m_tokens["text2"] = "#A7B0C6";
        m_tokens["text3"] = "#6D768D";
        m_tokens["accent"] = accent;
        m_tokens["accent2"] = accent2;
        m_tokens["accent_hi"] = accentHi;
        m_tokens["on_accent"] = onAccent;
        m_tokens["ok"] = "#10B981";
        m_tokens["success"] = "#10B981";
        m_tokens["danger"] = "#EF4444";
        m_tokens["warn"] = "#F59E0B";
        m_tokens["glass"] = rgbaHexStr(darkSurf, 195);
        m_tokens["shadow"] = "#90000000";
    } else {
        const QString lightBg0 = hslToHex(h, 0.25, 0.965);
        const QString lightBg1 = hslToHex(h, 0.22, 0.935);
        const QString lightBg2 = hslToHex(h, 0.18, 0.895);
        const QString lightSurf = "#FFFFFF";
        const QString lightSurfHi = hslToHex(h, 0.30, 0.95);

        m_tokens["bg0"] = lightBg0;
        m_tokens["bg1"] = lightBg1;
        m_tokens["bg2"] = lightBg2;
        m_tokens["surface"] = lightSurf;
        m_tokens["surface_hi"] = lightSurfHi;
        m_tokens["border"] = rgbaHexStr(hslToHex(h, 0.28, 0.25), 32);
        m_tokens["border_hi"] = rgbaHexStr(accent, 110);
        m_tokens["text"] = "#0F1420";
        m_tokens["text2"] = "#465068";
        m_tokens["text3"] = "#7B849B";
        m_tokens["accent"] = hslToHex(h, sAcc, std::max(0.42, lAcc - 0.06));
        m_tokens["accent2"] = accent2;
        m_tokens["accent_hi"] = accent;
        m_tokens["on_accent"] = "#FFFFFF";
        m_tokens["ok"] = "#059669";
        m_tokens["success"] = "#059669";
        m_tokens["danger"] = "#DC2626";
        m_tokens["warn"] = "#F59E0B";
        m_tokens["glass"] = rgbaHexStr("#FFFFFF", 220);
        m_tokens["shadow"] = "#28090D16";
    }

    m_structTokens.bg = m_tokens["bg0"];
    m_structTokens.surface = m_tokens["surface"];
    m_structTokens.card = m_tokens["bg1"];
    m_structTokens.cardHover = m_tokens["surface_hi"];
    m_structTokens.sidebarBg = m_tokens["bg1"];
    m_structTokens.border = m_tokens["border"];
    m_structTokens.text = m_tokens["text"];
    m_structTokens.textMuted = m_tokens["text2"];
    m_structTokens.textDim = m_tokens["text3"];
    m_structTokens.accent = m_tokens["accent"];
    m_structTokens.accentHover = m_tokens["accent_hi"];
    m_structTokens.success = m_tokens["success"];
    m_structTokens.warning = m_tokens["warn"];
    m_structTokens.danger = m_tokens["danger"];
}

QColor Theme::c(const QString& token, int alpha) const {
    QColor col(m_tokens.value(token, "#8B5CF6"));
    if (alpha < 255) col.setAlpha(qBound(0, alpha, 255));
    return col;
}

QString Theme::tokenHex(const QString& token) const {
    return m_tokens.value(token, "#8B5CF6");
}

} // namespace vmacro