//
// Created by nikita on 05.04.24.
//

#include <filesystem>
#include <duckx/duckx.hpp>
#include "FileManager.h"

bool FileManager::copyFile(const std::string &sourcePath, const std::string &destinationPath) {
    {
        std::ifstream sourceFile(sourcePath, std::ios::binary);
        if (!sourceFile) {
//            std::cout << "Failed to open source file." << std::endl;
            return false;
        }

        std::ofstream destinationFile(destinationPath, std::ios::binary);
        if (!destinationFile) {
//            std::cout << "Failed to create destination file." << std::endl;
            return false;
        }

        destinationFile << sourceFile.rdbuf();

        if (sourceFile.fail()) {
//            std::cout << "Error occurred while reading from source file." << std::endl;
            return false;
        }

        if (destinationFile.fail()) {
//            std::cout << "Error occurred while writing to destination file." << std::endl;
            return false;
        }

        sourceFile.close();
        destinationFile.close();

        return true;


    }
}

std::string FileManager::getFileName(const std::string &filePath) {
    std::filesystem::path path(filePath);
    return path.filename().string();
}

std::string FileManager::read_docx(std::string filePath) {
    std::string ans;
    auto doc = duckx::Document(filePath);
    doc.open();
    for (auto p = doc.paragraphs(); p.has_next(); p.next()) {
        for (auto r = p.runs(); r.has_next(); r.next()) {
            ans += r.get_text();
        }
        ans += '\n';
    }
    return ans;
}
