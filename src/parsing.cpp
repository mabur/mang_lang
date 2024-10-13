#include "parsing.h"

#include <string>

#include <carma/carma.h>

#include "expression.h"
#include "exceptions.h"
#include "factory.h"
#include "string.h"

template<typename Predicate>
CodeRange parseWhile(CodeRange code, Predicate predicate) {
    while (!IS_EMPTY(code) && predicate(firstCharacter(code))) {
        DROP_FRONT(code);
    }
    return code;
}

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

template<typename Predicate>
CodeRange parseOptionalCharacterIf(CodeRange code, Predicate predicate) {
    if (!IS_EMPTY(code) && predicate(firstCharacter(code))) {
        DROP_FRONT(code);
    }
    return code;
}

CodeRange firstPart(CodeRange whole, CodeRange last_part) {
    return CodeRange{whole.data, whole.count - last_part.count};
}

CodeRange lastPart(CodeRange whole, CodeRange middle_part) {
    auto whole_end = whole.data + whole.count;
    auto middle_part_end = middle_part.data + middle_part.count;
    auto count = static_cast<size_t>(whole_end - middle_part_end);
    return CodeRange{middle_part_end, count};
}

std::string rawString(CodeRange code) {
    auto s = std::string{};
    s.reserve(code.count);
    FOR_EACH(c, code) {
        s.push_back(c->character);
    }
    return s;
}

bool isDigit(char c) {
    return isdigit(c);
}

bool isSign(char c) {
    return c == '+' || c == '-';
}

bool isLetter(char c) {
    return std::isalpha(c);
}

bool isNameCharacter(char c) {
    return isLetter(c) || isDigit(c) || c == '_';
}

bool isWhiteSpace(char c) {
    return isspace(c);
}

bool isKeyword(CodeRange code, const char* word) {
    auto word_range = makeStaticString(word);
    FOR_EACH2(it0, it1, code, word_range) {
        if (it0->character != *it1) {
            return false;
        }
    }
    if (it1 != END_POINTER(word_range)) {
        return false;
    }
    if (it0 == END_POINTER(code)) {
        return true;
    }
    return !isNameCharacter(it0->character);
}

bool startsWith(CodeRange code, const char* word) {
    auto word_range = makeStaticString(word);
    FOR_EACH2(it0, it1, code, word_range) {
        if (it0->character != *it1) {
            return false;
        }
    }
    return it1 == END_POINTER(word_range);
}

bool startsWith(CodeRange code, char c) {
    return !IS_EMPTY(code) && firstCharacter(code) == c;
}

CodeRange parseWhiteSpace(CodeRange code) {
    return parseWhile(code, isWhiteSpace);
}

CodeRange parseCharacter(CodeRange code) {
    throwIfEmpty(code);
    DROP_FRONT(code);
    return code;
}

CodeRange parseCharacter(CodeRange code, char expected) {
    throwIfEmpty(code);
    const auto actual = firstCharacter(code);
    if (actual != expected) {
        throwException(
            "Parsing expected \'%c\' but got \'%c\'%s",
            expected,
            actual,
            describeLocation(code)
        );
    }
    DROP_FRONT(code);
    return code;
}

CodeRange parseOptionalCharacter(CodeRange code, char c) {
    if (!IS_EMPTY(code) && firstCharacter(code) == c) {
        DROP_FRONT(code);
    }
    return code;
}

CodeRange parseKeyword(CodeRange code, const char* keyword) {
    auto it = keyword;
    for (; *it != '\0'; ++it) {
        if (IS_EMPTY(code)) {
            throwException(
                "Reached end of file when parsing %s%s",
                keyword,
                describeLocation(code)
            );
        }
        code = parseCharacter(code, *it);
    }
    return code;
}

void throwIfEmpty(CodeRange code) {
    if (IS_EMPTY(code)) {
        throwException(
            "Unexpected end of source while parsing%s", describeLocation(code)
        );
    }
}

void throwParseException(CodeRange code) {
    throwException("Does not recognize expression to parse%s", describeLocation(code));
}

CodeRange parseRawNumber(CodeRange code) {
    auto whole = code;
    code = parseOptionalCharacterIf(code, isSign);
    code = parseCharacterIf(code, isDigit);
    code = parseWhile(code, isDigit);
    code = parseOptionalCharacter(code, '.');
    code = parseWhile(code, isDigit);
    return firstPart(whole, code);
}

CodeRange parseRawName(CodeRange code) {
    return parseWhile(code, isNameCharacter);    
}
