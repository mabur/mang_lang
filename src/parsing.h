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

struct CodeCharacters {
    CodeCharacter* data;
    size_t count;
};

struct CodeRange {
    const CodeCharacter* first = nullptr;
    const CodeCharacter* last = nullptr;
    const CodeCharacter* begin() const {return first;}
    const CodeCharacter* end() const {return last;}
    bool empty() const {return first == last;}
    size_t size() const {return last - first;}
};

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
    ++it;
    return CodeRange{it, code.end()};
}

CodeRange parseOptionalCharacter(CodeRange code, char c);

template<typename Predicate>
CodeRange parseOptionalCharacter(CodeRange code, Predicate predicate) {
    auto it = code.first;
    if (code.empty()) {
        return code;
    }
    if (predicate(*it)) {
        ++it;
    }
    return CodeRange{it, code.end()};
}

CodeRange parseKeyword(CodeRange code, const std::string& keyword);

template<typename Predicate>
CodeRange parseWhile(CodeRange code, Predicate predicate) {
    return CodeRange{std::find_if_not(code.begin(), code.end(), predicate), code.end()};
}
