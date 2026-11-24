#ifndef DOCSMANAGER_SQLITEREPOSITORY_H
#define DOCSMANAGER_SQLITEREPOSITORY_H

#include "../Interfaces/IRepository.h"
#include <sqlite3.h>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace storage {
class Statement {
public:
    Statement(sqlite3 *db, const std::string &sql) : db_(db) {
        sqlite3_stmt *statement = nullptr;
        check(sqlite3_prepare_v2(db_, sql.c_str(), -1, &statement, nullptr));
        statement_.reset(statement);
    }

    void bind(int index, int id) {
        check(sqlite3_bind_int(statement_.get(), index, id));
    }

    void bind(int index, const std::string &value) {
        check(sqlite3_bind_text64(statement_.get(), index, value.data(), value.size(),
                                  SQLITE_TRANSIENT, SQLITE_UTF8));
    }

    bool next() {
        const int result = sqlite3_step(statement_.get());
        if (result == SQLITE_ROW) return true;
        if (result == SQLITE_DONE) return false;
        throw std::runtime_error(sqlite3_errmsg(db_));
    }

    int integer(int column) const {
        const auto value = sqlite3_column_int64(statement_.get(), column);
        if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
            throw std::runtime_error("Library record count or ID exceeds supported range");
        }
        return static_cast<int>(value);
    }

    std::string text(int column) const {
        const auto *value = sqlite3_column_text(statement_.get(), column);
        if (!value) throw std::runtime_error("Unreadable data in library record");
        return {reinterpret_cast<const char *>(value),
                static_cast<std::size_t>(sqlite3_column_bytes(statement_.get(), column))};
    }

private:
    void check(int result) const {
        if (result != SQLITE_OK) throw std::runtime_error(sqlite3_errmsg(db_));
    }

    sqlite3 *db_;
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> statement_{nullptr, sqlite3_finalize};
};
}

template<typename Record>
class SqliteRepository : public IRepository<Record> {
public:
    void add(Record record) override {
        storage::Statement insert(db_.get(), "INSERT INTO " + table_ + " (data) VALUES (?)");
        insert.bind(1, record.to_string());
        insert.next();
    }

    Record get(int id) override {
        storage::Statement select(db_.get(), "SELECT id, data FROM " + table_ + " WHERE id = ?");
        select.bind(1, id);
        if (!select.next()) throwMissing(id);
        return read(select);
    }

    void remove(int id) override {
        storage::Statement erase(db_.get(), "DELETE FROM " + table_ + " WHERE id = ? RETURNING id");
        erase.bind(1, id);
        finishChange(erase, id);
    }

    void update_by_id(int id, const Record &record) {
        storage::Statement updateRow(db_.get(), "UPDATE " + table_ + " SET data = ? WHERE id = ? RETURNING id");
        updateRow.bind(1, record.to_string());
        updateRow.bind(2, id);
        finishChange(updateRow, id);
    }

    std::vector<Record> get_all() override {
        storage::Statement select(db_.get(), "SELECT id, data FROM " + table_ + " ORDER BY id");
        std::vector<Record> records;
        while (select.next()) records.push_back(read(select));
        return records;
    }

    int get_size() override {
        storage::Statement count(db_.get(), "SELECT COUNT(*) FROM " + table_);
        count.next();
        return count.integer(0);
    }

    // All writes use SQLite autocommit. Reads query the database directly.
    void save() override {}
    void update() override {}

protected:
    SqliteRepository(std::shared_ptr<sqlite3> db, const char *table) : db_(std::move(db)), table_(table) {
        execute("CREATE TABLE IF NOT EXISTS " + table_ + " (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT NOT NULL)");
        // Earlier releases allowed duplicate names. Preserve those rows, but reject new conflicts.
        const std::string name = "json_extract(NEW.data, '$.name')";
        const std::string indexedName = "CASE WHEN json_valid(data) THEN json_extract(data, '$.name') END";
        execute("CREATE INDEX IF NOT EXISTS " + table_ + "_name ON " + table_ + " (" + indexedName + ")");
        const std::string existing = "EXISTS (SELECT 1 FROM " + table_ + " WHERE " + indexedName + " = " + name + ")";
        const std::string reject = " BEGIN SELECT RAISE(ABORT, 'Name already exists in " + table_ + "'); END";
        execute("CREATE TRIGGER IF NOT EXISTS " + table_ + "_check_insert BEFORE INSERT ON " + table_ +
                " WHEN " + existing + reject);
        execute("CREATE TRIGGER IF NOT EXISTS " + table_ + "_check_rename BEFORE UPDATE OF data ON " + table_ +
                " WHEN json_extract(OLD.data, '$.name') IS NOT " + name + " AND " + existing + reject);
    }

private:
    void finishChange(storage::Statement &statement, int id) const {
        if (!statement.next()) throwMissing(id);
        // Step past RETURNING to observe errors at commit time.
        statement.next();
    }

    void execute(const std::string &sql) {
        storage::Statement statement(db_.get(), sql);
        statement.next();
    }

    Record read(const storage::Statement &row) const {
        const int id = row.integer(0);
        Record record;
        try {
            record.from_string(row.text(1));
        } catch (const std::runtime_error &error) {
            throw std::runtime_error(table_ + " record " + std::to_string(id) + ": " + error.what());
        }
        record.set_id(id);
        return record;
    }

    [[noreturn]] void throwMissing(int id) const {
        throw std::out_of_range(table_ + " record " + std::to_string(id) + " does not exist");
    }

    std::shared_ptr<sqlite3> db_;
    std::string table_;
};

#endif
