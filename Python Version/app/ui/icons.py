# -*- coding: utf-8 -*-
"""
Procedural QPainter Icons & Cyber-Tech 'V-MacroPlus' Logo (`logo_pixmap`):
- Zero religious or octagonal ornaments!
- Sleek dark rounded-square hardware keycap badge (`bg1 -> bg0`) with a subtle tech circuit rim.
- Bold, razor-sharp 3D metallic 'V' blade whose right wing transforms into a dynamic precision Cursor
  and high-frequency Macro Pulse wave (`accent_hi -> accent -> accent2`).
- Includes `"grip"` (6-dot drag handle icon) and `"test"` / `"flask"` icon for the Test Lab tab.
"""

from __future__ import annotations
import math
from PySide6.QtCore import QPointF, QRectF, Qt
from PySide6.QtGui import (
    QColor,
    QIcon,
    QLinearGradient,
    QPainter,
    QPainterPath,
    QPen,
    QPixmap,
    QPolygonF,
    QRadialGradient,
)
from app.theme import T


_LOGO_PIXMAP_CACHE: dict[tuple[int, float, str, bool, str], QPixmap] = {}


def logo_pixmap(size: int = 64, dpr: float = 2.0) -> QPixmap:
    """
    Render the signature Cyber-Automation 'V-MacroPlus' Logo procedurally with QPainter.
    Features a bold futuristic 'V' integrated with a Macro Pulse waveform and Cursor spearhead.
    """
    cache_key = (size, dpr, T.palette_id, T.is_dark(), T.custom_accent)
    cached = _LOGO_PIXMAP_CACHE.get(cache_key)
    if cached is not None:
        return cached
    px_size = max(16, int(round(size * dpr)))
    pix = QPixmap(px_size, px_size)
    pix.fill(Qt.GlobalColor.transparent)
    pix.setDevicePixelRatio(dpr)

    p = QPainter(pix)
    p.setRenderHints(
        QPainter.RenderHint.Antialiasing
        | QPainter.RenderHint.SmoothPixmapTransform
        | QPainter.RenderHint.TextAntialiasing
    )

    s = float(size)
    cx, cy = s * 0.5, s * 0.5
    show_micro = size >= 40

    bg0 = T.c("bg0", force_dark=True)
    bg1 = T.c("bg1", force_dark=True)
    accent = T.c("accent")
    accent2 = T.c("accent2")
    accent_hi = T.c("accent_hi")

    # 1. Hardware Keycap Squircle Base
    pad = max(1.0, s * 0.04)
    badge_rect = QRectF(pad, pad, s - 2 * pad, s - 2 * pad)
    radius = s * 0.24

    bg_grad = QLinearGradient(badge_rect.topLeft(), badge_rect.bottomRight())
    bg_grad.setColorAt(0.0, bg1.lighter(125))
    bg_grad.setColorAt(0.55, bg0)
    bg_grad.setColorAt(1.0, QColor(7, 8, 14))

    p.setPen(Qt.PenStyle.NoPen)
    p.setBrush(bg_grad)
    p.drawRoundedRect(badge_rect, radius, radius)

    # 2. Internal Tech Glow
    glow = QRadialGradient(QPointF(cx, s * 0.54), s * 0.44)
    c_glow = QColor(accent)
    c_glow.setAlpha(85 if show_micro else 65)
    c_zero = QColor(accent2)
    c_zero.setAlpha(0)
    glow.setColorAt(0.0, c_glow)
    glow.setColorAt(1.0, c_zero)
    p.setBrush(glow)
    p.drawRoundedRect(badge_rect, radius, radius)

    # 3. Subtle Inner Keycap Bevel & Macro Pulse Wave (when size >= 40)
    if show_micro:
        inner_key = badge_rect.adjusted(s * 0.10, s * 0.10, -s * 0.10, -s * 0.10)
        p.setBrush(Qt.BrushStyle.NoBrush)
        p.setPen(QPen(QColor(accent_hi.red(), accent_hi.green(), accent_hi.blue(), 42), max(1.0, s * 0.015)))
        p.drawRoundedRect(inner_key, radius * 0.72, radius * 0.72)

        pulse = QPainterPath()
        pulse.moveTo(s * 0.14, s * 0.54)
        pulse.lineTo(s * 0.26, s * 0.54)
        pulse.lineTo(s * 0.31, s * 0.43)
        pulse.lineTo(s * 0.36, s * 0.62)
        pulse.lineTo(s * 0.41, s * 0.54)
        pulse.moveTo(s * 0.62, s * 0.54)
        pulse.lineTo(s * 0.67, s * 0.44)
        pulse.lineTo(s * 0.72, s * 0.61)
        pulse.lineTo(s * 0.77, s * 0.54)
        pulse.lineTo(s * 0.86, s * 0.54)
        pulse_pen = QPen(QColor(accent_hi.red(), accent_hi.green(), accent_hi.blue(), 95), max(1.1, s * 0.020))
        pulse_pen.setCapStyle(Qt.PenCapStyle.RoundCap)
        pulse_pen.setJoinStyle(Qt.PenJoinStyle.RoundJoin)
        p.setPen(pulse_pen)
        p.drawPath(pulse)

    # 4. Bold 3D Metallic 'V' Blade Integrated with an Upward Precision Cursor
    left_wing = QPainterPath()
    left_wing.moveTo(s * 0.19, s * 0.22)
    left_wing.lineTo(s * 0.36, s * 0.22)
    left_wing.lineTo(cx, s * 0.60)
    left_wing.lineTo(cx, s * 0.83)
    left_wing.closeSubpath()

    left_grad = QLinearGradient(QPointF(s * 0.19, s * 0.22), QPointF(cx, s * 0.83))
    left_grad.setColorAt(0.0, QColor("#FFFFFF"))
    left_grad.setColorAt(0.25, accent_hi)
    left_grad.setColorAt(0.75, accent)
    left_grad.setColorAt(1.0, accent2)

    p.setPen(Qt.PenStyle.NoPen)
    p.setBrush(left_grad)
    p.drawPath(left_wing)

    right_wing = QPainterPath()
    right_wing.moveTo(cx, s * 0.60)
    right_wing.lineTo(s * 0.64, s * 0.26)
    right_wing.lineTo(s * 0.57, s * 0.26)
    right_wing.lineTo(s * 0.77, s * 0.16)
    right_wing.lineTo(s * 0.83, s * 0.36)
    right_wing.lineTo(s * 0.76, s * 0.31)
    right_wing.lineTo(cx, s * 0.83)
    right_wing.closeSubpath()

    right_grad = QLinearGradient(QPointF(s * 0.80, s * 0.16), QPointF(cx, s * 0.83))
    right_grad.setColorAt(0.0, QColor("#FFFFFF"))
    right_grad.setColorAt(0.28, accent_hi)
    right_grad.setColorAt(0.65, accent)
    right_grad.setColorAt(1.0, accent2.darker(145))

    p.setBrush(right_grad)
    p.drawPath(right_wing)

    ridge_pen = QPen(QColor(255, 255, 255, 200), max(1.1, s * 0.018))
    ridge_pen.setCapStyle(Qt.PenCapStyle.RoundCap)
    p.setPen(ridge_pen)
    p.drawLine(QPointF(s * 0.27, s * 0.23), QPointF(cx, s * 0.82))
    p.drawLine(QPointF(s * 0.77, s * 0.17), QPointF(cx, s * 0.82))

    if size >= 26:
        plus_pen = QPen(QColor(255, 255, 255, 235), max(1.3, s * 0.026))
        plus_pen.setCapStyle(Qt.PenCapStyle.RoundCap)
        p.setPen(plus_pen)
        px_c, py_c = s * 0.46, s * 0.33
        arm = s * 0.052
        p.drawLine(QPointF(px_c - arm, py_c), QPointF(px_c + arm, py_c))
        p.drawLine(QPointF(px_c, py_c - arm), QPointF(px_c, py_c + arm))

    # 5. Outer Metallic Keycap Rim
    rim_grad = QLinearGradient(badge_rect.topLeft(), badge_rect.bottomRight())
    rim_grad.setColorAt(0.0, QColor(accent_hi.red(), accent_hi.green(), accent_hi.blue(), 190))
    rim_grad.setColorAt(0.5, QColor(accent.red(), accent.green(), accent.blue(), 100))
    rim_grad.setColorAt(1.0, QColor(accent2.red(), accent2.green(), accent2.blue(), 160))

    p.setBrush(Qt.BrushStyle.NoBrush)
    p.setPen(QPen(rim_grad, max(1.2, s * 0.026)))
    p.drawRoundedRect(badge_rect.adjusted(0.6, 0.6, -0.6, -0.6), radius, radius)

    p.end()
    _LOGO_PIXMAP_CACHE[cache_key] = pix
    return pix


