//
// Created by nikita on 08.04.24.
//

#ifndef DOCSMANAGER_REPMASTER_H
#define DOCSMANAGER_REPMASTER_H


#include <memory>

#include <leveldb/db.h>
#include "DocumentRepository.h"
#include "TemplateRepository.h"
#include "../sqlite/sqlite3.h"

class RepMaster {
public:
    std::shared_ptr<DocumentRepository> documentRepository_;
    std::shared_ptr<TemplateRepository> templateRepository_;
    sqlite3 *db_;


    RepMaster() {
        if (sqlite3_open("docsmanager.db", &db_)) {
            throw std::runtime_error("Can't open database: " + std::string(sqlite3_errmsg(db_)));
        }

        // Initialize tables if not exist
        const char *init_docs_sql = "CREATE TABLE IF NOT EXISTS Documents (id INTEGER PRIMARY KEY, data TEXT);";
        const char *init_temps_sql = "CREATE TABLE IF NOT EXISTS Templates (id INTEGER PRIMARY KEY, data TEXT);";

        char *errMsg = nullptr;
        if (sqlite3_exec(db_, init_docs_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string error = errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error("SQL error: " + error);
        }

        if (sqlite3_exec(db_, init_temps_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string error = errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error("SQL error: " + error);
        }

        templateRepository_ = std::make_shared<TemplateRepository>(db_);
        documentRepository_ = std::make_shared<DocumentRepository>(db_);
    }

    int save();

    int update();

    ~RepMaster() {
        sqlite3_close(db_);
    }
};


#endif //DOCSMANAGER_REPMASTER_H
