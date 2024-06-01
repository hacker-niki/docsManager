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


class InfoWindow : public QDialog {
Q_OBJECT

public:
    InfoWindow(QMainWindow *parent = nullptr, Template *temp = nullptr);

private:
    int id_;
};

#endif //DOCSMANAGER_INFOWINDOW_H
