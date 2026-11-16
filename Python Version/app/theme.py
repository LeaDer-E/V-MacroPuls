# -*- coding: utf-8 -*-
"""Design Tokens + 36 Palettes + HSL gen_palette() + ThemeManager singleton (T)."""

from __future__ import annotations
import colorsys
from dataclasses import dataclass
from typing import Dict, List, Optional
from PySide6.QtCore import QObject, Signal, Qt
from PySide6.QtGui import QColor, QGuiApplication


def _hex_to_hsl(hex_str: str) -> tuple[float, float, float]:
    c = QColor(hex_str)
    r, g, b = c.redF(), c.greenF(), c.blueF()
    h, l, s = colorsys.rgb_to_hls(r, g, b)
    return h * 360.0, s, l


def _hsl_to_hex(h: float, s: float, l: float) -> str:
    h_norm = (h % 360.0) / 360.0
    s_clamped = max(0.0, min(1.0, s))
    l_clamped = max(0.0, min(1.0, l))
    r, g, b = colorsys.hls_to_rgb(h_norm, l_clamped, s_clamped)
    return QColor.fromRgbF(r, g, b).name()


def _rgba_str(hex_str: str, alpha: int) -> str:
    c = QColor(hex_str)
    c.setAlpha(max(0, min(255, alpha)))
    return c.name(QColor.NameFormat.HexArgb)


@dataclass
class PaletteDef:
    id: str
    ar_name: str
    en_name: str
    accent_hex: str
    dark_tokens: Dict[str, str]
    light_tokens: Dict[str, str]


def gen_palette(
    palette_id: str,
    ar_name: str,
    en_name: str,
    accent_hex: str,
    hue: Optional[float] = None,
) -> PaletteDef:
    """Generate full Dark and Light semantic token maps from a single accent color using HSL math."""
    h, s, l = _hex_to_hsl(accent_hex)
    if hue is not None:
        h = hue
    s_acc = max(0.62, min(0.95, s))
    l_acc = max(0.48, min(0.64, l))

    accent = _hsl_to_hex(h, s_acc, l_acc)
    accent2 = _hsl_to_hex((h + 18) % 360.0, min(0.95, s_acc * 0.92), max(0.36, l_acc - 0.12))
    accent_hi = _hsl_to_hex((h - 8) % 360.0, min(1.0, s_acc * 1.05), min(0.78, l_acc + 0.16))

    # Determine optimal contrast text on accent button
    ac_q = QColor(accent)
    luma = 0.299 * ac_q.redF() + 0.587 * ac_q.greenF() + 0.114 * ac_q.blueF()
    on_accent = "#090B10" if luma > 0.62 else "#FFFFFF"

    # Dark tokens (tinted subtly by hue)
    dark_bg0 = _hsl_to_hex(h, 0.26, 0.055)
    dark_bg1 = _hsl_to_hex(h, 0.24, 0.082)
    dark_bg2 = _hsl_to_hex(h, 0.22, 0.115)
    dark_surf = _hsl_to_hex(h, 0.20, 0.145)
    dark_surf_hi = _hsl_to_hex(h, 0.22, 0.195)
    dark_border = _rgba_str(_hsl_to_hex(h, 0.30, 0.72), 34)
    dark_border_hi = _rgba_str(accent_hi, 96)

    dark_tokens = {
        "bg0": dark_bg0,
        "bg1": dark_bg1,
        "bg2": dark_bg2,
        "surface": dark_surf,
        "surface_hi": dark_surf_hi,
        "border": dark_border,
        "border_hi": dark_border_hi,
        "text": "#F4F6FB",
        "text2": "#A7B0C6",
        "text3": "#6D768D",
        "accent": accent,
        "accent2": accent2,
        "accent_hi": accent_hi,
        "on_accent": on_accent,
        "ok": "#10B981",
        "danger": "#EF4444",
        "glass": _rgba_str(dark_surf, 195),
        "shadow": "#90000000",
    }

    # Light tokens (clean crisp daylight surfaces tinted subtly by hue)
    light_bg0 = _hsl_to_hex(h, 0.25, 0.965)
    light_bg1 = _hsl_to_hex(h, 0.22, 0.935)
    light_bg2 = _hsl_to_hex(h, 0.18, 0.895)
    light_surf = "#FFFFFF"
    light_surf_hi = _hsl_to_hex(h, 0.30, 0.95)
    light_border = _rgba_str(_hsl_to_hex(h, 0.28, 0.25), 32)
    light_border_hi = _rgba_str(accent, 110)

    light_tokens = {
        "bg0": light_bg0,
        "bg1": light_bg1,
        "bg2": light_bg2,
        "surface": light_surf,
        "surface_hi": light_surf_hi,
        "border": light_border,
        "border_hi": light_border_hi,
        "text": "#0F1420",
        "text2": "#465068",
        "text3": "#7B849B",
        "accent": _hsl_to_hex(h, s_acc, max(0.42, l_acc - 0.06)),
        "accent2": accent2,
        "accent_hi": accent,
        "on_accent": "#FFFFFF",
        "ok": "#059669",
        "danger": "#DC2626",
        "glass": _rgba_str("#FFFFFF", 220),
        "shadow": "#28090D16",
    }

    return PaletteDef(
        id=palette_id,
        ar_name=ar_name,
        en_name=en_name,
        accent_hex=accent,
        dark_tokens=dark_tokens,
        light_tokens=light_tokens,
    )


