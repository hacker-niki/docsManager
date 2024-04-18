//
// Created by nikita on 05.04.24.
//

#ifndef DOCSMANAGER_FILEMANAGER_H
#define DOCSMANAGER_FILEMANAGER_H


#include <string>
#include <fstream>

class FileManager {

public:
    static bool copyFile(const std::string &sourcePath, const std::string &destinationPath);

    static std::string getFileName(const std::string &filePath);
    
    static std::string read_docx(std::string filePath);

    static bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
};

#endif //DOCSMANAGER_FILEMANAGER_H
