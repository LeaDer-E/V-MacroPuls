#pragma once
#include <QColor>
#include <QLabel>
#include <QPointer>
#include <QFont>
#include <QHBoxLayout>
#include <QLayout>
#include <QLineEdit>
#include <QList>
#include <QPair>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPoint>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QStackedWidget>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>

namespace vmacro {

class AppState;

QFont makeFont(int sizePx = 14, QFont::Weight weight = QFont::Medium);
int alignFlag(bool preferStart = true, bool vcenter = true);
QString modernScrollStyle();
QString framedHorizontalScrollStyle(const QString& tintHex = QString());
QPixmap getExeIconPixmap(const QString& exePath, int size = 20);
void refreshWidgetTreeTheme(QWidget* root);

struct SearchComboItem {
    QString key;
    QString label;
    QPixmap pixmap;
    QString tintHex;

    SearchComboItem() = default;
    SearchComboItem(const QString& k, const QString& l, const QPixmap& p = QPixmap(), const QString& tint = QString())
        : key(k), label(l), pixmap(p), tintHex(tint) {}
    SearchComboItem(const QString& k, const QString& l, const QString& exePath, const QString& tint = QString())
        : key(k), label(l), pixmap(getExeIconPixmap(exePath, 20)), tintHex(tint) {}
};

class TintedScrollBox : public QWidget {
    Q_OBJECT
public:
    explicit TintedScrollBox(const QString& tintHex = QString(), QWidget* parent = nullptr);
    explicit TintedScrollBox(QWidget* parent);
    void setTintHex(const QString& tintHex);
    QString tintHex() const { return m_tintHex; }
    void applyTheme() { update(); }
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QString m_tintHex;
};

class DragGripHandle : public QWidget {
    Q_OBJECT
public:
    explicit DragGripHandle(QWidget* parent = nullptr);
signals:
    void dragStarted();
    void dragMoved(int globalY);
    void dragFinished();
protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
private:
    bool m_hover = false;
    bool m_dragging = false;
};

class Button : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double hoverProgress READ hoverProgress WRITE setHoverProgress)
public:
    explicit Button(const QString& text = QString(),
                    const QString& variant = "primary",
                    const QString& iconName = QString(),
                    int height = 38,
                    QWidget* parent = nullptr);
    explicit Button(const QString& text,
                    const QString& variant,
                    const QString& iconName,
                    const QString& tintHex,
                    bool fixedCompact = false,
                    QWidget* parent = nullptr);

    void setFixedHeight(int h);
    int naturalCompactWidth() const;

    double hoverProgress() const { return m_hover; }
    void setHoverProgress(double val);

    void setText(const QString& text);
    QString text() const { return m_text; }

    void setVariant(const QString& variant);
    QString variant() const { return m_variant; }

    void setIconName(const QString& iconName);
    QString iconName() const { return m_iconName; }

    void setAppPixmap(const QPixmap& pix);
    void setTintHex(const QString& tintHex);
    QString tintHex() const { return m_tintHex; }

    void setFixedCompact(bool compact);
    void setForceLtrText(bool force) { m_forceLtrText = force; update(); }
    void setBusy(bool busy);
    void applyTheme() { update(); }

    QSize sizeHint() const override;

signals:
    void clicked();

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void recalcMinWidth();
    void tickSpin();

    QString m_text;
    QString m_variant = "primary";
    QString m_iconName;
    QPixmap m_appPixmap;
    bool m_fixedCompact = false;
    bool m_forceLtrText = false;
    QString m_tintHex;
    double m_hover = 0.0;
    bool m_pressed = false;
    bool m_busy = false;
    int m_spinAngle = 0;
    QPropertyAnimation* m_anim = nullptr;
    QTimer* m_spinTimer = nullptr;
};

