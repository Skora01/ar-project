#include "parser.hpp"

#include <cctype>
#include <string>
#include <vector>

namespace tablo {

namespace {

enum class TokenKind { LParen, RParen, Not, And, Or, Impl, Eq, Atom, True, False, End };

struct Token {
    TokenKind kind;
    std::string name;
};

std::vector<Token> tokenize(const std::string& s) {
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < s.size()) {
        char c = s[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            ++i;
            continue;
        }

        switch (c) {
            case '(': tokens.push_back({TokenKind::LParen, ""}); ++i; continue;
            case ')': tokens.push_back({TokenKind::RParen, ""}); ++i; continue;
            case '~': tokens.push_back({TokenKind::Not, ""}); ++i; continue;
            case '&': tokens.push_back({TokenKind::And, ""}); ++i; continue;
            case '|': tokens.push_back({TokenKind::Or, ""}); ++i; continue;
            case '-':
                // Jedini validan nastavak je ->
                if (i + 1 < s.size() && s[i + 1] == '>') {
                    tokens.push_back({TokenKind::Impl, ""});
                    i += 2;
                    continue;
                }
                throw ParseError("ocekivano '->' na poziciji " + std::to_string(i));
            case '<':
                // Jedini validan nastavak je <->
                if (i + 2 < s.size() && s[i + 1] == '-' && s[i + 2] == '>') {
                    tokens.push_back({TokenKind::Eq, ""});
                    i += 3;
                    continue;
                }
                throw ParseError("ocekivano '<->' na poziciji " + std::to_string(i));
            default:
                break;
        }

        // Atom ili konstanta.
        if (std::isalpha(static_cast<unsigned char>(c))) {
            size_t start = i;
            while (i < s.size() &&
                   (std::isalnum(static_cast<unsigned char>(s[i])) || s[i] == '_')) {
                ++i;
            }
            std::string word = s.substr(start, i - start);
            if (word == "true")       tokens.push_back({TokenKind::True, ""});
            else if (word == "false") tokens.push_back({TokenKind::False, ""});
            else                      tokens.push_back({TokenKind::Atom, word});
            continue;
        }

        throw ParseError(std::string("nepoznat znak '") + c + "' na poziciji " +
                         std::to_string(i));
    }

    tokens.push_back({TokenKind::End, ""});
    return tokens;
}

// Rekurzivni spust nad nizom tokena.
class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

    FormulaPtr parse() {
        FormulaPtr f = parseEq();
        expect(TokenKind::End, "visak simbola nakon kraja formule");
        return f;
    }

private:
    std::vector<Token> tokens_;
    size_t pos_ = 0;

    bool check(TokenKind k) const { return tokens_[pos_].kind == k; }

    bool accept(TokenKind k) {
        if (check(k)) { ++pos_; return true; }
        return false;
    }

    void expect(TokenKind k, const char* msg) {
        if (!accept(k)) throw ParseError(msg);
    }

    //(najnizi prioritet).
    FormulaPtr parseEq() {
        FormulaPtr left = parseImpl();
        if (accept(TokenKind::Eq)) {
            return ptr(Binary{Binary::Eq, left, parseEq()});
        }
        return left;
    }

    FormulaPtr parseImpl() {
        FormulaPtr left = parseOr();
        if (accept(TokenKind::Impl)) {
            return ptr(Binary{Binary::Impl, left, parseImpl()});
        }
        return left;
    }

    FormulaPtr parseOr() {
        FormulaPtr left = parseAnd();
        while (accept(TokenKind::Or)) {
            left = ptr(Binary{Binary::Or, left, parseAnd()});
        }
        return left;
    }

    FormulaPtr parseAnd() {
        FormulaPtr left = parseNot();
        while (accept(TokenKind::And)) {
            left = ptr(Binary{Binary::And, left, parseNot()});
        }
        return left;
    }

    // ~ ima najvisi prioritet i moze da se gomila (~~p).
    FormulaPtr parseNot() {
        if (accept(TokenKind::Not)) {
            return ptr(Not{parseNot()});
        }
        return parsePrimary();
    }

    FormulaPtr parsePrimary() {
        if (accept(TokenKind::LParen)) {
            FormulaPtr f = parseEq();
            expect(TokenKind::RParen, "ocekivana zatvorena zagrada ')'");
            return f;
        }
        if (check(TokenKind::Atom)) {
            std::string name = tokens_[pos_].name;
            ++pos_;
            return ptr(Atom{name});
        }
        if (accept(TokenKind::True))  return ptr(True{});
        if (accept(TokenKind::False)) return ptr(False{});

        throw ParseError("ocekivan atom, konstanta ili '('");
    }
};

}

FormulaPtr parse(const std::string& input) {
    Parser parser(tokenize(input));
    return parser.parse();
}

}
