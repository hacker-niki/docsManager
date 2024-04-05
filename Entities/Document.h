//
// Created by nikita on 22.03.24.
//

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <vector>

#include "IDocument.h"
#include "Field.h"

class Document : public IDocument {
protected:
    std::string name;
    std::string type;
    std::string filepath;
    std::vector<Field> fields;


public:
    Document(const std::string &name, const std::string &filepath);

    virtual std::string get_name() override;

    virtual std::string get_type() override;

    virtual std::string get_path() override;

};

#endif //DOCUMENT_H
