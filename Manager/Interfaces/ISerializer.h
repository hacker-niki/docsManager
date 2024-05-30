//
// Created by nikita on 21.05.24.
//

#ifndef DOCSMANAGER_ISERIALIZER_H
#define DOCSMANAGER_ISERIALIZER_H

#include <string>
#include "../Entities/Document.h"

template<typename T>
class ISerializer {
    virtual std::string to_string() const = 0;

    virtual T from_string(const std::string &str) = 0;
};

#endif //DOCSMANAGER_ISERIALIZER_H
