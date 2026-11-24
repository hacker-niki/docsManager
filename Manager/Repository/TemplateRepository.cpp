#include "TemplateRepository.h"

TemplateRepository::TemplateRepository(std::shared_ptr<sqlite3> db)
    : SqliteRepository<Template>(std::move(db), "Templates") {}
