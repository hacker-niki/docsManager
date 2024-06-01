//
// Created by nikita on 22.03.24.
//

#ifndef IGENERATOR_H
#define IGENERATOR_H

#include "../Entities/Document.h"
#include "../Entities/Template.h"

class IGenerator {
public:
//    ~IGenerator() = delete;

    virtual Document generate(const Template &aTemplate, const std::string &name) = 0;

    virtual void generate(Template &aTemplate) = 0;

    virtual void generate(Document &aDocument) = 0;
};

#endif //IGENERATOR_H