def logo_icon(size: int = 64) -> QIcon:
    return QIcon(logo_pixmap(size=size, dpr=2.0))


def draw_icon(
    p: QPainter,
    name: str,
    rect: QRectF,
    color: QColor | None = None,
    stroke_width: float = 1.9,
) -> None:
    """Draw vector icon procedurally on a normalized 24x24 grid inside `rect`."""
    c = color if color is not None else T.c("text")
    p.save()
    p.setRenderHint(QPainter.RenderHint.Antialiasing, True)

    scale = min(rect.width() / 24.0, rect.height() / 24.0)
    ox = rect.x() + (rect.width() - 24.0 * scale) * 0.5
    oy = rect.y() + (rect.height() - 24.0 * scale) * 0.5
    p.translate(ox, oy)
    p.scale(scale, scale)

    pen = QPen(c, stroke_width, Qt.PenStyle.SolidLine, Qt.PenCapStyle.RoundCap, Qt.PenJoinStyle.RoundJoin)
    p.setPen(pen)
    p.setBrush(Qt.BrushStyle.NoBrush)

    if name == "grip":
        p.setPen(Qt.PenStyle.NoPen)
        p.setBrush(c)
        for gx in (8.5, 15.5):
            for gy in (6.0, 12.0, 18.0):
                p.drawEllipse(QPointF(gx, gy), 1.6, 1.6)

    elif name == "pause":
        p.setPen(Qt.PenStyle.NoPen)
        p.setBrush(c)
        p.drawRoundedRect(QRectF(6.5, 5.0, 3.6, 14.0), 1.4, 1.4)
        p.drawRoundedRect(QRectF(13.9, 5.0, 3.6, 14.0), 1.4, 1.4)

    elif name == "note":
        p.drawRoundedRect(QRectF(4.5, 3.5, 15.0, 17.0), 2.6, 2.6)
        p.drawLine(QPointF(8.0, 8.5), QPointF(16.0, 8.5))
        p.drawLine(QPointF(8.0, 12.5), QPointF(16.0, 12.5))
        p.drawLine(QPointF(8.0, 16.5), QPointF(13.0, 16.5))

    elif name in ("test", "flask", "lab"):
        path = QPainterPath()
        path.moveTo(9.0, 3.5)
        path.lineTo(15.0, 3.5)
        path.moveTo(10.2, 3.5)
        path.lineTo(10.2, 8.8)
        path.lineTo(5.0, 18.2)
        path.cubicTo(4.4, 19.5, 5.2, 20.5, 6.8, 20.5)
        path.lineTo(17.2, 20.5)
        path.cubicTo(18.8, 20.5, 19.6, 19.5, 19.0, 18.2)
        path.lineTo(13.8, 8.8)
        path.lineTo(13.8, 3.5)
        p.drawPath(path)
        p.drawLine(QPointF(7.5, 14.5), QPointF(16.5, 14.5))

    elif name == "dashboard":
        p.drawRoundedRect(QRectF(3, 3, 8, 8), 2.2, 2.2)
        p.drawRoundedRect(QRectF(13, 3, 8, 5), 2.0, 2.0)
        p.drawRoundedRect(QRectF(13, 10, 8, 11), 2.2, 2.2)
        p.drawRoundedRect(QRectF(3, 13, 8, 8), 2.2, 2.2)

    elif name in ("recorder", "record"):
        p.drawEllipse(QPointF(12, 12), 8.5, 8.5)
        p.setBrush(c)
        p.setPen(Qt.PenStyle.NoPen)
        p.drawEllipse(QPointF(12, 12), 4.2, 4.2)

    elif name in ("combos", "bolt"):
        path = QPainterPath()
        path.moveTo(13.5, 2.5)
        path.lineTo(5.5, 13.2)
        path.lineTo(11.8, 13.2)
        path.lineTo(10.5, 21.5)
        path.lineTo(18.5, 10.8)
        path.lineTo(12.2, 10.8)
        path.closeSubpath()
        p.drawPath(path)

    elif name in ("terminal", "cmd", "powershell"):
        p.drawRoundedRect(QRectF(2.5, 4.0, 19.0, 16.0), 2.5, 2.5)
        p.drawLine(QPointF(6.5, 9.0), QPointF(10.5, 12.0))
        p.drawLine(QPointF(10.5, 12.0), QPointF(6.5, 15.0))
        p.drawLine(QPointF(12.5, 15.0), QPointF(17.5, 15.0))

    elif name == "crosshair":
        p.drawEllipse(QPointF(12, 12), 7.0, 7.0)
        p.drawLine(QPointF(12, 2.5), QPointF(12, 6.5))
        p.drawLine(QPointF(12, 17.5), QPointF(12, 21.5))
        p.drawLine(QPointF(2.5, 12), QPointF(6.5, 12))
        p.drawLine(QPointF(17.5, 12), QPointF(21.5, 12))

    elif name == "shield":
        path = QPainterPath()
        path.moveTo(12, 2.8)
        path.lineTo(19.5, 6.0)
        path.cubicTo(19.5, 13.0, 16.5, 18.2, 12, 21.2)
        path.cubicTo(7.5, 18.2, 4.5, 13.0, 4.5, 6.0)
        path.closeSubpath()
        p.drawPath(path)

    elif name == "mail":
        p.drawRoundedRect(QRectF(3.0, 5.5, 18.0, 13.0), 2.2, 2.2)
        p.drawLine(QPointF(4.0, 6.5), QPointF(12.0, 12.5))
        p.drawLine(QPointF(20.0, 6.5), QPointF(12.0, 12.5))

    elif name == "phone":
        p.drawRoundedRect(QRectF(6.5, 3.0, 11.0, 18.0), 2.5, 2.5)
        p.drawLine(QPointF(10.5, 18.0), QPointF(13.5, 18.0))

    elif name in ("library", "folder"):
        p.drawRoundedRect(QRectF(3, 6, 18, 14), 2.5, 2.5)
        p.drawLine(QPointF(3, 10), QPointF(21, 10))
        p.drawLine(QPointF(8, 14.5), QPointF(16, 14.5))

    elif name in ("themes", "palette"):
        p.drawEllipse(QPointF(12, 12), 9.0, 9.0)
        p.setBrush(c)
        p.setPen(Qt.PenStyle.NoPen)
        for cx_i, cy_i in [(8.5, 9.5), (12.0, 7.5), (15.5, 9.5), (15.0, 14.0)]:
            p.drawEllipse(QPointF(cx_i, cy_i), 1.4, 1.4)

    elif name == "settings":
        p.drawEllipse(QPointF(12, 12), 3.4, 3.4)
        for i in range(8):
            ang = i * (math.pi / 4.0)
            x1, y1 = 12 + math.cos(ang) * 6.2, 12 + math.sin(ang) * 6.2
            x2, y2 = 12 + math.cos(ang) * 9.0, 12 + math.sin(ang) * 9.0
            p.drawLine(QPointF(x1, y1), QPointF(x2, y2))

    elif name in ("about", "info"):
        p.drawEllipse(QPointF(12, 12), 9.0, 9.0)
        p.drawLine(QPointF(12, 11.2), QPointF(12, 16.5))
        p.setBrush(c)
        p.setPen(Qt.PenStyle.NoPen)
        p.drawEllipse(QPointF(12, 7.8), 1.2, 1.2)

    elif name == "play":
        poly = QPolygonF([QPointF(7.5, 5.0), QPointF(18.5, 12.0), QPointF(7.5, 19.0)])
        p.setBrush(c)
        p.drawPolygon(poly)

    elif name == "stop":
        p.setBrush(c)
        p.drawRoundedRect(QRectF(6.5, 6.5, 11.0, 11.0), 2.2, 2.2)

    elif name in ("plus", "add"):
        p.drawLine(QPointF(12, 5.5), QPointF(12, 18.5))
        p.drawLine(QPointF(5.5, 12), QPointF(18.5, 12))

    elif name in ("trash", "delete"):
        p.drawLine(QPointF(4.5, 7), QPointF(19.5, 7))
        p.drawRoundedRect(QRectF(6.5, 7, 11, 13), 2.0, 2.0)
        p.drawLine(QPointF(9.5, 4.5), QPointF(14.5, 4.5))
        p.drawLine(QPointF(10, 11), QPointF(10, 16.5))
        p.drawLine(QPointF(14, 11), QPointF(14, 16.5))

    elif name == "save":
        p.drawRoundedRect(QRectF(4, 4, 16, 16), 2.5, 2.5)
        p.drawRoundedRect(QRectF(8, 4, 8, 6), 1.2, 1.2)
        p.drawRoundedRect(QRectF(7, 13, 10, 7), 1.5, 1.5)

    elif name in ("keyboard", "key"):
        p.drawRoundedRect(QRectF(2.5, 6.0, 19.0, 12.0), 2.5, 2.5)
        p.drawLine(QPointF(6.5, 10.0), QPointF(8.0, 10.0))
        p.drawLine(QPointF(11.0, 10.0), QPointF(13.0, 10.0))
        p.drawLine(QPointF(16.0, 10.0), QPointF(17.5, 10.0))
        p.drawLine(QPointF(7.5, 14.2), QPointF(16.5, 14.2))

    elif name == "mouse":
        p.drawRoundedRect(QRectF(6.5, 3.5, 11.0, 17.0), 5.5, 5.5)
        p.drawLine(QPointF(12.0, 7.0), QPointF(12.0, 10.5))

    elif name in ("clock", "wait"):
        p.drawEllipse(QPointF(12, 12), 8.5, 8.5)
        p.drawLine(QPointF(12, 7.2), QPointF(12, 12))
        p.drawLine(QPointF(12, 12), QPointF(15.5, 14.2))

    elif name == "check":
        p.drawLine(QPointF(5.5, 12.5), QPointF(10.0, 17.0))
        p.drawLine(QPointF(10.0, 17.0), QPointF(18.8, 7.2))

    elif name == "copy":
        p.drawRoundedRect(QRectF(8.5, 8.5, 11.0, 11.0), 2.0, 2.0)
        p.drawRoundedRect(QRectF(4.5, 4.5, 11.0, 11.0), 2.0, 2.0)

    elif name == "search":
        p.drawEllipse(QPointF(10.5, 10.5), 6.2, 6.2)
        p.drawLine(QPointF(15.2, 15.2), QPointF(19.5, 19.5))

    elif name == "chevron_down":
        p.drawLine(QPointF(6.5, 9.5), QPointF(12.0, 15.0))
        p.drawLine(QPointF(12.0, 15.0), QPointF(17.5, 9.5))

    elif name == "chevron_up":
        p.drawLine(QPointF(6.5, 14.5), QPointF(12.0, 9.0))
        p.drawLine(QPointF(12.0, 9.0), QPointF(17.5, 14.5))

    elif name == "minimize":
        p.drawLine(QPointF(6.0, 12.0), QPointF(18.0, 12.0))

    elif name == "maximize":
        p.drawRoundedRect(QRectF(6.0, 6.0, 12.0, 12.0), 2.0, 2.0)

    elif name == "close":
        p.drawLine(QPointF(6.8, 6.8), QPointF(17.2, 17.2))
        p.drawLine(QPointF(17.2, 6.8), QPointF(6.8, 17.2))

    elif name in ("globe", "lang"):
        p.drawEllipse(QPointF(12, 12), 8.5, 8.5)
        p.drawLine(QPointF(3.5, 12), QPointF(20.5, 12))
        p.drawEllipse(QPointF(12, 12), 4.2, 8.5)

    elif name == "bell":
        path = QPainterPath()
        path.moveTo(12, 4)
        path.cubicTo(8, 4, 6, 7.5, 6, 12)
        path.lineTo(4.5, 16)
        path.lineTo(19.5, 16)
        path.lineTo(18, 12)
        path.cubicTo(18, 7.5, 16, 4, 12, 4)
        p.drawPath(path)
        p.drawLine(QPointF(10, 19), QPointF(14, 19))

    else:
        p.drawEllipse(QPointF(12, 12), 6.0, 6.0)

    p.restore()


def make_qicon(name: str, size: int = 20, color: QColor | None = None) -> QIcon:
    dpr = 2.0
    px = int(size * dpr)
    pix = QPixmap(px, px)
    pix.fill(Qt.GlobalColor.transparent)
    pix.setDevicePixelRatio(dpr)
    p = QPainter(pix)
    draw_icon(p, name, QRectF(2, 2, size - 4, size - 4), color=color, stroke_width=2.0)
    p.end()
    return QIcon(pix)
