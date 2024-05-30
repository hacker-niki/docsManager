//
// Created by nikita on 22.03.24.
//

#include <stdexcept>
#include "../Entities/Document.h"
#include "TemplateRepository.h"

void TemplateRepository::add(Template temp) {
    templates_.push_back(temp);
}

Template &TemplateRepository::get(int id) {
    while (true) {
        if (id < 0 || id >= templates_.size())
            break;
        return templates_[id];
    }
    throw std::out_of_range("There's no template with such id🙂");
}

void TemplateRepository::remove(int id) {
    while (true) {
        if (id < 0 || id >= templates_.size())
            break;
        templates_.erase(std::next(templates_.begin(), id));
        return;
    }
    throw std::out_of_range("There's no template with such id🙂");

}

std::vector<Template> TemplateRepository::get_all() {
    return templates_;
}

void TemplateRepository::update() {
    templates_.clear();

    const char *select_sql = "SELECT data FROM Templates;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, select_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text) {
            Template temp;
            temp.from_string(reinterpret_cast<const char *>(text));
            templates_.push_back(temp);
        }
    }

    sqlite3_finalize(stmt);
}

void TemplateRepository::save() {
    // Clear the existing Templates table
    char *errMsg;
    const char *clear_sql = "DELETE FROM Templates;";
    if (sqlite3_exec(db_, clear_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error_message = "SQL error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error_message);
    }

    // Insert current templates into the Templates table
    sqlite3_stmt *stmt;
    const char *insert_sql = "INSERT INTO Templates (data) VALUES (?);";
    for (const auto &temp: templates_) {
        if (sqlite3_prepare_v2(db_, insert_sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        std::string serialized = temp.to_string();
        if (sqlite3_bind_text(stmt, 1, serialized.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            throw std::runtime_error("Failed to bind text");
        }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute statement");
        }

        sqlite3_finalize(stmt);
    }
}

TemplateRepository::TemplateRepository(sqlite3 *db) : db_(db) {
    // Ensure the Documents table exists
    char *errMsg;
    const char *sql_documents = "CREATE TABLE IF NOT EXISTS Documents ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "data TEXT NOT NULL);";
    if (sqlite3_exec(db_, sql_documents, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error_message = "SQL error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error_message);
    }

    // Ensure the Templates table exists
    const char *sql_templates = "CREATE TABLE IF NOT EXISTS Templates ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "data TEXT NOT NULL);";
    if (sqlite3_exec(db_, sql_templates, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error_message = "SQL error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error_message);
    }
}
