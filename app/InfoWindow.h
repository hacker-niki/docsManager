//
// Created by nikita on 01.06.24.
//

#ifndef DOCSMANAGER_INFOWINDOW_H
#define DOCSMANAGER_INFOWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "../Manager/Entities/Template.h"
#include "../Manager/Manager.h"


class InfoWindow : public QDialog {
Q_OBJECT

public:
    explicit InfoWindow(QMainWindow *parent, Template temp, Manager &manager);

private slots:

    void onSaveButtonClicked();

    void onOpenButtonClicked();

private:
    Template template_;
    std::vector<QLineEdit *> fieldEdits_;
    Manager &manager_;
    QMainWindow *parent_;
};

#endif //DOCSMANAGER_INFOWINDOW_H
