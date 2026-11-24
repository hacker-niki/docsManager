#pragma once

#include "../Entities/Document.h"
#include "../Entities/Template.h"

class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual Document generate(const Template &source, const std::string &name) = 0;
    virtual void generate(Template &source) = 0;
    virtual void generate(Document &document) = 0;
};
