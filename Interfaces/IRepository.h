//
// Created by nikita on 22.03.24.
//

#ifndef IREPOSITORY_H
#define IREPOSITORY_H

#include <vector>

#include "../Entities/Document.h"
#include "../Entities/Template.h"

template<typename T>
class IRepository {
public:
    virtual void add(T) = 0;

    virtual T &get(int id) = 0;

    virtual void remove(int id) = 0;

    virtual std::vector<T> get_all() = 0;

    virtual int get_size() = 0;
    
    virtual void save() = 0;
    
    virtual void update() = 0;

};

#endif //IREPOSITORY_H
