#include "DocxPackage.h"
#include "FileManager.h"
#include "Generator.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <zip.h>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const char *message) {
    if (!condition) throw std::runtime_error(message);
}

template<typename Action>
void mustFail(Action action, const char *message) {
    try {
        action();
    } catch (const std::exception &) {
        return;
    }
    throw std::runtime_error(message);
}

void putPart(const std::string &path, const char *part, const std::string &bytes) {
    std::unique_ptr<zip_t, decltype(&zip_discard)> archive(zip_open(path.c_str(), 0, nullptr), zip_discard);
    require(bool(archive), "Cannot open test archive");
    auto source = zip_source_buffer(archive.get(), bytes.data(), bytes.size(), 0);
    require(source, "Cannot create test source");
    if (zip_file_add(archive.get(), part, source, ZIP_FL_OVERWRITE) < 0) {
        zip_source_free(source);
        throw std::runtime_error("Cannot add test part");
    }
    require(zip_close(archive.get()) == 0, "Cannot save test archive");
    archive.release();
}

std::string getPart(const std::string &path, const char *part) {
    std::unique_ptr<zip_t, decltype(&zip_discard)> archive(zip_open(path.c_str(), ZIP_RDONLY, nullptr), zip_discard);
    require(bool(archive), "Cannot open test result");
    zip_stat_t info{};
    require(zip_stat(archive.get(), part, 0, &info) == 0, "Missing result part");
    std::unique_ptr<zip_file_t, decltype(&zip_fclose)> entry(zip_fopen(archive.get(), part, 0), zip_fclose);
    std::string result(info.size, '\0');
    require(entry && zip_fread(entry.get(), result.data(), info.size) == static_cast<zip_int64_t>(info.size),
            "Cannot read result part");
    return result;
}

std::string body(const std::string &contents) {
    return R"(<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"><w:body>)" +
           contents + R"(</w:body></w:document>)";
}

void checkRecovery(const QTemporaryDir &library) {
    const auto fileName = QStringLiteral("pending.docx");
    const auto destination = library.filePath(fileName).toStdString();
    const auto digest = QCryptographicHash::hash(fileName.toUtf8(), QCryptographicHash::Sha256).toHex();
    QDir stage(library.filePath(".docsmanager-" + QString::fromLatin1(digest) + "-stage"));
    require(QDir().mkdir(stage.path()) &&
            QFile::setPermissions(stage.path(), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner),
            "Cannot create interrupted staging fixture");
    auto touch = [&](const QString &name) {
        QFile f(stage.filePath(name));
        require(f.open(QIODevice::WriteOnly | QIODevice::NewOnly), "Cannot create recovery fixture file");
    };
    touch("staged.docx");
    DocxPackage::discardStagingFor(destination);
    require(stage.exists("staged.docx"), "Recovery removed files without an ownership marker");
    touch(".docsmanager-stage-v1");
    touch("staged.docx.AbC123.part");
    touch("unrelated.txt");
    DocxPackage::discardStagingFor(destination);
    require(stage.exists("unrelated.txt") && stage.exists("staged.docx"), "Recovery removed unrelated files");
    require(stage.remove("unrelated.txt"), "Could not prepare recovery fixture");
    DocxPackage::discardStagingFor(library.filePath("different.docx").toStdString());
    require(stage.exists("staged.docx"), "Recovery touched a different operation");
    DocxPackage::discardStagingFor(destination);
    require(!stage.exists(), "Interrupted copy and libzip intermediate survived recovery");
}

