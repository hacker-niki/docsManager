#pragma once

#include <vector>
#include <QDialog>
#include "../Manager/Entities/Template.h"

class QLineEdit;
class Manager;

class InfoWindow : public QDialog {
    Q_OBJECT

public:
    InfoWindow(QWidget *parent, Template source, Manager &manager);
    QString documentName() const;

private:
    void createDocument();

    Template source_;
    Manager &manager_;
    QLineEdit *nameEdit_;
    std::vector<QLineEdit *> fieldEdits_;
};
