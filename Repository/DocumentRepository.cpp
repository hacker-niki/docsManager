//
// Created by nikita on 08.04.24.
//

#include <stdexcept>
#include "DocumentRepository.h"

void DocumentRepository::add(Document document) {
    documents_.push_back(document);
}

Document &DocumentRepository::get(int id) {
    while (true) {
        if (id < 0 || id >= documents_.size())
            break;
        return documents_[id];
    }
    throw std::out_of_range("There's no document with such id🙂");
}

void DocumentRepository::remove(int id) {
    while (true) {
        if (id < 0 || id >= documents_.size())
            break;
        documents_.erase(std::next(documents_.begin(), id));
        return;
    }
    throw std::out_of_range("There's no document with such id🙂");
}

std::vector<Document> DocumentRepository::get_all() {
    return documents_;
}

void DocumentRepository::update() {
    documents_.clear();

    const char *select_sql = "SELECT data FROM Documents;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, select_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        if (text) {
            Document document;
            document.from_string(reinterpret_cast<const char *>(text));
            documents_.push_back(document);
        }
    }

    sqlite3_finalize(stmt);
}

void DocumentRepository::save() {
    // Clear the existing Documents table
    char *errMsg;
    const char *clear_sql = "DELETE FROM Documents;";
    if (sqlite3_exec(db_, clear_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error_message = "SQL error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error_message);
    }

    // Insert current documents into the Documents table
    sqlite3_stmt *stmt;
    const char *insert_sql = "INSERT INTO Documents (data) VALUES (?);";
    for (const auto &document: documents_) {
        if (sqlite3_prepare_v2(db_, insert_sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement");
        }

        std::string serialized = document.to_string();
        if (sqlite3_bind_text(stmt, 1, serialized.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
            throw std::runtime_error("Failed to bind text");
        }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            throw std::runtime_error("Failed to execute statement");
        }

        sqlite3_finalize(stmt);
    }
}

DocumentRepository::DocumentRepository(sqlite3 *db) : db_(db) {
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
}
