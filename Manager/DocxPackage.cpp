#include "DocxPackage.h"
#include "Entities/Field.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#ifdef Q_OS_UNIX
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <libxml/chvalid.h>
#include <libxml/parser.h>
#include <zip.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace {
constexpr zip_uint64_t maxArchiveBytes = 128ULL * 1024 * 1024;
constexpr zip_uint64_t maxXmlBytes = 16ULL * 1024 * 1024;
constexpr size_t maxFields = 512;
constexpr char wordNs[] = "http://schemas.openxmlformats.org/wordprocessingml/2006/main";
constexpr char strictWordNs[] = "http://purl.oclc.org/ooxml/wordprocessingml/main";
constexpr char relNs[] = "http://schemas.openxmlformats.org/package/2006/relationships";
constexpr char typesNs[] = "http://schemas.openxmlformats.org/package/2006/content-types";
constexpr char officeRel[] =
    "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument";
constexpr char strictOfficeRel[] =
    "http://purl.oclc.org/ooxml/officeDocument/relationships/officeDocument";
constexpr char mainType[] =
    "application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml";

using Archive = std::unique_ptr<zip_t, decltype(&zip_discard)>;
using Xml = std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)>;
using XmlString = std::unique_ptr<xmlChar, xmlFreeFunc>;

void checkSize(const QString &source) {
    const QFileInfo file(source);
    if (!file.isFile() || file.size() < 0 ||
        static_cast<zip_uint64_t>(file.size()) > maxArchiveBytes) {
        throw std::runtime_error("Файл DOCX отсутствует или превышает 128 МиБ.");
    }
}

constexpr char stageMarker[] = ".docsmanager-stage-v1";

QDir stagingDirectory(const QFileInfo &target) {
    const auto key = QCryptographicHash::hash(target.fileName().toUtf8(), QCryptographicHash::Sha256);
    return QDir(target.absoluteDir().filePath(".docsmanager-" + QString::fromLatin1(key.toHex()) + "-stage"));
}

bool createPrivateDirectory(const QDir &dir) {
#ifdef Q_OS_UNIX
    return ::mkdir(QFile::encodeName(dir.absolutePath()).constData(), S_IRWXU) == 0;
#else
    return QDir().mkdir(dir.absolutePath());
#endif
}

template<typename Writer>
void writeNew(const std::string &destination, Writer write) {
    const QFileInfo target(QString::fromStdString(destination));
    if (target.exists() || target.isSymLink()) {
        throw std::runtime_error("Файл назначения уже существует.");
    }
    if (QFileInfo(target.absolutePath()).isSymLink()) {
        throw std::runtime_error("Каталог назначения не должен быть символической ссылкой.");
    }
    const auto stage = stagingDirectory(target);
    if (!createPrivateDirectory(stage)) {
        throw std::runtime_error("Не удалось создать временный каталог рядом с файлом назначения.");
    }
    try {
        QFile marker(stage.filePath(stageMarker));
        if (!marker.open(QIODevice::WriteOnly | QIODevice::NewOnly)) {
            throw std::runtime_error("Не удалось отметить временный каталог документа.");
        }
        marker.close();
        const auto temporary = stage.filePath("staged.docx");
        write(temporary);
        QFile staged(temporary);
        if (!staged.rename(target.absoluteFilePath())) {
            throw std::runtime_error("Не удалось сохранить файл: " + staged.errorString().toStdString());
        }
    } catch (...) {
        DocxPackage::discardStagingFor(destination);
        throw;
    }
    DocxPackage::discardStagingFor(destination);
}

void copy(const QString &source, const QString &destination) {
    if (!QFileInfo(source).isFile()) {
        throw std::runtime_error("Исходный файл не найден.");
    }
    QFile input(source);
    QFile output(destination);
    if (!input.open(QIODevice::ReadOnly) || !output.open(QIODevice::WriteOnly | QIODevice::NewOnly) ||
        !output.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
        throw std::runtime_error("Не удалось открыть файл для копирования.");
    }
    char block[65536];
    qint64 count;
    while ((count = input.read(block, sizeof block)) > 0) {
        if (output.write(block, count) != count) throw std::runtime_error("Ошибка записи копии файла.");
    }
    if (count < 0 || !output.flush()) throw std::runtime_error("Не удалось завершить копирование файла.");
}

Archive openZip(const std::string &path, int flags) {
    int error = 0;
    Archive archive(zip_open(path.c_str(), flags, &error), zip_discard);
    if (!archive) {
        throw std::runtime_error("Не удалось открыть DOCX (ошибка ZIP " +
                                 std::to_string(error) + ").");
    }
    return archive;
}

