#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

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
    const CodeCharacter* last = nullptr;
    const CodeCharacter* begin() const {return data;}
    const CodeCharacter* end() const {return last;}
    bool empty() const {return data == end();}
    size_t size() const {return end() - begin();}
};

inline
CodeRange makeCodeRange(const CodeCharacter* first, const CodeCharacter* last) {
    return CodeRange{first, last};
}

inline
CodeRange makeCodeRange(const CodeCharacter* data, size_t count) {
    return CodeRange{data, data + count};
}

inline CodeRange firstPart(CodeRange whole, CodeRange last_part) {
    return makeCodeRange(whole.begin(), last_part.begin());
}

inline CodeRange lastPart(CodeRange whole, CodeRange first_part) {
    return makeCodeRange(first_part.end(), whole.end());
}

CodeRange dropFirst(CodeRange code);

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
    auto it = code.begin();
    if (!predicate(*it)) {
        throw ParseException(std::string{"Parser got unexpected char"} + it->character);
    }
    return dropFirst(code);
}

CodeRange parseOptionalCharacter(CodeRange code, char c);

template<typename Predicate>
CodeRange parseOptionalCharacter(CodeRange code, Predicate predicate) {
    if (!code.empty() && predicate(*code.data)) {
        return dropFirst(code);
    }
    return code;
}

CodeRange parseKeyword(CodeRange code, const std::string& keyword);

template<typename Predicate>
CodeRange parseWhile(CodeRange code, Predicate predicate) {
    for (; !code.empty() && predicate(*code.begin()); ++code.data) {
    }
    return code;
}
