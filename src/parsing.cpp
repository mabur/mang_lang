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

CodeRange firstPart(CodeRange whole, CodeRange last_part) {
    return CodeRange{whole.data, CharacterIndex(whole.count - last_part.count)};
}

CodeRange lastPart(CodeRange whole, CodeRange middle_part) {
    auto whole_end = whole.data + whole.count;
    auto middle_part_end = CharacterIndex(middle_part.data + middle_part.count);
    auto count = CharacterIndex(whole_end - middle_part_end);
    return CodeRange{middle_part_end, count};
}

static
bool isDigit(char c) {
    return isdigit(c);
}

static
bool isLetter(char c) {
    return std::isalpha(c);
}

static
bool isNameCharacter(char c) {
    return isLetter(c) || isDigit(c) || c == '_';
}

static
bool isWhiteSpace(char c) {
    return isspace(c);
}

bool isKeyword(CodeRange code, const char* word) {
    auto word_range = STRING_VIEW(word);
    FOR_EACH2(it0, it1, code, word_range) {
        if (storage.code_characters.data[it0] != *it1) {
            return false;
        }
    }
    if (it1 != END_POINTER(word_range)) {
        return false;
    }
    if (it0 == END_POINTER(code)) {
        return true;
    }
    return !isNameCharacter(storage.code_characters.data[it0]);
}

bool startsWith(CodeRange code, const char* word) {
    auto word_range = STRING_VIEW(word);
    FOR_EACH2(it0, it1, code, word_range) {
        if (storage.code_characters.data[it0] != *it1) {
            return false;
        }
    }
    return it1 == END_POINTER(word_range);
}

bool startsWith(CodeRange code, char c) {
    return !IS_EMPTY(code) && firstCharacter(code) == c;
}

bool startsWithDigit(CodeRange code) {
    return !IS_EMPTY(code) && isDigit(firstCharacter(code));
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
            "Unexpected end of source while parsing %s", describeLocation(code)
        );
    }
}

void throwParseException(CodeRange code) {
    throwException("Does not recognize expression to parse %s", describeLocation(code));
}

CodeRange parseRawName(CodeRange code) {
    return parseWhile(code, isNameCharacter);    
}


static double parseDigitAsDouble(CodeRange code) {
    return firstCharacter(code) - '0';
}

ParseResult parseDecimal(CodeRange code) {
    if (IS_EMPTY(code)) {
        throwException("Reached end of file when parsing number %s", describeLocation(code));
    }
    auto start = code;
    bool is_negative = false;
    if (startsWith(code, '+')) {
        DROP_FRONT(code);
    }
    else if (startsWith(code, '-')) {
        is_negative = true;
        DROP_FRONT(code);
    }
    if (IS_EMPTY(code)) {
        throwException("Reached end of file when parsing number %s", describeLocation(code));
    }
    double integer_part = 0.0;
    while (startsWithDigit(code)) {
        integer_part = integer_part * 10 + parseDigitAsDouble(code);
        DROP_FRONT(code);
    }
    double fraction_part = 0.0;
    if (startsWith(code, '.')) {
        DROP_FRONT(code);
        double divisor = 10.0;
        while (startsWithDigit(code)) {
            fraction_part += parseDigitAsDouble(code) / divisor;
            divisor *= 10;
            DROP_FRONT(code);
        }
    }
    double result = integer_part + fraction_part;
    if (is_negative) {
        result = -result;
    }
    return {result, (CharacterIndex)(code.data - start.data)};
}
