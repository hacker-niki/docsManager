//
// Created by nikita on 22.03.24.
//

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../Interfaces/IRepository.h"

class Repository : public IRepository {
public:

    Repository() {}

    void add(Document document) override;

    void add(Template temp) override;

    int save_all() override;

    int update_all() override;

    Document get_doc(int id) override;

    Template get_temp(int id) override;

    void remove_doc(int id) override;

    void remove_temp(int id) override;

    std::vector<Document> get_all_docs() override;

    std::vector<Template> get_all_temp() override;
};

#endif //REPOSITORY_H
