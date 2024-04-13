//
// Created by nikita on 22.03.24.
//

#include "../Entities/Document.h"
#include "TemplateRepository.h"

void TemplateRepository::add(const Template &temp) {
    templates_.push_back(temp);
}

Template &TemplateRepository::get(int id) {
    return templates_[id];
}

void TemplateRepository::remove(int id) {
    templates_.erase(std::next(templates_.begin(), id));
}

std::vector<Template> TemplateRepository::get_all() {
    return templates_;
}
