#include "parse.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"
#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"
#include "../expressions/List.h"
#include "../expressions/LookupChild.h"
#include "../expressions/LookupFunction.h"
#include "../expressions/LookupSymbol.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"

#include "../factory.h"

#include "starts_with.h"

ExpressionPointer parseCharacterExpression(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '\'');
    const auto it = code.begin();
    code = parseCharacter(code);
    code = parseCharacter(code, '\'');
    const auto value = it->character;
    return std::make_shared<Character>(CodeRange{first, code.begin()}, nullptr, value);
}

ExpressionPointer parseConditional(CodeRange code) {
    auto first = code.begin();

    code = parseKeyword(code, "if");
    code = parseWhiteSpace(code);
    auto expression_if = parseExpression(code);
    code.first = expression_if->end();
    code = parseWhiteSpace(code);

    code = parseKeyword(code, "then");
    code = parseWhiteSpace(code);
    auto expression_then = parseExpression(code);
    code.first = expression_then->end();
    code = parseWhiteSpace(code);

    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code.first = expression_else->end();
    code = parseWhiteSpace(code);

    return std::make_shared<Conditional>(
        CodeRange{first, code.begin()},
        nullptr,
        std::move(expression_if),
        std::move(expression_then),
        std::move(expression_else)
    );
}

NamePointer parseName(CodeRange code) {
    auto first = code.begin();
    code = parseWhile(code, isNameCharacter);
    return std::make_shared<Name>(
        CodeRange{first, code.first}, nullptr, rawString({first, code.first})
    );
}

DictionaryElementPointer parseNamedElement(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = name->end();
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '=');
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code.first = expression->end();
    code = parseWhiteSpace(code);
    return std::make_shared<NamedElement>(
        CodeRange{first, code.first}, nullptr, std::move(name), std::move(expression), 0
    );
}

DictionaryElementPointer parseWhileElement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "while");
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code.first = expression->end();
    code = parseWhiteSpace(code);
    return std::make_shared<WhileElement>(
        CodeRange{first, code.first}, nullptr, std::move(expression)
    );
}

DictionaryElementPointer parseEndElement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return std::make_shared<EndElement>(
        CodeRange{first, code.first}, nullptr
    );
}

DictionaryElementPointer parseDictionaryElement(CodeRange code) {
    code = parseWhiteSpace(code);
    throwIfEmpty(code);
    if (startsWithWhileElement(code)) {
        return parseWhileElement(code);
    }
    if (startsWithEndElement(code)) {
        return parseEndElement(code);
    }
    return parseNamedElement(code);
}

ExpressionPointer parseDictionary(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto elements = std::vector<DictionaryElementPointer>{};
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        auto element = parseDictionaryElement(code);
        code.first = element->end();
        elements.push_back(element);
    }
    code = parseCharacter(code, '}');
    setContext(elements);
    auto result = std::make_shared<Dictionary>(CodeRange{first, code.begin()}, nullptr);
    for (auto& element : elements) {
        result->elements.push_back(std::move(element));
    }
    return result;
}

ExpressionPointer parseFunction(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    auto input_name = parseName(code);
    code.first = input_name->end();
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = body->end();
    return std::make_shared<Function>(
        CodeRange{first, code.begin()}, nullptr, input_name, body
    );
}

ExpressionPointer parseFunctionDictionary(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto input_names = std::vector<NamePointer>{};
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        const auto name = parseName(code);
        code.first = name->end();
        input_names.push_back(name);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, '}');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = body->end();
    return std::make_shared<FunctionDictionary>(
        CodeRange{first, code.begin()}, nullptr, input_names, body
    );
}

ExpressionPointer parseFunctionList(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto input_names = std::vector<NamePointer>{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        const auto name = parseName(code);
        code.first = name->end();
        input_names.push_back(name);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = body->end();
    return std::make_shared<FunctionList>(
        CodeRange{first, code.begin()}, nullptr, input_names, body
    );
}

ExpressionPointer parseList(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto expressions = InternalList{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        auto expression = parseExpression(code);
        code.first = expression->end();
        expressions = ::prepend(expressions, std::move(expression));
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    return std::make_shared<List>(
        CodeRange{first, code.first}, nullptr, ::reverse(expressions)
    );
}

ExpressionPointer parseLookupChild(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = name->end();
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '@');
    code = parseWhiteSpace(code);
    auto child = parseExpression(code);
    code.first = child->end();
    return std::make_shared<LookupChild>(
        CodeRange{first, code.first}, nullptr, name, std::move(child)
    );
}

ExpressionPointer parseLookupFunction(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = name->end();
    const auto expected = ::startsWith(code, '!') ? '!' : '?';
    code = parseCharacter(code, expected);
    auto child = parseExpression(code);
    code.first = child->end();
    return std::make_shared<LookupFunction>(
        CodeRange{first, code.first}, nullptr, name, std::move(child)
    );
}

ExpressionPointer parseLookupSymbol(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    return std::make_shared<LookupSymbol>(
        CodeRange{first, name->end()}, nullptr, name
    );
}

ExpressionPointer parseNumber(CodeRange code) {
    auto first = code.begin();
    code = parseOptionalCharacter(code, isSign);
    code = parseCharacter(code, isDigit);
    code = parseWhile(code, isDigit);
    code = parseOptionalCharacter(code, '.');
    code = parseWhile(code, isDigit);
    const auto value = std::stod(rawString({first, code.first}));
    return makeNumber(
        std::make_shared<Number>(CodeRange{first, code.first}, nullptr, value)
    );
}

bool isNotEndOfString(CodeCharacter c) {
    return c.character != '"';
}

ExpressionPointer parseString(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '"');
    const auto first_character = code.begin();
    code = parseWhile(code, isNotEndOfString);
    const auto last_character = code.begin();
    code = parseCharacter(code, '"');
    auto value = InternalList{};
    for (auto it = first_character; it != last_character; ++it) {
        auto item = std::make_shared<Character>(
            CodeRange{it, it + 1}, nullptr, it->character
        );
        value = ::prepend<ExpressionPointer>(value, item);
    }
    value = ::reverse(value);
    return std::make_shared<String>(CodeRange{first, code.begin()}, nullptr, value);
}

ExpressionPointer parseExpression(CodeRange code) {
    code = parseWhiteSpace(code);
    throwIfEmpty(code);
    if (startsWithList(code)) {return parseList(code);}
    if (startsWithDictionary(code)) {return parseDictionary(code);}
    if (startsWithNumber(code)) {return parseNumber(code);}
    if (startsWithCharacter(code)) {return parseCharacterExpression(code);}
    if (startsWithString(code)) {return parseString(code);}
    if (startsWithString(code)) {return parseString(code);}
    if (startsWithConditional(code)) {return parseConditional(code);}
    if (startsWithFunctionDictionary(code)) {return parseFunctionDictionary(code);}
    if (startsWithFunctionList(code)) {return parseFunctionList(code);}
    if (startsWithFunction(code)) {return parseFunction(code);}
    if (startsWithLookupChild(code)) {return parseLookupChild(code);}
    if (startsWithLookupFunction(code)) {return parseLookupFunction(code);}
    if (startsWithLookupSymbol(code)) {return parseLookupSymbol(code);}
    throwParseException(code);
    return {};
}
