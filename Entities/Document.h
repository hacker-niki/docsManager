//
// Created by nikita on 22.03.24.
//

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <vector>

#include "../Interfaces/IDocument.h"
#include "Field.h"

class Document : public IDocument {
protected:
    std::string name_;
    std::string type_;
    std::string filepath_;
    std::vector<Field> fields_;


public:
    Document(const std::string &name, const std::string &filepath);

    virtual std::string get_name() override;

    virtual std::string get_type() override;

    virtual std::string get_path() override;

};

#endif //DOCUMENT_H
