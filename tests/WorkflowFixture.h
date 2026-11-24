#pragma once

#include "FileManager.h"
#include <zip.h>
#include <filesystem>
#include <stdexcept>

namespace workflow {
inline void require(bool result, const char *message) {
    if (!result) throw std::runtime_error(message);
}

inline void makeTemplate(const std::filesystem::path &path) {
    FileManager::createBlankDocx(path.u8string());
    const std::string xml = R"(<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"><w:body><w:p><w:r><w:t>Получатель: %%Имя%%</w:t></w:r></w:p></w:body></w:document>)";
    zip_t *archive = zip_open(path.u8string().c_str(), 0, nullptr);
    require(archive, "Fixture DOCX could not be opened");
    zip_source_t *part = zip_source_buffer(archive, xml.data(), xml.size(), 0);
    if (!part || zip_file_add(archive, "word/document.xml", part, ZIP_FL_OVERWRITE) < 0) {
        if (part) zip_source_free(part);
        zip_discard(archive);
        throw std::runtime_error("Fixture DOCX could not be edited");
    }
    if (zip_close(archive) < 0) {
        zip_discard(archive);
        throw std::runtime_error("Fixture DOCX could not be saved");
    }
}
}
