#include "MainWindow.h"
#include "../Manager/Manager.h"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <QApplication>
#include <QCommandLineParser>
#include <iostream>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QCoreApplication::setApplicationName("docsManager");
    QCoreApplication::setApplicationVersion(DOCSMANAGER_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Локальная библиотека документов и шаблонов DOCX."));
    const auto help = parser.addHelpOption();
    const auto version = parser.addVersionOption();
    QCommandLineOption dataDir("data-dir", QObject::tr("Каталог базы данных, документов и шаблонов."),
                               QObject::tr("каталог"));
    parser.addOption(dataDir);
    // Informational commands also work on hosts without a window system.
    QStringList arguments;
    for (int i = 0; i < argc; ++i) arguments.append(QString::fromLocal8Bit(argv[i]));
    if (parser.parse(arguments) && (parser.isSet(help) || parser.isSet(version) || parser.isSet("help-all"))) {
        QCoreApplication cli(argc, argv);
        parser.process(cli);
        return EXIT_SUCCESS;
    }

    QApplication app(argc, argv);
    app.setApplicationDisplayName(QObject::tr("Менеджер документов"));
    QTranslator qtRussian;
    if (qtRussian.load("qtbase_ru", QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtRussian);
    parser.process(app);
    if (!parser.positionalArguments().isEmpty())
        parser.showHelp(EXIT_FAILURE);

    try {
        if (parser.isSet(dataDir) && parser.value(dataDir).isEmpty()) parser.showHelp(EXIT_FAILURE);
        Manager manager = parser.isSet(dataDir)
            ? Manager(std::filesystem::u8path(parser.value(dataDir).toStdString())) : Manager();
        MainWindow window(manager);
        window.show();
        return app.exec();
    } catch (const std::exception &error) {
        std::cerr << "Не удалось открыть библиотеку: " << error.what() << '\n';
        if (app.platformName() != "offscreen" && app.platformName() != "minimal") {
            QMessageBox box(QMessageBox::Critical, QObject::tr("Не удалось открыть библиотеку"),
                            QString::fromUtf8(error.what()), QMessageBox::Ok);
            box.setTextFormat(Qt::PlainText);
            box.exec();
        }
        return EXIT_FAILURE;
    }
}
