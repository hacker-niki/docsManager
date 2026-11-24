#include "Generator.h"
#include "DocxPackage.h"

#include <stdexcept>

Document Generator::generate(const Template &source, const std::string &name) {
    if (name.empty() || name == "." || name == ".." ||
        name.find_first_of("/\\:*?\"<>|") != std::string::npos ||
        name.find('\0') != std::string::npos) {
        throw std::invalid_argument("Недопустимое имя файла документа.");
    }
    return generateTo(source, name, "./documents/" + name + ".docx");
}

Document Generator::generateTo(const Template &source, const std::string &name,
                               const std::string &destination) {
    DocxPackage::render(source.get_path(), destination, fieldSymbol_, source.fields_);
    return Document(name, destination);
}

void Generator::generate(Template &source) {
    const auto names = DocxPackage::fieldNames(source.get_path(), fieldSymbol_);
    std::vector<Field> fields;
    fields.reserve(names.size());
    for (const auto &name : names) {
        fields.emplace_back(name);
    }
    source.fields_ = std::move(fields);
}

void Generator::generate(Document &document) {
    DocxPackage::validate(document.get_path());
}
