#pragma once

#include <string>

class FileManager {
public:
    static bool copyFile(const std::string &sourcePath, const std::string &destinationPath);
    static std::string getFileName(const std::string &filePath);
    static std::string read_docx(const std::string &path);
    static void validateDocx(const std::string &path);
    static void createBlankDocx(const std::string &path);
    static bool fileExists(const std::string &path);
    static void create_directory(const std::string &path);
};
