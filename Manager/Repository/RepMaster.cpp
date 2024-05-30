//
// Created by nikita on 08.04.24.
//

#include "RepMaster.h"

int RepMaster::save() {
    documentRepository_->save();
    templateRepository_->save();
    return 0;
}

int RepMaster::update() {
    documentRepository_->update();
    templateRepository_->update();
    return 0;
}

RepMaster::RepMaster() {
    db_ = nullptr;
    if (sqlite3_open("docsmanager.db", &db_)) {
        throw std::runtime_error("Can't open database: " + std::string(sqlite3_errmsg(db_)));
    }

    templateRepository_ = std::make_shared<TemplateRepository>(db_);
    documentRepository_ = std::make_shared<DocumentRepository>(db_);
}
