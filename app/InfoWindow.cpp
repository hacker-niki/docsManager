// Created by nikita on 01.06.24.
//

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QProcess>
#include "InfoWindow.h"
#include "ui_InfoWindow.h"
#include "MainWindow.h"

InfoWindow::InfoWindow(QMainWindow *parent, Template temp, Manager &manager)
        : QDialog(parent), template_(temp), manager_(manager), parent_(parent) {

    setMinimumSize(400, 300);

    auto *layout = new QVBoxLayout(this);

    auto *titleLabel = new QLabel(QString(temp.get_name().c_str()), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    for (const auto &field: temp.get_fields()) {
        auto *fieldLayout = new QHBoxLayout();

        auto *nameLabel = new QLabel(QString(field.get_name().c_str()), this);
        fieldLayout->addWidget(nameLabel);

        auto *contentEdit = new QLineEdit(QString(field.get_content().c_str()), this);
        fieldLayout->addWidget(contentEdit);

        fieldEdits_.push_back(contentEdit);

        layout->addLayout(fieldLayout);
    }

    auto *saveButton = new QPushButton("Save", this);
    layout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &InfoWindow::onSaveButtonClicked);

    setLayout(layout);
}

void InfoWindow::onSaveButtonClicked() {
    for (size_t i = 0; i < fieldEdits_.size(); ++i) {
        template_.set_field(i, fieldEdits_[i]->text().toStdString());
    }
    bool ok;
    QString name = QInputDialog::getText(this, tr(""), tr("Название документа:"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        manager_.create_document_from_template(template_, name.toStdString());
        MainWindow *mainWindow = qobject_cast<MainWindow *>(parentWidget());
        if (mainWindow) {
            mainWindow->updateTemplateList();
            mainWindow->updateDocumentList();
        }
        QMessageBox::information(this, "Save", "Документ успешно создан!");
    }
}

void InfoWindow::onOpenButtonClicked() {
    QString sourceFilePath = QString::fromStdString(template_.get_path());
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