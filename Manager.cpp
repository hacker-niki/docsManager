//
// Created by nikita on 22.03.24.
//

#include "Manager.h"

#include <memory>
#include "Generator.h"
#include "FileManager.h"

Manager::Manager() {
    repMaster_ = std::make_shared<RepMaster>();
    generator_ = std::make_shared<Generator>();
    folder_name_ = "data";
}

void Manager::create_template(std::string path, std::string name) {
    auto temp = Template(name, path);
    generator_->generate(temp);
    repMaster_->templateRepository_->add(temp);
}

void Manager::add_document(int template_id, std::string name) {
    auto &templ = repMaster_->templateRepository_->get(template_id);
    generator_->generate(templ, name);
}

void Manager::add_document(std::string filepath, std::string name) {
    auto doc = Document(name, "./" + folder_name_ + "/" + FileManager::getFileName(filepath));
    repMaster_->documentRepository_->add(doc);
}

std::vector<Document> Manager::get_documents() {
    return repMaster_->documentRepository_->get_all();
}

std::vector<Template> Manager::get_templates() {
    return repMaster_->templateRepository_->get_all();
}

Template &Manager::getTempate(int id) {
    return repMaster_->templateRepository_->get(id);
}

Document &Manager::getDocument(int id) {
    return repMaster_->documentRepository_->get(id);
}

Manager::~Manager() = default;
