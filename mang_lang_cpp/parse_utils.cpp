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

std::string rawString(const CodeCharacter* first, const CodeCharacter* last) {
    auto s = std::string{};
    std::transform(first, last, std::back_inserter(s), rawCharacter);
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

bool isKeyword(
    const CodeCharacter* first,
    const CodeCharacter* last,
    const std::string& keyword
) {
    const auto w = makeCodeCharacters(keyword);
    if (last - first < w.end() - w.begin()) {
        return false;
    }
    return std::equal(w.begin(), w.end(), first, haveSameCharacters);
}

const CodeCharacter* parseWhiteSpace(
    const CodeCharacter* first, const CodeCharacter* last
) {
    return std::find_if_not(first, last, isWhiteSpace);
}

const CodeCharacter* parseCharacter(
    const CodeCharacter* it, const CodeCharacter* last, char expected
) {
    verifyThisIsNotTheEnd(it, last);
    const auto actual = it->character;
    if (it->character != expected) {
        const auto description = std::string{"Parsing expected \'"}
            + expected + "\' but got \'" + actual + "\'";
        throw ParseException(description, it);
    }
    ++it;
    return it;
}

const CodeCharacter* parseOptionalCharacter(const CodeCharacter* it, char c) {
    if (it->character == c) {
        ++it;
    }
    return it;
}

const CodeCharacter* parseKeyword(
    const CodeCharacter* first, const CodeCharacter* last, std::string keyword) {
    if (last - first < keyword.end() - keyword.begin()) {
        throw ParseException("Reached end of file when parsing " + keyword, first);
    }
    auto it = first;
    for (const auto c : keyword) {
        it = parseCharacter(it, last, c);
    }
    return it;
}

void verifyThisIsNotTheEnd(const CodeCharacter* it, const CodeCharacter* last) {
    if (it == last) {
        throw ParseException("Unexpected end of source while parsing", (it - 1));
    }
}

void throwParseException(const CodeCharacter* it, const CodeCharacter*) {
    throw ParseException("Does not recognize expression to parse", it);
}
