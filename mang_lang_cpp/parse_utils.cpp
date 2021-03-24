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

std::string rawString(CodeRange code_range) {
    auto s = std::string{};
    std::transform(code_range.begin(), code_range.end(), std::back_inserter(s), rawCharacter);
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

bool isKeyword(CodeRange code_range, const std::string& keyword) {
    const auto w = makeCodeCharacters(keyword);
    if (code_range.size() < w.size()) {
        return false;
    }
    return std::equal(w.begin(), w.end(), code_range.begin(), haveSameCharacters);
}

bool isAnyKeyword(CodeRange code_range, const std::vector<std::string>& keywords) {
    for (const auto& keyword : keywords) {
        if (isKeyword(code_range, keyword)) {
            return true;
        }
    }
    return false;
}

CodeRange parseWhiteSpace(CodeRange code_range) {
    auto first = std::find_if_not(code_range.begin(), code_range.end(), isWhiteSpace);
    return {first, code_range.end()};
}

CodeRange parseCharacter(CodeRange code_range, char expected) {
    verifyThisIsNotTheEnd(code_range);
    auto it = code_range.begin();
    const auto actual = it->character;
    if (it->character != expected) {
        const auto description = std::string{"Parsing expected \'"}
            + expected + "\' but got \'" + actual + "\'";
        throw ParseException(description, it);
    }
    ++it;
    return {it, code_range.end()};
}

CodeRange parseOptionalCharacter(CodeRange code_range, char c) {
    if (code_range.empty()) {
        return code_range;
    }
    auto it = code_range.begin();
    if (it->character == c) {
        ++it;
    }
    return {it, code_range.end()};
}

CodeRange parseKeyword(CodeRange code_range, std::string keyword) {
    if (code_range.size() < keyword.size()) {
        throw ParseException(
            "Reached end of file when parsing " + keyword, code_range.begin()
        );
    }
    auto range = code_range;
    for (const auto c : keyword) {
        range = parseCharacter(range, c);
    }
    return range;
}

void verifyThisIsNotTheEnd(CodeRange code_range) {
    if (code_range.first == code_range.last) {
        throw ParseException(
            "Unexpected end of source while parsing", (code_range.first - 1)
        );
    }
}

void throwParseException(CodeRange code_range) {
    throw ParseException("Does not recognize expression to parse", code_range.begin());
}
