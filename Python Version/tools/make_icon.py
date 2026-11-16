# -*- coding: utf-8 -*-
"""
Generates multi-size assets/icon.ico (256, 128, 64, 48, 32, 24, 16px) & assets/icon.png
from the Cyber-Automation V-Blade + Cursor + Pulse Logo (`logo_pixmap` in `app/ui/icons.py`),
with a matching high-precision Pillow renderer when run in pre-Qt environments.
"""

from __future__ import annotations
import io
import struct
import sys
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parent.parent
if str(ROOT_DIR) not in sys.path:
    sys.path.insert(0, str(ROOT_DIR))


def _render_via_qt(size: int) -> bytes:
    from PySide6.QtCore import QBuffer, QByteArray, QIODevice
    from PySide6.QtGui import QGuiApplication
    from app.ui.icons import logo_pixmap

    _ = QGuiApplication.instance() or QGuiApplication(sys.argv)
    pix = logo_pixmap(size=size, dpr=1.0)
    ba = QByteArray()
    buf = QBuffer(ba)
    buf.open(QIODevice.OpenModeFlag.WriteOnly)
    pix.save(buf, "PNG")
    buf.close()
    return bytes(ba)


def _render_via_pillow(size: int) -> bytes:
    from PIL import Image, ImageDraw

    ss = 4
    w = size * ss
    cx = w * 0.5
    img = Image.new("RGBA", (w, w), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)

    pad = int(w * 0.04)
    rad = int(w * 0.24)
    d.rounded_rectangle(
        [pad, pad, w - pad, w - pad],
        radius=rad,
        fill=(11, 13, 24, 255),
        outline=(168, 85, 247, 220),
        width=max(2, int(w * 0.026)),
    )

    if size >= 40:
        in_pad = int(w * 0.12)
        d.rounded_rectangle(
            [in_pad, in_pad, w - in_pad, w - in_pad],
            radius=int(rad * 0.72),
            outline=(192, 132, 252, 65),
            width=max(1, int(w * 0.015)),
        )

    # Left bright wing of the V
    left_wing = [
        (w * 0.19, w * 0.22),
        (w * 0.36, w * 0.22),
        (cx, w * 0.60),
        (cx, w * 0.83),
    ]
    d.polygon(left_wing, fill=(192, 132, 252, 255))

    # Right wing of the V morphing into upward Cursor Arrow
    right_wing = [
        (cx, w * 0.60),
        (w * 0.64, w * 0.26),
        (w * 0.57, w * 0.26),
        (w * 0.77, w * 0.16),
        (w * 0.83, w * 0.36),
        (w * 0.76, w * 0.31),
        (cx, w * 0.83),
    ]
    d.polygon(right_wing, fill=(124, 58, 237, 255))

    out = img.resize((size, size), Image.Resampling.LANCZOS)
    buf = io.BytesIO()
    out.save(buf, format="PNG")
    return buf.getvalue()


def get_png_frame(size: int) -> bytes:
    try:
        return _render_via_qt(size)
    except Exception:
        return _render_via_pillow(size)


def build_multi_size_ico(ico_path: Path, png_path: Path) -> None:
    sizes = [256, 128, 64, 48, 32, 24, 16]
    frames: list[tuple[int, bytes]] = []
    for s in sizes:
        png_data = get_png_frame(s)
        frames.append((s, png_data))
        if s == 256:
            png_path.write_bytes(png_data)

    header = struct.pack("<HHH", 0, 1, len(frames))
    dir_entries = bytearray()
    payloads = bytearray()
    offset = 6 + 16 * len(frames)

    for s, data in frames:
        w_byte = 0 if s >= 256 else s
        h_byte = 0 if s >= 256 else s
        entry = struct.pack("<BBBBHHII", w_byte, h_byte, 0, 0, 1, 32, len(data), offset)
        dir_entries.extend(entry)
        payloads.extend(data)
        offset += len(data)

    ico_path.write_bytes(header + bytes(dir_entries) + bytes(payloads))


def main() -> None:
    assets = ROOT_DIR / "assets"
    assets.mkdir(parents=True, exist_ok=True)
    ico_file = assets / "icon.ico"
    png_file = assets / "icon.png"
    build_multi_size_ico(ico_file, png_file)
    print(f"Generated multi-size icon: {ico_file} & {png_file}")


if __name__ == "__main__":
    main()
