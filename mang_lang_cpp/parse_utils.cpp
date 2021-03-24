#include "parse_utils.h"
#include <algorithm>
#include <cassert>

ParseException::ParseException(const std::string& description, const CodeCharacter* it)
    : std::runtime_error(
        description
        + " at row " + std::to_string(it->row)
        + " and column " + std::to_string(it->column)
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

bool isKeyword(CodeRange code, const std::string& keyword) {
    const auto w = makeCodeCharacters(keyword);
    if (code.size() < w.size()) {
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

CodeRange parseWhiteSpace(CodeRange code) {
    auto first = std::find_if_not(code.begin(), code.end(), isWhiteSpace);
    return {first, code.end()};
}

CodeRange parseCharacter(CodeRange code, char expected) {
    verifyThisIsNotTheEnd(code);
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
    auto range = code;
    for (const auto c : keyword) {
        range = parseCharacter(range, c);
    }
    return range;
}

void verifyThisIsNotTheEnd(CodeRange code) {
    if (code.first == code.last) {
        throw ParseException(
            "Unexpected end of source while parsing", (code.first - 1)
        );
    }
}

void throwParseException(CodeRange code) {
    throw ParseException("Does not recognize expression to parse", code.begin());
}
