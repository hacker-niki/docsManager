#pragma once

#include "Interfaces/IGenerator.h"

class Generator : public IGenerator {
public:
    explicit Generator(char fieldSymbol = '%') : fieldSymbol_(fieldSymbol) {}

    Document generate(const Template &source, const std::string &name) override;
    Document generateTo(const Template &source, const std::string &name,
                        const std::string &destination);
    void generate(Template &source) override;
    void generate(Document &document) override;

private:
    char fieldSymbol_;
};
