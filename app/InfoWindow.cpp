#include "InfoWindow.h"
#include "UiError.h"
#include "../Manager/Manager.h"

#include <utility>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

InfoWindow::InfoWindow(QWidget *parent, Template source, Manager &manager)
    : QDialog(parent), source_(std::move(source)), manager_(manager) {
    setWindowTitle(tr("Создать документ — %1").arg(QString::fromStdString(source_.get_name())));
    resize(480, 360);
    auto *layout = new QVBoxLayout(this);
    auto *nameLayout = new QFormLayout;
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setObjectName("documentName");
    nameLayout->addRow(tr("Название документа"), nameEdit_);
    layout->addLayout(nameLayout);

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    auto *contents = new QWidget;
    auto *fields = new QFormLayout(contents);
    fields->setRowWrapPolicy(QFormLayout::WrapLongRows);
    for (const auto &field : source_.get_fields()) {
        auto *label = new QLabel(QString::fromStdString(field.get_name()), contents);
        label->setTextFormat(Qt::PlainText);
        label->setWordWrap(true);
        auto *value = new QLineEdit(QString::fromStdString(field.get_content()), contents);
        value->setObjectName(QStringLiteral("fieldValue%1").arg(fieldEdits_.size()));
        label->setBuddy(value);
        fields->addRow(label, value);
        fieldEdits_.push_back(value);
    }
    scroll->setWidget(contents);
    layout->addWidget(scroll);
    scroll->setVisible(!fieldEdits_.empty());

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    auto *create = buttons->addButton(tr("Создать"), QDialogButtonBox::AcceptRole);
    create->setObjectName("createButton");
    create->setDefault(true);
    create->setEnabled(false);
    connect(nameEdit_, &QLineEdit::textChanged, this, [create](const QString &text) {
        create->setEnabled(!text.trimmed().isEmpty());
    });
    connect(buttons, &QDialogButtonBox::accepted, this, &InfoWindow::createDocument);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
    nameEdit_->setFocus();
}

QString InfoWindow::documentName() const {
    return nameEdit_->text().trimmed();
}

void InfoWindow::createDocument() {
    const auto name = documentName();
    if (name.isEmpty()) return;
    runWithErrorDialog(this, tr("Создание документа"), [&] {
        for (size_t i = 0; i < fieldEdits_.size(); ++i)
            source_.set_field(static_cast<int>(i), fieldEdits_[i]->text().toStdString());
        manager_.create_document_from_template(source_, name.toStdString());
        accept();
    });
}
