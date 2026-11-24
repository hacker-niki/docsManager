#ifndef DOCSMANAGER_REPMASTER_H
#define DOCSMANAGER_REPMASTER_H

#include "DocumentRepository.h"
#include "TemplateRepository.h"

class RepMaster {
public:
    explicit RepMaster(const std::string &databasePath = "docsmanager.db");

    std::shared_ptr<DocumentRepository> documentRepository_;
    std::shared_ptr<TemplateRepository> templateRepository_;

    int save();
    int update();

    void queueFilePath(const std::string &relativePath);
    std::vector<std::string> queuedFilePaths() const;
    void forgetFilePath(const std::string &relativePath);

private:
    std::shared_ptr<sqlite3> db_;
};

#endif
