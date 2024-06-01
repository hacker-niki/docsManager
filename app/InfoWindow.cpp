//
// Created by nikita on 01.06.24.
//

// You may need to build the project (run Qt uic code generator) to get "ui_InfoWindow.h" resolved

#include "InfoWindow.h"
#include "ui_InfoWindow.h"


InfoWindow::InfoWindow(QMainWindow *parent, Template *temp) : QDialog(parent) {
    auto * layout = new QVBoxLayout(this);
    auto *label = new QLabel(QString(temp->get_name().c_str()), this);

    layout->addWidget(label);
    for (auto x: temp->get_fields()) {
        auto *qLabel = new QLabel(QString(x.get_content().c_str()), this);
        layout->addWidget(qLabel);
    }
    setLayout(layout);
}   
