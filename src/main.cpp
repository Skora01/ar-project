#include <iostream>

#include "formula.hpp"
#include "parser.hpp"
#include "signed_formula.hpp"

using namespace tablo;

static void printExpansion(const SignedFormula& sf) {
    Expansion e = classify(sf);
    std::cout << toString(sf) << "   =>   ";
    switch (e.kind) {
        case RuleKind::Alpha: {
            std::cout << "ALFA: ";
            for (const auto& c : e.components) std::cout << toString(c) << "  ";
            break;
        }
        case RuleKind::Beta: {
            std::cout << "BETA: ";
            for (const auto& br : e.branches) {
                std::cout << "{ ";
                for (const auto& c : br) std::cout << toString(c) << " ";
                std::cout << "} ";
            }
            break;
        }
        case RuleKind::Literal:   std::cout << "LITERAL"; break;
        case RuleKind::Closed:    std::cout << "CLOSED BRANCH"; break;
        case RuleKind::Satisfied: std::cout << "TRIVIALY CORRECT"; break;
    }
    std::cout << '\n';
}

int main() {
    FormulaPtr and_ = parse("a & b");
    FormulaPtr impl = parse("a -> b");
    FormulaPtr eq   = parse("a <-> b");

    printExpansion(T(and_));   
    printExpansion(F(and_));   
    printExpansion(T(impl));   
    printExpansion(F(impl));   
    printExpansion(T(eq));     
    printExpansion(F(eq));     
    printExpansion(T(parse("~a")));   
    printExpansion(T(parse("p")));    
    printExpansion(F(parse("true"))); 

    return 0;
}
