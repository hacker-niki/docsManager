//
// Created by nikita on 08.04.24.
//

#ifndef DOCSMANAGER_DOCUMENTREPOSITORY_H
#define DOCSMANAGER_DOCUMENTREPOSITORY_H


#include "../Interfaces/IRepository.h"


class DocumentRepository : public IRepository<Document> {
private:
    std::vector<Document> documents_;
public:

    DocumentRepository() = default;

    void add(const Document &document) override;

    Document& get(int id) override;

    void remove(int id) override;

    std::vector<Document> get_all() override;
};


#endif //DOCSMANAGER_DOCUMENTREPOSITORY_H
