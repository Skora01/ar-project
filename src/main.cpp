#include <iostream>

#include "formula.hpp"

using namespace tablo;

int main() {
    FormulaPtr f = ptr(Binary{
        Binary::And,
        ptr(Atom{"p"}),
        ptr(Not{ptr(Atom{"q"})}),
    });

    std::cout << toString(f) << '\n';
    return 0;
}
