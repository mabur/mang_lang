#include "parsing.h"
#include <algorithm>
#include <cassert>
#include <iostream>

std::string serializeCodeCharacter(const CodeCharacter* c) {
    return "row " + std::to_string(c->row + 1) + " and column "
        + std::to_string(c->column + 1);
}

std::string describeLocation(CodeRange code) {
    if (code.begin() == nullptr || code.end() == nullptr) {
        return " at unknown location.";
    }
    return " between " +
        serializeCodeCharacter(code.begin()) + " and " +
        serializeCodeCharacter(code.end());
}

ParseException::ParseException(const std::string& description, CodeRange code)
    : std::runtime_error(description + describeLocation(code))
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
        result.push_back(CodeCharacter{character, row, column});
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

bool isKeyword(CodeRange code, const std::string& word) {
    const auto w = makeCodeCharacters(word);
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
    return !isNameCharacter(*after);
}

bool startsWith(CodeRange code, const std::string& word) {
    const auto w = makeCodeCharacters(word);
    if (code.size() < w.size()) {
        return false;
    }
    return std::equal(w.begin(), w.end(), code.begin(), haveSameCharacters);
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
    return CodeRange{it, code.end()};
}

CodeRange parseCharacter(CodeRange code, char expected) {
    throwIfEmpty(code);
    auto it = code.begin();
    const auto actual = it->character;
    if (it->character != expected) {
        const auto description = std::string{"Parsing expected \'"}
            + expected + "\' but got \'" + actual + "\'";
        throw ParseException(description, CodeRange{it, code.end()});
    }
    ++it;
    return CodeRange{it, code.end()};
}

CodeRange parseOptionalCharacter(CodeRange code, char c) {
    if (code.empty()) {
        return code;
    }
    auto it = code.begin();
    if (it->character == c) {
        ++it;
    }
    return CodeRange{it, code.end()};
}

CodeRange parseKeyword(CodeRange code, const std::string& keyword) {
    if (code.size() < keyword.size()) {
        throw ParseException(
            "Reached end of file when parsing " + keyword, code
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
            "Unexpected end of source while parsing", code
        );
    }
}

void throwParseException(CodeRange code) {
    throw ParseException("Does not recognize expression to parse", code);
}
