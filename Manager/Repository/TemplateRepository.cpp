//
// Created by nikita on 22.03.24.
//

#include <stdexcept>
#include <iostream>
#include "../Entities/Document.h"
#include "TemplateRepository.h"

void TemplateRepository::add(Template temp) {
    sqlite3_stmt *stmt;
    const char *insert_sql = "INSERT INTO Templates (data) VALUES (?);";

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
    update();
}

const Template &TemplateRepository::get(int id) {
    for (auto &temp: templates_) {
        if (temp.get_id() == id) {
            return temp;
        }
    }
    throw std::out_of_range("There's no template with such id🙂");
}

void TemplateRepository::remove(int id) {
    sqlite3_stmt *stmt;
    const char *delete_sql = "DELETE FROM Templates WHERE id = ?;";

    if (sqlite3_prepare_v2(db_, delete_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare delete statement");
    }

    // Bind the ID parameter
    if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind ID");
    }

    // Execute the delete statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute delete statement");
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    update();
//    throw std::out_of_range("There's no template with such id🙂");
}

void TemplateRepository::update_by_id(int id, const Template &temp) {
    sqlite3_stmt *stmt;
    const char *update_sql = "UPDATE Templates SET data = ? WHERE id = ?;";

    if (sqlite3_prepare_v2(db_, update_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare update statement");
    }

    std::string serialized = temp.to_string();
    if (sqlite3_bind_text(stmt, 1, serialized.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind text");
    }

    if (sqlite3_bind_int(stmt, 2, id) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind ID");
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute update statement");
    }

    sqlite3_finalize(stmt);
    update(); // Refresh the in-memory data
}

std::vector<Template> TemplateRepository::get_all() {
    return templates_;
}

void TemplateRepository::update() {
    templates_.clear();

    const char *select_sql = "SELECT id, data FROM Templates;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, select_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0); // Retrieve the id column
        const unsigned char *text = sqlite3_column_text(stmt, 1); // Retrieve the data column
        if (text) {
            Template temp;
            temp.set_id(id); // Set the template id
            temp.from_string(reinterpret_cast<const char *>(text)); // Set the template data
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
    // Ensure the Templates table exists`
    char *errMsg;
    const char *sql_templates = "CREATE TABLE IF NOT EXISTS Templates ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "data TEXT NOT NULL);";
    if (sqlite3_exec(db_, sql_templates, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error_message = "SQL error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error_message);
    }
}
