#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "../Manager/Manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

public
    slots:

    void updateTemplateList();

    void updateDocumentList();
    

private
    slots:

    void onOpenButtonClicked();

    void onNewButtonClicked();

    void on_openNewWindowButton_clicked(QListWidgetItem *item);

    void on_openDocumentWindowButton_clicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    Manager manager_;
};

#endif // MAINWINDOW_H