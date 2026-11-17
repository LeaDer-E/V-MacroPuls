#pragma once
#include <QJsonObject>
#include <QList>
#include <QPair>
#include <QString>
#include <atomic>
#include <functional>

namespace vmacro {

struct PythonPresetInfo {
    QString key;
    QString name;
    QString code;
    bool isUserSaved = false;
};

class PythonScriptRunner {
public:
    static QList<PythonPresetInfo> builtinPresets();
    static QList<PythonPresetInfo> loadUserScripts();
    static QList<QPair<QString, QString>> dropdownItems();
    static QString codeByKey(const QString& key);
    static QString nameByKey(const QString& key);
    static QPair<QString, QString> upsertUserScript(const QString& name, const QString& code, const QString& existingKey = QString());
    static bool deleteUserScript(const QString& key);

    using ActionCallback = std::function<bool(const QString& cmd, const QJsonObject& args, QString& outReason)>;

    static bool executeScript(
        const QString& userCode,
        int methodNum,
        const QString& methodId,
        const QString& methodName,
        const QString& category,
        const QString& targetApp,
        quintptr targetHwnd,
        const QString& mode,
        ActionCallback onAction,
        std::atomic<bool>* stopFlag,
        QString& outReason
    );
};

} // namespace vmacro
