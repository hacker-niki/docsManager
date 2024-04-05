//
// Created by nikita on 22.03.24.
//

#include <duckx/duckx.hpp>
#include "Template.h"


void Template::set_field(int id, std::string content) {
    fields[id].set_content(content);
}

Template::Template(const std::string &name, const std::string &filepath) : Document(name, filepath) {}


Field Template::get_field(int id) {
    if (id < 0 || id > fields.size() - 1)
        throw std::out_of_range("OUT OF RANGE");
    return fields[id];
}


std::vector<Field> Template::get_fields() {
    return fields;
}

