//
// Created by nikita on 22.03.24.
//

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "Document.h"


class Template : public Document {

private:
    std::vector<Field> fields;

public:
    Template(const std::string &name, const std::string &filepath);

    Field get_field(int id);

    int get_fields_count() {
        return static_cast<int>(fields.size());
    }

    std::vector<Field> get_fields();

    void set_field(int id, std::string content);

    friend class Generator;

};

#endif //TEMPLATE_H
