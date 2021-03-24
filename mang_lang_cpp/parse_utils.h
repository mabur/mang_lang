#pragma once

#include <stdexcept>
#include <string>
#include <vector>

struct CodeCharacter {
    char character;
    size_t row;
    size_t column;
};

struct CodeRange {
    const CodeCharacter* first;
    const CodeCharacter* last;
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

void verifyThisIsNotTheEnd(CodeRange code_range);
void throwParseException(CodeRange code_range);

char rawCharacter(CodeCharacter c);

std::string rawString(CodeRange code_range);

std::vector<CodeCharacter> makeCodeCharacters(const std::string& string);

bool haveSameCharacters(CodeCharacter a, CodeCharacter b);

bool isDigit(CodeCharacter c);

bool isSign(CodeCharacter c);

bool isLetter(CodeCharacter c);

bool isNameCharacter(CodeCharacter c);

bool isWhiteSpace(CodeCharacter c);

bool isKeyword(CodeRange code_range, const std::string& keyword);

bool isAnyKeyword(CodeRange code_range, const std::vector<std::string>& keywords);

CodeRange parseWhiteSpace(CodeRange code_range);

CodeRange parseCharacter(CodeRange code_range, char expected);

template<typename Predicate>
CodeRange parseCharacter(CodeRange code_range, Predicate predicate) {
    verifyThisIsNotTheEnd(code_range);
    auto it = code_range.begin();
    if (!predicate(*it)) {
        throw ParseException(std::string{"Parser got unexpected char"} + it->character);
    }
    ++it;
    return {it, code_range.end()};
}

CodeRange parseOptionalCharacter(CodeRange code_range, char c);

template<typename Predicate>
CodeRange parseOptionalCharacter(CodeRange code_range, Predicate predicate) {
    auto it = code_range.first;
    if (it == code_range.last) {
        return {it, code_range.end()};
    }
    if (predicate(*it)) {
        ++it;
    }
    return {it, code_range.end()};
}

CodeRange parseKeyword(CodeRange code_range, std::string keyword);
