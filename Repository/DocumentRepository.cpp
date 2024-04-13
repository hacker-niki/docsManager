//
// Created by nikita on 08.04.24.
//

#include "DocumentRepository.h"

void DocumentRepository::add(const Document &document) {
    documents_.push_back(document);

}

Document& DocumentRepository::get(int id) {
    return documents_[id];

}

void DocumentRepository::remove(int id) {
    documents_.erase(std::next(documents_.begin(), id));

}

std::vector<Document> DocumentRepository::get_all() {
    return documents_;
}
