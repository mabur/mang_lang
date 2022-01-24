#include "parse.h"

#include <iostream>

#include "../factory.h"
#include "../container.h"

const CodeCharacter* end(Expression expression) {
    return expression.range.last;
}

Expression parseCharacterExpression(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '\'');
    const auto it = code.begin();
    code = parseCharacter(code);
    code = parseCharacter(code, '\'');
    const auto value = it->character;
    return makeCharacter(CodeRange{first, code.begin()}, {value});
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

    return makeConditional(CodeRange{first, code.begin()}, {
        expression_if,
        expression_then,
        expression_else
    });
}

Expression parseIs(CodeRange code) {
    auto first = code.begin();

    code = parseKeyword(code, "is");
    code = parseWhiteSpace(code);
    auto input = parseExpression(code);
    code.first = end(input);
    code = parseWhiteSpace(code);

    auto alternatives = std::vector<Alternative>{};

    while (!isKeyword(code, "else")) {
        auto left = parseExpression(code);
        code.first = end(left);
        code = parseWhiteSpace(code);
        code = parseKeyword(code, "then");
        code = parseWhiteSpace(code);
        auto right = parseExpression(code);
        code.first = end(right);
        code = parseWhiteSpace(code);
        alternatives.push_back(Alternative{left, right});
    }

    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code.first = end(expression_else);
    code = parseWhiteSpace(code);

    return makeIs(
        CodeRange{first, code.begin()},
        IsExpression{input, alternatives, expression_else}
    );
}

Expression parseName(CodeRange code) {
    auto first = code.begin();
    code = parseWhile(code, isNameCharacter);
    return makeName(
        CodeRange{first, code.first},
        Name{rawString({first, code.first})}
    );
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
    return makeDefinition(
        CodeRange{first, code.first}, Definition{std::move(name), expression}
    );
}

Expression parseDynamicNamedElement(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '[');
    code = parseWhiteSpace(code);
    auto dynamic_name = parseExpression(code);
    code.first = end(dynamic_name);
    code = parseWhiteSpace(code);
    code = parseCharacter(code, ']');
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '=');
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code.first = end(expression);
    code = parseWhiteSpace(code);
    return makeDynamicDefinition(
        CodeRange{first, code.first}, DynamicDefinition{dynamic_name, expression}
    );
}

Expression parseWhileStatement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "while");
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code.first = end(expression);
    code = parseWhiteSpace(code);
    return makeWhileStatement(CodeRange{first, code.first}, {expression, 0});
}

Expression parseEndStatement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return makeEndStatement(CodeRange{first, code.first}, {1});
}

Expression parseDictionaryElement(CodeRange code) {
    code = parseWhiteSpace(code);
    throwIfEmpty(code);
    if (isKeyword(code, "while")) {
        return parseWhileStatement(code);
    }
    if (isKeyword(code, "end")) {
        return parseEndStatement(code);
    }
    if (::startsWith(code, '[')) {
        return parseDynamicNamedElement(code);
    }
    return parseNamedElement(code);
}

Expression parseDictionary(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto statements = Statements{};
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        auto statement = parseDictionaryElement(code);
        code.first = end(statement);
        statements.push_back(statement);
    }
    code = parseCharacter(code, '}');
    return makeDictionary(
        CodeRange{first, code.begin()},
        Dictionary{setContext(statements)}
    );
}

Expression parseFunction(CodeRange code) {
    auto first = code.begin();
    auto input_name = parseName(code);
    code.first = end(input_name);
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunction(
        CodeRange{first, code.begin()},
        {Expression{}, input_name, body}
    );
}

Expression parseFunctionDictionary(CodeRange code) {
    auto first = code.begin();
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
    return makeFunctionDictionary(
        CodeRange{first, code.begin()},
        {Expression{}, input_names, body}
    );
}

Expression parseFunctionList(CodeRange code) {
    auto first = code.begin();
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
    return makeFunctionList(
        CodeRange{first, code.begin()},
        {Expression{}, input_names, body}
    );
}

Expression parseAnyFunction(CodeRange code) {
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    if (startsWith(code, '{')) {return parseFunctionDictionary(code);}
    if (startsWith(code, '(')) {return parseFunctionList(code);}
    return parseFunction(code);
}

