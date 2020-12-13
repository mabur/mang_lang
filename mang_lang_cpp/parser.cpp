#include "parser.h"

#include <algorithm>
#include <cctype>
#include <memory>

using std::find_if_not;

char rawCharacter(CodeCharacter c) {
    return c.character;
}

std::string rawString(const CodeCharacter* first, const CodeCharacter* last) {
    auto s = std::string{};
    std::transform(first, last, std::back_inserter(s), rawCharacter);
    return s;
}

bool isDigit(CodeCharacter c) {
    return isdigit(c.character);
}

bool isSign(CodeCharacter c) {
    return c.character == '+' || c.character == '-';
}

bool isNumber(CodeCharacter c) {
    return isSign(c) || isDigit(c);
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

bool isList(CodeCharacter c) {
    return c.character == '[';
}

bool isDictionary(CodeCharacter c) {
    return c.character == '{';
}

bool isStringSeparator(CodeCharacter c) {
    return c.character == '"';
}

const CodeCharacter* parseWhiteSpace(
    const CodeCharacter* first, const CodeCharacter* last
) {
    return find_if_not(first, last, isWhiteSpace);
}

const CodeCharacter* parseCharacter(const CodeCharacter* it, char c) {
    if (it->character != c) {
        throw ParseException(
            std::string{"Failed parsing: "} + c
        );
    }
    ++it;
    return it;
}

const CodeCharacter* parseOptionalCharacter(const CodeCharacter* it, char c) {
    if (it->character == c) {
        ++it;
    }
    return it;
}

std::string parseName(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    if (!isLetter(*it)) {
        throw ParseException("could not parse name");
    }
    it = find_if_not(it, last, isNameCharacter);
    return rawString(first, it);
}

Number parseNumber(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    if (isSign(*it)) {
        ++it;
    }
    it = find_if_not(it, last, isDigit);
    if (it == first) {
        throw ParseException("could not parse number");
    }
    it = parseOptionalCharacter(it, '.');
    it = find_if_not(it, last, isDigit);
    const auto value = std::stod(rawString(first, it));
    return Number(first, it, value);
}

String parseString(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '"');
    const auto first_character = it;
    it = std::find_if(it, last, isStringSeparator);
    const auto last_character = it;
    it = parseCharacter(it, '"');
    const auto value = rawString(first_character, last_character);
    return String(first, it, value);
}

List parseList(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '[');
    it = parseWhiteSpace(it, last);
    auto expressions = std::vector<std::unique_ptr<Expression>>{};
    while (it->character != ']') {
        auto expression = parseExpression(it, last);
        it = expression->end();
        expressions.push_back(std::move(expression));
        it = parseWhiteSpace(it, last);
        it = parseOptionalCharacter(it, ',');
    }
    it = parseCharacter(it, ']');
    return List(first, it, std::move(expressions));
}

Dictionary parseDictionary(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '{');
    auto elements = std::vector<DictionaryElement>{};
    while (it->character != '}') {
        const auto name = parseName(it, last);
        it = parseWhiteSpace(it, last);
        it = parseCharacter(it, '=');
        it = parseWhiteSpace(it, last);
        auto expression = parseExpression(it, last);
        elements.emplace_back(name, std::move(expression));
        it = parseWhiteSpace(it, last);
        it = parseOptionalCharacter(it, ',');
    }
    it = parseCharacter(it, '}');
    return Dictionary(first, it, std::move(elements));
}

std::unique_ptr<Expression> parseExpression(
    const CodeCharacter* first, const CodeCharacter* last
) {
    auto it = first;
    it = parseWhiteSpace(it, last);
    if (it == last) {
        throw ParseException("could not parse expression");
    }
    if (isList(*it)) {
        return std::make_unique<List>(parseList(it, last));
    }
    if (isDictionary(*it)) {
        return std::make_unique<Dictionary>(parseDictionary(it, last));
    }
    if (isNumber(*it)) {
        return std::make_unique<Number>(parseNumber(it, last));
    }
    if (isStringSeparator(*it)) {
        return std::make_unique<String>(parseString(it, last));
    }
    throw ParseException("could not parse expression");
}

std::unique_ptr<Expression> parse(const std::string& string) {
    auto result = std::vector<CodeCharacter>{};
    auto column = size_t{};
    auto row = size_t{};
    for (const auto& character : string) {
        result.push_back({character, row, column});
        ++column;
        if (character == '\n') {
            ++row;
            column = 0;
        }
    }
    return parseExpression(result.data(), result.data() + result.size());
}
