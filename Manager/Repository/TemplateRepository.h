//
// Created by nikita on 22.03.24.
//

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../Interfaces/IRepository.h"
#include "../sqlite/sqlite3.h"

class TemplateRepository : public IRepository<Template> {

private:
    std::vector<Template> templates_;
    sqlite3 *db_;

public:


    TemplateRepository(sqlite3 *db);

    void add(Template temp) override;

    const Template & get(int id) override;

    void remove(int id) override;

    int get_size() override {
        return templates_.size();
    }

    std::vector<Template> get_all() override;
    
    void save() override;

    void update() override;

    void update_by_id(int id, const Template &temp);
};

#endif //REPOSITORY_H
