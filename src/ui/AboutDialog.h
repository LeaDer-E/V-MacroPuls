#pragma once
#include <QPointer>
#include <QLabel>
#include <QLabel>
#include <QWidget>

namespace vmacro {

class IconButton;

class AboutDialog : public QWidget {
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = nullptr);
    void showModal();
    void refreshStyles();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    QWidget* m_card = nullptr;
    IconButton* m_closeBtn = nullptr;
    QLabel* m_lblAppName = nullptr;
    QLabel* m_lblSubVer = nullptr;
    QLabel* m_lblDesc = nullptr;
    QLabel* m_lblTech = nullptr;
    QLabel* m_lblBadge = nullptr;
    QLabel* m_lblDevHdr = nullptr;
    QLabel* m_lblDevName = nullptr;
    QWidget* m_pillEmail = nullptr;
    QWidget* m_pillPhone = nullptr;
    QLabel* m_lblArchFooter = nullptr;
    QLabel* m_lblAssisted = nullptr;
    QLabel* m_lblCare = nullptr;
};

} // namespace vmacro
