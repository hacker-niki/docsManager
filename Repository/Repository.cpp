//
// Created by nikita on 22.03.24.
//

#include "../Entities/Document.h"
#include "Repository.h"

void Repository::add(Document document) {
}

void Repository::add(Template temp) {

}

Document Repository::get_doc(int id) {
    return Document("", "");
}

Template Repository::get_temp(int id) {
    return Template("", "");
}

std::vector<Document> Repository::get_all_docs() {
    return std::vector{Document{"", ""}};
}

std::vector<Template> Repository::get_all_temp() {
    return std::vector{Template{"", ""}};
}

void Repository::remove_doc(int id) {

}

void Repository::remove_temp(int id) {

}

int Repository::update_all() {
    return 0;
}

int Repository::save_all() {
    return 0;
}