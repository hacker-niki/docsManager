#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "InfoWindow.h"
#include "DocumentWindow.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Connect menu actions to slots
    connect(ui->OpenButton, &QAction::triggered, this, &MainWindow::onOpenButtonClicked);
    connect(ui->NewButton, &QAction::triggered, this, &MainWindow::onNewButtonClicked);
    connect(ui->templatesWidget, &QListWidget::itemClicked, this, &MainWindow::on_openNewWindowButton_clicked);
    connect(ui->documentsWidget, &QListWidget::itemClicked, this, &MainWindow::on_openDocumentWindowButton_clicked);

    // Initial population of lists
    updateTemplateList();
    updateDocumentList();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onOpenButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Добавить локумент"), "", tr("All Files (*)"));
    bool ok;
    QString name = QInputDialog::getText(this, tr("Документ"), tr("Название документа:"), QLineEdit::Normal, "", &ok);
    if (ok && !filePath.isEmpty()) {
        manager_.add_document(filePath.toStdString(), name.toStdString());
        updateDocumentList();
    }
}

void MainWindow::onNewButtonClicked() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Новый шаблон"), tr("Название:"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        QString path = QFileDialog::getOpenFileName(this, tr("Save Template"), "",
                                                    tr("Template Files (*.docx);;All Files (*)"));
        if (!path.isEmpty()) {
            manager_.create_template(path.toStdString(), name.toStdString());
            updateTemplateList();
        }
    }
}

void MainWindow::updateTemplateList() {
    ui->templatesWidget->clear();
    auto templates = manager_.get_templates();
    for (auto &temp: templates) {
        auto *item = new QListWidgetItem(QString::fromStdString(temp.get_name()));
        item->setData(Qt::UserRole, temp.get_id());
        ui->templatesWidget->addItem(item);
    }
}


void MainWindow::updateDocumentList() {
    ui->documentsWidget->clear();
    auto documents = manager_.get_documents();
    for (auto &doc: documents) {
        auto *item = new QListWidgetItem(QString::fromStdString(doc.get_name()));
        item->setData(Qt::UserRole, doc.get_id());
        ui->documentsWidget->addItem(item);
    }
}

void MainWindow::on_openNewWindowButton_clicked(QListWidgetItem *item) {
    if (item) {
        int id = item->data(Qt::UserRole).toInt();
        auto infoWindow = new InfoWindow(this, manager_.getTemplate(id), manager_);
        infoWindow->show();
        infoWindow->raise();
    }
}

void MainWindow::on_openDocumentWindowButton_clicked(QListWidgetItem *item) {
    if (item) {
        int id = item->data(Qt::UserRole).toInt();
        auto document = manager_.getDocument(id);
        auto documentWindow = new DocumentWindow(this, document, manager_);
        documentWindow->show();
        documentWindow->raise();
    }
}
