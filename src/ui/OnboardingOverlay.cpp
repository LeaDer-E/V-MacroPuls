#include "OnboardingOverlay.h"
#include "Icons.h"
#include "Widgets.h"
#include "../core/AppState.h"
#include "../core/I18n.h"
#include "../core/Theme.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPen>
#include <QSizePolicy>
#include <QVBoxLayout>

namespace vmacro {

OnboardingOverlay::OnboardingOverlay(AppState* state, QWidget* parent)
    : QWidget(parent), m_state(state) {
    buildUi();
}

void OnboardingOverlay::buildUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 20, 24, 20);
    root->setAlignment(Qt::AlignCenter);

    m_card = new QWidget(this);
    m_card->setMinimumSize(760, 490);
    m_card->setMaximumSize(820, 540);
    m_card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    auto* cLay = new QVBoxLayout(m_card);
    cLay->setContentsMargins(32, 22, 32, 22);
    cLay->setSpacing(12);

    auto* hdr = new QHBoxLayout();
    hdr->setAlignment(Qt::AlignCenter);
    m_logoLbl = new QLabel(m_card);
    m_logoLbl->setPixmap(Icons::logoPixmap(56, 2.0));
    hdr->addWidget(m_logoLbl);
    cLay->addLayout(hdr);

    m_lblTitle = new QLabel(I18n::t("onb_title"), m_card);
    m_lblTitle->setFont(makeFont(17, QFont::ExtraBold));
    m_lblTitle->setAlignment(Qt::AlignCenter);
    m_lblTitle->setWordWrap(true);
    m_lblTitle->setMinimumHeight(46);
    m_lblTitle->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));
    cLay->addWidget(m_lblTitle);

    m_summaryCard = new GlassCard(true, m_card);
    m_summaryCard->setMinimumHeight(64);
    auto* sLay = new QVBoxLayout(m_summaryCard);
    sLay->setContentsMargins(18, 10, 18, 10);
    m_lblSteps = new QLabel(I18n::t("onb_step_summary"), m_summaryCard);
    m_lblSteps->setFont(makeFont(12, QFont::Bold));
    m_lblSteps->setAlignment(Qt::AlignCenter);
    m_lblSteps->setWordWrap(true);
    m_lblSteps->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));
    sLay->addWidget(m_lblSteps);
    cLay->addWidget(m_summaryCard);

    m_lblPickLang = new QLabel(I18n::t("onb_pick_lang"), m_card);
    m_lblPickLang->setFont(makeFont(13, QFont::Bold));
    m_lblPickLang->setAlignment(Qt::AlignCenter);
    m_lblPickLang->setFixedHeight(24);
    m_lblPickLang->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));
    cLay->addWidget(m_lblPickLang);

    auto* gridW = new QWidget(m_card);
    gridW->setMinimumHeight(92);
    auto* grid = new QGridLayout(gridW);
    grid->setContentsMargins(4, 2, 4, 2);
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);

    const QString curLang = I18n::language();
    const auto langs = I18n::supportedLanguages();
    for (int idx = 0; idx < langs.size(); ++idx) {
        const auto& info = langs[idx];
        auto* btn = new Button(info.name, info.code == curLang ? "primary" : "glass", QString(), 38, gridW);
        btn->setMinimumWidth(132);
        const QString code = info.code;
        connect(btn, &Button::clicked, this, [this, code]() { onLangChanged(code); });
        const int rowI = idx / 5;
        const int colI = idx % 5;
        grid->addWidget(btn, rowI, colI);
        grid->setColumnStretch(colI, 1);
        m_langButtons.insert(code, btn);
    }

    cLay->addWidget(gridW);
    cLay->addSpacing(4);

    auto* bot = new QHBoxLayout();
    bot->setAlignment(Qt::AlignCenter);
    m_btnStart = new Button(I18n::t("btn_finish"), "primary", "bolt", 44, m_card);
    m_btnStart->setFixedSize(220, 44);
    connect(m_btnStart, &Button::clicked, this, &OnboardingOverlay::onFinish);
    bot->addWidget(m_btnStart);
    cLay->addLayout(bot);

    root->addWidget(m_card);
}

void OnboardingOverlay::onLangChanged(const QString& lang) {
    if (m_state) m_state->setLanguage(lang);
    refreshTexts();
}

void OnboardingOverlay::refreshTexts() {
    const QString cur = I18n::language();
    m_lblTitle->setFont(makeFont(17, QFont::ExtraBold));
    m_lblTitle->setText(I18n::t("onb_title"));
    m_lblTitle->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text")));

    m_lblSteps->setFont(makeFont(12, QFont::Bold));
    m_lblSteps->setText(I18n::t("onb_step_summary"));
    m_lblSteps->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("accent_hi")));

    m_lblPickLang->setFont(makeFont(13, QFont::Bold));
    m_lblPickLang->setText(I18n::t("onb_pick_lang"));
    m_lblPickLang->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Theme::instance().tokenHex("text2")));

    m_btnStart->setText(I18n::t("btn_finish"));
    for (auto it = m_langButtons.begin(); it != m_langButtons.end(); ++it) {
        it.value()->setVariant(it.key() == cur ? "primary" : "glass");
    }
    m_card->adjustSize();
    update();
}

void OnboardingOverlay::onFinish() {
    if (m_state) m_state->updateSetting("first_run_completed", true);
    hide();
    emit finished();
}

void OnboardingOverlay::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), QColor(6, 9, 16, 195));
    QRectF cr(m_card->geometry());
    p.setBrush(Theme::instance().c("bg1"));
    p.setPen(QPen(Theme::instance().c("border_hi"), 1.5));
    p.drawRoundedRect(cr, 26.0, 26.0);
}

} // namespace vmacro
