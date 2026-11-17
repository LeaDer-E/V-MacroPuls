#include "core/AppState.h"
#include "core/I18n.h"
#include "core/Paths.h"
#include "core/Theme.h"
#include "engine/MacroEngine.h"
#include "ui/MainWindow.h"
#include "ui/Widgets.h"
#include <QApplication>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QLockFile>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("V-MacroPlus"));
    app.setApplicationVersion(QStringLiteral("3.9.0"));
    app.setOrganizationName(QStringLiteral("LeaDer-E"));
    app.setQuitOnLastWindowClosed(false);

    vmacro::Paths::ensureDirectories();

    // Single-instance guard
    QLockFile lockFile(vmacro::Paths::dataDir() + QStringLiteral("/v_macroplus.lock"));
    if (!lockFile.tryLock(150)) {
        return 0;
    }

    // Load bundled Tajawal fonts
    const QStringList fontPaths = {
        QStringLiteral(":/assets/fonts/Tajawal-Medium.ttf"),
        QStringLiteral(":/assets/fonts/Tajawal-SemiBold.ttf"),
        QStringLiteral(":/assets/fonts/Tajawal-Bold.ttf"),
        QStringLiteral(":/assets/fonts/Tajawal-ExtraBold.ttf")
    };
    for (const QString& fp : fontPaths) {
        QFontDatabase::addApplicationFont(fp);
    }

    vmacro::AppState state;
    vmacro::I18n::applyLanguage(state.config.language);
    vmacro::Theme::apply(state.config.themeMode, state.config.paletteId, state.config.customAccent);
    app.setFont(vmacro::makeFont(13, QFont::Medium));

    vmacro::MacroEngine macroEngine(&state);
    macroEngine.loadAllData();
    macroEngine.startHooks();

    vmacro::MainWindow win(&state, &macroEngine);
    win.show();

    int rc = app.exec();
    macroEngine.shutdown();
    return rc;
}
