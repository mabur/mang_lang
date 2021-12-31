#include "parsing.h"
#include <algorithm>
#include <cassert>
#include <iostream>

ParseException::ParseException(const std::string& description, const CodeCharacter* it)
    : std::runtime_error(
        description
        + " at row " + std::to_string(it->row + 1)
        + " and column " + std::to_string(it->column + 1)
    )
{}

char rawCharacter(CodeCharacter c) {
    return c.character;
}

std::string rawString(CodeRange code) {
    auto s = std::string{};
    std::transform(code.begin(), code.end(), std::back_inserter(s), rawCharacter);
    return s;
}

std::vector<CodeCharacter> makeCodeCharacters(const std::string& string) {
    auto result = std::vector<CodeCharacter>{};
    auto column = size_t{0};
    auto row = size_t{0};
    for (const auto& character : string) {
        result.push_back({character, row, column});
        ++column;
        if (character == '\n') {
            ++row;
            column = 0;
        }
    }
    return result;
}

bool isDigit(CodeCharacter c) {
    return isdigit(c.character);
}

bool isSign(CodeCharacter c) {
    return c.character == '+' || c.character == '-';
}

bool isLetter(CodeCharacter c) {
    return std::isalpha(c.character);
}

bool isNameCharacter(CodeCharacter c) {
    return isLetter(c) || isDigit(c) || c.character == '_';
}

bool isWhiteSpace(CodeCharacter c) {
    return isspace(c.character);
}

bool haveSameCharacters(CodeCharacter a, CodeCharacter b) {
    return a.character == b.character;
}

bool isLiteral(CodeRange code, const std::string& literal) {
    const auto w = makeCodeCharacters(literal);
    if (code.size() < w.size()) {
        return false;
    }
    if (!std::equal(w.begin(), w.end(), code.begin(), haveSameCharacters)) {
        return false;
    }
    const auto after = code.begin() + w.size();
    if (after == code.end()) {
        return true;
    }
    return !isLetter(*after);
}

bool isKeyword(CodeRange code, const std::string& keyword) {
    const auto w = makeCodeCharacters(keyword);
    if (code.size() <= w.size()) {
        return false;
    }
    const auto after = code.begin() + w.size();
    if (isNameCharacter(*after)) {
        return false;
    }
    return std::equal(w.begin(), w.end(), code.begin(), haveSameCharacters);
}

bool isAnyKeyword(CodeRange code, const std::vector<std::string>& keywords) {
    for (const auto& keyword : keywords) {
        if (isKeyword(code, keyword)) {
            return true;
        }
    }
    return false;
}

bool startsWith(CodeRange code, char c) {
    return !code.empty() && code.begin()->character == c;
}

CodeRange parseWhiteSpace(CodeRange code) {
    return parseWhile(code, isWhiteSpace);
}

CodeRange parseCharacter(CodeRange code) {
    throwIfEmpty(code);
    auto it = code.begin();
    ++it;
    return {it, code.end()};
}

CodeRange parseCharacter(CodeRange code, char expected) {
    throwIfEmpty(code);
    auto it = code.begin();
    const auto actual = it->character;
    if (it->character != expected) {
        const auto description = std::string{"Parsing expected \'"}
            + expected + "\' but got \'" + actual + "\'";
        throw ParseException(description, it);
    }
    ++it;
    return {it, code.end()};
}

CodeRange parseOptionalCharacter(CodeRange code, char c) {
    if (code.empty()) {
        return code;
    }
    auto it = code.begin();
    if (it->character == c) {
        ++it;
    }
    return {it, code.end()};
}

CodeRange parseKeyword(CodeRange code, std::string keyword) {
    if (code.size() < keyword.size()) {
        throw ParseException(
            "Reached end of file when parsing " + keyword, code.begin()
        );
    }
    for (const auto c : keyword) {
        code = parseCharacter(code, c);
    }
    return code;
}

void throwIfEmpty(CodeRange code) {
    if (code.empty()) {
        throw ParseException(
            "Unexpected end of source while parsing", (code.first - 1)
        );
    }
}

void throwParseException(CodeRange code) {
    throw ParseException("Does not recognize expression to parse", code.begin());
}
