#include "Manager.h"
#include "WorkflowFixture.h"

#include <QTemporaryDir>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using workflow::require;

namespace {
template<typename Operation>
void mustFail(Operation operation) {
    try {
        operation();
    } catch (const std::exception &) {
        return;
    }
    throw std::runtime_error("Operation unexpectedly succeeded");
}

std::size_t countFiles(const fs::path &path) {
    return std::distance(fs::directory_iterator(path), fs::directory_iterator{});
}

void run() {
    QTemporaryDir tmp;
    require(tmp.isValid(), "No temporary directory");
    const fs::path root = fs::u8path(tmp.path().toStdString());
    const auto initial = root / "library", moved = root / "перенос";
    const auto source = root / "бланк.docx";
    workflow::makeTemplate(source);
    const auto original = root / "note.txt";
    std::ofstream(original) << "original";

    int templateId, generatedId;
    {
        Manager manager(initial);
        mustFail([&] { Manager second(initial); });
        manager.create_template(source.u8string(), "Заявление");
        const auto templates = manager.get_templates();
        require(templates.size() == 1, "Imported template missing");
        auto form = templates.front();
        templateId = form.get_id();
        require(form.get_fields_count() == 1 && form.get_field(0).get_name() == "Имя", "Field discovery failed");
        form.set_field(0, "Ирина & Марк");
        manager.create_document_from_template(form, "  Ответ  ");
        const auto doc = manager.get_documents().at(0);
        generatedId = doc.get_id();
        require(doc.get_name() == "Ответ" && fs::is_regular_file(doc.get_path()), "Generated record is invalid");
        require(FileManager::read_docx(doc.get_path()).find("Получатель: Ирина & Марк") != std::string::npos,
                "Generated file contains wrong text");

        mustFail([&] { manager.create_document_from_template(form, "Ответ"); });
        mustFail([&] { manager.create_template(source.u8string(), "Заявление"); });
        mustFail([&] { manager.add_document(original.u8string(), " ../bad "); });
        mustFail([&] { manager.add_document((root / "missing").u8string(), "Missing"); });
        const auto broken = root / "broken.DoCx";
        std::ofstream(broken) << "broken";
        mustFail([&] { manager.add_document(broken.u8string(), "Broken"); });
        mustFail([&] { manager.create_template(broken.u8string(), "Broken"); });
        require(countFiles(initial / "documents") == 1 && countFiles(initial / "templates") == 1,
                "Failed operation left additional managed files");

        manager.add_document(original.u8string(), "Копия");
        manager.add_document(original.u8string(), "Ещё копия");
        const auto copies = manager.get_documents();
        require(copies.at(1).get_path() != copies.at(2).get_path(), "Imports overwrote each other");
        mustFail([&] { manager.renameDocument(copies.at(2).get_id(), "Копия"); });
        manager.deleteDocument(copies.at(1).get_id());
        require(fs::exists(original) && !fs::exists(copies.at(1).get_path()) && fs::exists(copies.at(2).get_path()),
                "Delete removed the wrong file");
        manager.renameDocument(generatedId, "Итог");
        manager.renameTemplate(templateId, "Новое заявление");
        manager.create_template_form_blank("Пустой");
        manager.add_document(manager.get_templates().at(1).get_id(), "Из пустого");
    }

    fs::rename(initial, moved);
    {
        Manager manager(moved);
        const auto doc = manager.getDocument(generatedId);
        require(doc.get_name() == "Итог" && fs::exists(doc.get_path()), "Move or reopen lost generated document");
        require(manager.getTemplate(templateId).get_name() == "Новое заявление", "Template rename did not persist");
        manager.deleteTemplate(templateId);
        require(fs::exists(source) && fs::exists(doc.get_path()), "Deleting a template removed source or generation");

        // Legacy rows can share a path, reference missing files or point outside the library.
        const auto others = manager.get_documents();
        auto shared = others.at(1);
        shared.set_path(doc.get_path());
        manager.updateDocument(shared.get_id(), shared);
        manager.deleteDocument(generatedId);
        require(fs::exists(doc.get_path()), "Shared file deleted while still referenced");
        manager.deleteDocument(shared.get_id());
        require(!fs::exists(doc.get_path()), "Last reference did not remove a managed file");

        auto external = others.at(2);
        external.set_path(original.u8string());
        manager.updateDocument(external.get_id(), external);
        require(manager.getDocument(external.get_id()).get_path().empty(), "External record can open a file outside the library");
        manager.deleteDocument(external.get_id());
        require(fs::exists(original), "Deleting metadata removed an external file");
    }
    const auto manualFile = moved / "documents" / "manual.txt";
    const auto unfinished = moved / "documents" / "unfinished.docx";
    const auto legacy = moved / "documents" / "legacy.txt";
    std::ofstream(manualFile) << "not registered";
    std::ofstream(unfinished) << "pending import";
    std::ofstream(legacy) << "legacy data";
    {
        RepMaster db((moved / "docsmanager.db").u8string());
        Document row("Старый документ", legacy.u8string());
        row.set_path("./documents/legacy.txt");
        db.documentRepository_->add(row);
        db.queueFilePath("documents/legacy.txt"); // Crash after SQL INSERT or before SQL DELETE.
        db.queueFilePath("documents/unfinished.docx"); // Crash before INSERT or after DELETE.
        db.queueFilePath("../note.txt");
        db.queueFilePath(std::string("documents/manual.txt\0other", 26));
    }
    {
        Manager recovered(moved);
        require(fs::exists(legacy) && recovered.get_documents().at(0).get_path() == legacy.u8string(),
                "Recovery dropped a registered legacy file");
        require(!fs::exists(unfinished), "Recovery left a pending unregistered file");
        require(fs::exists(manualFile) && fs::exists(original), "Recovery touched an unrelated file");
    }
    {
        RepMaster db((moved / "docsmanager.db").u8string());
        require(db.queuedFilePaths().empty(), "Completed intents remained in the database");
    }

}
}

int main() {
    try { run(); } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
