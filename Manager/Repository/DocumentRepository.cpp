#include "DocumentRepository.h"

DocumentRepository::DocumentRepository(std::shared_ptr<sqlite3> db)
    : SqliteRepository<Document>(std::move(db), "Documents") {}
