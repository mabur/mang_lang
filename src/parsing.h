#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <carma/carma.h>

struct CodeCharacter {
    char character = 'a';
    size_t row = 0;
    size_t column = 0;
};

struct CodeRange {
    CodeCharacter* data;
    size_t count;
};

CodeRange firstPart(CodeRange whole, CodeRange last_part);

CodeRange lastPart(CodeRange whole, CodeRange middle_part);

struct ParseException : public std::runtime_error
{
    ParseException(const std::string& description, CodeRange code);
    using runtime_error::runtime_error;
};

std::string describeLocation(CodeRange code);

void throwIfEmpty(CodeRange code);
void throwParseException(CodeRange code);

std::string rawString(CodeRange code);

CodeRange makeCodeCharacters(const std::string& string);

bool isDigit(CodeCharacter c);

bool isSign(CodeCharacter c);

bool isLetter(CodeCharacter c);

bool isNameCharacter(CodeCharacter c);

bool isWhiteSpace(CodeCharacter c);

bool isKeyword(CodeRange code, const std::string& word);

bool startsWith(CodeRange code, const std::string& word);

bool startsWith(CodeRange code, char c);

CodeRange parseWhiteSpace(CodeRange code);

CodeRange parseCharacter(CodeRange code);

CodeRange parseCharacter(CodeRange code, char expected);

template<typename Predicate>
CodeRange parseCharacter(CodeRange code, Predicate predicate) {
    throwIfEmpty(code);
    auto it = code.data;
    if (!predicate(*it)) {
        throw ParseException(std::string{"Parser got unexpected char"} + it->character);
    }
    DROP_FIRST(code);
    return code;
}

CodeRange parseOptionalCharacter(CodeRange code, char c);

template<typename Predicate>
CodeRange parseOptionalCharacter(CodeRange code, Predicate predicate) {
    if (!IS_EMPTY(code) && predicate(*code.data)) {
        DROP_FIRST(code);
    }
    return code;
}

CodeRange parseKeyword(CodeRange code, const std::string& keyword);

template<typename Predicate>
CodeRange parseWhile(CodeRange code, Predicate predicate) {
    while (!IS_EMPTY(code) && predicate(*code.data)) {
        DROP_FIRST(code);
    }
    return code;
}
