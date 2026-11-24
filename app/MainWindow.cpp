#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DocumentWindow.h"
#include "InfoWindow.h"
#include "UiError.h"
#include "../Manager/Manager.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>

namespace {
QString requestName(QWidget *parent, const QString &title, const QString &initial) {
    bool ok = false;
    const QString name = QInputDialog::getText(parent, title, QObject::tr("Название:"),
                                               QLineEdit::Normal, initial, &ok).trimmed();
    if (ok && name.isEmpty())
        QMessageBox::warning(parent, title, QObject::tr("Введите название."));
    return ok ? name : QString();
}

template <typename Rows>
void populate(QListWidget *list, const Rows &rows) {
    const auto *previous = list->currentItem();
    const QVariant selectedId = previous ? previous->data(Qt::UserRole) : QVariant();
    list->clear();
    for (auto &row : rows) {
        auto *item = new QListWidgetItem(QString::fromStdString(row.get_name()), list);
        item->setData(Qt::UserRole, row.get_id());
        if (selectedId.isValid() && selectedId.toInt() == row.get_id())
            list->setCurrentItem(item);
    }
}
}

MainWindow::MainWindow(Manager &manager, QWidget *parent)
    : QMainWindow(parent), manager_(manager), ui(std::make_unique<Ui::MainWindow>()) {
    ui->setupUi(this);
    connect(ui->OpenButton, &QAction::triggered, this, [this] { importFile(false); });
    connect(ui->NewButton, &QAction::triggered, this, [this] { importFile(true); });
    connect(ui->addButton, &QPushButton::clicked, this, [this] { importFile(showingTemplates()); });
    connect(ui->openButton, &QPushButton::clicked, this, [this] { openSelected(activeList()->currentItem()); });
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelected);
    connect(ui->renameButton, &QPushButton::clicked, this, &MainWindow::renameSelected);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateActions);
    for (auto *list : {ui->templatesWidget, ui->documentsWidget}) {
        connect(list, &QListWidget::itemActivated, this, &MainWindow::openSelected);
        connect(list, &QListWidget::currentItemChanged, this, &MainWindow::updateActions);
    }
    connect(ui->HelpInfo, &QAction::triggered, this, [this] {
        QMessageBox::information(this, tr("Как создать шаблон"),
            tr("В документе DOCX обозначьте каждое поле двойными знаками процента, например %%Имя%%. "
               "Сохраните файл и добавьте его во вкладке «Шаблоны». "
               "Выберите шаблон и нажмите «Создать документ…», чтобы заполнить поля."));
    });
    refreshList(true);
    refreshList(false);
    updateActions();
}

MainWindow::~MainWindow() = default;

bool MainWindow::showingTemplates() const {
    return ui->tabWidget->currentWidget() == ui->templates;
}

QListWidget *MainWindow::activeList() const {
    return showingTemplates() ? ui->templatesWidget : ui->documentsWidget;
}

void MainWindow::updateActions() {
    const bool hasSelection = activeList()->currentItem() != nullptr;
    ui->addButton->setText(showingTemplates() ? tr("Добавить шаблон…") : tr("Добавить документ…"));
    ui->openButton->setText(showingTemplates() ? tr("Создать документ…") : tr("Открыть…"));
    ui->openButton->setEnabled(hasSelection);
    ui->renameButton->setEnabled(hasSelection);
    ui->deleteButton->setEnabled(hasSelection);
}

void MainWindow::refreshList(bool isTemplate) {
    if (isTemplate)
        populate(ui->templatesWidget, manager_.get_templates());
    else
        populate(ui->documentsWidget, manager_.get_documents());
    updateActions();
}

void MainWindow::importFile(bool isTemplate) {
    const QString title = isTemplate ? tr("Добавить шаблон") : tr("Добавить документ");
    const QString path = QFileDialog::getOpenFileName(this, title, {},
        isTemplate ? tr("Документы Word (*.docx *.DOCX)") : tr("Все файлы (*)"));
    if (path.isEmpty()) return;
    const QString name = requestName(this, title, QFileInfo(path).completeBaseName());
    if (name.isEmpty()) return;
    runWithErrorDialog(this, title, [&] {
        if (isTemplate)
            manager_.create_template(path.toStdString(), name.toStdString());
        else
            manager_.add_document(path.toStdString(), name.toStdString());
        refreshList(isTemplate);
        ui->tabWidget->setCurrentWidget(isTemplate ? ui->templates : ui->documents);
        const auto matches = activeList()->findItems(name, Qt::MatchExactly | Qt::MatchCaseSensitive);
        if (!matches.isEmpty()) activeList()->setCurrentItem(matches.front());
        statusBar()->showMessage(isTemplate ? tr("Шаблон добавлен.") : tr("Документ добавлен."), 5000);
    });
}

void MainWindow::openSelected(QListWidgetItem *item) {
    if (!item) return;
    const int id = item->data(Qt::UserRole).toInt();
    runWithErrorDialog(this, tr("Открытие"), [&] {
        if (showingTemplates()) {
            InfoWindow dialog(this, manager_.getTemplate(id), manager_);
            if (dialog.exec() == QDialog::Accepted) {
                refreshList(false);
                ui->tabWidget->setCurrentWidget(ui->documents);
                const auto matches = activeList()->findItems(dialog.documentName(),
                                                             Qt::MatchExactly | Qt::MatchCaseSensitive);
                if (!matches.isEmpty()) activeList()->setCurrentItem(matches.front());
                statusBar()->showMessage(tr("Документ создан."), 5000);
            }
        } else {
            DocumentWindow dialog(this, manager_.getDocument(id));
            dialog.exec();
        }
    });
}

void MainWindow::renameSelected() {
    const auto *item = activeList()->currentItem();
    if (!item) return;
    const bool isTemplate = showingTemplates();
    const int id = item->data(Qt::UserRole).toInt();
    const QString oldName = item->text();
    const QString name = requestName(this, tr("Переименовать"), oldName);
    if (name.isEmpty() || name == oldName) return;
    runWithErrorDialog(this, tr("Переименование"), [&] {
        if (isTemplate)
            manager_.renameTemplate(id, name.toStdString());
        else
            manager_.renameDocument(id, name.toStdString());
        refreshList(isTemplate);
    });
}

void MainWindow::deleteSelected() {
    const auto *item = activeList()->currentItem();
    if (!item) return;
    const bool isTemplate = showingTemplates();
    const int id = item->data(Qt::UserRole).toInt();
    QMessageBox confirm(QMessageBox::Question, tr("Удаление"),
                        tr("Удалить «%1» из библиотеки?").arg(item->text()),
                        QMessageBox::Yes | QMessageBox::Cancel, this);
    confirm.setTextFormat(Qt::PlainText);
    confirm.setDefaultButton(QMessageBox::Cancel);
    if (confirm.exec() != QMessageBox::Yes) return;
    runWithErrorDialog(this, tr("Удаление"), [&] {
        if (isTemplate)
            manager_.deleteTemplate(id);
        else
            manager_.deleteDocument(id);
        refreshList(isTemplate);
        statusBar()->showMessage(isTemplate ? tr("Шаблон удалён.") : tr("Документ удалён."), 5000);
    });
}
