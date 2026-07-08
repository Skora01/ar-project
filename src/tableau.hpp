#ifndef TABLEAU_HPP
#define TABLEAU_HPP

#include <map>
#include <string>

#include "formula.hpp"

namespace tablo {

using Valuation = std::map<std::string, bool>;

bool isValid(const FormulaPtr& f, Valuation* counterexample = nullptr);

bool isSatisfiable(const FormulaPtr& f, Valuation* model = nullptr);

}

#endif  // TABLEAU_HPP
