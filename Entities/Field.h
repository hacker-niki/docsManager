//
// Created by nikita on 26.03.24.
//

#ifndef DOCSMANAGER_FIELD_H
#define DOCSMANAGER_FIELD_H

#include <string>

class Field {
public:
    explicit Field(const std::string &field) : name_(field), content_(field) {}

    std::string get_name() {
        return name_;
    }

    void set_content(const std::string &field_content) {
        this->content_ = field_content;
    }

    std::string get_content() {
        return this->content_;
    }

protected:
    std::string name_;
    std::string content_;
};


#endif //DOCSMANAGER_FIELD_H
