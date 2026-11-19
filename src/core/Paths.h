#pragma once
#include <QString>
#include <QStringList>

namespace vmacro {

class Paths {
public:
    static QString appDataDir();
    static QString dataDir() { return appDataDir(); }
    static void ensureDirectories() {
        appDataDir();
        macrosDir();
        driversDir();
        firmwareDir();
    }
    static QString configPath();
    static QString combosPath();
    static QString combosFile() { return combosPath(); }
    static QString macrosDir();
    static QString macrosFile() { return appDataDir() + QStringLiteral("/saved_macros.json"); }
    static QString pythonScriptsPath();
    static QString autopresserWorkspacePath();
    static QString driversDir();
    static QString firmwareDir();
    static QStringList driverSearchDirs();
};

} // namespace vmacro
