#ifndef FORMULA_HPP
#define FORMULA_HPP

#include <memory>
#include <string>
#include <variant>

namespace tablo {

struct False;
struct True;
struct Atom;
struct Not;
struct Binary;

using Formula = std::variant<False, True, Atom, Not, Binary>;

using FormulaPtr = std::shared_ptr<Formula>;

struct False {};

struct True {};

struct Atom {
    std::string name;
};

struct Not {
    FormulaPtr subformula;
};

struct Binary {
    enum Type { And, Or, Impl, Eq } type;
    FormulaPtr left, right;
};

FormulaPtr ptr(const Formula& f) {
    return std::make_shared<Formula>(f);
}

template <typename T>
bool is(const FormulaPtr& f) {
    return std::holds_alternative<T>(*f);
}

template <typename T>
T as(const FormulaPtr& f) {
    return std::get<T>(*f);
}

const char* symbol(Binary::Type type) {
    switch (type) {
        case Binary::And:  return "&";
        case Binary::Or:   return "|";
        case Binary::Impl: return "->";
        case Binary::Eq:   return "<->";
    }
    return "?"; 
}

std::string toString(const FormulaPtr& f) {
    if (is<False>(f)) return "⊥";
    if (is<True>(f))  return "⊤";
    if (is<Atom>(f))  return as<Atom>(f).name;
    if (is<Not>(f))   return "~" + toString(as<Not>(f).subformula);

    Binary b = as<Binary>(f);
    return "(" + toString(b.left) + " " + symbol(b.type) + " " + toString(b.right) + ")";
}

} 

#endif  // FORMULA_HPP
