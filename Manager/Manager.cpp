//
// Created by nikita on 22.03.24.
//

#include "Manager.h"

#include <memory>
#include <utility>
#include "Generator.h"
#include "FileManager.h"

Manager::Manager() {
    repMaster_ = std::make_shared<RepMaster>();
    generator_ = std::make_shared<Generator>();
    folder_name_ = "documents";
    FileManager::create_directory("documents");
    FileManager::create_directory("templates");
    FileManager::create_directory("data");
    repMaster_->update();
}


void Manager::create_template(std::string path, std::string name) {
    std::string filePath = "./templates/" + FileManager::getFileName(path);
    FileManager::copyFile(path, filePath);
    auto temp = Template(std::move(name), filePath);
    generator_->generate(temp);
    repMaster_->templateRepository_->add(temp);
}

void Manager::add_document(int template_id, std::string name) {
    auto &templ = repMaster_->templateRepository_->get(template_id);
    std::string filePath = "./templates/" + FileManager::getFileName(templ.get_path());
    FileManager::copyFile(templ.get_path(), filePath);
    generator_->generate(templ, name);
}

void Manager::add_document(std::string filepath, std::string name) {
    std::string filePath = "./documents/" + FileManager::getFileName(filepath);
    FileManager::copyFile(filepath, filePath);
    auto doc = Document(name, filePath);
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

void Manager::create_template_form_blank(std::string name) {
    std::string filePath = "./data/blank.docx";
    std::string filepath = "./template/" + name + ".docx";
    FileManager::copyFile(filePath, filepath);
    auto temp = Template(std::move(name), filePath);
    generator_->generate(temp);
    repMaster_->templateRepository_->add(temp);
}

Manager::~Manager() = default;
