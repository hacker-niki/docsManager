#include <duckx/duckx.hpp>
#include <iostream>
#include "Entities/Template.h"
#include "Generator.h"

using namespace std;

int main() {

    auto temp = Template("template", "my_test.docx");
    auto gen = Generator();
    gen.generate(temp);
    auto ff = temp.get_fields();
    for (auto f: ff) {
        cout << f.get_content();
    }
    return 0;
}