class ResponsiveFlowBar : public QWidget {
    Q_OBJECT
public:
    explicit ResponsiveFlowBar(const QList<Button*>& buttons,
                               int rowHeight = 32,
                               int hSpacing = 6,
                               int vSpacing = 6,
                               QWidget* parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void forceRelayout();
protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
private:
    void relayout(int availW);
    QList<Button*> m_buttons;
    int m_rowH = 32;
    int m_hSpace = 6;
    int m_vSpace = 6;
    int m_lastAvailW = -1;
    int m_lastRtl = -1;
};

class IconButton : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double hoverProgress READ hoverProgress WRITE setHoverProgress)
public:
    explicit IconButton(const QString& iconName,
                        const QString& tooltip = QString(),
                        int size = 34,
                        const QString& variant = "glass",
                        QWidget* parent = nullptr);
    explicit IconButton(const QString& iconName,
                        int size,
                        const QString& variant = "glass",
                        const QString& tooltip = QString(),
                        QWidget* parent = nullptr);

    double hoverProgress() const { return m_hover; }
    void setHoverProgress(double val);

    void setIconName(const QString& name);
    void setVariant(const QString& variant);
    void applyTheme() { update(); }

signals:
    void clicked();

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_iconName;
    QString m_variant = "glass";
    double m_hover = 0.0;
    bool m_pressed = false;
    QPropertyAnimation* m_anim = nullptr;
};

class ToggleSwitch : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double thumbPos READ thumbPos WRITE setThumbPos)
public:
    explicit ToggleSwitch(bool checked = false, QWidget* parent = nullptr);
    bool isChecked() const { return m_checked; }
    void setChecked(bool checked, bool emitSignal = false);
    double thumbPos() const { return m_pos; }
    void setThumbPos(double val);
    void applyTheme() { update(); }

signals:
    void toggled(bool checked);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_checked = false;
    double m_pos = 0.0;
    QPropertyAnimation* m_anim = nullptr;
};

class SegmentedControl : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double thumbIndex READ thumbIndex WRITE setThumbIndex)
public:
    explicit SegmentedControl(const QList<QPair<QString, QString>>& items,
                              const QString& selectedKey = QString(),
                              QWidget* parent = nullptr);
    double thumbIndex() const { return m_thumbIdx; }
    void setThumbIndex(double val);

    void setItems(const QList<QPair<QString, QString>>& items, const QString& selectedKey = QString());
    QString currentKey() const;
    void setCurrentKey(const QString& key) { setSelectedKey(key, false); }
    void setSelectedKey(const QString& key, bool emitSignal = false);
    void applyTheme() { update(); }

signals:
    void changed(const QString& key);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void recalcMinWidth();
    QList<QPair<QString, QString>> m_items;
    int m_selectedIdx = 0;
    double m_thumbIdx = 0.0;
    QPropertyAnimation* m_anim = nullptr;
};

class NumberInput : public QWidget {
    Q_OBJECT
public:
    explicit NumberInput(double value = 1.0,
                         double minVal = 0.0,
                         double maxVal = 999999.0,
                         double step = 1.0,
                         const QString& suffix = QString(),
                         int width = 130,
                         QWidget* parent = nullptr);
    explicit NumberInput(double value,
                         double minVal,
                         double maxVal,
                         double step,
                         int decimals,
                         const QString& suffix = QString(),
                         QWidget* parent = nullptr);

    double doubleValue() const { return m_val; }
    int value() const { return static_cast<int>(qRound(m_val)); }
    void setValue(double val, bool emitSignal = false);
    void setSuffix(const QString& suffix);
    void applyTheme() { update(); }

signals:
    void valueChanged(int val);
    void doubleValueChanged(double val);
    void focused();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void startInlineEdit();
    void commitInlineEdit();

    double m_val = 1.0;
    double m_min = 0.0;
    double m_max = 999999.0;
    double m_step = 1.0;
    int m_decimals = 0;
    QString m_suffix;
    int m_hoverPart = 0;
    QLineEdit* m_inlineEdit = nullptr;
};

class SearchPopup : public QWidget {
    Q_OBJECT
public:
    explicit SearchPopup(QWidget* parent = nullptr);
    void populate(const QList<SearchComboItem>& items);
    QLineEdit* searchEdit() const { return m_searchEdit; }

signals:
    void itemSelected(const QString& key, const QString& label);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void onFilter(const QString& q);
    void rebuildRows(const QList<SearchComboItem>& rows);
    void pick(const QString& k, const QString& lbl);

