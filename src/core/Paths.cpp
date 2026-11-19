#include "Paths.h"
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

namespace vmacro {

QString Paths::appDataDir() {
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (base.isEmpty()) base = QDir::homePath() + "/.v_macroplus";
    QDir d(base);
    if (!d.exists()) d.mkpath(".");
    return d.absolutePath();
}

QString Paths::configPath() { return appDataDir() + "/settings.json"; }
QString Paths::combosPath() { return appDataDir() + "/smart_combos.json"; }
QString Paths::pythonScriptsPath() { return appDataDir() + "/python_scripts.json"; }
QString Paths::autopresserWorkspacePath() { return appDataDir() + "/autopresser_workspace.json"; }

QString Paths::macrosDir() {
    QString p = appDataDir() + "/macros";
    QDir d(p);
    if (!d.exists()) d.mkpath(".");
    return d.absolutePath();
}

QString Paths::driversDir() {
    QString exeDir = QCoreApplication::applicationDirPath();
    QString local = exeDir + "/drivers";
    if (QDir(local).exists()) return local;
    QString appDrv = appDataDir() + "/drivers";
    QDir(appDrv).mkpath(".");
    return appDrv;
}

QString Paths::firmwareDir() {
    QString exeDir = QCoreApplication::applicationDirPath();
    QString local = exeDir + "/firmware";
    if (QDir(local).exists()) return local;
    QString appFw = appDataDir() + "/firmware";
    QDir(appFw).mkpath(".");
    return appFw;
}

QStringList Paths::driverSearchDirs() {
    QStringList out;
    QString exeDir = QCoreApplication::applicationDirPath();
    out << exeDir << (exeDir + "/drivers") << (exeDir + "/assets/drivers")
        << (appDataDir() + "/drivers") << QDir::currentPath() << (QDir::currentPath() + "/drivers");
    return out;
}

} // namespace vmacro
