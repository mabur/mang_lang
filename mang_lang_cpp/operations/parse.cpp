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

#include "end.h"
#include "starts_with.h"

ExpressionPointer parseCharacterExpression(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '\'');
    const auto it = code.begin();
    code = parseCharacter(code);
    code = parseCharacter(code, '\'');
    const auto value = it->character;
    return makeCharacter(
        new Character{CodeRange{first, code.begin()}, ExpressionPointer{}, value}
    );
}

ExpressionPointer parseConditional(CodeRange code) {
    auto first = code.begin();

    code = parseKeyword(code, "if");
    code = parseWhiteSpace(code);
    auto expression_if = parseExpression(code);
    code.first = end(expression_if);
    code = parseWhiteSpace(code);

    code = parseKeyword(code, "then");
    code = parseWhiteSpace(code);
    auto expression_then = parseExpression(code);
    code.first = end(expression_then);
    code = parseWhiteSpace(code);

    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code.first = end(expression_else);
    code = parseWhiteSpace(code);

    return makeConditional(new Conditional{
        CodeRange{first, code.begin()},
        ExpressionPointer{},
        std::move(expression_if),
        std::move(expression_then),
        std::move(expression_else)
    });
}

NamePointer parseName(CodeRange code) {
    auto first = code.begin();
    code = parseWhile(code, isNameCharacter);
    return std::make_shared<Name>(Name{
        CodeRange{first, code.first},
        ExpressionPointer{},
        rawString({first, code.first})
    });
}

ExpressionPointer parseNamedElement(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = name->range.end();
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '=');
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code.first = end(expression);
    code = parseWhiteSpace(code);
    return makeTypedDictionaryElement(
        std::make_shared<DictionaryElement>(DictionaryElement{
            CodeRange{first, code.first},
            ExpressionPointer{},
            std::move(name),
            std::move(expression),
            1,
            0,
            0
        }),
        NAMED_ELEMENT
    );
}

ExpressionPointer parseWhileElement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "while");
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code.first = end(expression);
    code = parseWhiteSpace(code);
    return makeTypedDictionaryElement(
        std::make_shared<DictionaryElement>(DictionaryElement{
            CodeRange{first, code.first},
            ExpressionPointer{},
            NamePointer{},
            std::move(expression),
            1,
            0,
            0
        }),
        WHILE_ELEMENT
    );
}

ExpressionPointer parseEndElement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return makeTypedDictionaryElement(
        std::make_shared<DictionaryElement>(DictionaryElement{
            CodeRange{first, code.first},
            ExpressionPointer{},
            NamePointer{},
            ExpressionPointer{},
            1,
            0,
            0
        }),
        END_ELEMENT
    );
}

ExpressionPointer parseDictionaryElement(CodeRange code) {
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
    auto elements = DictionaryElements{};
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        auto element = parseDictionaryElement(code);
        code.first = end(element);
        elements.push_back(element);
    }
    code = parseCharacter(code, '}');
    return makeDictionary(
        new Dictionary{
            CodeRange{first, code.begin()},
            ExpressionPointer{},
            setContext(elements)
        }
    );
}

ExpressionPointer parseFunction(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    auto input_name = parseName(code);
    code.first = input_name->range.end();
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunction(new Function{
        CodeRange{first, code.begin()}, ExpressionPointer{}, input_name, body
    });
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
        code.first = name->range.end();
        input_names.push_back(name);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, '}');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunctionDictionary(std::make_shared<FunctionDictionary>(FunctionDictionary{
        CodeRange{first, code.begin()}, ExpressionPointer{}, input_names, body
    }));
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
        code.first = name->range.end();
        input_names.push_back(name);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunctionList(std::make_shared<FunctionList>(FunctionList{
        CodeRange{first, code.begin()}, ExpressionPointer{}, input_names, body
    }));
}

ExpressionPointer parseList(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto expressions = InternalList{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        expressions = ::prepend(expressions, std::move(expression));
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    return makeList(std::make_shared<List>(List{
        CodeRange{first, code.first},
        ExpressionPointer{},
        ::reverse(expressions)
    }));
}

ExpressionPointer parseLookupChild(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = name->range.end();
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '@');
    code = parseWhiteSpace(code);
    auto child = parseExpression(code);
    code.first = end(child);
    return makeLookupChild(std::make_shared<LookupChild>(LookupChild{
        CodeRange{first, code.first}, ExpressionPointer{}, name, std::move(child)
    }));
}

ExpressionPointer parseLookupFunction(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = name->range.end();
    const auto expected = ::startsWith(code, '!') ? '!' : '?';
    code = parseCharacter(code, expected);
    auto child = parseExpression(code);
    code.first = end(child);
    return makeLookupFunction(std::make_shared<LookupFunction>(LookupFunction{
        CodeRange{first, code.first}, ExpressionPointer{}, name, std::move(child)
    }));
}

ExpressionPointer parseLookupSymbol(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    return makeLookupSymbol(std::make_shared<LookupSymbol>(LookupSymbol{
        CodeRange{first, name->range.end()}, ExpressionPointer{}, name
    }));
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
        new Number(Number{CodeRange{first, code.first}, ExpressionPointer{}, value})
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
        auto item = makeCharacter(
            new Character{CodeRange{it, it + 1}, ExpressionPointer{}, it->character}
        );
        value = ::prepend<ExpressionPointer>(value, item);
    }
    value = ::reverse(value);
    return makeString(std::make_shared<String>(String{CodeRange{first, code.begin()}, ExpressionPointer{}, value}));
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
