#include "Manager.h"
#include "FileManager.h"
#include "DocxPackage.h"
#include "Generator.h"

#include <QDebug>
#include <QLockFile>
#include <QStandardPaths>
#include <QString>
#include <QUuid>

#include <algorithm>
#include <stdexcept>

namespace fs = std::filesystem;

namespace {
std::string checkedName(const std::string &input) {
    const QString name = QString::fromStdString(input).trimmed();
    if (name.isEmpty() || name == "." || name == ".." || name.size() > 160 ||
        name.contains('/') || name.contains('\\') ||
        std::any_of(name.cbegin(), name.cend(), [](QChar c) {
            return c.category() == QChar::Other_Control;
        })) {
        throw std::invalid_argument("Введите название до 160 символов без разделителей пути и управляющих символов.");
    }
    return name.toStdString();
}

fs::path checkedDirectory(const fs::path &root, const char *subdir) {
    const auto path = root / subdir;
    if (fs::is_symlink(path)) {
        throw std::runtime_error("Папка файлов библиотеки не должна быть символической ссылкой.");
    }
    fs::create_directories(path);
    return path;
}

fs::path newFile(const fs::path &root, const char *subdir, const std::string &extension) {
    return checkedDirectory(root, subdir) /
           (QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString() + extension);
}

std::string storedPath(const fs::path &root, const std::string &value) {
    const auto path = fs::u8path(value).lexically_normal();
    const auto relative = path.lexically_relative(root);
    if (!relative.empty() && *relative.begin() != "..") {
        return relative.generic_u8string();
    }
    return path.generic_u8string();
}

bool inFileDirectory(const fs::path &root, const fs::path &file) {
    const auto parent = file.parent_path();
    return parent == root / "documents" || parent == root / "templates";
}

template<typename T>
T resolved(const fs::path &root, T item) {
    const auto stored = item.get_path();
    if (stored.find('\0') != std::string::npos) {
        item.set_path("");
        return item;
    }
    const auto file = (root / fs::u8path(stored)).lexically_normal();
    // Legacy blank templates can point to data/blank.docx. Keep moved or missing
    // files visible in the list, but never let a stored path open an external file.
    const bool inside = inFileDirectory(root, file) || file == root / "data" / "blank.docx";
    item.set_path(inside && !fs::is_symlink(file.parent_path()) && !fs::is_symlink(file)
                      ? file.u8string() : "");
    return item;
}

void copy(const std::string &source, const fs::path &path) {
    if (!FileManager::copyFile(source, path.u8string())) {
        throw std::runtime_error("Не удалось скопировать файл в библиотеку.");
    }
}
} // namespace

Manager::Manager()
    : Manager(fs::u8path(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString())) {}

Manager::Manager(fs::path dataDirectory) {
    if (dataDirectory.empty()) {
        throw std::invalid_argument("Не указан каталог данных.");
    }
    if (fs::create_directories(dataDirectory)) {
        fs::permissions(dataDirectory, fs::perms::owner_all);
    }
    directory_ = fs::canonical(dataDirectory);
    lock_ = std::make_unique<QLockFile>(QString::fromStdString((directory_ / ".docsmanager.lock").u8string()));
    lock_->setStaleLockTime(0);
    if (!lock_->tryLock()) {
        throw std::runtime_error(lock_->error() == QLockFile::LockFailedError
            ? "Эта библиотека уже открыта в другом экземпляре Docs Manager."
            : "Не удалось заблокировать библиотеку. Проверьте права доступа к каталогу данных.");
    }

    checkedDirectory(directory_, "documents");
    checkedDirectory(directory_, "templates");
    repositories_ = std::make_unique<RepMaster>((directory_ / "docsmanager.db").u8string());
    for (const auto &pending : repositories_->queuedFilePaths()) {
        finishFileChange(pending);
    }
}

Manager::~Manager() = default;

void Manager::create_template(std::string path, std::string name) {
    name = checkedName(name);
    const auto target = newFile(directory_, "templates", ".docx");
    changeFile(target, FileChange::Add, [&] {
        copy(path, target);
        Template item(name, target.u8string());
        Generator().generate(item);
        item.set_path(storedPath(directory_, item.get_path()));
        repositories_->templateRepository_->add(item);
    });
}

void Manager::create_template_form_blank(std::string name) {
    name = checkedName(name);
    const auto target = newFile(directory_, "templates", ".docx");
    changeFile(target, FileChange::Add, [&] {
        FileManager::createBlankDocx(target.u8string());
        repositories_->templateRepository_->add(Template(name, storedPath(directory_, target.u8string())));
    });
}

void Manager::create_document_from_template(Template temp, std::string name) {
    name = checkedName(name);
    if (temp.get_id() > 0) {
        temp.set_path(getTemplate(temp.get_id()).get_path());
    }
    const auto target = newFile(directory_, "documents", ".docx");
    changeFile(target, FileChange::Add, [&] {
        auto doc = Generator().generateTo(temp, name, target.u8string());
        doc.set_path(storedPath(directory_, doc.get_path()));
        repositories_->documentRepository_->add(doc);
    });
}

