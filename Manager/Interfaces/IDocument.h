//
// Created by nikita on 22.03.24.
//

#ifndef IDOCUMENT_H
#define IDOCUMENT_H

#include <string>

class IDocument {
public:

    virtual ~IDocument() = default;
    
    virtual std::string get_name() = 0;

    virtual std::string get_type() = 0;

    virtual std::string get_path() = 0;
};

#endif //IDOCUMENT_H
