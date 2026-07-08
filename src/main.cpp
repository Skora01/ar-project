#include <iostream>

#include "formula.hpp"
#include "parser.hpp"

using namespace tablo;

int main() {
    const char* primeri[] = {
        "p & ~q",
        "p & (q | ~p) -> r",
        "~~p <-> p",
        "a -> b -> c", 
        "a & b | c & d",
    };

    for (const char* s : primeri) {
        try {
            FormulaPtr f = parse(s);
            std::cout << s << "  =>  " << toString(f) << '\n';
        } catch (const ParseError& e) {
            std::cout << s << "  =>  GRESKA: " << e.what() << '\n';
        }
    }

    return 0;
}
