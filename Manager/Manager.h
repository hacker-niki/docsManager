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
    std::shared_ptr<RepMaster> repMaster_;
    std::shared_ptr<IGenerator> generator_;
    std::string folder_name_;
    
public:
    Manager();

    ~Manager();

    std::vector<Document> get_documents();

    std::vector<Template> get_templates();

    void create_template(std::string path, std::string name);

    void create_template_form_blank(std::string name);

    void add_document(int templateId, std::string name);

    void add_document(std::string document, std::string name);

    Template getTemplate(int id);

    Document getDocument(int id);

    void deleteDocument(int id);

    void deleteTemplate(int id);

    void updateDocument(int id, Document document);

    void updateTemplate(int id, Template temp);

    void create_document_from_template(Template temp, std::string name);
};

#endif //MANAGER_H
