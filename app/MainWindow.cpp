#include "MainWindow.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->createTemplateButton, &QPushButton::clicked, this, &MainWindow::onCreateTemplateClicked);
    connect(ui->addDocumentButton, &QPushButton::clicked, this, &MainWindow::onAddDocumentClicked);

    loadDocuments();
    loadTemplates();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadDocuments() {
    ui->documentListWidget->clear();
    std::vector<Document> documents = manager_.get_documents();
    for (auto &doc: documents) {
        ui->documentListWidget->addItem(QString::fromStdString(doc.get_name()));
    }
}

void MainWindow::loadTemplates() {
    ui->templateListWidget->clear();
    std::vector<Template> templates = manager_.get_templates();
    for (auto &temp: templates) {
        ui->templateListWidget->addItem(QString::fromStdString(temp.get_name()));
    }
}

void MainWindow::onCreateTemplateClicked() {
    QString path = ui->templatePathLineEdit->text();
    QString name = ui->templateNameLineEdit->text();
    manager_.create_template(path.toStdString(), name.toStdString());
    loadTemplates();
}

void MainWindow::onAddDocumentClicked() {
    QString name = ui->documentNameLineEdit->text();
    int templateId = ui->templateListWidget->currentRow(); // Assuming the row index matches the template ID
    manager_.add_document(templateId, name.toStdString());
    loadDocuments();
}