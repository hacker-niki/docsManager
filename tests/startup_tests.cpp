#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QProcess>
#include <QTemporaryDir>
#include <iostream>
#include <stdexcept>

namespace {
void require(bool ok, const char *message) {
    if (!ok) throw std::runtime_error(message);
}

void run(const QString &binary) {
    QTemporaryDir temp;
    require(temp.isValid(), "Temporary directory unavailable");
    const auto directory = temp.filePath("библиотека");
    const QStringList args{"--data-dir", directory};
    QProcess app;
    app.setWorkingDirectory(temp.path());
    app.start(binary, args);
    require(app.waitForStarted(), "Application did not start");

    QElapsedTimer timer;
    timer.start();
    while (QFileInfo(QDir(directory).filePath("docsmanager.db")).size() == 0 &&
           app.state() == QProcess::Running && timer.elapsed() < 5000) {
        app.waitForFinished(20);
    }
    require(QFileInfo(QDir(directory).filePath("docsmanager.db")).size() > 0, "Library did not initialize");
    require(!QFile::exists(temp.filePath("docsmanager.db")), "Library leaked into working directory");

    QProcess second;
    second.start(binary, args);
    require(second.waitForFinished(5000) && second.exitStatus() == QProcess::NormalExit &&
            second.exitCode() != 0, "Second writer was not rejected");
    require(QString::fromUtf8(second.readAllStandardError()).contains("уже открыта"),
            "Startup failure did not explain the library lock");
    app.kill();
    require(app.waitForFinished(5000), "Could not stop test process");

    // Restart after an abrupt exit. A stale lock must not strand the library.
    app.start(binary, args);
    require(app.waitForStarted() && !app.waitForFinished(500), "Restart failed with a stale lock");
    app.terminate();
    require(app.waitForFinished(5000), "Could not terminate test process");
}
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    try {
        require(argc == 2, "Pass the path to docsManager");
        run(QString::fromLocal8Bit(argv[1]));
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
