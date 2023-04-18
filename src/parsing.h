#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

struct CodeCharacter {
    char character;
    size_t row;
    size_t column;
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
    ParseException(const std::string& description, const CodeCharacter* it);
    using runtime_error::runtime_error;
};

void throwIfEmpty(CodeRange code);
void throwParseException(CodeRange code);

char rawCharacter(CodeCharacter c);

std::string rawString(CodeRange code);

std::vector<CodeCharacter> makeCodeCharacters(const std::string& string);

bool haveSameCharacters(CodeCharacter a, CodeCharacter b);

bool isDigit(CodeCharacter c);

bool isSign(CodeCharacter c);

bool isLetter(CodeCharacter c);

bool isNameCharacter(CodeCharacter c);

bool isWhiteSpace(CodeCharacter c);

bool isKeyword(CodeRange code, const std::string& word);

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
    return {it, code.end()};
}

CodeRange parseOptionalCharacter(CodeRange code, char c);

template<typename Predicate>
CodeRange parseOptionalCharacter(CodeRange code, Predicate predicate) {
    auto it = code.first;
    if (it == code.last) {
        return {it, code.end()};
    }
    if (predicate(*it)) {
        ++it;
    }
    return {it, code.end()};
}

CodeRange parseKeyword(CodeRange code, const std::string& keyword);

template<typename Predicate>
CodeRange parseWhile(CodeRange code, Predicate predicate) {
    return {std::find_if_not(code.begin(), code.end(), predicate), code.end()};
}
