#include "parser.h"

#include <algorithm>
#include <memory>

using std::find_if_not;

Name parseName(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, isLetter);
    it = find_if_not(it, last, isNameCharacter);
    return Name(first, it, nullptr, rawString(first, it));
}

LookupSymbol parseLookupSymbol(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, isLetter);
    it = find_if_not(it, last, isNameCharacter);
    return LookupSymbol(first, it, nullptr, rawString(first, it));
}

Number parseNumber(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseOptionalCharacter(it, isSign);
    it = parseCharacter(it, isDigit);
    it = find_if_not(it, last, isDigit);
    it = parseOptionalCharacter(it, '.');
    it = find_if_not(it, last, isDigit);
    const auto value = std::stod(rawString(first, it));
    return Number(first, it, nullptr, value);
}

String parseString(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '"');
    const auto first_character = it;
    it = std::find_if(it, last, isStringSeparator);
    const auto last_character = it;
    it = parseCharacter(it, '"');
    const auto value = rawString(first_character, last_character);
    return String(first, it, nullptr, value);
}

List parseList(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '[');
    it = parseWhiteSpace(it, last);
    auto expressions = std::vector<ExpressionPointer>{};
    while (it->character != ']') {
        auto expression = parseExpression(it, last);
        it = expression->end();
        expressions.push_back(std::move(expression));
        it = parseWhiteSpace(it, last);
        it = parseOptionalCharacter(it, ',');
    }
    it = parseCharacter(it, ']');
    return List(first, it, nullptr, std::move(expressions));
}

Dictionary parseDictionary(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;
    it = parseCharacter(it, '{');
    it = parseWhiteSpace(it, last);
    auto result = Dictionary(first, it, nullptr);
    while (it->character != '}') {
        const auto name = parseName(it, last);
        it = name.end();
        it = parseWhiteSpace(it, last);
        it = parseCharacter(it, '=');
        it = parseWhiteSpace(it, last);
        auto expression = parseExpression(it, last);
        it = expression->end();
        result.add(DictionaryElement{name, std::move(expression)});
        it = parseWhiteSpace(it, last);
        it = parseOptionalCharacter(it, ',');
        it = parseWhiteSpace(it, last);
    }
    it = parseCharacter(it, '}');
    result.last_ = it;
    return result;
}

Conditional parseConditional(const CodeCharacter* first, const CodeCharacter* last) {
    auto it = first;

    it = parseKeyword(it, "if");
    it = parseWhiteSpace(it, last);
    auto expression_if = parseExpression(it, last);
    it = expression_if->end();
    it = parseWhiteSpace(it, last);

    it = parseKeyword(it, "then");
    it = parseWhiteSpace(it, last);
    auto expression_then = parseExpression(it, last);
    it = expression_then->end();
    it = parseWhiteSpace(it, last);

    it = parseKeyword(it, "else");
    it = parseWhiteSpace(it, last);
    auto expression_else = parseExpression(it, last);
    it = expression_else->end();
    it = parseWhiteSpace(it, last);

    return Conditional(
        first,
        it,
        nullptr,
        std::move(expression_if),
        std::move(expression_then),
        std::move(expression_else)
    );
}

ExpressionPointer parseExpression(
    const CodeCharacter* first, const CodeCharacter* last
) {
    auto it = first;
    it = parseWhiteSpace(it, last);
    if (it == last) {
        throw ParseException("could not parse expression");
    }
    if (isList(*it)) {
        return std::make_shared<List>(parseList(it, last));
    }
    if (isDictionary(*it)) {
        return std::make_shared<Dictionary>(parseDictionary(it, last));
    }
    if (isNumber(*it)) {
        return std::make_shared<Number>(parseNumber(it, last));
    }
    if (isStringSeparator(*it)) {
        return std::make_shared<String>(parseString(it, last));
    }
    if (isConditional(it)) {
        return std::make_shared<Conditional>(parseConditional(it, last));
    }
    if (isLetter(*it)) {
        return std::make_shared<LookupSymbol>(parseLookupSymbol(it, last));
    }
    throw ParseException("could not parse expression");
}

ExpressionPointer parse(const std::string& string) {
    const auto result = makeCodeCharacters(string);
    return parseExpression(result.data(), result.data() + result.size());
}
