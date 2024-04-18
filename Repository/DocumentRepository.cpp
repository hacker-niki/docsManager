//
// Created by nikita on 08.04.24.
//

#include <stdexcept>
#include "DocumentRepository.h"

void DocumentRepository::add(const Document &document) {
    documents_.push_back(document);
}

Document &DocumentRepository::get(int id) {
    while (true) {
        if (id < 0 || id >= documents_.size())
            break;
        return documents_[id];
    }
    throw std::out_of_range("There's no document with such id🙂");
}

void DocumentRepository::remove(int id) {
    while (true) {
        if (id < 0 || id >= documents_.size())
            break;
        documents_.erase(std::next(documents_.begin(), id));
        return;
    }
    throw std::out_of_range("There's no document with such id🙂");
}

std::vector<Document> DocumentRepository::get_all() {
    return documents_;
}
