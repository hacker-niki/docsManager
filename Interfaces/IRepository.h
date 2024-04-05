//
// Created by nikita on 22.03.24.
//

#ifndef IREPOSITORY_H
#define IREPOSITORY_H

#include <vector>

#include "../Entities/Document.h"
#include "../Entities/Template.h"

class IRepository {
public:

    virtual int save_all() = 0;

    virtual int update_all() = 0;

    virtual void add(Document document) = 0;

    virtual void add(Template temp) = 0;

    virtual Document get_doc(int id) = 0;

    virtual Template get_temp(int id) = 0;

    virtual void remove_doc(int id) = 0;

    virtual void remove_temp(int id) = 0;

    virtual std::vector<Document> get_all_docs() = 0;

    virtual std::vector<Template> get_all_temp() = 0;


};

#endif //IREPOSITORY_H
