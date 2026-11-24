#ifndef DOCSMANAGER_RECORDJSON_H
#define DOCSMANAGER_RECORDJSON_H

#include "Field.h"
#include <string>
#include <vector>

namespace RecordJson {
struct Data {
    std::string name;
    std::string type;
    std::string path;
    std::vector<Field> fields;
};

Data decode(const std::string &serialized);
std::string encode(const std::string &name, const std::string &type, const std::string &path,
                   const std::vector<Field> &fields);
}

#endif
