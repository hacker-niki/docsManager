//
// Created by nikita on 08.04.24.
//

#ifndef DOCSMANAGER_REPMASTER_H
#define DOCSMANAGER_REPMASTER_H


#include <memory>
#include "DocumentRepository.h"
#include "TemplateRepository.h"

class RepMaster {
public:
    std::shared_ptr<DocumentRepository> documentRepository_;
    std::shared_ptr<TemplateRepository> templateRepository_;

    RepMaster() {
        templateRepository_ = std::make_shared<TemplateRepository>();
        documentRepository_ = std::make_shared<DocumentRepository>();
    }

    int save();

    int update();
};


#endif //DOCSMANAGER_REPMASTER_H
