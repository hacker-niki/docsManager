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
    std::shared_ptr<DocumentRepository> documentRepository;
    std::shared_ptr<TemplateRepository> templateRepository;

    RepMaster() {
        templateRepository = std::make_shared<TemplateRepository>();
        documentRepository = std::make_shared<DocumentRepository>();

    }

    int save();

    int update();
};


#endif //DOCSMANAGER_REPMASTER_H
