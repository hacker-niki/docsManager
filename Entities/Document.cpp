//
// Created by nikita on 22.03.24.
//

#include <stdexcept>
#include "Document.h"

Document::Document(const std::string &name, const std::string &filepath) {
    this->name = name;
    this->filepath = filepath;
    this->type = "Document";
}


std::string Document::get_name() {
    return name;
}

std::string Document::get_type() {
    return type;
}

std::string Document::get_path() {
    return filepath;
}


