#include <iostream>
#include "Manager.h"

using namespace std;

int main() {
    auto man = Manager();
    man.create_template("gen_report4.docx", "template");
    int cnt = 0;
    for (auto x: man.getTempate(0).get_fields()) {
        std::cout << x.get_content() << '\n';
        if (x.get_name() == "worknumber") {
            man.getTempate(0).set_field(cnt, "12");
        }
        if (x.get_name() == "worktheme") {
            man.getTempate(0).set_field(cnt, "aboba");
        }
        ++cnt;
    }

    man.add_document(0, "document");
    man.get_documents();
    return 0;
    
}