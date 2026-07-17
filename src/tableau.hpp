#ifndef TABLEAU_HPP
#define TABLEAU_HPP

#include <iostream>
#include <map>
#include <string>

#include "formula.hpp"

namespace tablo {

using Valuation = std::map<std::string, bool>;

bool isValid(const FormulaPtr& f, Valuation* counterexample = nullptr);

bool isSatisfiable(const FormulaPtr& f, Valuation* model = nullptr);

void printValidityTableau(const FormulaPtr& f, std::ostream& os = std::cout);

void printSatisfiabilityTableau(const FormulaPtr& f, std::ostream& os = std::cout);

}

#endif  // TABLEAU_HPP
