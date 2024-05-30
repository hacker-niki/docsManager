//
// Created by nikita on 08.04.24.
//

#ifndef DOCSMANAGER_REPMASTER_H
#define DOCSMANAGER_REPMASTER_H


#include <memory>

#include <leveldb/db.h>
#include "DocumentRepository.h"
#include "TemplateRepository.h"

class RepMaster {
public:
    std::shared_ptr<DocumentRepository> documentRepository_;
    std::shared_ptr<TemplateRepository> templateRepository_;
    leveldb::DB *dbDocs_;
    leveldb::DB *dbTemps_;
    
    
    RepMaster() {
        templateRepository_ = std::make_shared<TemplateRepository>();
        documentRepository_ = std::make_shared<DocumentRepository>();

        leveldb::Options options;
        options.create_if_missing = true;
        leveldb::Status status_doc = leveldb::DB::Open(options, "/data/dbdocs", &dbDocs_);
        leveldb::Status status_temp = leveldb::DB::Open(options, "/data/dbdocs", &dbTemps_);
//        leveldb::
    }

    int save();

    int update();

    ~RepMaster() {
        delete dbDocs_;
        delete dbTemps_;
    }
};


#endif //DOCSMANAGER_REPMASTER_H
