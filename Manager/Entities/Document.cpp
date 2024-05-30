//
// Created by nikita on 22.03.24.
//

#include <stdexcept>
#include "Document.h"
#include "../FileManager.h"

Document::Document(const std::string &name, const std::string &filepath) : type_("Document") {

    while (true) {
        this->name_ = name;
        if (!FileManager::fileExists(filepath))
            break;
        this->filepath_ = filepath;
        return;
    }
    throw std::invalid_argument("Document didn't created 🙂");
}

std::string Document::get_name() {
    return name_;
}

std::string Document::get_type() {
    return type_;
}

std::string Document::get_path() {
    return filepath_;
}

std::string Document::to_string() const {
    nlohmann::json j;
    j["name"] = name_;
    j["type"] = type_;
    j["filepath"] = filepath_;
    j["fields_"] = nlohmann::json::array();

    for (auto &field: fields_) {
        // Assuming Field class has a to_string method
        j["fields_"].push_back(field.to_string());
    }

    return j.dump();
}

Document Document::from_string(const std::string &str) {
    nlohmann::json j = nlohmann::json::parse(str);

    name_ = j["name"].get<std::string>();
    type_ = j["type"].get<std::string>();
    filepath_ = j["filepath"].get<std::string>();

    fields_.clear();

    for (const auto &field_json: j["fields_"]) {
        Field field;
        // Assuming Field class has a from_string method
        field.from_string(field_json.get<std::string>());
        fields_.push_back(field);
    }

    return *this;
}
