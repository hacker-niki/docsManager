#ifndef DOCSMANAGER_FIELD_H
#define DOCSMANAGER_FIELD_H

#include "../Interfaces/ISerializer.h"
#include <string>

class Field : public ISerializer<Field> {
public:
    Field() = default;
    explicit Field(const std::string &name) : name_(name), content_(name) {}

    std::string get_name() const { return name_; }
    std::string get_content() const { return content_; }
    void set_content(const std::string &content) { content_ = content; }

    std::string to_string() const override;
    Field from_string(const std::string &data) override;

private:
    std::string name_;
    std::string content_;
};

#endif
