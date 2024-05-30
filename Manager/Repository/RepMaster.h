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
    
    RepMaster();

    int save();

    int update();

    ~RepMaster() {
        sqlite3_close(db_);
    }
};


#endif //DOCSMANAGER_REPMASTER_H
