#pragma once

#include <exception>
#include <QMessageBox>

template <typename Action>
void runWithErrorDialog(QWidget *parent, const QString &title, Action action) {
    try {
        action();
    } catch (const std::exception &error) {
        QMessageBox box(QMessageBox::Warning, title, QString::fromUtf8(error.what()), QMessageBox::Ok, parent);
        box.setTextFormat(Qt::PlainText);
        box.exec();
    }
}
