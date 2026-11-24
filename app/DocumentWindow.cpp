#include "DocumentWindow.h"
#include "../Manager/Entities/Document.h"

#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QPushButton>
#include <QRegularExpression>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUrl>
#include <QVBoxLayout>

namespace {
bool canOpen(const QFileInfo &file) {
    if (!file.isFile() || !file.isReadable() || file.isExecutable()) return false;
    const QString suffix = file.suffix().toLower();
    const QString mime = QMimeDatabase().mimeTypeForFile(file, QMimeDatabase::MatchContent).name();
    if (suffix == "docx")
        return mime == "application/vnd.openxmlformats-officedocument.wordprocessingml.document"
            || mime == "application/zip";
    if (suffix == "pdf") return mime == "application/pdf";
    return suffix == "txt" && (mime == "text/plain" || file.size() == 0);
}
}

DocumentWindow::DocumentWindow(QWidget *parent, const Document &document)
    : QDialog(parent), sourcePath_(QString::fromStdString(document.get_path())),
      displayName_(QString::fromStdString(document.get_name())) {
    setWindowTitle(tr("Документ"));
    auto *layout = new QVBoxLayout(this);
    auto *title = new QLabel(displayName_, this);
    title->setTextFormat(Qt::PlainText);
    title->setWordWrap(true);
    layout->addWidget(title);

    const QFileInfo file(sourcePath_);
    auto *details = new QLabel(this);
    details->setTextFormat(Qt::PlainText);
    details->setWordWrap(true);
    layout->addWidget(details);
    auto *view = new QPushButton(tr("Открыть в приложении"), this);
    view->setObjectName("viewButton");
    auto *save = new QPushButton(tr("Сохранить копию…"), this);
    save->setObjectName("exportButton");
    const bool readable = file.isFile() && file.isReadable();
    const bool viewable = canOpen(file);
    save->setEnabled(readable);
    view->setEnabled(viewable);
    if (!readable)
        details->setText(tr("Файл недоступен: %1").arg(QDir::toNativeSeparators(sourcePath_)));
    else if (!viewable)
        details->setText(tr("Для работы с этим файлом сохраните копию."));
    else
        details->setText(tr("%1 · %2").arg(file.suffix().toUpper(), QLocale().formattedDataSize(file.size())));
    layout->addWidget(view);
    layout->addWidget(save);
    auto *close = new QDialogButtonBox(QDialogButtonBox::Close, this);
    layout->addWidget(close);
    connect(view, &QPushButton::clicked, this, &DocumentWindow::openDocument);
    connect(save, &QPushButton::clicked, this, &DocumentWindow::exportDocument);
    connect(close, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void DocumentWindow::openDocument() {
    const QFileInfo file(sourcePath_);
    if (!canOpen(file) || !QDesktopServices::openUrl(QUrl::fromLocalFile(file.absoluteFilePath())))
        QMessageBox::warning(this, tr("Открытие документа"),
                             tr("Не удалось открыть файл. Можно сохранить копию и выбрать приложение вручную."));
}

void DocumentWindow::exportDocument() {
    const QFileInfo sourceInfo(sourcePath_);
    QString suggested = displayName_;
    suggested.replace(QRegularExpression(QStringLiteral(R"([<>:"/\\|?*\x00-\x1f])")), "_");
    if (suggested.isEmpty() || suggested == "." || suggested == "..") suggested = tr("Документ");
    const QString suffix = sourceInfo.suffix();
    if (!suffix.isEmpty() && !suggested.endsWith("." + suffix, Qt::CaseInsensitive))
        suggested += "." + suffix;
    const QDir documents(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    const QString path = QFileDialog::getSaveFileName(this, tr("Сохранить копию"), documents.filePath(suggested));
    if (path.isEmpty()) return;
    if (QFileInfo(path) == sourceInfo) {
        QMessageBox::warning(this, tr("Сохранение копии"), tr("Выберите другой путь для копии."));
        return;
    }
    QFile input(sourcePath_);
    QSaveFile output(path);
    auto report = [&](const QString &reason) {
        QMessageBox::warning(this, tr("Сохранение копии"), reason);
    };
    if (!input.open(QIODevice::ReadOnly)) return report(input.errorString());
    if (!output.open(QIODevice::WriteOnly)) return report(output.errorString());

    char buffer[65536];
    qint64 count;
    while ((count = input.read(buffer, sizeof buffer)) > 0) {
        if (output.write(buffer, count) != count) return report(output.errorString());
    }
    if (count < 0) return report(input.errorString());
    if (!output.commit()) return report(output.errorString());
    accept();
}
