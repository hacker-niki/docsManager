#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Manager/Manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:

    void onOpenButtonClicked();

    void onNewButtonClicked();

    void updateTemplateList();

    void updateDocumentList();

private:
    Ui::MainWindow *ui;
    Manager manager_;
};

#endif // MAINWINDOW_H