void finish(Archive &archive) {
    if (zip_close(archive.get()) < 0) {
        throw std::runtime_error("Не удалось сохранить DOCX: " + std::string(zip_strerror(archive.get())));
    }
    archive.release(); // zip_close consumes the archive on success.
}

// libzip borrows the supplied bytes until zip_close.
void setPart(zip_t *archive, const char *name, std::string_view bytes) {
    auto source = zip_source_buffer(archive, bytes.data(), bytes.size(), 0);
    if (!source) {
        throw std::runtime_error("Не удалось подготовить данные для записи в DOCX.");
    }
    if (zip_file_add(archive, name, source, ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8) < 0) {
        zip_source_free(source);
        throw std::runtime_error("Не удалось записать часть DOCX: " + std::string(name));
    }
}

Xml readXml(zip_t *archive, const std::string &name) {
    zip_stat_t info;
    zip_stat_init(&info);
    if (zip_stat(archive, name.c_str(), 0, &info) < 0 || !(info.valid & ZIP_STAT_SIZE) ||
        info.size == 0 || info.size > maxXmlBytes) {
        throw std::runtime_error("Часть DOCX отсутствует, пуста или превышает 16 МиБ: " + name);
    }
    std::unique_ptr<zip_file_t, decltype(&zip_fclose)> entry(
        zip_fopen(archive, name.c_str(), 0), zip_fclose);
    std::string data(info.size, '\0');
    if (!entry || zip_fread(entry.get(), data.data(), info.size) != static_cast<zip_int64_t>(info.size)) {
        throw std::runtime_error("Не удалось распаковать часть DOCX: " + name);
    }
    char extra;
    if (zip_fread(entry.get(), &extra, 1) != 0) {
        throw std::runtime_error("Повреждённая часть DOCX: " + name);
    }
    Xml doc(xmlReadMemory(data.data(), static_cast<int>(data.size()), nullptr, nullptr,
                          XML_PARSE_NONET | XML_PARSE_NOERROR | XML_PARSE_NOWARNING), xmlFreeDoc);
    if (!doc || doc->intSubset || doc->extSubset) {
        throw std::runtime_error("Некорректный XML в части DOCX: " + name);
    }
    return doc;
}

bool element(xmlNode *node, const char *name, const char *ns) {
    return node && node->type == XML_ELEMENT_NODE && node->ns &&
           xmlStrEqual(node->name, BAD_CAST name) && xmlStrEqual(node->ns->href, BAD_CAST ns);
}

bool word(xmlNode *node, const char *name) {
    return element(node, name, wordNs) || element(node, name, strictWordNs);
}

std::string property(xmlNode *node, const char *name) {
    XmlString value(xmlGetNoNsProp(node, BAD_CAST name), xmlFree);
    return value ? reinterpret_cast<const char *>(value.get()) : "";
}

std::string mainPart(zip_t *archive) {
    const auto relationships = readXml(archive, "_rels/.rels");
    auto root = xmlDocGetRootElement(relationships.get());
    if (!element(root, "Relationships", relNs)) {
        throw std::runtime_error("Некорректные связи в DOCX.");
    }
    std::string part;
    for (auto rel = root->children; rel; rel = rel->next) {
        if (!element(rel, "Relationship", relNs)) continue;
        const auto type = property(rel, "Type");
        if (type != officeRel && type != strictOfficeRel) continue;
        if (!part.empty() || property(rel, "TargetMode") == "External") {
            throw std::runtime_error("Некорректная ссылка на основной документ DOCX.");
        }
        part = property(rel, "Target");
        if (!part.empty() && part.front() == '/') part.erase(0, 1);
    }
    const auto types = readXml(archive, "[Content_Types].xml");
    root = xmlDocGetRootElement(types.get());
    if (part.empty() || !element(root, "Types", typesNs)) {
        throw std::runtime_error("Отсутствует основная часть DOCX.");
    }
    for (auto type = root->children; type; type = type->next) {
        if (element(type, "Override", typesNs) && property(type, "PartName") == "/" + part &&
            property(type, "ContentType") == mainType) {
            return part;
        }
    }
    throw std::runtime_error("Выберите документ Word в формате .docx.");
}

struct Package {
    Archive archive;
    std::string part;
    Xml document;
    xmlNode *body;
};

