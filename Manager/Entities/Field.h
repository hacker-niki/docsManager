//
// Created by nikita on 26.03.24.
//

#ifndef DOCSMANAGER_FIELD_H
#define DOCSMANAGER_FIELD_H

#include <string>
#include <nlohmann/json.hpp>
#include "../Interfaces/ISerializer.h"
#include "Document.h"

class Field : public ISerializer<Field> {
public:
    Field() = default;

    explicit Field(const std::string &field) : name_(field), content_(field) {}

    std::string get_name() const {
        return name_;
    }

    void set_content(const std::string &field_content) {
        this->content_ = field_content;
    }

    std::string get_content() const {
        std::cout << content_;
        return this->content_;
    }

    std::string to_string() const override {
        nlohmann::json j;
        j["name"] = name_;
        j["content"] = content_;
        return j.dump();
    }

    Field from_string(const std::string &str) override {
        nlohmann::json j = nlohmann::json::parse(str);
        name_ = j["name"].get<std::string>();
        content_ = j["content"].get<std::string>();
        return *this;
    }

protected:
    std::string name_;
    std::string content_;
};

#endif //DOCSMANAGER_FIELD_H