# 36 Curated Premium Palettes
RAW_PALETTE_SPECS = [
    ("cyber_violet", "بنفسجي سيبراني", "Cyber Violet", "#8B5CF6"),
    ("arc_indigo", "نيلي آرك", "Arc Indigo", "#6366F1"),
    ("linear_blue", "أزرق لينير", "Linear Sapphire", "#3B82F6"),
    ("neon_cyan", "سماوي نيون", "Neon Cyan", "#06B6D4"),
    ("emerald_pulse", "زمردي نابض", "Emerald Pulse", "#10B981"),
    ("matrix_mint", "نعناعي رقمي", "Matrix Mint", "#14B8A6"),
    ("amber_forge", "كهرمان متوهج", "Amber Forge", "#F59E0B"),
    ("crimson_laser", "قرمزي ليزر", "Crimson Laser", "#F43F5E"),
    ("royal_magenta", "أرجواني ملكي", "Royal Magenta", "#D946EF"),
    ("sunset_coral", "مرجاني الغروب", "Sunset Coral", "#FB7185"),
    ("electric_lime", "ليموني كهربائي", "Electric Lime", "#84CC16"),
    ("oceanic_teal", "فيروزي محيطي", "Oceanic Teal", "#0EA5E9"),
    ("obsidian_gold", "ذهبي الأوبسيديان", "Obsidian Gold", "#EAB308"),
    ("titanium_ice", "تيتانيوم جليدي", "Titanium Ice", "#38BDF8"),
    ("plasma_pink", "وردي البلازما", "Plasma Pink", "#EC4899"),
    ("volcanic_orange", "برتقالي بركاني", "Volcanic Orange", "#F97316"),
    ("aurora_green", "أخضر الشفق", "Aurora Green", "#22C55E"),
    ("amethyst_dream", "حلم الجمشت", "Amethyst Dream", "#A855F7"),
    ("cobalt_storm", "عاصفة الكوبالت", "Cobalt Storm", "#2563EB"),
    ("ruby_flare", "وهج الياقوت", "Ruby Flare", "#E11D48"),
    ("jade_horizon", "أفق اليشم", "Jade Horizon", "#0D9488"),
    ("copper_spark", "شرارة النحاس", "Copper Spark", "#EA580C"),
    ("nebula_orchid", "أوركيد السديم", "Nebula Orchid", "#C084FC"),
    ("glacier_azure", "أزور جليدي", "Glacier Azure", "#0284C7"),
    ("forest_moss", "أخضر الغابات", "Forest Moss", "#16A34A"),
    ("solar_citrine", "سترين شمسي", "Solar Citrine", "#CA8A04"),
    ("velvet_plum", "برقوقي مخملي", "Velvet Plum", "#9333EA"),
    ("arctic_slate", "رمادي قطبي", "Arctic Slate", "#64748B"),
    ("crimson_ember", "جمر قرمزي", "Crimson Ember", "#DC2626"),
    ("hyper_lavender", "لافندر فائق", "Hyper Lavender", "#818CF8"),
    ("cyber_peach", "خوخي مستقبلي", "Cyber Peach", "#FB923C"),
    ("quantum_aqua", "أكوا كوانتم", "Quantum Aqua", "#2DD4BF"),
    ("midnight_berry", "توتي منتصف الليل", "Midnight Berry", "#DB2777"),
    ("spring_pistachio", "فستقي ربيعي", "Spring Pistachio", "#65A30D"),
    ("deep_cerulean", "سيروليان عميق", "Deep Cerulean", "#0369A1"),
    ("bronze_flame", "لهب البرونز", "Bronze Flame", "#D97706"),
]