Package load(const std::string &path, bool writable = false) {
    checkSize(QString::fromStdString(path));
    auto archive = openZip(path, ZIP_CHECKCONS | (writable ? 0 : ZIP_RDONLY));
    auto part = mainPart(archive.get());
    auto doc = readXml(archive.get(), part);
    auto root = xmlDocGetRootElement(doc.get());
    if (word(root, "document")) {
        for (auto node = root->children; node; node = node->next) {
            if (word(node, "body")) {
                return {std::move(archive), std::move(part), std::move(doc), node};
            }
        }
    }
    throw std::runtime_error("Некорректная основная часть документа Word.");
}

using Runs = std::vector<xmlNode *>;
using Visitor = std::function<void(const Runs &, char)>;

// Word may split a single visible marker over several runs, even within a word.
// Paragraph ends, tabs and line breaks are visible boundaries.
void visit(xmlNode *node, Runs &runs, const Visitor &apply) {
    auto flush = [&](char separator) {
        apply(runs, separator);
        runs.clear();
    };
    for (; node; node = node->next) {
        if (word(node, "t")) {
            runs.push_back(node);
        } else if (word(node, "tab")) {
            flush('\t');
        } else if (word(node, "br") || word(node, "cr")) {
            flush('\n');
        } else if (word(node, "p")) {
            flush(0);
            visit(node->children, runs, apply);
            flush('\n');
        } else if (!word(node, "del")) {
            visit(node->children, runs, apply);
        }
    }
}

std::string textOf(xmlNode *node) {
    XmlString text(xmlNodeGetContent(node), xmlFree);
    return text ? reinterpret_cast<const char *>(text.get()) : "";
}

void setText(xmlNode *node, const std::string &text) {
    xmlNodeSetContent(node, nullptr);
    xmlAddChild(node, xmlNewTextLen(BAD_CAST text.data(), static_cast<int>(text.size())));
    auto xmlNs = xmlSearchNsByHref(node->doc, node, XML_XML_NAMESPACE);
    xmlSetNsProp(node, xmlNs, BAD_CAST "space", BAD_CAST "preserve");
}

void checkValue(const std::string &value) {
    for (size_t i = 0; i < value.size();) {
        int length = static_cast<int>(std::min<size_t>(4, value.size() - i));
        int code = xmlGetUTF8Char(BAD_CAST (value.data() + i), &length);
        if (code < 0 || !xmlIsCharQ(static_cast<unsigned>(code))) {
            throw std::runtime_error("Значение поля содержит недопустимый символ XML.");
        }
        i += static_cast<size_t>(length);
    }
}

struct Replacement {
    size_t start;
    size_t end;
    std::string value;
};

void replaceRuns(const Runs &runs, const std::vector<size_t> &ends, const std::string &text,
                 const std::vector<Replacement> &changes) {
    size_t next = 0;
    size_t start = 0;
    for (size_t i = 0; i < runs.size() && next < changes.size(); ++i) {
        const auto end = ends[i];
        if (end > start && changes[next].start < end) {
            std::string updated;
            auto cursor = start;
            while (next < changes.size() && changes[next].start < end) {
                const auto &change = changes[next];
                if (change.start >= start) {
                    updated.append(text, cursor, change.start - cursor);
                    updated += change.value;
                }
                cursor = std::min(end, change.end);
                if (change.end > end) break;
                ++next;
            }
            updated.append(text, cursor, end - cursor);
            setText(runs[i], updated);
        }
        start = end;
    }
}

std::vector<std::string> process(Package &package, char symbol, const std::vector<Field> *fields) {
    const std::string delimiter(2, symbol);
    std::vector<std::string> names;
    size_t valueBytes = 0;
    Runs runs;
    visit(package.body, runs, [&](const Runs &group, char) {
        if (group.empty()) return;
        std::string text;
        std::vector<size_t> ends;
        for (auto node : group) {
            text += textOf(node);
            ends.push_back(text.size());
        }
        std::vector<Replacement> changes;
        size_t start = 0;
        while ((start = text.find(delimiter, start)) != std::string::npos) {
            const auto close = text.find(delimiter, start + 2);
            if (close == std::string::npos || close == start + 2 || close - (start + 2) > 256) {
                throw std::runtime_error("Проверьте парные разделители поля и длину имени (1–256 байт UTF-8).");
            }
            if (names.size() == maxFields) {
                throw std::runtime_error("В шаблоне допустимо до 512 полей.");
            }
            auto name = text.substr(start + 2, close - (start + 2));
            if (fields) {
                if (names.size() >= fields->size() || (*fields)[names.size()].get_name() != name) {
                    throw std::runtime_error("Список полей изменился. Импортируйте актуальный шаблон.");
                }
                auto value = (*fields)[names.size()].get_content();
                valueBytes += value.size();
                if (valueBytes > maxXmlBytes) throw std::runtime_error("Суммарный размер значений полей превышает 16 МиБ.");
                checkValue(value);
                changes.push_back({start, close + 2, std::move(value)});
            }
            names.push_back(std::move(name));
            start = close + 2;
        }
        if (!changes.empty()) replaceRuns(group, ends, text, changes);
    });
    if (fields && names.size() != fields->size()) {
        throw std::runtime_error("Список полей изменился. Импортируйте актуальный шаблон.");
    }
    return names;
}
}

