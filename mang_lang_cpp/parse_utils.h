#pragma once

#include <stdexcept>
#include <string>

struct CodeCharacter {
    char character;
    size_t row;
    size_t column;
};

struct ParseException : public std::runtime_error
{
    using runtime_error::runtime_error;
};

char rawCharacter(CodeCharacter c);

std::string rawString(const CodeCharacter* first, const CodeCharacter* last);

bool isDigit(CodeCharacter c);

bool isSign(CodeCharacter c);

bool isNumber(CodeCharacter c);

bool isLetter(CodeCharacter c);

bool isNameCharacter(CodeCharacter c);

bool isWhiteSpace(CodeCharacter c);

bool isList(CodeCharacter c);

bool isDictionary(CodeCharacter c);

bool isStringSeparator(CodeCharacter c);

const CodeCharacter* parseWhiteSpace(
    const CodeCharacter* first, const CodeCharacter* last
);

const CodeCharacter* parseCharacter(const CodeCharacter* it, char expected);

template<typename Predicate>
const CodeCharacter* parseCharacter(const CodeCharacter* it, Predicate predicate) {
    if (!predicate(*it)) {
        throw ParseException(std::string{"Parser got unexpected char"} + it->character);
    }
    ++it;
    return it;
}

const CodeCharacter* parseOptionalCharacter(const CodeCharacter* it, char c);

template<typename Predicate>
const CodeCharacter* parseOptionalCharacter(const CodeCharacter* it, Predicate predicate) {
    if (predicate(*it)) {
        ++it;
    }
    return it;
}
