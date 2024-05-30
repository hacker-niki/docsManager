//
// Created by nikita on 08.04.24.
//

#include "RepMaster.h"

int RepMaster::save() {
    documentRepository_->save();
    templateRepository_->save();
    return 0;
}

int RepMaster::update() {
    documentRepository_->update();
    templateRepository_->update();
    return 0;
}
