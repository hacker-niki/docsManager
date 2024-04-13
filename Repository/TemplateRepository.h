//
// Created by nikita on 22.03.24.
//

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../Interfaces/IRepository.h"

class TemplateRepository : public IRepository<Template> {

private:
    std::vector<Template> templates_;
public:

    TemplateRepository() {}

    void add(const Template &temp) override;

    Template &get(int id) override;

    void remove(int id) override;

    std::vector<Template> get_all() override;

};

#endif //REPOSITORY_H
