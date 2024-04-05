//
// Created by nikita on 22.03.24.
//

#include "Manager.h"
#include "Generator.h"

Manager::Manager() {
    repository_ = new Repository();
    generator_ = new Generator();
}
