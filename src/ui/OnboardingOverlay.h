#pragma once
#include <QPointer>
#include <QLabel>
#include <QLabel>
#include <QMap>
#include <QWidget>

namespace vmacro {

class AppState;
class Button;
class GlassCard;

class OnboardingOverlay : public QWidget {
    Q_OBJECT
public:
    explicit OnboardingOverlay(AppState* state, QWidget* parent = nullptr);
    void refreshTexts();
    void applyTheme() { refreshTexts(); }

signals:
    void finished();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void buildUi();
    void onLangChanged(const QString& lang);
    void onFinish();

    AppState* m_state = nullptr;
    QWidget* m_card = nullptr;
    QLabel* m_logoLbl = nullptr;
    QLabel* m_lblTitle = nullptr;
    GlassCard* m_summaryCard = nullptr;
    QLabel* m_lblSteps = nullptr;
    QLabel* m_lblPickLang = nullptr;
    Button* m_btnStart = nullptr;
    QMap<QString, Button*> m_langButtons;
};

} // namespace vmacro
