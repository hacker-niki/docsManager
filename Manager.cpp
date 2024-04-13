//
// Created by nikita on 22.03.24.
//

#include "Manager.h"

#include <memory>
#include "Generator.h"
#include "FileManager.h"

Manager::Manager() {
    repMaster = std::make_shared<RepMaster>();
    generator_ = std::make_shared<Generator>();
}

void Manager::create_template(std::string path, std::string name) {
    auto temp = Template(name, path);
    generator_->generate(temp);
    repMaster->templateRepository->add(temp);
}

void Manager::add_document(int template_id, std::string name) {
    auto &templ = repMaster->templateRepository->get(template_id);
    generator_->generate(templ, name);
}

void Manager::add_document(std::string filepath, std::string name) {
//    FileManager::copyFile(filepath, "/data/" + FileManager::getFileName(filepath));
    auto doc = Document(name, "/data/" + FileManager::getFileName(filepath));
    repMaster->documentRepository->add(doc);
}

std::vector<Document> Manager::get_documents() {
    return repMaster->documentRepository->get_all();
}

std::vector<Template> Manager::get_templates() {
    return repMaster->templateRepository->get_all();
}

Template &Manager::getTempate(int id) {
    return repMaster->templateRepository->get(id);
}

Document &Manager::getDocument(int id) {
    return repMaster->documentRepository->get(id);
}

Manager::~Manager() = default;
