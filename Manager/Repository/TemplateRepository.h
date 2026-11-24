#ifndef DOCSMANAGER_TEMPLATEREPOSITORY_H
#define DOCSMANAGER_TEMPLATEREPOSITORY_H

#include "../Entities/Template.h"
#include "SqliteRepository.h"

class TemplateRepository : public SqliteRepository<Template> {
public:
    explicit TemplateRepository(std::shared_ptr<sqlite3> db);
};

#endif
