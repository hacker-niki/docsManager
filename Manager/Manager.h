#pragma once

#include "Repository/RepMaster.h"

#include <filesystem>
#include <functional>
#include <memory>

class QLockFile;

class Manager {
public:
    Manager();
    explicit Manager(std::filesystem::path dataDirectory);
    ~Manager();

    std::vector<Document> get_documents();
    std::vector<Template> get_templates();
    Document getDocument(int id);
    Template getTemplate(int id);

    void create_template(std::string path, std::string name);
    void create_template_form_blank(std::string name);
    void create_document_from_template(Template temp, std::string name);
    void add_document(int templateId, std::string name);
    void add_document(std::string path, std::string name);

    void deleteDocument(int id);
    void deleteTemplate(int id);
    void updateDocument(int id, Document document);
    void updateTemplate(int id, Template temp);
    void renameDocument(int id, std::string name);
    void renameTemplate(int id, std::string name);

private:
    enum class FileChange { Add, Remove };
    void changeFile(const std::filesystem::path &file, FileChange kind, const std::function<void()> &change);
    void finishFileChange(const std::string &relative, bool justAdded = false);

    std::filesystem::path directory_;
    std::unique_ptr<QLockFile> lock_;
    std::unique_ptr<RepMaster> repositories_;
};