    QList<SearchComboItem> m_items;
    QLineEdit* m_searchEdit = nullptr;
    QScrollArea* m_scroll = nullptr;
    QWidget* m_listContainer = nullptr;
    QVBoxLayout* m_listLayout = nullptr;
};

class SearchCombo : public QWidget {
    Q_OBJECT
public:
    explicit SearchCombo(QWidget* parent = nullptr);
    explicit SearchCombo(const QList<QPair<QString, QString>>& items,
                         const QString& selectedKey = QString(),
                         QWidget* parent = nullptr);
    

    void setItems(const QList<QPair<QString, QString>>& items, const QString& selectedKey = QString());
    void setRichItems(const QList<SearchComboItem>& items, const QString& selectedKey = QString());

    QString currentKey() const { return m_selectedKey; }
    QString currentLabel() const;
    QPixmap currentPixmap() const;
    QString currentTintHex() const;

    void setCurrentKey(const QString& key) { setSelectedKey(key, false); }
    void setSelectedKey(const QString& key, bool emitSignal = false);
    void applyTheme() { update(); }

signals:
    void currentChanged(const QString& key);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void onSelected(const QString& key, const QString& lbl);

    QList<SearchComboItem> m_items;
    QString m_selectedKey;
    bool m_hover = false;
    SearchPopup* m_popup = nullptr;
};

class KeyCaptureButton : public QWidget {
    Q_OBJECT
public:
    explicit KeyCaptureButton(AppState* state,
                              const QString& initialKey = QString(),
                              bool allowChord = true,
                              QWidget* parent = nullptr);
    explicit KeyCaptureButton(const QString& currentKey,
                              int scanCode = 0,
                              bool isExtended = false,
                              const QList<QPair<int, bool>>& chord = {},
                              bool allowChord = true,
                              AppState* state = nullptr,
                              bool largeHero = false,
                              QWidget* parent = nullptr);

    QString keyString() const { return m_keyStr; }
    QString keyText() const { return m_keyStr; }
    int scanCode() const;
    bool isExtended() const;
    QList<QPair<int, bool>> chordData() const { return m_chord; }

    void clearKey();
    void setChordData(const QList<QPair<int, bool>>& chord, const QString& label = QString());
    void setKeyData(const QString& keyStr, int scanCode = 0, bool isExtended = false, const QList<QPair<int, bool>>& chord = {});
    void setKeyString(const QString& k) { setKeyData(k); }
    void setKeyText(const QString& k, int scan = 0, bool ext = false) { setKeyData(k, scan, ext); }
    void setLargeHero(bool largeHero);
    void applyTheme() { update(); }

    static QString formatChordLabel(const QList<QPair<int, bool>>& chord);

signals:
    void keyCaptured(const QString& keyText, int scanCode, bool isExtended);
    void keyStringCaptured(const QString& keyText);
    void scanKeyCaptured(int scanCode, bool isExtended, const QString& keyText);
    void chordCaptured(const QList<QPair<int, bool>>& chord, const QString& keyText);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void startCapture();
    void finalizeCapture();
    void onHookKeyEvent(int scanCode, bool isExtended, bool isDown, const QString& label);

    AppState* m_state = nullptr;
    bool m_largeHero = false;
    bool m_allowChord = true;
    bool m_listening = false;
    bool m_hover = false;
    QList<QPair<int, bool>> m_chord;
    QList<QPair<int, bool>> m_pendingChord;
    int m_heldCount = 0;
    QString m_keyStr;
};

class StepCard : public QWidget {
    Q_OBJECT
public:
    explicit StepCard(int index, const QString& accentHex = "#8B5CF6", QWidget* parent = nullptr);
    explicit StepCard(const QString& tintHex, bool enabled = true, QWidget* parent = nullptr);

