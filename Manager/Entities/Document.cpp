#include "Document.h"
#include "RecordJson.h"
#include <filesystem>
#include <stdexcept>

Document::Document(const std::string &name, const std::string &filepath) : name_(name), filepath_(filepath) {
    if (!std::filesystem::is_regular_file(std::filesystem::u8path(filepath))) {
        throw std::invalid_argument("Document file does not exist: " + filepath);
    }
}

std::string Document::to_string() const {
    return RecordJson::encode(name_, type_, filepath_, fields_);
}

Document Document::from_string(const std::string &str) {
    auto data = RecordJson::decode(str);
    name_ = std::move(data.name);
    type_ = std::move(data.type);
    filepath_ = std::move(data.path);
    fields_ = std::move(data.fields);
    return *this;
}
