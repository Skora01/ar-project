#ifndef SIGNED_FORMULA_HPP
#define SIGNED_FORMULA_HPP

#include <string>
#include <vector>

#include "formula.hpp"

namespace tablo {

enum class Sign { T, F };

struct SignedFormula {
    Sign sign;
    FormulaPtr formula;
};

inline std::string toString(const SignedFormula& sf) {
    return (sf.sign == Sign::T ? "T " : "F ") + toString(sf.formula);
}


//   Alpha     - grana se produzava komponentama (sve iz `components`)
//   Beta      - grana se racva; svaka podlista iz `branches` je jedna grana
//   Literal   - oznaceni atom; nema razlaganja (bitan za zatvaranje)
//   Closed    - sam po sebi protivrecan (F T ili T F) -> grana se zatvara
//   Satisfied - trivijalno tacan (T T ili F F) -> moze se odbaciti
enum class RuleKind { Alpha, Beta, Literal, Closed, Satisfied };

struct Expansion {
    RuleKind kind;
    std::vector<SignedFormula> components;               // za Alpha
    std::vector<std::vector<SignedFormula>> branches;    // za Beta
};

inline SignedFormula T(const FormulaPtr& f) { return {Sign::T, f}; }
inline SignedFormula F(const FormulaPtr& f) { return {Sign::F, f}; }

/**
 * Klasifikuje oznacenu formulu i vraca njeno alfa/beta razlaganje.
 *
 * @param sf Oznacena formula.
 * @return Expansion cija `kind` govori kako tablo tretira formulu.
 */
inline Expansion classify(const SignedFormula& sf) {
    const FormulaPtr& f = sf.formula;
    bool t = (sf.sign == Sign::T);

    if (is<Atom>(f)) {
        return {RuleKind::Literal, {}, {}};
    }

    if (is<True>(f)) {
        return {t ? RuleKind::Satisfied : RuleKind::Closed, {}, {}};
    }
    if (is<False>(f)) {
        return {t ? RuleKind::Closed : RuleKind::Satisfied, {}, {}};
    }

    // Negacija je uvek alfa: T ~A -> F A ; F ~A -> T A.
    if (is<Not>(f)) {
        FormulaPtr sub = as<Not>(f).subformula;
        return {RuleKind::Alpha, {{t ? Sign::F : Sign::T, sub}}, {}};
    }

    Binary b = as<Binary>(f);
    FormulaPtr l = b.left;
    FormulaPtr r = b.right;

    switch (b.type) {
        case Binary::And:
            // T(A&B) alfa: TA, TB   |   F(A&B) beta: FA | FB
            return t ? Expansion{RuleKind::Alpha, {T(l), T(r)}, {}}
                     : Expansion{RuleKind::Beta, {}, {{F(l)}, {F(r)}}};

        case Binary::Or:
            // T(A|B) beta: TA | TB   |   F(A|B) alfa: FA, FB
            return t ? Expansion{RuleKind::Beta, {}, {{T(l)}, {T(r)}}}
                     : Expansion{RuleKind::Alpha, {F(l), F(r)}, {}};

        case Binary::Impl:
            // T(A->B) beta: FA | TB   |   F(A->B) alfa: TA, FB
            return t ? Expansion{RuleKind::Beta, {}, {{F(l)}, {T(r)}}}
                     : Expansion{RuleKind::Alpha, {T(l), F(r)}, {}};

        case Binary::Eq:
            // T(A<->B) beta: {TA,TB} | {FA,FB}
            // F(A<->B) beta: {TA,FB} | {FA,TB}
            return t ? Expansion{RuleKind::Beta, {}, {{T(l), T(r)}, {F(l), F(r)}}}
                     : Expansion{RuleKind::Beta, {}, {{T(l), F(r)}, {F(l), T(r)}}};
    }

    return {RuleKind::Literal, {}, {}}; 
}

}

#endif  // SIGNED_FORMULA_HPP
