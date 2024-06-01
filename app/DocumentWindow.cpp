#include "DocumentWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>
#include <QDir>

DocumentWindow::DocumentWindow(QMainWindow *parent, Document doc, Manager &manager)
        : QDialog(parent), document_(doc), manager_(manager), parent_(parent) {

    setMinimumSize(400, 300);

    auto *layout = new QVBoxLayout(this);

    auto *titleLabel = new QLabel(QString::fromStdString(doc.get_name()), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    auto *openButton = new QPushButton("Open Document", this);
    layout->addWidget(openButton);

    connect(openButton, &QPushButton::clicked, this, &DocumentWindow::onOpenButtonClicked);

    setLayout(layout);
}

void DocumentWindow::onOpenButtonClicked() {
    QString sourceFilePath = QString::fromStdString(document_.get_path());
    if (sourceFilePath.isEmpty()) {
        QMessageBox::warning(this, tr("Open Document"), tr("Document path is empty."));
        return;
    }

    // Prompt the user to select the destination directory
    QString destinationDir = QFileDialog::getExistingDirectory(this, tr("Select Destination Directory"));
    if (destinationDir.isEmpty()) {
        QMessageBox::warning(this, tr("Operation Cancelled"), tr("No destination directory selected."));
        return;
    }

    // Construct the destination file path
    QFileInfo fileInfo(sourceFilePath);
    QString destinationFilePath = destinationDir + QDir::separator() + fileInfo.fileName();

    // Copy the file to the destination directory
    if (QFile::copy(sourceFilePath, destinationFilePath)) {
        QMessageBox::information(this, tr("Success"), tr("File copied successfully."));

        // Open the file explorer at the destination directory
#ifdef Q_OS_WIN
        QProcess::startDetached("explorer", QStringList() << destinationFilePath);
#elif defined(Q_OS_MAC)
        QProcess::startDetached("open", QStringList() << destinationFilePath);
#elif defined(Q_OS_LINUX)
        QProcess::startDetached("xdg-open", QStringList() << destinationFilePath);
#else
        QMessageBox::warning(this, tr("Error"), tr("Opening Explorer is not supported on this platform."));
#endif
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Failed to copy the file."));
    }
}