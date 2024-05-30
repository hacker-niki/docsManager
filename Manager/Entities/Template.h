//
// Created by nikita on 22.03.24.
//

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <utility>

#include "Document.h"


class Template : public IDocument, public ISerializer<Template> {

private:
    std::string name_;
    std::string type_;
    std::string filepath_;
    std::vector<Field> fields_;

public:

    Template() : type_("Template") {}

    Template(std::string name, std::string filepath) : name_(std::move(name)), filepath_(std::move(filepath)),
                                                       type_("Template") {}

    std::string get_name() override {
        return name_;
    }

    std::string get_type() override {
        return type_;
    }

    std::string get_path() override {
        return filepath_;
    }

    Field get_field(int id);

    int get_fields_count() {
        return static_cast<int>(fields_.size());
    }

    std::vector<Field> get_fields();

    void set_field(int id, std::string content);

    friend class Generator;

    std::string to_string() const override;

    Template from_string(const std::string &str) override;

};

#endif //TEMPLATE_H
