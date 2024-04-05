//
// Created by nikita on 22.03.24.
//

#ifndef MANAGER_H
#define MANAGER_H

#include "Interfaces/IGenerator.h"
#include "Interfaces/IRepository.h"
#include "Repository/Repository.h"

class Manager {
    IRepository *repository_;
    IGenerator *generator_;

    Manager();
};

#endif //MANAGER_H
