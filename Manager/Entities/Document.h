#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "../Interfaces/IDocument.h"
#include "Field.h"
#include <utility>
#include <vector>

class Document : public IDocument, public ISerializer<Document> {
public:
    Document() = default;
    Document(const std::string &name, const std::string &filepath);

    std::string get_name() const override { return name_; }
    void set_name(std::string name) { name_ = std::move(name); }
    std::string get_type() const override { return type_; }
    std::string get_path() const override { return filepath_; }
    void set_path(std::string path) { filepath_ = std::move(path); }
    int get_id() const { return id_; }
    void set_id(int id) { id_ = id; }

    std::string to_string() const override;
    Document from_string(const std::string &str) override;

private:
    std::string name_;
    std::string type_ = "Document";
    std::string filepath_;
    std::vector<Field> fields_;
    int id_ = 0;
};

#endif
