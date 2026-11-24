#pragma once

#include <memory>
#include <QMainWindow>

class QListWidget;
class QListWidgetItem;
class Manager;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Manager &manager, QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void importFile(bool isTemplate);
    void refreshList(bool isTemplate);
    void updateActions();
    void openSelected(QListWidgetItem *item);
    void renameSelected();
    void deleteSelected();
    bool showingTemplates() const;
    QListWidget *activeList() const;

    Manager &manager_;
    std::unique_ptr<Ui::MainWindow> ui;
};
