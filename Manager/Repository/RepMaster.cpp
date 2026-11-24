#include "RepMaster.h"

RepMaster::RepMaster(const std::string &databasePath) {
    sqlite3 *handle = nullptr;
    const int result = sqlite3_open_v2(databasePath.c_str(), &handle,
                                       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOFOLLOW, nullptr);
    db_.reset(handle, sqlite3_close_v2);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Cannot open library database " + databasePath + ": " + sqlite3_errmsg(handle));
    }
    if (sqlite3_busy_timeout(handle, 3000) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(handle));
    }
    templateRepository_ = std::make_shared<TemplateRepository>(db_);
    documentRepository_ = std::make_shared<DocumentRepository>(db_);
    storage::Statement create(handle, "CREATE TABLE IF NOT EXISTS PendingFiles (path TEXT PRIMARY KEY NOT NULL) WITHOUT ROWID");
    create.next();
}

void RepMaster::queueFilePath(const std::string &relativePath) {
    storage::Statement insert(db_.get(), "INSERT INTO PendingFiles(path) VALUES (?) ON CONFLICT(path) DO NOTHING");
    insert.bind(1, relativePath);
    insert.next();
}

std::vector<std::string> RepMaster::queuedFilePaths() const {
    storage::Statement select(db_.get(), "SELECT path FROM PendingFiles ORDER BY path");
    std::vector<std::string> paths;
    while (select.next()) paths.push_back(select.text(0));
    return paths;
}

void RepMaster::forgetFilePath(const std::string &relativePath) {
    storage::Statement erase(db_.get(), "DELETE FROM PendingFiles WHERE path = ?");
    erase.bind(1, relativePath);
    erase.next();
}

int RepMaster::save() {
    documentRepository_->save();
    templateRepository_->save();
    return 0;
}

int RepMaster::update() {
    documentRepository_->update();
    templateRepository_->update();
    return 0;
}
