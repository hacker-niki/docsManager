//
// Created by nikita on 22.03.24.
//

#include <duckx/duckx.hpp>
#include "Template.h"


void Template::set_field(int id, std::string content) {
    while (true) {
        if (id < 0 || id >= fields.size())
            break;
        fields[id].set_content(content);
        return;
    }
    throw std::out_of_range("There's no field with such id🙂");
}

Template::Template(const std::string &name, const std::string &filepath) : Document(name, filepath) {}


Field Template::get_field(int id) {
    while(true) {
        if (id < 0 || id >= fields.size())
            break;
        return fields[id];
    }
    throw std::out_of_range("There's no field with such id🙂");
}


std::vector<Field> Template::get_fields() {
    return fields;
}