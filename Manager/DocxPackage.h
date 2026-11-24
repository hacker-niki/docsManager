#pragma once

#include <string>
#include <vector>

class Field;

namespace DocxPackage {
// Called with the library lock held, for a destination recorded in the file journal.
void discardStagingFor(const std::string &destination);
void copyNew(const std::string &source, const std::string &destination);
void createBlank(const std::string &destination);
void validate(const std::string &path);
std::string readText(const std::string &path);
std::vector<std::string> fieldNames(const std::string &path, char symbol);
void render(const std::string &source, const std::string &destination, char symbol,
            const std::vector<Field> &fields);
}
