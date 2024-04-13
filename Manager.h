//
// Created by nikita on 22.03.24.
//

#ifndef MANAGER_H
#define MANAGER_H

#include <memory>
#include "Interfaces/IGenerator.h"
#include "Interfaces/IRepository.h"
#include "Repository/TemplateRepository.h"
#include "Repository/RepMaster.h"

class Manager {
    std::shared_ptr<RepMaster> repMaster;
    std::shared_ptr<IGenerator> generator_;
public:
    Manager();

    ~Manager();

    int generate_document(int template_id);

    std::vector<Document> get_documents();

    std::vector<Template> get_templates();

    void create_template(std::string path, std::string name);

    void add_document(int templateId, std::string name);

    void add_document(std::string document, std::string name);

    Template &getTempate(int id);

    Document &getDocument(int id);
};

#endif //MANAGER_H
