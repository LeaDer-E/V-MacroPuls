# -*- coding: utf-8 -*-
"""Saved Macros Library Page: Play, Load into Recorder, Export/Import JSON, or Delete."""

from __future__ import annotations
import json
from pathlib import Path
from typing import Any, Dict
from PySide6.QtGui import QFont
from PySide6.QtWidgets import (
    QFileDialog,
    QHBoxLayout,
    QLabel,
    QScrollArea,
    QVBoxLayout,
    QWidget,
)
from app.domain.macro_engine import MacroEngine
from app.i18n import ltr_value, t, tl
from app.notifier import NOTIFIER
from app.state import AppState
from app.theme import T
from app.ui.widgets import Button, GlassCard, make_font


class LibraryPage(QWidget):
    """Saved Macros Library manager."""

    def __init__(self, state: AppState, engine: MacroEngine, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self.state = state
        self.engine = engine
        self._build_ui()
        self.state.macros_library_updated.connect(self._refresh_list)
        self._refresh_list()

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(24, 16, 24, 20)
        root.setSpacing(16)

        hdr = GlassCard(accent_tint=True)
        h_lay = QHBoxLayout(hdr)
        h_lay.setContentsMargins(22, 16, 22, 16)

        t_col = QVBoxLayout()
        t_col.setSpacing(4)
        title = QLabel(t("lib_title"))
        title.setFont(make_font(20, QFont.Weight.ExtraBold))
        title.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        sub = QLabel(t("lib_subtitle"))
        sub.setFont(make_font(12, QFont.Weight.Medium))
        sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        t_col.addWidget(title)
        t_col.addWidget(sub)
        h_lay.addLayout(t_col, 1)

        btn_import = Button(t("btn_import_json"), variant="primary", icon_name="folder")
        btn_import.clicked.connect(self._import_macro)
        h_lay.addWidget(btn_import)
        root.addWidget(hdr)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        scroll.setStyleSheet("background: transparent;")
        self.container = QWidget()
        self.container.setStyleSheet("background: transparent;")
        self.list_layout = QVBoxLayout(self.container)
        self.list_layout.setContentsMargins(0, 0, 4, 0)
        self.list_layout.setSpacing(12)
        scroll.setWidget(self.container)
        root.addWidget(scroll, 1)

    def _refresh_list(self) -> None:
        while self.list_layout.count():
            it = self.list_layout.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.setParent(None)
                w.deleteLater()

        for m in self.state.saved_macros:
            card = GlassCard()
            c_lay = QHBoxLayout(card)
            c_lay.setContentsMargins(20, 14, 20, 14)
            c_lay.setSpacing(12)

            info = QVBoxLayout()
            nm = QLabel(m.get("name", "Macro"))
            nm.setFont(make_font(16, QFont.Weight.Bold))
            nm.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
            ev_count = len(m.get("events", []))
            meta = QLabel(
                tl(
                    f"عدد الأحداث: {ev_count}  •  تاريخ الحفظ: {ltr_value(m.get('created_at', ''))}",
                    f"Events: {ev_count}  •  Saved: {ltr_value(m.get('created_at', ''))}",
                )
            )
            meta.setFont(make_font(12, QFont.Weight.Medium))
            meta.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            info.addWidget(nm)
            info.addWidget(meta)
            c_lay.addLayout(info, 1)

            b_play = Button(t("btn_play_macro"), variant="primary", icon_name="play")
            b_play.clicked.connect(lambda md=m: self.engine.start_playback(events=md.get("events", [])))

            b_load = Button(t("btn_load_to_editor"), variant="glass", icon_name="recorder")
            b_load.clicked.connect(lambda md=m: self._load_into_recorder(md))

            b_exp = Button(t("btn_export_json"), variant="ghost", icon_name="save")
            b_exp.clicked.connect(lambda md=m: self._export_macro(md))

            b_del = Button(t("btn_delete_combo"), variant="danger", icon_name="trash")
            b_del.setFixedWidth(90)
            b_del.clicked.connect(lambda mid=m.get("id", ""): self.engine.delete_saved_macro(mid))

            c_lay.addWidget(b_play)
            c_lay.addWidget(b_load)
            c_lay.addWidget(b_exp)
            c_lay.addWidget(b_del)
            self.list_layout.addWidget(card)

        self.list_layout.addStretch(1)

    def _load_into_recorder(self, m: Dict[str, Any]) -> None:
        self.state.current_macro_name = m.get("name", "Macro")
        self.state.recorded_events = [dict(e) for e in m.get("events", [])]
        self.state.events_updated.emit()
        self.state.navigation_requested.emit("recorder")
        NOTIFIER.toast(tl("تم تحميل الماكرو في المسجل للتعديل", "Loaded macro into Recorder"), "ok")

    def _export_macro(self, m: Dict[str, Any]) -> None:
        path, _ = QFileDialog.getSaveFileName(
            self,
            t("btn_export_json"),
            f"{m.get('name', 'macro')}.json",
            "JSON Files (*.json)",
        )
        if path:
            Path(path).write_text(json.dumps(m, ensure_ascii=False, indent=2), encoding="utf-8")
            NOTIFIER.toast(tl("تم تصدير ملف الماكرو بنجاح", "Macro exported successfully"), "ok")

    def _import_macro(self) -> None:
        path, _ = QFileDialog.getOpenFileName(
            self,
            t("btn_import_json"),
            "",
            "JSON Files (*.json)",
        )
        if path:
            try:
                data = json.loads(Path(path).read_text(encoding="utf-8"))
                if "events" in data:
                    self.state.recorded_events = data["events"]
                    self.engine.save_current_macro_to_library(data.get("name", Path(path).stem))
            except Exception as exc:
                NOTIFIER.toast(f"Import Error: {exc}", "danger")
