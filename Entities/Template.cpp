//
// Created by nikita on 22.03.24.
//

#include <duckx/duckx.hpp>
#include "Template.h"


void Template::set_field(int id, std::string content) {
    while (true) {
        if (id < 0 || id >= fields_.size())
            break;
        fields_[id].set_content(content);
        return;
    }
    throw std::out_of_range("There's no field with such id🙂");
}


Field Template::get_field(int id) {
    while (true) {
        if (id < 0 || id >= fields_.size())
            break;
        return fields_[id];
    }
    throw std::out_of_range("There's no field with such id🙂");
}


std::vector<Field> Template::get_fields() {
    return fields_;
}

Template Template::from_string(const std::string &str) {
    nlohmann::json j = nlohmann::json::parse(str);

    name_ = j["name"].get<std::string>();
    type_ = j["type"].get<std::string>();
    filepath_ = j["filepath"].get<std::string>();

    fields_.clear();

    for (const auto &field_json: j["fields_"]) {
        Field field("");
        // Assuming Field class has a from_string method
        field.from_string(field_json.get<std::string>());
        fields_.push_back(field);
    }

    return *this;
}

std::string Template::to_string() const {
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
