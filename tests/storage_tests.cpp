#include "Repository/RepMaster.h"
#include <QTemporaryDir>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
void require(bool condition, const char *message) {
    if (!condition) throw std::runtime_error(message);
}

template<typename Function>
void expectError(const std::string &message, Function action) {
    try {
        action();
    } catch (const std::exception &error) {
        if (std::string(error.what()).find(message) != std::string::npos) return;
        throw std::runtime_error("Unexpected error: " + std::string(error.what()));
    }
    throw std::runtime_error("Expected error containing: " + message);
}

void execute(const std::string &path, const char *sql) {
    sqlite3 *raw = nullptr;
    const int result = sqlite3_open(path.c_str(), &raw);
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> db(raw, sqlite3_close);
    require(result == SQLITE_OK, "Could not open test database");
    if (sqlite3_exec(raw, sql, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(raw));
    }
}

void testStorage() {
    QTemporaryDir directory;
    require(directory.isValid(), "Cannot create test directory");
    const auto path = directory.filePath("docsmanager.db").toStdString();
    execute(path, R"SQL(
        CREATE TABLE Templates (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT NOT NULL);
        INSERT INTO Templates VALUES (13,'{"name":"Письмо","type":"Template","filepath":"./templates/Письмо.docx","fields_":["{\"name\":\"Кому\",\"content\":\"Иван\"}"]}');
        CREATE TABLE Documents (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT NOT NULL);
        INSERT INTO Documents VALUES (42,'{"name":"Old","type":"Document","filepath":"./documents/old.txt","fields_":[]}');
        INSERT INTO Documents VALUES (53,'{"name":"Old","type":"Document","filepath":"./documents/older.txt","fields_":[]}');
    )SQL");
    {
        RepMaster library(path);
        require(library.queuedFilePaths().empty(), "Legacy database has unexpected cleanup entries");
        library.queueFilePath("documents/Файл.txt");
        library.queueFilePath("documents/Файл.txt");
        library.queueFilePath("templates/draft.docx");
        library.forgetFilePath("templates/draft.docx");
        library.forgetFilePath("templates/draft.docx");
        auto &documents = *library.documentRepository_;
        auto &templates = *library.templateRepository_;
        auto items = documents.get_all();
        require(items.size() == 2 && items[0].get_id() == 42 && items[1].get_id() == 53,
                "Legacy duplicate rows or IDs lost");
        auto letter = templates.get(13);
        require(letter.get_field(0).get_name() == "Кому" && letter.get_field(0).get_content() == "Иван",
                "Legacy template fields lost");
        const std::string content("a\0b", 3);
        letter.set_field(0, content);
        templates.update_by_id(13, letter);
        require(templates.get(13).get_field(0).get_content() == content, "Field content changed during roundtrip");
        expectError("already exists", [&] { templates.add(letter); });
        expectError("does not exist", [&] { templates.update_by_id(500, letter); });
        expectError("does not exist", [&] { documents.remove(500); });
        auto renamed = documents.get(53);
        renamed.set_name("Revised");
        documents.update_by_id(53, renamed);
        renamed.set_name("Old");
        expectError("already exists", [&] { documents.update_by_id(53, renamed); });
        require(documents.get(53).get_name() == "Revised", "Failed rename affected original row");
        RepMaster other(path);
        other.templateRepository_->add(Template("Second", "./templates/second.docx"));
        expectError("already exists", [&] { templates.add(Template("Second", "./templates/second.docx")); });
        documents.remove(42);
        library.save();
        other.save();
        require(other.documentRepository_->get_size() == 1 && other.documentRepository_->get(53).get_name() == "Revised",
                "Save renumbered or restored deleted rows");
        require(templates.get_all().size() == 2, "Stale template listing after independent write");
        execute(path, "CREATE TRIGGER injected_failure AFTER INSERT ON Documents "
                      "BEGIN SELECT RAISE(ABORT, 'test rollback'); END");
        expectError("test rollback", [&] { documents.add(renamed); });
        require(documents.get_size() == 1, "Failed insert committed a row");
    }
    {
        RepMaster reopened(path);
        require(reopened.queuedFilePaths() == std::vector<std::string>{"documents/Файл.txt"},
                "Pending file cleanup did not survive reopening or retry");
        reopened.forgetFilePath("documents/Файл.txt");
        require(reopened.queuedFilePaths().empty(), "Completed cleanup remained pending");
        require(reopened.documentRepository_->get(53).get_name() == "Revised", "Reopen lost metadata");
        require(reopened.templateRepository_->get(13).get_field(0).get_content() == std::string("a\0b", 3),
                "Saved field did not survive reopen");
    }
    {
        const auto damagedPath = directory.filePath("legacy.db").toStdString();
        execute(damagedPath, "CREATE TABLE Templates (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT NOT NULL);"
                             "INSERT INTO Templates VALUES (99, '{broken')");
        RepMaster damaged(damagedPath);
        expectError("Templates record 99", [&] { damaged.templateRepository_->get_all(); });
        require(damaged.templateRepository_->get_size() == 1, "Malformed record was discarded");
        damaged.templateRepository_->add(Template("Valid", "./templates/valid.docx"));
        require(damaged.templateRepository_->get(100).get_name() == "Valid", "Malformed record damaged valid data");
    }
#ifdef Q_OS_UNIX
    {
        const auto link = directory.filePath("linked.db").toStdString();
        std::filesystem::create_symlink(path, link);
        expectError("Cannot open library database", [&] { RepMaster linked(link); });
    }
#endif
}
}

int main() {
    try {
        testStorage();
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
