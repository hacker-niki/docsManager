#include "DocumentWindow.h"
#include "Entities/Document.h"
#include "FileManager.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QPushButton>
#include <QTemporaryDir>
#include <QTimer>

namespace {
void require(bool condition, const char *message) {
    if (!condition) qFatal("%s", message);
}

QPushButton *button(DocumentWindow &dialog, const char *name) {
    auto *result = dialog.findChild<QPushButton *>(name);
    require(result != nullptr, "Document action is missing");
    return result;
}

QFileDialog *activeFileDialog() {
    auto *dialog = qobject_cast<QFileDialog *>(QApplication::activeModalWidget());
    require(dialog != nullptr, "Expected the export file chooser");
    return dialog;
}
}

class UrlReceiver : public QObject {
    Q_OBJECT
public:
    QUrl opened;

public slots:
    void record(const QUrl &url) { opened = url; }
};

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QApplication app(argc, argv);
    QTemporaryDir dir;
    require(dir.isValid(), "Could not create temporary library");
    QFile source(dir.filePath("Письмо 1.txt"));
    const QByteArray content("Hello from docsManager.\n");
    require(source.open(QIODevice::WriteOnly) && source.write(content) == content.size(),
            "Could not write test document");
    source.close();

    DocumentWindow text(nullptr, Document("Письмо", source.fileName().toStdString()));
    auto *view = button(text, "viewButton");
    require(view->isEnabled(), "Plain text should open in an external viewer");
    UrlReceiver receiver;
    QDesktopServices::setUrlHandler("file", &receiver, "record");
    view->click();
    QDesktopServices::unsetUrlHandler("file");
    require(receiver.opened == QUrl::fromLocalFile(source.fileName()), "Wrong document sent to the viewer");

    const auto wordPath = dir.filePath("generated.docx").toStdString();
    FileManager::createBlankDocx(wordPath);
    DocumentWindow word(nullptr, Document("Бланк", wordPath));
    require(button(word, "viewButton")->isEnabled(), "A generated DOCX should open in a viewer");

    const QString opaquePath = dir.filePath("item.bin");
    require(QFile::copy(source.fileName(), opaquePath), "Could not create arbitrary import");
    DocumentWindow opaque(nullptr, Document("Копия", opaquePath.toStdString()));
    require(!button(opaque, "viewButton")->isEnabled(), "This file type should only be exported");
    auto *save = button(opaque, "exportButton");
    require(save->isEnabled(), "The imported file should be exportable");

    bool cancelled = false;
    QTimer::singleShot(0, [&] {
        activeFileDialog()->reject();
        cancelled = true;
    });
    save->click();
    require(cancelled && opaque.result() != QDialog::Accepted, "Cancelling export closed the document");

    QFile copy(dir.filePath("Экспорт.bin"));
    QTimer::singleShot(0, [&] {
        auto *dialog = activeFileDialog();
        require(QFileInfo(dialog->selectedFiles().value(0)).fileName() == QStringLiteral("Копия.bin"),
                "Suggested export filename should use the library name and the original extension");
        dialog->selectFile(copy.fileName());
        static_cast<QDialog *>(dialog)->accept();
    });
    save->click();
    require(opaque.result() == QDialog::Accepted, "Export did not complete");
    require(copy.open(QIODevice::ReadOnly) && copy.readAll() == content, "Exported content changed");

    Document removed("Удалён", opaquePath.toStdString());
    require(QFile::remove(opaquePath), "Could not remove the file outside the library");
    DocumentWindow missing(nullptr, removed);
    require(!button(missing, "viewButton")->isEnabled() && !button(missing, "exportButton")->isEnabled(),
            "Missing documents must not advertise available file actions");
    return 0;
}

#include "document_window_tests.moc"
