#ifndef DOCSMANAGER_DOCUMENTREPOSITORY_H
#define DOCSMANAGER_DOCUMENTREPOSITORY_H

#include "../Entities/Document.h"
#include "SqliteRepository.h"

class DocumentRepository : public SqliteRepository<Document> {
public:
    explicit DocumentRepository(std::shared_ptr<sqlite3> db);
};

#endif
