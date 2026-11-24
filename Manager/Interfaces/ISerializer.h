#ifndef DOCSMANAGER_ISERIALIZER_H
#define DOCSMANAGER_ISERIALIZER_H

#include <string>

template<typename T>
class ISerializer {
public:
    virtual ~ISerializer() = default;
    virtual std::string to_string() const = 0;
    virtual T from_string(const std::string &str) = 0;
};

#endif
