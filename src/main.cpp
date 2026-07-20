#include <iostream>
#include <string>

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

static void runDemo() {
    std::cout << "== Tautology ==\n";
    checkIsTautology("(p -> q) -> (~q -> ~p)");                 // tautologija
    checkIsTautology("p | ~p");                                 // tautologija
    checkIsTautology("p -> q");                                 // nije: kontramodel p=1,q=0
    checkIsTautology("(a | (b & c)) <-> ((a | b) & (a | c))");  // tautologija
    checkIsTautology("p | true");                               // tautologija

    std::cout << "\n== SAT ==\n";
    checkIsSatisfiable("p & ~p");             // nezadovoljiva
    checkIsSatisfiable("(p -> q) & p & ~q");  // nezadovoljiva
    checkIsSatisfiable("(p | q) & (~p | r)"); // zadovoljiva p=1 r=1
    checkIsSatisfiable("p -> false");         // zadovoljiva p=0

    std::cout << "\n== Tablo stablo (valjanost kontrapozicije) ==\n";
    printValidityTableau(parse("(p -> q) -> (~q -> ~p)"));

    std::cout << "\n== Tablo stablo (zadovoljivost) ==\n";
    printSatisfiabilityTableau(parse("(p | q) & (~p | r)"));
}

static void printUsage(const char* prog) {
    std::cerr
        << "Upotreba:\n"
        << "  " << prog << " valid \"<formula>\" [--tree]   provera tautologije\n"
        << "  " << prog << " sat   \"<formula>\" [--tree]   provera zadovoljivosti\n"
        << "  " << prog << "                              demo primeri (bez argumenata)\n"
        << "\nVeznici: ~  &  |  ->  <->    konstante: true false    atomi: p, q, r, ...\n";
}

int main(int argc, char** argv) {
    if (argc == 1) {
        runDemo();
        return 0;
    }

    if (argc < 3) {
        printUsage(argv[0]);
        return 2;
    }

    const std::string mode = argv[1];
    const std::string input = argv[2];

    // --tree.
    bool showTree = false;
    for (int i = 3; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--tree") {
            showTree = true;
        } else {
            std::cerr << "Nepoznat argument: " << arg << "\n\n";
            printUsage(argv[0]);
            return 2;
        }
    }

    FormulaPtr f;
    try {
        f = parse(input);
    } catch (const ParseError& e) {
        std::cerr << "Greska u parsiranju: " << e.what() << '\n';
        return 1;
    }

    if (mode == "valid") {
        Valuation cex;
        const bool valid = isValid(f, &cex);
        std::cout << input << "  =>  " << (valid ? "TAUTOLOGIJA" : "NIJE TAUTOLOGIJA");
        if (!valid) std::cout << ", kontramodel " << toString(cex);
        std::cout << '\n';
        if (showTree) {
            std::cout << '\n';
            printValidityTableau(f);
        }
    } else if (mode == "sat") {
        Valuation model;
        const bool sat = isSatisfiable(f, &model);
        std::cout << input << "  =>  " << (sat ? "ZADOVOLJIVA" : "NEZADOVOLJIVA");
        if (sat) std::cout << ", model " << toString(model);
        std::cout << '\n';
        if (showTree) {
            std::cout << '\n';
            printSatisfiabilityTableau(f);
        }
    } else {
        std::cerr << "Nepoznat rezim: " << mode << "\n\n";
        printUsage(argv[0]);
        return 2;
    }

    return 0;
}
