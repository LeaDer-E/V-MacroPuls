# -*- coding: utf-8 -*-
"""Dashboard Page: Hero V-Logo banner, 4 live KPI metric cards, quick actions, and live activity feed."""

from __future__ import annotations
from PySide6.QtCore import QRectF, Qt
from PySide6.QtGui import QFont, QPainter, QPen
from PySide6.QtWidgets import (
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QScrollArea,
    QVBoxLayout,
    QWidget,
)
from app.domain.macro_engine import MacroEngine
from app.i18n import bidi_text, is_rtl, ltr_value, t, tl
from app.state import AppState
from app.theme import T
from app.ui.icons import draw_icon, logo_pixmap
from app.ui.widgets import Button, GlassCard, ToggleSwitch, align_flag, make_font


class _StatCard(QWidget):
    """Custom QPainter Glass KPI Metric Card."""

    def __init__(self, icon_name: str, label_key: str, value_str: str, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self._icon = icon_name
        self._label_key = label_key
        self._val = value_str
        self.setMinimumHeight(104)

    def setValue(self, val: str) -> None:
        self._val = val
        self.update()

    def paintEvent(self, event) -> None:
        p = QPainter(self)
        p.setRenderHints(QPainter.RenderHint.Antialiasing | QPainter.RenderHint.TextAntialiasing)
        r = QRectF(self.rect()).adjusted(1, 1, -1, -1)

        p.setBrush(T.c("glass"))
        p.setPen(QPen(T.c("border"), 1.0))
        p.drawRoundedRect(r, 20.0, 20.0)

        # Icon badge
        ib = (
            QRectF(r.right() - 56, r.top() + 18, 38, 38)
            if is_rtl()
            else QRectF(r.left() + 18, r.top() + 18, 38, 38)
        )
        p.setBrush(T.c("accent", 36))
        p.setPen(QPen(T.c("accent_hi", 85), 1.0))
        p.drawRoundedRect(ib, 12.0, 12.0)
        draw_icon(p, self._icon, ib.adjusted(8, 8, -8, -8), color=T.c("accent_hi"), stroke_width=2.0)

        # Value & Label
        txt_r = (
            QRectF(r.left() + 16, r.top() + 16, r.width() - 82, 36)
            if is_rtl()
            else QRectF(r.left() + 68, r.top() + 16, r.width() - 82, 36)
        )
        p.setFont(make_font(24, QFont.Weight.ExtraBold))
        p.setPen(T.c("text"))
        p.drawText(txt_r, align_flag(prefer_start=True), ltr_value(self._val))

        sub_r = (
            QRectF(r.left() + 16, r.top() + 54, r.width() - 82, 28)
            if is_rtl()
            else QRectF(r.left() + 68, r.top() + 54, r.width() - 82, 28)
        )
        p.setFont(make_font(13, QFont.Weight.DemiBold))
        p.setPen(T.c("text2"))
        p.drawText(sub_r, align_flag(prefer_start=True), bidi_text(t(self._label_key)))
        p.end()


class DashboardPage(QWidget):
    """Main Dashboard overview with live KPIs, quick action triggers, and real-time activity feed."""

    def __init__(self, state: AppState, engine: MacroEngine, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self.state = state
        self.engine = engine
        self._build_ui()
        self._connect_signals()
        self._refresh_stats()
        self._refresh_feed()

    def _build_ui(self) -> None:
        root = QVBoxLayout(self)
        root.setContentsMargins(24, 16, 24, 20)
        root.setSpacing(16)

        # 1. Hero Banner Card
        hero = GlassCard(accent_tint=True)
        h_lay = QHBoxLayout(hero)
        h_lay.setContentsMargins(24, 20, 24, 20)
        h_lay.setSpacing(18)

        logo_lbl = QLabel()
        logo_lbl.setPixmap(logo_pixmap(64, 2.0))
        h_lay.addWidget(logo_lbl)

        h_text = QVBoxLayout()
        h_text.setSpacing(4)
        welcome = QLabel(t("dash_welcome"))
        welcome.setFont(make_font(22, QFont.Weight.ExtraBold))
        welcome.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")
        sub = QLabel(t("dash_subtitle"))
        sub.setFont(make_font(13, QFont.Weight.Medium))
        sub.setWordWrap(True)
        sub.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
        h_text.addWidget(welcome)
        h_text.addWidget(sub)
        h_lay.addLayout(h_text, 1)

        # Quick Actions in Hero
        act_col = QHBoxLayout()
        act_col.setSpacing(10)
        btn_rec = Button(t("btn_new_recording"), variant="primary", icon_name="record")
        btn_rec.clicked.connect(lambda: self.state.navigation_requested.emit("recorder"))
        btn_combo = Button(t("btn_new_combo"), variant="glass", icon_name="bolt")
        btn_combo.clicked.connect(lambda: self.state.navigation_requested.emit("combos"))
        btn_stop = Button(t("btn_panic_stop"), variant="danger", icon_name="stop")
        btn_stop.clicked.connect(self.engine.stop_all)
        act_col.addWidget(btn_rec)
        act_col.addWidget(btn_combo)
        act_col.addWidget(btn_stop)
        h_lay.addLayout(act_col)
        root.addWidget(hero)

        # 2. Four KPI Metric Cards
        kpi_grid = QGridLayout()
        kpi_grid.setSpacing(14)
        self.card_macros = _StatCard("library", "stat_saved_macros", "0")
        self.card_combos = _StatCard("bolt", "stat_active_combos", "0")
        self.card_runs = _StatCard("play", "stat_total_runs", "0")
        self.card_inputs = _StatCard("keyboard", "stat_simulated_events", "0")
        kpi_grid.addWidget(self.card_macros, 0, 0)
        kpi_grid.addWidget(self.card_combos, 0, 1)
        kpi_grid.addWidget(self.card_runs, 0, 2)
        kpi_grid.addWidget(self.card_inputs, 0, 3)
        root.addLayout(kpi_grid)

        # 3. Bottom Two-Column Split: Active Smart Combos Quick-Trigger + Live Activity Feed
        bot_row = QHBoxLayout()
        bot_row.setSpacing(16)

        # Left/Start: Active Smart Key Combos Quick Panel
        combos_card = GlassCard()
        cc_lay = QVBoxLayout(combos_card)
        cc_lay.setContentsMargins(20, 18, 20, 18)
        cc_lay.setSpacing(12)

        cc_hdr = QHBoxLayout()
        cc_title = QLabel(t("nav_combos"))
        cc_title.setFont(make_font(16, QFont.Weight.Bold))
        cc_title.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        self.master_tog = ToggleSwitch(self.state.combos_active)
        self.master_tog.toggled.connect(lambda _: self.engine.toggle_combos_master())
        cc_hdr.addWidget(cc_title, 1)
        cc_hdr.addWidget(self.master_tog)
        cc_lay.addLayout(cc_hdr)

        self.quick_combos_box = QVBoxLayout()
        self.quick_combos_box.setSpacing(8)
        cc_lay.addLayout(self.quick_combos_box)
        cc_lay.addStretch(1)
        bot_row.addWidget(combos_card, 5)

        # Right/End: Live Activity Log Card
        feed_card = GlassCard()
        f_lay = QVBoxLayout(feed_card)
        f_lay.setContentsMargins(20, 18, 20, 18)
        f_lay.setSpacing(10)

        f_title = QLabel(t("recent_activity"))
        f_title.setFont(make_font(16, QFont.Weight.Bold))
        f_title.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")
        f_lay.addWidget(f_title)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QScrollArea.Shape.NoFrame)
        scroll.setStyleSheet("background: transparent;")
        self.feed_container = QWidget()
        self.feed_container.setStyleSheet("background: transparent;")
        self.feed_layout = QVBoxLayout(self.feed_container)
        self.feed_layout.setContentsMargins(0, 0, 0, 0)
        self.feed_layout.setSpacing(6)
        scroll.setWidget(self.feed_container)
        f_lay.addWidget(scroll, 1)
        bot_row.addWidget(feed_card, 5)

        root.addLayout(bot_row, 1)

    def _connect_signals(self) -> None:
        self.state.macros_library_updated.connect(self._refresh_stats)
        self.state.combos_list_updated.connect(self._refresh_stats)
        self.state.live_counter_updated.connect(lambda *_: self._refresh_stats())
        self.state.activity_logged.connect(lambda *_: self._refresh_feed())
        self.state.combos_master_changed.connect(lambda v: self.master_tog.setChecked(v, emit_signal=False))

    def _refresh_stats(self) -> None:
        self.card_macros.setValue(str(len(self.state.saved_macros)))
        active_c = sum(1 for c in self.state.smart_combos if c.get("enabled", True))
        self.card_combos.setValue(str(active_c))
        self.card_runs.setValue(str(self.state.config.total_executions))
        self.card_inputs.setValue(str(self.state.config.total_keys_simulated))

        # Rebuild quick combos list
        while self.quick_combos_box.count():
            it = self.quick_combos_box.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.setParent(None)
                w.deleteLater()

        for c in self.state.smart_combos[:4]:
            row_w = QWidget()
            r_lay = QHBoxLayout(row_w)
            r_lay.setContentsMargins(10, 6, 10, 6)
            r_lay.setSpacing(10)
            badge = QLabel(ltr_value(f"[{c.get('trigger_key', 'Ctrl')}]"))
            badge.setFont(make_font(13, QFont.Weight.ExtraBold))
            badge.setStyleSheet(f"color: {T.token_hex('accent_hi')}; background: transparent;")

            nm = QLabel(c.get("name", ""))
            nm.setFont(make_font(13, QFont.Weight.DemiBold))
            nm.setStyleSheet(f"color: {T.token_hex('text')}; background: transparent;")

            run_btn = Button(tl("تشغيل", "Run"), variant="ghost", icon_name="play")
            run_btn.setFixedHeight(30)
            run_btn.setFixedWidth(86)
            run_btn.clicked.connect(lambda cd=c: self.engine.execute_smart_combo(cd))

            r_lay.addWidget(badge)
            r_lay.addWidget(nm, 1)
            r_lay.addWidget(run_btn)
            self.quick_combos_box.addWidget(row_w)

    def _refresh_feed(self) -> None:
        while self.feed_layout.count():
            it = self.feed_layout.takeAt(0)
            w = it.widget()
            if w is not None:
                w.hide()
                w.setParent(None)
                w.deleteLater()

        for ts, msg in self.state.activity_log[:12]:
            lbl = QLabel(f"{ltr_value(ts)}  •  {msg}")
            lbl.setFont(make_font(12, QFont.Weight.Medium))
            lbl.setStyleSheet(f"color: {T.token_hex('text2')}; background: transparent;")
            self.feed_layout.addWidget(lbl)
        self.feed_layout.addStretch(1)
