//
// Created by nikita on 22.03.24.
//

#include <stdexcept>
#include "../Entities/Document.h"
#include "TemplateRepository.h"

void TemplateRepository::add(const Template &temp) {
    templates_.push_back(temp);
}

Template &TemplateRepository::get(int id) {
    while (true) {
        if (id < 0 || id >= templates_.size())
            break;
        return templates_[id];
    }
    throw std::out_of_range("There's no template with such id🙂");
}

void TemplateRepository::remove(int id) {
    while (true) {
        if (id < 0 || id >= templates_.size())
            break;
        templates_.erase(std::next(templates_.begin(), id));
        return;
    }
    throw std::out_of_range("There's no template with such id🙂");

}

std::vector<Template> TemplateRepository::get_all() {
    return templates_;
}
