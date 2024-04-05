//
// Created by nikita on 22.03.24.
//

#include <duckx/duckx.hpp>
#include "Generator.h"
#include "Entities/Document.h"

Generator::Generator(char field_symbol) : field_symbol(field_symbol) {}


Document Generator::generate(Template &aTemplate, const std::string &name) {
    auto document = Document(name, aTemplate.get_path());
    duckx::Document doc(document.get_path());
    doc.open();
    for (auto p = doc.paragraphs(); p.has_next(); p.next()) {

        VectorStrings vectorStrings;
        for (auto r = p.runs(); r.has_next(); r.next()) {
            vectorStrings.vs.emplace_back(r.get_text());
        }
        vectorStrings.init();
        for (int i = 0; i < vectorStrings.size - 1; i++) {
            if (vectorStrings[i] == field_symbol && vectorStrings[i + 1] == field_symbol) {
                std::string tmp;
                for (int j = i + 2; j < vectorStrings.size - 1; j++) {
                    if (vectorStrings[j] != field_symbol && vectorStrings[j + 1] != field_symbol)
                        tmp += vectorStrings[j];
                    else {
                        i = j + 2;
                        break;
                    }
                }
                aTemplate.fields.emplace_back(tmp);
            }
        }
    }
    return document;
}


void Generator::generate(Template &aTemplate) {
    duckx::Document doc(aTemplate.get_path());
    doc.open();
    for (auto p = doc.paragraphs(); p.has_next(); p.next()) {

        VectorStrings vectorStrings;
        for (auto r = p.runs(); r.has_next(); r.next()) {
            vectorStrings.vs.emplace_back(r.get_text());
        }
        vectorStrings.init();
        for (int i = 0; i < vectorStrings.size - 1; i++) {
            if (vectorStrings[i] == field_symbol && vectorStrings[i + 1] == field_symbol) {
                std::string tmp;
                for (int j = i + 2; j < vectorStrings.size - 1; j++) {
                    if (vectorStrings[j] != field_symbol && vectorStrings[j + 1] != field_symbol)
                        tmp += vectorStrings[j];
                    else {
                        i = j + 2;
                        break;
                    }
                }
                aTemplate.fields.emplace_back(tmp);
            }
        }
    }
}

void Generator::generate(Document &aDocument) {

}
