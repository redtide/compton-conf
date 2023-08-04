#include "maindialog.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // UseHighDpiPixmaps is default from Qt6
#if QT_VERSION < 0x060000
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Picom Conf"));
    const QString VERINFO = QStringLiteral(PROJECT_VERSION
                                           "\nQt        " QT_VERSION_STR);
    app.setApplicationVersion(VERINFO);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    // load translations
    QTranslator qtTranslator, translator;

    // install the translations built-into Qt itself
#if 1
    QLocale locale = QLocale::system();
#else
    QLocale locale(QLocale("it"));
    QLocale::setDefault(locale);
#endif
    if (qtTranslator.load(QStringLiteral("qt_") + locale.name(),
#if QT_VERSION < 0x060000
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
#else
                      QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
#endif
        app.installTranslator(&qtTranslator);

    // E.g. "<appname>_en"
    QString translationsFileName = QCoreApplication::applicationName().toLower() + '_' + locale.name();
    // Try first in the same binary directory, in case we are building,
    // otherwise read from system data
    QString translationsPath = QCoreApplication::applicationDirPath();

    bool isLoaded = translator.load(translationsFileName, translationsPath);
    if (!isLoaded) {
        // "/usr/share/<appname>/translations
        isLoaded = translator.load(translationsFileName,
                                    QStringLiteral(PROJECT_DATA_DIR) + QStringLiteral("/translations"));
    }
    if (isLoaded)
        app.installTranslator(&translator);

    MainDialog dlg;
    dlg.setWindowIcon(QIcon::fromTheme("preferences-system", QIcon(":/preferences-system")));
    dlg.show();

    return app.exec();
}
