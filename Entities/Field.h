//
// Created by nikita on 26.03.24.
//

#ifndef DOCSMANAGER_FIELD_H
#define DOCSMANAGER_FIELD_H

#include <string>

class Field {
public:
    Field(const std::string &field);

    std::string get_name() {
        return name;
    }

    void set_content(const std::string &field_content) {
        this->content = field_content;
    }

    std::string get_content() {
        return this->content;
    }

protected:
    std::string name;
    std::string content;
};


#endif //DOCSMANAGER_FIELD_H
