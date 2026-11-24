#ifndef IREPOSITORY_H
#define IREPOSITORY_H

#include <vector>

template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void add(T) = 0;
    virtual T get(int id) = 0;
    virtual void remove(int id) = 0;
    virtual std::vector<T> get_all() = 0;
    virtual int get_size() = 0;
    virtual void save() = 0;
    virtual void update() = 0;
};

#endif