Expression parseList(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto list = makeEmptyList({}, {});
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        list = putList(list, expression);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    return reverseList(CodeRange{first, code.first}, list);
}

Expression parseSubstitution(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = end(name);
    code = parseWhiteSpace(code);
    if (startsWith(code, '@')) {
        code = parseCharacter(code);
        code = parseWhiteSpace(code);
        auto child = parseExpression(code);
        code.first = end(child);
        return makeLookupChild(CodeRange{first, code.first}, {name, child});
    }
    if (startsWith(code, '!') or startsWith(code, '?')) {
        code = parseCharacter(code);
        auto child = parseExpression(code);
        code.first = end(child);
        return makeFunctionApplication(CodeRange{first, code.first}, {name, child});
    }
    return makeLookupSymbol(CodeRange{first, end(name)}, {name});
}

Expression parseNumber(CodeRange code) {
    auto first = code.begin();
    code = parseOptionalCharacter(code, isSign);
    code = parseCharacter(code, isDigit);
    code = parseWhile(code, isDigit);
    code = parseOptionalCharacter(code, '.');
    code = parseWhile(code, isDigit);
    const auto value = std::stod(rawString({first, code.first}));
    return makeNumber(CodeRange{first, code.first}, {value});
}

CodeRange parseKeyWordContent(CodeRange code, std::string keyword) {
    return {code.begin(), parseKeyword(code, keyword).begin()};
}

Expression parseYes(CodeRange code) {
    return makeBoolean(parseKeyWordContent(code, "yes"), {true});
}

Expression parseNo(CodeRange code) {
    return makeBoolean(parseKeyWordContent(code, "no"), {false});
}

Expression parseInf(CodeRange code) {
    return makeNumber(
        parseKeyWordContent(code, "inf"),
        {std::numeric_limits<double>::infinity()}
    );
}

Expression parseNegInf(CodeRange code) {
    return makeNumber(
        parseKeyWordContent(code, "-inf"),
        {-std::numeric_limits<double>::infinity()}
    );
}

Expression parseNan(CodeRange code) {
    return makeNumber(
        parseKeyWordContent(code, "nan"),
        {std::numeric_limits<double>::quiet_NaN()}
    );
}

Expression parseString(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '"');
    auto value = makeEmptyString({first, first + 1}, {});
    for (; code.first->character != '"'; ++code.first) {
        auto item = makeCharacter(
            CodeRange{code.first, code.first + 1},
            {code.first->character}
        );
        value = putString(value, item);
    }
    code = parseCharacter(code, '"');
    const auto last = code.begin();
    return reverseString(CodeRange{first, last}, value);
}

Expression parseExpression(CodeRange code) {
    try {
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        const auto c = code.first->character;
        if (c == '(') {return parseList(code);}
        if (c == '{') {return parseDictionary(code);}
        if (c == '\'') {return parseCharacterExpression(code);}
        if (c == '\"') {return parseString(code);}
        if (isKeyword(code, "yes")) {return parseYes(code);}
        if (isKeyword(code, "no")) {return parseNo(code);}
        if (isKeyword(code, "nan")) {return parseNan(code);}
        if (isKeyword(code, "inf")) {return parseInf(code);}
        if (isKeyword(code, "-inf")) {return parseNegInf(code);}
        if (isKeyword(code, "if")) {return parseConditional(code);}
        if (isKeyword(code, "is")) {return parseIs(code);}
        if (isKeyword(code, "in")) {return parseAnyFunction(code);}
        if (isKeyword(code, "out")) {throwParseException(code);}
        if (isKeyword(code, "then")) {throwParseException(code);}
        if (isKeyword(code, "else")) {throwParseException(code);}
        if (isKeyword(code, "while")) {throwParseException(code);}
        if (isKeyword(code, "end")) {throwParseException(code);}
        if (isdigit(c) or c == '+' or c == '-') {return parseNumber(code);}
        if (isalpha(c) or c == '_') {return parseSubstitution(code);}
        throwParseException(code);
        return {};
    } catch (std::runtime_error e) {
        std::cout << "Exception while parsing: " << e.what();
        throw e;
    }
}
