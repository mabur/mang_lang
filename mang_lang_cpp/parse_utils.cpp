#include "parse_utils.h"
#include <algorithm>

char rawCharacter(CodeCharacter c) {
    return c.character;
}

std::string rawString(const CodeCharacter* first, const CodeCharacter* last) {
    auto s = std::string{};
    std::transform(first, last, std::back_inserter(s), rawCharacter);
    return s;
}

bool isDigit(CodeCharacter c) {
    return isdigit(c.character);
}

bool isSign(CodeCharacter c) {
    return c.character == '+' || c.character == '-';
}

bool isNumber(CodeCharacter c) {
    return isSign(c) || isDigit(c);
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

bool isList(CodeCharacter c) {
    return c.character == '[';
}

bool isDictionary(CodeCharacter c) {
    return c.character == '{';
}

bool isStringSeparator(CodeCharacter c) {
    return c.character == '"';
}

const CodeCharacter* parseWhiteSpace(
    const CodeCharacter* first, const CodeCharacter* last
) {
    return std::find_if_not(first, last, isWhiteSpace);
}

const CodeCharacter* parseCharacter(const CodeCharacter* it, char expected) {
    const auto actual = it->character;
    if (it->character != expected) {
        throw ParseException(
            std::string{"Parsing expected "} + expected + " but got " + actual
        );
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
