#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "../Interfaces/IDocument.h"
#include "Field.h"
#include <utility>
#include <vector>

class Template : public IDocument, public ISerializer<Template> {
    friend class Generator;

public:
    Template() = default;
    Template(std::string name, std::string filepath)
        : name_(std::move(name)), filepath_(std::move(filepath)) {}

    std::string get_name() const override { return name_; }
    void set_name(std::string name) { name_ = std::move(name); }
    std::string get_type() const override { return type_; }
    std::string get_path() const override { return filepath_; }
    void set_path(std::string path) { filepath_ = std::move(path); }
    int get_id() const { return id_; }
    void set_id(int id) { id_ = id; }

    Field get_field(int id) const;
    int get_fields_count() const { return static_cast<int>(fields_.size()); }
    std::vector<Field> get_fields() const { return fields_; }
    void set_field(int id, std::string content);

    std::string to_string() const override;
    Template from_string(const std::string &str) override;

private:
    std::string name_;
    std::string type_ = "Template";
    std::string filepath_;
    std::vector<Field> fields_;
    int id_ = 0;
};

#endif
