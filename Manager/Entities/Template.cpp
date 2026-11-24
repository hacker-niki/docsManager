#include "Template.h"
#include "RecordJson.h"

void Template::set_field(int id, std::string content) {
    fields_.at(static_cast<std::size_t>(id)).set_content(content);
}

Field Template::get_field(int id) const {
    return fields_.at(static_cast<std::size_t>(id));
}

std::string Template::to_string() const {
    return RecordJson::encode(name_, type_, filepath_, fields_);
}

Template Template::from_string(const std::string &str) {
    auto data = RecordJson::decode(str);
    name_ = std::move(data.name);
    type_ = std::move(data.type);
    filepath_ = std::move(data.path);
    fields_ = std::move(data.fields);
    return *this;
}
