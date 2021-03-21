#pragma once

#include <stdexcept>
#include <string>
#include <vector>

struct CodeCharacter {
    char character;
    size_t row;
    size_t column;
};

struct ParseException : public std::runtime_error
{
    ParseException(const std::string& description, const CodeCharacter* it);
    using runtime_error::runtime_error;
};

void verifyThisIsNotTheEnd(const CodeCharacter* it, const CodeCharacter* last);
void throwParseException(const CodeCharacter* it, const CodeCharacter* last);

char rawCharacter(CodeCharacter c);

std::string rawString(const CodeCharacter* first, const CodeCharacter* last);

std::vector<CodeCharacter> makeCodeCharacters(const std::string& string);

bool haveSameCharacters(CodeCharacter a, CodeCharacter b);

bool isDigit(CodeCharacter c);

bool isSign(CodeCharacter c);

bool isLetter(CodeCharacter c);

bool isNameCharacter(CodeCharacter c);

bool isWhiteSpace(CodeCharacter c);

bool isKeyword(
    const CodeCharacter* first, const CodeCharacter* last, const std::string& keyword
);

const CodeCharacter* parseWhiteSpace(
    const CodeCharacter* first, const CodeCharacter* last
);

const CodeCharacter* parseCharacter(
    const CodeCharacter* it, const CodeCharacter* last, char expected);

template<typename Predicate>
const CodeCharacter* parseCharacter(
    const CodeCharacter* it, const CodeCharacter* last, Predicate predicate
) {
    verifyThisIsNotTheEnd(it, last);
    if (!predicate(*it)) {
        throw ParseException(std::string{"Parser got unexpected char"} + it->character);
    }
    ++it;
    return it;
}

const CodeCharacter* parseOptionalCharacter(const CodeCharacter* it, const CodeCharacter* last, char c);

template<typename Predicate>
const CodeCharacter* parseOptionalCharacter(const CodeCharacter* it, const CodeCharacter* last, Predicate predicate) {
    if (it == last) {
        return it;
    }
    if (predicate(*it)) {
        ++it;
    }
    return it;
}

const CodeCharacter* parseKeyword(const CodeCharacter* first, const CodeCharacter* last, std::string keyword);
