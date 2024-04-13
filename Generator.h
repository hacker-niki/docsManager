//
// Created by nikita on 22.03.24.
//

#ifndef GENERATOR_H
#define GENERATOR_H

#include <fstream>
#include "Entities/Template.h"
#include "Entities/Document.h"
#include "Interfaces/IGenerator.h"

class Generator : public IGenerator {
private:

    class VectorStrings {
    public:
        std::vector<std::string> vector_strings;
        int size;

        void init() {
            size = 0;
            for (auto x: vector_strings) {
                size += x.size();
            }
        }

        char operator[](int cnt) {
            int tmp = 0;
            int num = 0;
            while (tmp < cnt) {
                tmp += vector_strings[num].size();
                num++;
                if (tmp >= cnt) {
                    int k = vector_strings[num - 1].size() - 1;
                    while (tmp != cnt) {
                        k--;
                        tmp--;
                    }
                    return vector_strings[num - 1][k];
                }
            }
            return 0;
        }

        void operator()(int cnt) {
            int tmp = 0;
            int num = 0;
            while (tmp < cnt) {
                tmp += vector_strings[num].size();
                num++;
                if (tmp >= cnt) {
                    int k = vector_strings[num - 1].size() - 1;
                    while (tmp != cnt) {
                        k--;
                        tmp--;
                    }
                    vector_strings[num - 1].erase(k, 1);
                    --size;
                }
            }
        }

        void operator()(int cnt, std::string str) {
            int tmp = 0;
            int num = 0;
            while (tmp < cnt) {
                tmp += vector_strings[num].size();
                num++;
                if (tmp >= cnt) {
                    int k = vector_strings[num - 1].size() - 1;
                    while (tmp != cnt) {
                        k--;
                        tmp--;
                    }
                    vector_strings[num - 1].insert(k, str);
                    size += str.size();
                }
            }
        }
    };

    char field_symbol;

public:
    ~Generator();
    
    Generator() : field_symbol('%') {}

    explicit Generator(char field_symbol);

    Document generate(Template &aTemplate, const std::string &name) override;

    void generate(Template &aTemplate) override;

    void generate(Document &aDocument) override;

    
};

#endif //GENERATOR_H
