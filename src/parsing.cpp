#include "parsing.h"
#include <algorithm>

#include <carma/carma.h>

#include "exceptions.h"

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

CodeRange makeCodeCharacters(const std::string& string) {
    auto result = CodeRange{};
    INIT_RANGE(result, string.size());
    
    auto column = size_t{0};
    auto row = size_t{0};
    auto index = size_t{0};
    for (const auto& character : string) {
        result.data[index] = CodeCharacter{character, row, column};
        ++index;
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
    return !isNameCharacter(*it0);
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
    return !IS_EMPTY(code) && code.data->character == c;
}

CodeRange parseWhiteSpace(CodeRange code) {
    return parseWhile(code, isWhiteSpace);
}

CodeRange parseCharacter(CodeRange code) {
    throwIfEmpty(code);
    DROP_FIRST(code);
    return code;
}

CodeRange parseCharacter(CodeRange code, char expected) {
    throwIfEmpty(code);
    auto it = code.data;
    const auto actual = it->character;
    if (it->character != expected) {
        throwException(
            "Parsing expected \'%c\' but got \'%c\'",
            expected,
            actual,
            describeLocation(code).c_str()
        );
    }
    DROP_FIRST(code);
    return code;
}

CodeRange parseOptionalCharacter(CodeRange code, char c) {
    if (!IS_EMPTY(code) && code.data->character == c) {
        DROP_FIRST(code);
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
                describeLocation(code).c_str()
            );
        }
        code = parseCharacter(code, *it);
    }
    return code;
}

void throwIfEmpty(CodeRange code) {
    if (IS_EMPTY(code)) {
        throwException(
            "Unexpected end of source while parsing%s", describeLocation(code).c_str()
        );
    }
}

void throwParseException(CodeRange code) {
    throwException("Does not recognize expression to parse%s", describeLocation(code).c_str());
}
