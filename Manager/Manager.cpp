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
    for (auto x: repMaster_->templateRepository_->get_all()) {
        if (x.get_name() == name)
            throw new std::invalid_argument("There is already exists template with this name🙂");
    }
    std::string filePath = "./templates/" + name + ".docx";
    FileManager::copyFile(path, filePath);
    auto temp = Template(std::move(name), filePath);
    generator_->generate(temp);
    repMaster_->templateRepository_->add(temp);
}

void Manager::create_document_from_template(Template temp, std::string name) {
    auto doc = generator_->generate(temp, name);
    repMaster_->documentRepository_->add(doc);
}

void Manager::add_document(int template_id, std::string name) {
    for (auto x: repMaster_->documentRepository_->get_all()) {
        if (x.get_name() == name)
            throw std::invalid_argument("There is already exists document with this name🙂");
    }
    auto const &templ = repMaster_->templateRepository_->get(template_id);
    std::string filePath = "./templates/" + FileManager::getFileName(templ.get_path());
    FileManager::copyFile(templ.get_path(), filePath);
    generator_->generate(templ, name);
}

void Manager::add_document(std::string filepath, std::string name) {
    for (auto x: repMaster_->documentRepository_->get_all()) {
        if (x.get_name() == name)
            throw std::invalid_argument("There is already exists document with this name🙂");
    }
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

Template Manager::getTemplate(int id) {
    return repMaster_->templateRepository_->get(id);
}

Document Manager::getDocument(int id) {
    return repMaster_->documentRepository_->get(id);
}

void Manager::updateDocument(int id, Document document) {
    repMaster_->documentRepository_->update_by_id(id, document);
}

void Manager::updateTemplate(int id, Template temp) {
    repMaster_->templateRepository_->update_by_id(id, temp);
}

void Manager::create_template_form_blank(std::string name) {
    for (auto x: repMaster_->templateRepository_->get_all()) {
        if (x.get_name() == name)
            throw std::invalid_argument("There is already exists template with this name🙂");
    }

    std::string filePath = "./data/blank.docx";
    std::string filepath = "./template/" + name + ".docx";
    FileManager::copyFile(filePath, filepath);
    auto temp = Template(std::move(name), filePath);
    generator_->generate(temp);
    repMaster_->templateRepository_->add(temp);
}

void Manager::deleteDocument(int id) {
    repMaster_->documentRepository_->remove(id);
}

void Manager::deleteTemplate(int id) {
    repMaster_->templateRepository_->remove(id);
}

Manager::~Manager() = default;