void run() {
    QTemporaryDir temp;
    require(temp.isValid(), "Cannot create test directory");
    checkRecovery(temp);
    auto path = [&](const char *name) { return temp.filePath(name).toStdString(); };
    const auto source = path("исходник.docx");
    const auto output = path("result.docx");
    const auto invalid = path("invalid.docx");
    FileManager::createBlankDocx(source);
    require(FileManager::read_docx(source) == "\n", "Blank Word document is unreadable");
    mustFail([&] { FileManager::createBlankDocx(source); }, "Blank document overwrote an existing file");

    const std::string original = body(
        "<w:p><w:r><w:rPr><w:b/></w:rPr><w:t>До %</w:t></w:r>"
        "<w:r><w:t></w:t><w:t>%имя%</w:t></w:r><w:r><w:t>%%%дата%%!</w:t></w:r></w:p>"
        "<w:tbl><w:tr><w:tc><w:p><w:r><w:t>%%имя%%</w:t></w:r></w:p></w:tc></w:tr></w:tbl>"
        "<w:p><w:r><w:t>Конец</w:t><w:tab/><w:t>✓</w:t></w:r></w:p>");
    putPart(source, "word/document.xml", original);
    const std::string image("binary\0image", 12);
    putPart(source, "word/media/thumbnail.bin", image);
    Generator generator;
    Template form("Форма", source);
    generator.generate(form);
    generator.generate(form);
    require(form.get_fields_count() == 3 && form.get_field(0).get_name() == "имя" &&
            form.get_field(1).get_name() == "дата" && form.get_field(2).get_name() == "имя",
            "Wrong order, duplication or split-run field parsing");
    form.set_field(0, " &<Иван> ");
    form.set_field(1, "%%дата%%");
    form.set_field(2, "");
    auto result = generator.generateTo(form, "Документ", output);
    require(result.get_path() == output && result.get_name() == "Документ", "Wrong generated metadata");
    require(FileManager::read_docx(output) == "До  &<Иван> %%дата%%!\n\nКонец\t✓\n",
            "Adjacent/split placeholders, XML escaping or table text changed");
    const auto generatedXml = getPart(output, "word/document.xml");
    require(generatedXml.find("xml:space=\"preserve\"") != std::string::npos &&
            generatedXml.find("<w:b/>") != std::string::npos, "Run properties or spaces lost");
    require(getPart(output, "word/media/thumbnail.bin") == image, "Untouched package part changed");
    require(getPart(source, "word/document.xml") == original, "Source template changed");
    mustFail([&] { generator.generateTo(form, "Existing", output); }, "Generation overwrote an existing file");
    require(!FileManager::copyFile(source, output), "Import overwrote an existing file");
    require(!FileManager::copyFile(invalid, path("copy.docx")) && !FileManager::fileExists(path("copy.docx")),
            "Failed copy left destination");
    require(FileManager::copyFile(output, path("copy.docx")), "Import of a valid document failed");

    form.set_field(0, std::string("abc\0d", 5));
    mustFail([&] { generator.generateTo(form, "Invalid", invalid); }, "Accepted an invalid XML field value");
    require(!FileManager::fileExists(invalid), "Invalid value left partial output");
    putPart(source, "word/document.xml", body("<w:p><w:r><w:t>%%other%%</w:t></w:r></w:p>"));
    mustFail([&] { generator.generateTo(form, "Obsolete", invalid); }, "Obsolete field list accepted");
    require(!FileManager::fileExists(invalid), "Failed generation left partial output");

    putPart(source, "word/document.xml", body("<w:p><w:r><w:t>%%unclosed</w:t></w:r></w:p>"));
    mustFail([&] { generator.generate(form); }, "Unclosed placeholder accepted");
    require(form.get_fields_count() == 3, "Parsing error changed loaded fields");
    std::string tooMany;
    for (int i = 0; i < 513; ++i) tooMany += "%%x%%";
    putPart(source, "word/document.xml", body("<w:p><w:r><w:t>" + tooMany + "</w:t></w:r></w:p>"));
    mustFail([&] { generator.generate(form); }, "Template field count limit ignored");
    putPart(source, "word/document.xml", "<broken");
    mustFail([&] { FileManager::validateDocx(source); }, "Malformed document accepted");
    putPart(source, "word/document.xml", "<!DOCTYPE document>" + body("<w:p/>"));
    mustFail([&] { FileManager::validateDocx(source); }, "DTD in main part accepted");
    putPart(source, "word/document.xml", body("<w:p/>"));
    putPart(source, "[Content_Types].xml", "<Types/>");
    mustFail([&] { FileManager::validateDocx(source); }, "Non-Word ZIP accepted");
    require(QDir(temp.path()).entryList({".docsmanager-*"}, QDir::Hidden | QDir::Dirs).isEmpty(),
            "Failed operations left staged files");
}
}

int main() {
    try {
        run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
