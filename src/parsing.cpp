#include "parsing.h"
#include <algorithm>

#include <carma/carma.h>

CodeRange dropFirst(CodeRange code) {
    if (!code.empty()) {
        code.first++;    
    }
    return code;
}

std::string serializeCodeCharacter(const CodeCharacter* c) {
    return "row " + std::to_string(c->row + 1) + " and column "
        + std::to_string(c->column + 1);
}

std::string describeLocation(CodeRange code) {
    if (code.begin() == nullptr || code.end() == nullptr) {
        return " at unknown location.";
    }
    return " between " +
        serializeCodeCharacter(code.begin()) + " and " +
        serializeCodeCharacter(code.end());
}

ParseException::ParseException(const std::string& description, CodeRange code)
    : std::runtime_error(description + describeLocation(code))
{}

static
char rawCharacter(CodeCharacter c) {
    return c.character;
}

std::string rawString(CodeRange code) {
    auto s = std::string{};
    std::transform(code.begin(), code.end(), std::back_inserter(s), rawCharacter);
    return s;
}

CodeCharacters makeCodeCharacters(const std::string& string) {
    auto result = CodeCharacters{};
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

static
bool haveSameCharacters(char a, CodeCharacter b) {
    return a == b.character;
}

bool isKeyword(CodeRange code, const std::string& word) {
    if (code.size() < word.size()) {
        return false;
    }
    if (!std::equal(word.begin(), word.end(), code.begin(), haveSameCharacters)) {
        return false;
    }
    const auto after = code.begin() + word.size();
    if (after == code.end()) {
        return true;
    }
    return !isNameCharacter(*after);
}

bool startsWith(CodeRange code, const std::string& word) {
    if (code.size() < word.size()) {
        return false;
    }
    return std::equal(word.begin(), word.end(), code.begin(), haveSameCharacters);
}

bool startsWith(CodeRange code, char c) {
    return !code.empty() && code.begin()->character == c;
}

CodeRange parseWhiteSpace(CodeRange code) {
    return parseWhile(code, isWhiteSpace);
}

CodeRange parseCharacter(CodeRange code) {
    throwIfEmpty(code);
    return dropFirst(code);
}

CodeRange parseCharacter(CodeRange code, char expected) {
    throwIfEmpty(code);
    auto it = code.begin();
    const auto actual = it->character;
    if (it->character != expected) {
        const auto description = std::string{"Parsing expected \'"}
            + expected + "\' but got \'" + actual + "\'";
        throw ParseException(description, code);
    }
    return dropFirst(code);
}

CodeRange parseOptionalCharacter(CodeRange code, char c) {
    if (!code.empty() && code.begin()->character == c) {
        return dropFirst(code);
    }
    return code;
}

CodeRange parseKeyword(CodeRange code, const std::string& keyword) {
    if (code.size() < keyword.size()) {
        throw ParseException(
            "Reached end of file when parsing " + keyword, code
        );
    }
    for (const auto c : keyword) {
        code = parseCharacter(code, c);
    }
    return code;
}

void throwIfEmpty(CodeRange code) {
    if (code.empty()) {
        throw ParseException(
            "Unexpected end of source while parsing", code
        );
    }
}

void throwParseException(CodeRange code) {
    throw ParseException("Does not recognize expression to parse", code);
}
