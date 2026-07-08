#include "tableau.hpp"

#include <vector>

#include "signed_formula.hpp"

namespace tablo {

namespace {

/**
 * Smesta oznacenu formulu na granu.
 *
 * Literali se belieze u `val` (uz proveru protivrecnosti), konstante se
 * razresuju odmah, a slozene formule odlaze u `work` za kasnije razlaganje.
 *
 * @return true ako smestanje ODMAH zatvara granu (suprotan literal vec
 *         prisutan, ili protivrecna konstanta poput F ⊤).
 */
bool place(const SignedFormula& sf, std::vector<SignedFormula>& work, Valuation& val) {
    Expansion e = classify(sf);
    switch (e.kind) {
        case RuleKind::Closed:
            return true;
        case RuleKind::Satisfied:
            return false;
        case RuleKind::Literal: {
            const std::string& name = as<Atom>(sf.formula).name;
            bool want = (sf.sign == Sign::T);
            auto it = val.find(name);
            if (it != val.end()) {
                return it->second != want;
            }
            val[name] = want;
            return false;
        }
        default:
            work.push_back(sf);
            return false;
    }
}

size_t pickIndex(const std::vector<SignedFormula>& work) {
    for (size_t i = 0; i < work.size(); ++i) {
        if (classify(work[i]).kind == RuleKind::Alpha) return i;
    }
    return 0;
}

/**
 * Rekurzivno razvija jednu granu tabloa.
 *
 * @param work Slozene oznacene formule koje jos treba razloziti.
 * @param val  Do sada pridruzeni literali (delimicna valuacija).
 * @param model Ako nije nullptr, popunjava se valuacijom prve OTVORENE grane.
 * @return true ako se grana (i sve njene podgrane) zatvore; false ako je neka
 *         grana otvorena (tada je `model` popunjen).
 */
bool closes(std::vector<SignedFormula> work, Valuation val, Valuation* model) {
    while (!work.empty()) {
        size_t idx = pickIndex(work);
        SignedFormula sf = work[idx];
        work.erase(work.begin() + static_cast<long>(idx));

        Expansion e = classify(sf);

        if (e.kind == RuleKind::Alpha) {
            // Sve komponente idu na istu granu.
            for (const auto& comp : e.components) {
                if (place(comp, work, val)) return true;
            }
            continue;
        }

        // Beta: grana se racva; zatvara akko SVE opcije zatvore.
        for (const auto& option : e.branches) {
            std::vector<SignedFormula> work2 = work;
            Valuation val2 = val;
            bool optionClosed = false;
            for (const auto& comp : option) {
                if (place(comp, work2, val2)) {
                    optionClosed = true;
                    break;
                }
            }
            if (optionClosed) continue;  // ova grana zatvorena, probaj sledecu
            if (!closes(work2, val2, model)) return false;  // nadjena otvorena grana
        }
        return true;  // sve opcije zatvorene
    }

    if (model) *model = val;
    return false;
}

bool runTableau(const SignedFormula& root, Valuation* openModel) {
    std::vector<SignedFormula> work;
    Valuation val;
    if (place(root, work, val)) return true;
    return closes(std::move(work), std::move(val), openModel);
}

} 

bool isValid(const FormulaPtr& f, Valuation* counterexample) {
    // f je tautologija akko se tablo za F f zatvori (nema kontramodela).
    return runTableau(F(f), counterexample);
}

bool isSatisfiable(const FormulaPtr& f, Valuation* model) {
    // f je zadovoljiva akko tablo za T f ima otvorenu granu.
    return !runTableau(T(f), model);
}

}
