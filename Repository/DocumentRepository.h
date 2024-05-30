//
// Created by nikita on 08.04.24.
//

#ifndef DOCSMANAGER_DOCUMENTREPOSITORY_H
#define DOCSMANAGER_DOCUMENTREPOSITORY_H


#include "../Interfaces/IRepository.h"
#include "../sqlite/sqlite3.h"


class DocumentRepository : public IRepository<Document> {
private:
    std::vector<Document> documents_;
    sqlite3 *db_;
public:

    DocumentRepository(sqlite3 *db);

    void add(Document document) override;

    Document &get(int id) override;

    void remove(int id) override;

    int get_size() override {
        return documents_.size();
    }

    std::vector<Document> get_all() override;

    void update() override;

    void save() override;

};


#endif //DOCSMANAGER_DOCUMENTREPOSITORY_H