void DocxPackage::discardStagingFor(const std::string &destination) {
    const QFileInfo target(QString::fromStdString(destination));
    const auto dir = stagingDirectory(target);
    const QFileInfo stage(dir.absolutePath());
    if (QFileInfo(target.absolutePath()).isSymLink() || !stage.isDir() || stage.isSymLink()) return;
#ifdef Q_OS_UNIX
    const auto shared = QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup |
                        QFile::ReadOther | QFile::WriteOther | QFile::ExeOther;
    if (stage.ownerId() != geteuid() || (stage.permissions() & shared)) return;
#endif
    // libzip versions use staged.docx.XXXXXX or staged.docx.XXXXXX.part during ZIP publication.
    static const QRegularExpression payload(QStringLiteral(R"(^staged\.docx(?:\.[A-Za-z0-9]{6}(?:\.part)?)?$)"));
    const auto entries = dir.entryInfoList(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    QStringList files;
    QString marker;
    for (const auto &entry : entries) {
        if (!entry.isFile() || entry.isSymLink()) return;
        if (entry.fileName() == stageMarker && entry.size() == 0) {
            marker = entry.absoluteFilePath();
        } else if (payload.match(entry.fileName()).hasMatch()) {
            files.push_back(entry.absoluteFilePath());
        } else {
            return;
        }
    }
    // An interruption between mkdir and marker creation can leave an empty directory.
    if (!entries.isEmpty() && marker.isEmpty()) return;
    if (!marker.isEmpty()) files.push_back(marker);
    for (const auto &file : files) {
        if (!QFile::remove(file)) throw std::runtime_error("Не удалось убрать временный файл.");
    }
    if (!target.absoluteDir().rmdir(stage.fileName())) throw std::runtime_error("Не удалось убрать временный каталог.");
}

void DocxPackage::copyNew(const std::string &source, const std::string &destination) {
    writeNew(destination, [&](const QString &temporary) {
        copy(QString::fromStdString(source), temporary);
    });
}

void DocxPackage::validate(const std::string &path) {
    load(path);
}

std::string DocxPackage::readText(const std::string &path) {
    auto package = load(path);
    std::string result;
    Runs runs;
    visit(package.body, runs, [&](const Runs &group, char separator) {
        for (auto node : group) result += textOf(node);
        if (separator) result += separator;
    });
    return result;
}

std::vector<std::string> DocxPackage::fieldNames(const std::string &path, char symbol) {
    auto package = load(path);
    return process(package, symbol, nullptr);
}

void DocxPackage::render(const std::string &source, const std::string &destination, char symbol,
                         const std::vector<Field> &fields) {
    checkSize(QString::fromStdString(source));
    writeNew(destination, [&](const QString &temporary) {
        copy(QString::fromStdString(source), temporary);
        auto package = load(temporary.toStdString(), true);
        process(package, symbol, &fields);
        if (fields.empty()) return;

        xmlChar *bytes = nullptr;
        int length = 0;
        xmlDocDumpMemoryEnc(package.document.get(), &bytes, &length, "UTF-8");
        XmlString serialized(bytes, xmlFree);
        if (!serialized || length <= 0 || static_cast<unsigned>(length) > maxXmlBytes) {
            throw std::runtime_error("Не удалось сохранить основную часть DOCX в пределах 16 МиБ.");
        }
        setPart(package.archive.get(), package.part.c_str(),
                {reinterpret_cast<const char *>(serialized.get()), static_cast<size_t>(length)});
        finish(package.archive);
    });
}

void DocxPackage::createBlank(const std::string &destination) {
    static constexpr char types[] = R"(<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types"><Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/><Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/></Types>)";
    static constexpr char rels[] = R"(<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships"><Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/></Relationships>)";
    static constexpr char document[] = R"(<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"><w:body><w:p/><w:sectPr/></w:body></w:document>)";
    writeNew(destination, [&](const QString &temporary) {
        auto archive = openZip(temporary.toStdString(), ZIP_CREATE | ZIP_EXCL);
        setPart(archive.get(), "[Content_Types].xml", types);
        setPart(archive.get(), "_rels/.rels", rels);
        setPart(archive.get(), "word/document.xml", document);
        finish(archive);
    });
}