void Manager::add_document(int templateId, std::string name) {
    create_document_from_template(getTemplate(templateId), std::move(name));
}

void Manager::add_document(std::string path, std::string name) {
    name = checkedName(name);
    const auto extension = fs::u8path(path).extension().u8string();
    const auto target = newFile(directory_, "documents", extension);
    changeFile(target, FileChange::Add, [&] {
        copy(path, target);
        if (QString::fromStdString(extension).compare(".docx", Qt::CaseInsensitive) == 0) {
            FileManager::validateDocx(target.u8string());
        }
        Document doc(name, target.u8string());
        doc.set_path(storedPath(directory_, doc.get_path()));
        repositories_->documentRepository_->add(doc);
    });
}

std::vector<Document> Manager::get_documents() {
    auto items = repositories_->documentRepository_->get_all();
    for (auto &item : items) {
        item = resolved(directory_, std::move(item));
    }
    return items;
}

std::vector<Template> Manager::get_templates() {
    auto items = repositories_->templateRepository_->get_all();
    for (auto &item : items) {
        item = resolved(directory_, std::move(item));
    }
    return items;
}

Document Manager::getDocument(int id) {
    return resolved(directory_, repositories_->documentRepository_->get(id));
}

Template Manager::getTemplate(int id) {
    return resolved(directory_, repositories_->templateRepository_->get(id));
}

void Manager::updateDocument(int id, Document doc) {
    doc.set_name(checkedName(doc.get_name()));
    doc.set_path(storedPath(directory_, doc.get_path()));
    repositories_->documentRepository_->update_by_id(id, doc);
}

void Manager::updateTemplate(int id, Template temp) {
    temp.set_name(checkedName(temp.get_name()));
    temp.set_path(storedPath(directory_, temp.get_path()));
    repositories_->templateRepository_->update_by_id(id, temp);
}

void Manager::renameDocument(int id, std::string name) {
    auto record = repositories_->documentRepository_->get(id);
    record.set_name(std::move(name));
    updateDocument(id, std::move(record));
}

void Manager::renameTemplate(int id, std::string name) {
    auto record = repositories_->templateRepository_->get(id);
    record.set_name(std::move(name));
    updateTemplate(id, std::move(record));
}

void Manager::changeFile(const fs::path &file, FileChange kind, const std::function<void()> &change) {
    if (kind == FileChange::Add && fs::symlink_status(file).type() != fs::file_type::not_found) {
        throw std::runtime_error("Файл назначения уже существует.");
    }
    const auto relative = file.lexically_relative(directory_).generic_u8string();
    repositories_->queueFilePath(relative);
    try {
        change();
    } catch (...) {
        finishFileChange(relative);
        throw;
    }
    finishFileChange(relative, kind == FileChange::Add);
}

void Manager::finishFileChange(const std::string &relative, bool justAdded) {
    try {
        const auto path = fs::u8path(relative);
        const auto file = directory_ / path;
        const bool managed = relative.find('\0') == std::string::npos && !path.is_absolute() &&
                             path == path.lexically_normal() && inFileDirectory(directory_, file) &&
                             !fs::is_symlink(file.parent_path());
        if (!justAdded && managed) {
            DocxPackage::discardStagingFor(file.u8string());
            if (!fs::is_symlink(file) && fs::is_regular_file(file)) {
                // Older libraries could register the same managed file several times.
                const auto sharesFile = [&](const auto &row) {
                    return (directory_ / fs::u8path(row.get_path())).lexically_normal() == file;
                };
                const auto docs = repositories_->documentRepository_->get_all();
                const auto templates = repositories_->templateRepository_->get_all();
                if (std::none_of(docs.begin(), docs.end(), sharesFile) &&
                    std::none_of(templates.begin(), templates.end(), sharesFile)) {
                    fs::remove(file);
                }
            }
        }
        repositories_->forgetFilePath(relative);
    } catch (const std::exception &error) {
        // A committed record stays authoritative. Retry its pending cleanup on restart.
        qWarning() << "Library file cleanup deferred:" << error.what();
    }
}

void Manager::deleteDocument(int id) {
    const auto path = fs::u8path(getDocument(id).get_path());
    const auto erase = [&] { repositories_->documentRepository_->remove(id); };
    if (inFileDirectory(directory_, path)) changeFile(path, FileChange::Remove, erase);
    else erase();
}

void Manager::deleteTemplate(int id) {
    const auto path = fs::u8path(getTemplate(id).get_path());
    const auto erase = [&] { repositories_->templateRepository_->remove(id); };
    if (inFileDirectory(directory_, path)) changeFile(path, FileChange::Remove, erase);
    else erase();
}
