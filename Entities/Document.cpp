//
// Created by nikita on 22.03.24.
//

#include <stdexcept>
#include "Document.h"
#include "../FileManager.h"

Document::Document(const std::string &name, const std::string &filepath) {

    while (true) {
        this->name_ = name;
        if (!FileManager::fileExists(filepath))
            break;
        this->filepath_ = filepath;
        this->type_ = "Document";
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
