//
// Created by nikita on 01.06.24.
//

#ifndef DOCSMANAGER_DOCUMENTWINDOW_H
#define DOCSMANAGER_DOCUMENTWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMainWindow>
#include<QDesktopServices>
#include "../Manager/Entities/Document.h"
#include "../Manager/Manager.h"

class DocumentWindow : public QDialog {
Q_OBJECT

public:
    explicit DocumentWindow(QMainWindow *parent, Document doc, Manager &manager);

private slots:

    void onOpenButtonClicked();

private:
    Document document_;
    Manager &manager_;
    QMainWindow *parent_;
};

#endif //DOCSMANAGER_DOCUMENTWINDOW_H
