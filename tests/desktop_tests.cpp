#include "MainWindow.h"
#include "Manager.h"
#include "WorkflowFixture.h"

#include <QApplication>
#include <QDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTimer>
#include <iostream>

using workflow::require;

namespace {
template<typename Widget>
Widget *child(QWidget &parent, const char *name) {
    auto *widget = parent.findChild<Widget *>(QString::fromLatin1(name));
    require(widget, name);
    return widget;
}

class ModalSteps {
public:
    template<typename Dialog, typename Action>
    void next(Action action) {
        QTimer::singleShot(0, [this, action] {
            auto *active = QApplication::activeModalWidget();
            auto *dialog = qobject_cast<Dialog *>(active);
            try {
                require(dialog, "Unexpected modal dialog");
                action(*dialog);
            } catch (...) {
                error_ = std::current_exception();
                if (auto *current = qobject_cast<QDialog *>(active)) current->reject();
            }
        });
    }

    void check() {
        if (error_) std::rethrow_exception(error_);
    }
private:
    std::exception_ptr error_;
};

void run() {
    QTemporaryDir temp;
    require(temp.isValid(), "No temporary directory");
    const auto root = std::filesystem::u8path(temp.path().toStdString());
    const auto fixture = root / "Письмо.docx";
    workflow::makeTemplate(fixture);
    {
        Manager manager(root / "library");
        MainWindow window(manager);
        window.show();
        QApplication::processEvents();
        auto *templates = child<QListWidget>(window, "templatesWidget");
        auto *documents = child<QListWidget>(window, "documentsWidget");
        auto *tabs = child<QTabWidget>(window, "tabWidget");
        tabs->setCurrentWidget(child<QWidget>(window, "templates"));
        require(!child<QPushButton>(window, "openButton")->isEnabled(), "Open enabled with empty selection");
        ModalSteps modals;
        modals.next<QFileDialog>([&](QFileDialog &file) {
            file.selectFile(QString::fromStdString(fixture.u8string()));
            static_cast<QDialog &>(file).accept();
            modals.next<QInputDialog>([](QInputDialog &name) {
                name.setTextValue("Письмо");
                name.accept();
            });
        });
        child<QPushButton>(window, "addButton")->click();
        modals.check();
        require(templates->count() == 1 && templates->currentItem(), "Imported template not selected");

        modals.next<QDialog>([](QDialog &form) {
            child<QLineEdit>(form, "documentName")->setText("Ответ");
            child<QLineEdit>(form, "fieldValue0")->setText("Анна");
            child<QPushButton>(form, "createButton")->click();
        });
        child<QPushButton>(window, "openButton")->click();
        modals.check();
        require(documents->count() == 1 && documents->currentItem() && documents->currentItem()->text() == "Ответ",
                "Generated document not selected");
        require(FileManager::read_docx(manager.get_documents().at(0).get_path()).find("Анна") != std::string::npos,
                "Text from UI missing from DOCX");

        modals.next<QInputDialog>([](QInputDialog &name) { name.setTextValue("Готово"); name.accept(); });
        child<QPushButton>(window, "renameButton")->click();
        modals.check();
        require(documents->currentItem()->text() == "Готово", "Rename did not refresh document list");
        modals.next<QMessageBox>([](QMessageBox &box) { box.button(QMessageBox::Cancel)->click(); });
        child<QPushButton>(window, "deleteButton")->click();
        modals.check();
        require(documents->count() == 1, "Cancel deleted the document");
    }
    {
        Manager manager(root / "library");
        MainWindow window(manager);
        window.show();
        auto *documents = child<QListWidget>(window, "documentsWidget");
        child<QTabWidget>(window, "tabWidget")->setCurrentWidget(child<QWidget>(window, "documents"));
        documents->setCurrentRow(0);
        require(documents->currentItem() && documents->currentItem()->text() == "Готово", "Reopen lost document");
        ModalSteps modals;
        modals.next<QMessageBox>([](QMessageBox &box) { box.button(QMessageBox::Yes)->click(); });
        child<QPushButton>(window, "deleteButton")->click();
        modals.check();
        require(documents->count() == 0 && manager.get_documents().empty(), "Confirmed delete did not persist");
    }
}
}

int main(int argc, char **argv) {
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QApplication app(argc, argv);
    try { run(); } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
