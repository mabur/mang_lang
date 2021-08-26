#include "parse.h"

#include "../factory.h"
#include "../new_string.h"
#include "end.h"
#include "starts_with.h"

Expression parseCharacterExpression(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '\'');
    const auto it = code.begin();
    code = parseCharacter(code);
    code = parseCharacter(code, '\'');
    const auto value = it->character;
    return makeCharacter(
        new Character{CodeRange{first, code.begin()}, value}
    );
}

Expression parseConditional(CodeRange code) {
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
        expression_if,
        expression_then,
        expression_else
    });
}

Expression parseName(CodeRange code) {
    auto first = code.begin();
    code = parseWhile(code, isNameCharacter);
    return makeName(new Name{
        CodeRange{first, code.first}, rawString({first, code.first})
    });
}

Expression parseNamedElement(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = end(name);
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '=');
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code.first = end(expression);
    code = parseWhiteSpace(code);
    return makeNamedElement(
        new NamedElement{
            CodeRange{first, code.first},
            std::move(name),
            expression,
            0
        }
    );
}

Expression parseWhileElement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "while");
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code.first = end(expression);
    code = parseWhiteSpace(code);
    return makeWhileElement(
        new WhileElement{
            CodeRange{first, code.first},
            expression,
            0,
        }
    );
}

Expression parseEndElement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return makeEndElement(new EndElement{CodeRange{first, code.first}, 1});
}

Expression parseDictionaryElement(CodeRange code) {
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

Expression parseDictionary(CodeRange code) {
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
            Expression{},
            setContext(elements)
        }
    );
}

Expression parseFunction(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    auto input_name = parseName(code);
    code.first = end(input_name);
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunction(new Function{
        CodeRange{first, code.begin()}, Expression{}, input_name, body
    });
}

Expression parseFunctionDictionary(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto input_names = std::vector<Expression>{};
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        const auto name = parseName(code);
        code.first = end(name);
        input_names.push_back(name);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, '}');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunctionDictionary(new FunctionDictionary{
        CodeRange{first, code.begin()}, Expression{}, input_names, body
    });
}

Expression parseFunctionList(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto input_names = std::vector<Expression>{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        const auto name = parseName(code);
        code.first = end(name);
        input_names.push_back(name);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunctionList(new FunctionList{
        CodeRange{first, code.begin()}, Expression{}, input_names, body
    });
}

Expression parseList(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto expressions = InternalList{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        expressions = ::prepend(expressions, expression);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    return makeList(new List{CodeRange{first, code.first}, reverse(expressions)});
}

Expression parseLookupChild(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = end(name);
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '@');
    code = parseWhiteSpace(code);
    auto child = parseExpression(code);
    code.first = end(child);
    return makeLookupChild(new LookupChild{CodeRange{first, code.first}, name, child});
}

Expression parseLookupFunction(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = end(name);
    const auto expected = ::startsWith(code, '!') ? '!' : '?';
    code = parseCharacter(code, expected);
    auto child = parseExpression(code);
    code.first = end(child);
    return makeFunctionApplication(new FunctionApplication{
        CodeRange{first, code.first}, name, child
    });
}

Expression parseLookupSymbol(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    return makeLookupSymbol(new LookupSymbol{
        CodeRange{first, end(name)}, name
    });
}

Expression parseNumber(CodeRange code) {
    auto first = code.begin();
    code = parseOptionalCharacter(code, isSign);
    code = parseCharacter(code, isDigit);
    code = parseWhile(code, isDigit);
    code = parseOptionalCharacter(code, '.');
    code = parseWhile(code, isDigit);
    const auto value = std::stod(rawString({first, code.first}));
    return makeNumber(
        new Number(Number{CodeRange{first, code.first}, value})
    );
}

Expression parseNewString(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '"');
    auto value = makeNewEmptyString(new EmptyString{first, first + 1});
    for (; code.first->character != '"'; ++code.first) {
        auto item = makeCharacter(new Character{
            CodeRange{code.first, code.first + 1}, code.first->character
        });
        value = new_string::prepend(value, item);
    }
    code = parseCharacter(code, '"');
    const auto last = code.begin();
    return new_string::reverse(CodeRange{first, last}, value);
}

Expression parseExpression(CodeRange code) {
    code = parseWhiteSpace(code);
    throwIfEmpty(code);
    if (startsWithList(code)) {return parseList(code);}
    if (startsWithDictionary(code)) {return parseDictionary(code);}
    if (startsWithNumber(code)) {return parseNumber(code);}
    if (startsWithCharacter(code)) {return parseCharacterExpression(code);}
    if (startsWithString(code)) {return parseNewString(code);}
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
