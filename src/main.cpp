#include <iostream>

#include "formula.hpp"
#include "parser.hpp"
#include "tableau.hpp"

using namespace tablo;

static std::string toString(const Valuation& v) {
    std::string s = "{";
    bool first = true;
    for (const auto& [name, value] : v) {
        if (!first) s += ", ";
        s += name + "=" + (value ? "1" : "0");
        first = false;
    }
    return s + "}";
}

static void checkIsTautology(const std::string& s) {
    FormulaPtr f = parse(s);
    Valuation cex;
    bool valid = isValid(f, &cex);
    std::cout << s << "   =>   " << (valid ? "TAUTOLOGY" : "NOT TAUTOLOGY");
    if (!valid) std::cout << ", counter example " << toString(cex);
    std::cout << '\n';
}

static void checkIsSatisfiable(const std::string& s) {
    FormulaPtr f = parse(s);
    Valuation model;
    bool sat = isSatisfiable(f, &model);
    std::cout << s << "   =>   " << (sat ? "SAT" : "NOT SAT");
    if (sat) std::cout << ", model " << toString(model);
    std::cout << '\n';
}

int main() {
    std::cout << "== Tautology ==\n";
    checkIsTautology("(p -> q) -> (~q -> ~p)");   //tautologija
    checkIsTautology("p | ~p");                    //  tautologija
    checkIsTautology("p -> q");                     // nije: kontramodel p=1,q=0
    checkIsTautology("(a | (b & c)) <-> ((a | b) & (a | c))");  // tautologija
    checkIsTautology("p | true");                   // tautologija

    std::cout << "\n== SAT ==\n";
    checkIsSatisfiable("p & ~p");                 // nezadovoljiva
    checkIsSatisfiable("(p -> q) & p & ~q");      // nezadovoljiva
    checkIsSatisfiable("(p | q) & (~p | r)");     // zadovoljiva p = 1 r = 1
    checkIsSatisfiable("p -> false");             // zadovoljiva p = 0

    std::cout << "\n== Tablo stablo (valjanost kontrapozicije) ==\n";
    printValidityTableau(parse("(p -> q) -> (~q -> ~p)"));

    std::cout << "\n== Tablo stablo (zadovoljivost) ==\n";
    printSatisfiabilityTableau(parse("(p | q) & (~p | r)"));

    return 0;
}
