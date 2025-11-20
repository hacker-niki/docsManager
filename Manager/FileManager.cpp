//
// Created by nikita on 05.04.24.
//

#include <filesystem>
#include <duckx/duckx.hpp>
#include "FileManager.h"

bool FileManager::copyFile(const std::string &sourcePath, const std::string &destinationPath) {
    while (true) {
        std::ifstream sourceFile(sourcePath, std::ios::binary);
        std::ofstream destinationFile(destinationPath, std::ios::binary);

        if (!destinationFile || !sourceFile) {
            break;
        }

        destinationFile << sourceFile.rdbuf();

        if (sourceFile.fail() || destinationFile.fail()) {
            break;
        }

        sourceFile.close();
        destinationFile.close();

        return true;
    }
    return false;
}

std::string FileManager::getFileName(const std::string &filePath) {
    try {
        std::filesystem::path path(filePath);
        return path.filename().string();
    } catch (...) {
        throw std::invalid_argument("Error while getting filename🙂");
    }
}

std::string FileManager::read_docx(std::string filePath) {
    int flag = 0;
    while (true) {
        std::string ans;

        if (!fileExists(filePath)) {
            flag = 1;
            break;
        }
        auto doc = duckx::Document(filePath);
        try {
            doc.open();

            for (auto p = doc.paragraphs(); p.has_next(); p.next()) {
                for (auto r = p.runs(); r.has_next(); r.next()) {
                    ans += r.get_text();
                }
                ans += '\n';
            }
            return ans;
        } catch (...) {
            flag = 2;
            break;
        }
    }
    switch (flag) {
        case 1:
            throw std::invalid_argument("Error while reading document: file don't exists🙂");
        case 2:
            throw std::invalid_argument("Error while reading document: file unreadable🙂");
        default:
            throw std::invalid_argument("Error while reading document: unhandled exception🙂");
    }
}

void FileManager::create_directory(std::string dirName) {
    if (!std::filesystem::is_directory(dirName) || !std::filesystem::exists(dirName)) {
        std::filesystem::create_directory(dirName);
    }
}
