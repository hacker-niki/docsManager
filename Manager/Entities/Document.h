//
// Created by nikita on 22.03.24.
//

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <vector>
#include <iostream>

#include "../Interfaces/IDocument.h"
#include "Field.h"
#include "../Interfaces/ISerializer.h"

class Document : public IDocument, public ISerializer<Document> {
protected:
    std::string name_;
    std::string type_;
    std::string filepath_;
    std::vector<Field> fields_;
    int id;

public:
    Document() : type_("Document") {}

    Document(const std::string &name, const std::string &filepath);

    std::string get_name() override;

    std::string get_type() override;

    std::string get_path() override;

    std::string to_string() const override;

    int get_id() const { return this->id; };

    void set_id(int i) { 
        std::cout<<i;
        this->id = i; }

    Document from_string(const std::string &str) override;
};

#endif //DOCUMENT_H
