#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <carma/carma.h>

#include "exceptions.h"
#include "string.h"

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

const char* describeLocation(CodeRange code);

void throwIfEmpty(CodeRange code);
void throwParseException(CodeRange code);

std::string rawString(CodeRange code);

bool isDigit(CodeCharacter c);

bool isSign(CodeCharacter c);

bool isLetter(CodeCharacter c);

bool isNameCharacter(CodeCharacter c);

bool isWhiteSpace(CodeCharacter c);

bool isKeyword(CodeRange code, const char* word);

bool startsWith(CodeRange code, const char* word);

bool startsWith(CodeRange code, char c);

CodeRange parseWhiteSpace(CodeRange code);

CodeRange parseCharacter(CodeRange code);

CodeRange parseCharacter(CodeRange code, char expected);

template<typename Predicate>
CodeRange parseCharacter(CodeRange code, Predicate predicate) {
    throwIfEmpty(code);
    auto it = code.data;
    if (!predicate(*it)) {
        throwException(
            "Parser got unexpected char%c%s",
            it->character,
            describeLocation(code)
        );
    }
    DROP_FRONT(code);
    return code;
}

CodeRange parseOptionalCharacter(CodeRange code, char c);

template<typename Predicate>
CodeRange parseOptionalCharacter(CodeRange code, Predicate predicate) {
    if (!IS_EMPTY(code) && predicate(*code.data)) {
        DROP_FRONT(code);
    }
    return code;
}

CodeRange parseKeyword(CodeRange code, const char* keyword);

template<typename Predicate>
CodeRange parseWhile(CodeRange code, Predicate predicate) {
    while (!IS_EMPTY(code) && predicate(*code.data)) {
        DROP_FRONT(code);
    }
    return code;
}