    void setIndex(int newIndex);
    int index() const { return m_index; }
    void setDragging(bool dragging);
    void setStepEnabled(bool enabled);
    void setAccentHex(const QString& accentHex);
    void setTint(const QString& tintHex, bool enabled = true);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_index = 0;
    QString m_accentHex = "#8B5CF6";
    bool m_dragging = false;
    bool m_stepEnabled = true;
};

class GlassCard : public QWidget {
    Q_OBJECT
public:
    explicit GlassCard(bool accentTint = false, QWidget* parent = nullptr);
    explicit GlassCard(const QString& variant, QWidget* parent = nullptr);
    void setAccentTint(bool accentTint);
    void setVariant(const QString& variant);
    void applyTheme() { update(); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_accentTint = false;
};

class ToastOverlay : public QWidget {
    Q_OBJECT
public:
    explicit ToastOverlay(QWidget* parent = nullptr);
    void showToast(const QString& message, const QString& kind = "info", int durationMs = 2400);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_msg;
    QString m_kind = "info";
    QTimer* m_timer = nullptr;
};

class SlideStack : public QStackedWidget {
    Q_OBJECT
public:
    explicit SlideStack(QWidget* parent = nullptr);
    void setCurrentIndexSmooth(int idx);
};

class StepEnableToggleButton : public QWidget {
    Q_OBJECT
public:
    explicit StepEnableToggleButton(bool enabled = true, QWidget* parent = nullptr);
    bool isStepEnabled() const { return m_enabled; }
    void setStepEnabled(bool enabled, bool emitSignal = false);
    void applyTheme() { update(); }

signals:
    void toggled(bool enabled);
    void stepToggled(bool enabled);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_enabled = true;
    bool m_hover = false;
};

class StepNoteHoverCard : public QWidget {
    Q_OBJECT
public:
    explicit StepNoteHoverCard(QWidget* parent = nullptr);
    void showNoteNear(QWidget* anchorWidget, const QString& noteText);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_noteText;
    QLabel* m_lblTitle = nullptr;
    QLabel* m_lblBody = nullptr;
};

class StepNoteEditorPopup : public QWidget {
    Q_OBJECT
public:
    explicit StepNoteEditorPopup(const QString& initialNote = QString(), QWidget* parent = nullptr);
    QPlainTextEdit* editor() const { return m_edit; }

signals:
    void noteSaved(const QString& note);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void syncNoteEditDirection();
    void onSave();
    void onClear();

    QPlainTextEdit* m_edit = nullptr;
    Button* m_btnSave = nullptr;
    Button* m_btnClear = nullptr;
};

class StepNoteIconButton : public QWidget {
    Q_OBJECT
public:
    explicit StepNoteIconButton(const QString& noteText = QString(), QWidget* parent = nullptr);
    ~StepNoteIconButton() override;
    QString noteText() const { return m_note; }
    void setNoteText(const QString& noteText, bool emitSignal = false);
    void applyTheme() { update(); }

signals:
    void noteChanged(const QString& newNote);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void showHoverCardNow();
    void openEditorPopup();

    QString m_note;
    bool m_hover = false;
    StepNoteHoverCard* m_hoverCard = nullptr;
    StepNoteEditorPopup* m_editorPopup = nullptr;
};

class FlowLayout : public QLayout {
public:
    explicit FlowLayout(QWidget* parent = nullptr, int margin = 0, int hSpacing = 6, int vSpacing = 6);
    ~FlowLayout() override;
    void addItem(QLayoutItem* item) override;
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect& rect) override;
    QSize sizeHint() const override;
    QLayoutItem* takeAt(int index) override;
private:
    int doLayout(const QRect& rect, bool testOnly) const;
    QList<QLayoutItem*> m_itemList;
    int m_hSpace;
    int m_vSpace;
};

int getRowLogicalY(QWidget* w);

bool smoothReorderVBoxWidget(QVBoxLayout* vbox,
                             QList<QWidget*>& rowWidgets,
                             int fromIdx,
                             int toIdx,
                             int durationMs = 240);

void smoothReorderVBoxWidget(QVBoxLayout* vbox,
                             QWidget* container,
                             int fromIdx,
                             int toIdx,
                             std::function<void()> onDone = nullptr);

} // namespace vmacro
