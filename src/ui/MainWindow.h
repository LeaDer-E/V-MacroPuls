#pragma once
#include <QPointer>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QPoint>
#include <QRect>
#include <QRectF>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace vmacro {

class AppState;
class MacroEngine;
class IconButton;
class SearchPopup;
class SlideStack;
class ToastOverlay;
class AboutDialog;
class OnboardingOverlay;
class MainWindow;

class RecordingFloatingIndicator : public QWidget {
    Q_OBJECT
public:
    explicit RecordingFloatingIndicator(AppState* state);
    void showIndicator();
    void hideIndicator();
    void updateCount(int count);
    void recalcSizeAndPosition();

signals:
    void stopClicked();

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QString labelText() const;
    void togglePulse();

    AppState* m_state = nullptr;
    bool m_hovered = false;
    bool m_pulseOn = true;
    int m_eventCount = 0;
    QTimer* m_pulseTimer = nullptr;
};

class HeaderStatusAndAdminPill : public QWidget {
    Q_OBJECT
public:
    explicit HeaderStatusAndAdminPill(AppState* state, QWidget* parent = nullptr);
    void refresh();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void recalcWidth();
    AppState* m_state = nullptr;
    bool m_isAdmin = false;
};

class CenterTabsBar : public QWidget {
    Q_OBJECT
public:
    explicit CenterTabsBar(QWidget* parent = nullptr);
    void setActiveTab(const QString& tabId);

signals:
    void tabSelected(const QString& tabId);
    void aboutClicked();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    struct TabMeta {
        QString id;
        QString icon;
        QString trKey;
    };
    QList<QPair<QString, QRectF>> tabRects() const;

    QString m_activeTab = "combos";
    int m_hoverTab = -1;
    QList<TabMeta> m_tabs;
};

class TopHeaderBar : public QWidget {
    Q_OBJECT
public:
    explicit TopHeaderBar(AppState* state, QWidget* parent = nullptr);
    void setActiveTab(const QString& tabId);
    void refreshStatus();

signals:
    void tabSelected(const QString& tabId);
    void aboutClicked();
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void openLanguageDropPanel();

    AppState* m_state = nullptr;
    CenterTabsBar* m_tabsBar = nullptr;
    HeaderStatusAndAdminPill* m_statusPill = nullptr;
    IconButton* m_btnLang = nullptr;
    SearchPopup* m_langPopup = nullptr;
    IconButton* m_btnAbout = nullptr;
    IconButton* m_btnMin = nullptr;
    IconButton* m_btnMax = nullptr;
    IconButton* m_btnClose = nullptr;
};

class CornerResizeGrip : public QWidget {
    Q_OBJECT
public:
    explicit CornerResizeGrip(MainWindow* mainWin, bool isLeft = false);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    MainWindow* m_mainWin = nullptr;
    bool m_isLeft = false;
    bool m_dragging = false;
    QPoint m_dragStartGpos;
    QRect m_dragStartGeom;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    static constexpr int OUTER_MARGIN = 10;
    static constexpr int RESIZE_BORDER = 11;

    explicit MainWindow(AppState* state, MacroEngine* engine, QWidget* parent = nullptr);
    ~MainWindow() override;

    void navigateTo(const QString& pageId);
    void applyManualResizeDelta(const QPoint& delta, const QRect& startGeom, Qt::Edges edges);

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void presentOnboardingFull();
    void repositionCornerGrips();
    void onRecordingStateChanged(bool active);
    void onPlaybackStateChanged(bool active);
    void onFloatingStopClicked();
    void rebuildPages();
    void onThemeChanged();
    void onLanguageChanged();
    void showAbout();
    void toggleMaximize();
    void onCloseClicked();
    void quitApplication();
    void setupTray();
    void rebuildTrayMenu();
    void showAboutFromTray();
    void restoreFromTray();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void refreshStatusSurfaces();
    Qt::Edges hitTestEdges(const QPoint& pos) const;
    static bool cursorForEdges(Qt::Edges edges, Qt::CursorShape& outShape);

    AppState* m_state = nullptr;
    MacroEngine* m_engine = nullptr;

    bool m_manualResizing = false;
    Qt::Edges m_resizeEdges;
    QPoint m_resizeStartGpos;
    QRect m_resizeStartGeom;
    bool m_edgeCursorOverride = false;

    QWidget* m_central = nullptr;
    QVBoxLayout* m_rootLayout = nullptr;
    TopHeaderBar* m_headerBar = nullptr;
    SlideStack* m_pageStack = nullptr;
    ToastOverlay* m_toast = nullptr;
    AboutDialog* m_aboutOverlay = nullptr;
    OnboardingOverlay* m_onboarding = nullptr;
    CornerResizeGrip* m_gripBr = nullptr;
    CornerResizeGrip* m_gripBl = nullptr;
    RecordingFloatingIndicator* m_recFloatingPill = nullptr;
    QSystemTrayIcon* m_tray = nullptr;
    QMenu* m_trayMenu = nullptr;
    QMap<QString, int> m_pageMap;
};

} // namespace vmacro
