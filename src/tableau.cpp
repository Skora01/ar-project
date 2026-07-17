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

// ---- Vizuelizacija stabla -------------------------------------------------

// Cvor stabla za prikaz: labela (oznacena formula ili marker lista) i deca.
// 1 dete = alfa lanac; 2 dece = beta racvanje; 0 dece = list (X ili otvorena).
struct TableauNode {
    std::string label;
    std::vector<TableauNode> children;
};

// Formatira valuaciju otvorene grane, npr. "{p=1, q=0}".
std::string valuationLabel(const Valuation& val) {
    std::string s = "{";
    bool first = true;
    for (const auto& [name, value] : val) {
        if (!first) s += ", ";
        s += name + "=" + (value ? "1" : "0");
        first = false;
    }
    return s + "}";
}

std::vector<TableauNode> buildCont(std::vector<SignedFormula> work, Valuation val);

// Gradi lanac cvorova za listu komponenti (alfa komponente ili jedna beta
// opcija), pa nastavlja granu. Zatvaranje na literalu daje list "X".
TableauNode buildChain(const std::vector<SignedFormula>& comps, size_t i,
                       std::vector<SignedFormula>& work, Valuation& val) {
    TableauNode node{toString(comps[i]), {}};
    if (place(comps[i], work, val)) {
        node.children = {{"X (zatvorena)", {}}};
        return node;
    }
    if (i + 1 < comps.size()) {
        node.children = {buildChain(comps, i + 1, work, val)};
    } else {
        node.children = buildCont(std::move(work), std::move(val));
    }
    return node;
}

// Vraca decu tekuceg dna grane: alfa -> lanac (1), beta -> racvanje (2),
// prazna radna lista -> otvorena grana (list sa modelom).
std::vector<TableauNode> buildCont(std::vector<SignedFormula> work, Valuation val) {
    if (work.empty()) {
        return {{"o (otvorena) " + valuationLabel(val), {}}};
    }
    size_t idx = pickIndex(work);
    SignedFormula sf = work[idx];
    work.erase(work.begin() + static_cast<long>(idx));

    Expansion e = classify(sf);
    if (e.kind == RuleKind::Alpha) {
        return {buildChain(e.components, 0, work, val)};
    }

    std::vector<TableauNode> kids;
    for (const auto& option : e.branches) {
        std::vector<SignedFormula> work2 = work;
        Valuation val2 = val;
        kids.push_back(buildChain(option, 0, work2, val2));
    }
    return kids;
}

// Gradi celo stablo pocev od korena.
TableauNode buildTree(const SignedFormula& root) {
    std::vector<SignedFormula> work;
    Valuation val;
    TableauNode node{toString(root), {}};
    if (place(root, work, val)) {
        node.children = {{"X (zatvorena)", {}}};
        return node;
    }
    node.children = buildCont(std::move(work), std::move(val));
    return node;
}

// Ispisuje stablo: 1 dete se nastavlja u istoj koloni (alfa lanac),
// 2 dece se granaju box-crtama.
void render(const TableauNode& n, const std::string& prefix, std::ostream& os) {
    os << n.label << '\n';
    const auto& ch = n.children;
    if (ch.size() == 1) {
        os << prefix;
        render(ch[0], prefix, os);
    } else if (ch.size() == 2) {
        os << prefix << "|-- ";
        render(ch[0], prefix + "|   ", os);
        os << prefix << "`-- ";
        render(ch[1], prefix + "    ", os);
    }
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

void printValidityTableau(const FormulaPtr& f, std::ostream& os) {
    render(buildTree(F(f)), "", os);
}

void printSatisfiabilityTableau(const FormulaPtr& f, std::ostream& os) {
    render(buildTree(T(f)), "", os);
}

}