PALETTES: Dict[str, PaletteDef] = {
    pid: gen_palette(pid, ar, en, hx) for (pid, ar, en, hx) in RAW_PALETTE_SPECS
}


class ThemeManager(QObject):
    """Central theme manager singleton accessed via `T`."""

    theme_changed = Signal()

    def __init__(self) -> None:
        super().__init__()
        self._palette_id: str = "cyber_violet"
        self._mode: str = "dark"  # "dark", "light", "auto"
        self._custom_accent: str = "#8B5CF6"
        self._custom_palette: PaletteDef = gen_palette(
            "custom", "لون مخصص", "Custom Accent", self._custom_accent
        )
        self._prev_tokens: Dict[str, str] = dict(self.current_tokens())

    def current_tokens(self) -> Dict[str, str]:
        pal = self.active_palette()
        return dict(pal.dark_tokens if self.is_dark() else pal.light_tokens)

    @property
    def prev_tokens(self) -> Dict[str, str]:
        return self._prev_tokens

    def configure(self, palette_id: str, mode: str, custom_accent: str = "#8B5CF6") -> None:
        self._prev_tokens = self.current_tokens()
        self._palette_id = palette_id if (palette_id in PALETTES or palette_id == "custom") else "cyber_violet"
        self._mode = mode if mode in ("dark", "light", "auto") else "dark"
        self._custom_accent = custom_accent or "#8B5CF6"
        self._custom_palette = gen_palette(
            "custom", "لون مخصص", "Custom Accent", self._custom_accent
        )
        self.theme_changed.emit()

    @property
    def palette_id(self) -> str:
        return self._palette_id

    @property
    def mode(self) -> str:
        return self._mode

    @property
    def custom_accent(self) -> str:
        return self._custom_accent

    def is_dark(self) -> bool:
        if self._mode == "dark":
            return True
        if self._mode == "light":
            return False
        # Auto mode: query OS color scheme
        try:
            hints = QGuiApplication.styleHints()
            if hints and hasattr(hints, "colorScheme"):
                return hints.colorScheme() != Qt.ColorScheme.Light
        except Exception:
            pass
        return True

    def active_palette(self) -> PaletteDef:
        if self._palette_id == "custom":
            return self._custom_palette
        return PALETTES.get(self._palette_id, PALETTES["cyber_violet"])

    def token_hex(self, token: str, force_dark: bool = False) -> str:
        pal = self.active_palette()
        tokens = pal.dark_tokens if (force_dark or self.is_dark()) else pal.light_tokens
        return tokens.get(token, "#FF00FF")

    def c(self, token: str, alpha: Optional[int] = None, force_dark: bool = False) -> QColor:
        """Return QColor for design token dynamically at paint time."""
        qc = QColor(self.token_hex(token, force_dark=force_dark))
        if alpha is not None:
            qc.setAlpha(max(0, min(255, alpha)))
        return qc

    def all_palettes(self) -> List[PaletteDef]:
        return list(PALETTES.values())


# Global singleton
T = ThemeManager()
