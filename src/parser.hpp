#ifndef PARSER_HPP
#define PARSER_HPP

#include <stdexcept>
#include <string>

#include "formula.hpp"

namespace tablo {

struct ParseError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

FormulaPtr parse(const std::string& input);

}

#endif  // PARSER_HPP
