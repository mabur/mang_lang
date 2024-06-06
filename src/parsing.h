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

// TODO: merge CodeCharacters & CodeRange
struct CodeCharacters {
    CodeCharacter* data;
    size_t count;
};

struct CodeRange {
    const CodeCharacter* data = nullptr;
    size_t count;

    const CodeCharacter* end() const {return data + count;}
};

inline
CodeRange makeCodeRange(const CodeCharacter* data, size_t count) {
    return CodeRange{data, count};
}

inline CodeRange firstPart(CodeRange whole, CodeRange last_part) {
    return makeCodeRange(whole.data, whole.count - last_part.count);
}

inline CodeRange lastPart(CodeRange whole, CodeRange middle_part) {
    auto whole_end = whole.data + whole.count;
    auto middle_part_end = middle_part.data + middle_part.count;
    auto count = static_cast<size_t>(whole_end - middle_part_end);
    return makeCodeRange(middle_part_end, count);
}

inline
CodeRange dropFirst(CodeRange code) {
    code.data++;
    code.count--;
    return code;
}

struct ParseException : public std::runtime_error
{
    ParseException(const std::string& description, CodeRange code);
    using runtime_error::runtime_error;
};

std::string describeLocation(CodeRange code);

void throwIfEmpty(CodeRange code);
void throwParseException(CodeRange code);

std::string rawString(CodeRange code);

CodeCharacters makeCodeCharacters(const std::string& string);

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
    return dropFirst(code);
}

CodeRange parseOptionalCharacter(CodeRange code, char c);

template<typename Predicate>
CodeRange parseOptionalCharacter(CodeRange code, Predicate predicate) {
    if (!IS_EMPTY(code) && predicate(*code.data)) {
        return dropFirst(code);
    }
    return code;
}

CodeRange parseKeyword(CodeRange code, const std::string& keyword);

template<typename Predicate>
CodeRange parseWhile(CodeRange code, Predicate predicate) {
    for (; !IS_EMPTY(code) && predicate(*code.data); code = dropFirst(code)) {
    }
    return code;
}
