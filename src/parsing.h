#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <carma/carma.h>

#include "expression.h"
#include "exceptions.h"
#include "factory.h"
#include "string.h"

CodeRange firstPart(CodeRange whole, CodeRange last_part);

CodeRange lastPart(CodeRange whole, CodeRange middle_part);

const char* describeLocation(CodeRange code);

void throwIfEmpty(CodeRange code);
void throwParseException(CodeRange code);

std::string rawString(CodeRange code);

bool isDigit(char c);

bool isSign(char c);

bool isLetter(char c);

bool isNameCharacter(char c);

bool isWhiteSpace(char c);

bool isKeyword(CodeRange code, const char* word);

bool startsWith(CodeRange code, const char* word);

bool startsWith(CodeRange code, char c);

CodeRange parseWhiteSpace(CodeRange code);

CodeRange parseCharacter(CodeRange code);

CodeRange parseCharacter(CodeRange code, char expected);

template<typename Predicate>
CodeRange parseCharacterIf(CodeRange code, Predicate predicate) {
    throwIfEmpty(code);
    auto c = firstCharacter(code);
    if (!predicate(c)) {
        throwException("Parser got unexpected char%c%s", c, describeLocation(code));
    }
    DROP_FRONT(code);
    return code;
}

CodeRange parseOptionalCharacter(CodeRange code, char c);

template<typename Predicate>
CodeRange parseOptionalCharacter(CodeRange code, Predicate predicate) {
    if (!IS_EMPTY(code) && predicate(firstCharacter(code))) {
        DROP_FRONT(code);
    }
    return code;
}

CodeRange parseKeyword(CodeRange code, const char* keyword);

template<typename Predicate>
CodeRange parseWhile(CodeRange code, Predicate predicate) {
    while (!IS_EMPTY(code) && predicate(firstCharacter(code))) {
        DROP_FRONT(code);
    }
    return code;
}
