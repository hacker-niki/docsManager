#include "FileManager.h"
#include "DocxPackage.h"

#include <QDir>
#include <QFileInfo>
#include <stdexcept>

bool FileManager::copyFile(const std::string &sourcePath, const std::string &destinationPath) {
    try {
        DocxPackage::copyNew(sourcePath, destinationPath);
        return true;
    } catch (const std::runtime_error &) {
        return false;
    }
}

std::string FileManager::getFileName(const std::string &filePath) {
    return QFileInfo(QString::fromStdString(filePath)).fileName().toStdString();
}

std::string FileManager::read_docx(const std::string &path) {
    return DocxPackage::readText(path);
}

void FileManager::validateDocx(const std::string &path) {
    DocxPackage::validate(path);
}

void FileManager::createBlankDocx(const std::string &path) {
    DocxPackage::createBlank(path);
}

bool FileManager::fileExists(const std::string &path) {
    return QFileInfo(QString::fromStdString(path)).isFile();
}

void FileManager::create_directory(const std::string &path) {
    if (!QDir().mkpath(QString::fromStdString(path))) {
        throw std::runtime_error("Не удалось создать каталог: " + path);
    }
}
