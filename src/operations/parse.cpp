#include "parse.h"

#include <iostream>
#include <limits>

#include "../factory.h"
#include "../container.h"

namespace {

const CodeCharacter* end(Expression expression) {
    return expression.range.last;
}

Expression parseCharacterExpression(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '\'');
    const auto it = code.begin();
    code = parseCharacter(code);
    const auto value = it->character;
    code = parseCharacter(code, '\'');
    return makeCharacter(CodeRange{first, code.begin()}, {value});
}

Expression parseConditional(CodeRange code) {
    auto first = code.begin();

    code = parseKeyword(code, "if");
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

    return makeConditional(
        CodeRange{first, code.begin()},
        Conditional{alternatives, expression_else}
    );
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
        rawString({first, code.first})
    );
}

Expression parseNamedElement(CodeRange code) {
    auto first = code.begin();
    auto name = parseName(code);
    code.first = end(name);
    code = parseWhiteSpace(code);
    
    if (startsWith(code, '=')) {
        code = parseCharacter(code, '=');
        code = parseWhiteSpace(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        code = parseWhiteSpace(code);
        return makeDefinition(
            CodeRange{first, code.first},
            Definition{std::move(name), expression}
        );
    }
    else {
        code = parseKeyword(code, "+=");
        code = parseWhiteSpace(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        code = parseWhiteSpace(code);
        return makePutAssignment(
            CodeRange{first, code.first},
            PutAssignment{std::move(name), expression}
        );
    }

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

Expression parseForStatement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "for");
    code = parseWhiteSpace(code);
    auto name_item = parseName(code);
    code.first = end(name_item);
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    auto name_container = parseName(code);
    code.first = end(name_container);
    return makeForStatement(CodeRange{first, code.first},
        ForStatement{name_item, name_container, 0}
    );
}

Expression parseWhileEndStatement(CodeRange code, size_t while_index) {
    auto first = code.begin();
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return makeWhileEndStatement(CodeRange{first, code.first}, {while_index});
}

Expression parseForEndStatement(CodeRange code, size_t for_index) {
    auto first = code.begin();
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return makeForEndStatement(CodeRange{first, code.first}, {for_index});
}
    
bool isEndMatchingWhile(
    const std::vector<size_t>& while_indices,
    const std::vector<size_t>& for_indices,
    CodeRange code
) {
    if (while_indices.empty() && for_indices.empty()) {
        throw ParseException("end not matching while or for", code.begin());
    }
    if (while_indices.empty()) {
        return false;
    }
    if (for_indices.empty()) {
        return true;
    }
    return while_indices.back() > for_indices.back();
}

Expression parseDictionary(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto statements = Statements{};
    auto while_indices = std::vector<size_t>{};
    auto for_indices = std::vector<size_t>{};
    while (!::startsWith(code, '}')) {
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        if (isKeyword(code, "while")) {
            while_indices.push_back(statements.size());
            statements.push_back(parseWhileStatement(code));
        }
        else if (isKeyword(code, "for")) {
            for_indices.push_back(statements.size());
            statements.push_back(parseForStatement(code));
        }
        else if (isKeyword(code, "end")) {
            if (isEndMatchingWhile(while_indices, for_indices, code)) {
                const auto end_index = statements.size();
                const auto while_index = while_indices.back();
                while_indices.pop_back();
                const auto while_statement_pointer = statements.at(while_index);
                auto& while_statement = getMutableWhileStatement(while_statement_pointer);
                while_statement.end_index_ = end_index;
                statements.push_back(parseWhileEndStatement(code, while_index));
            } else {
                const auto end_index = statements.size();
                const auto for_index = for_indices.back();
                for_indices.pop_back();
                const auto for_statement_pointer = statements.at(for_index);
                auto& for_statement = getMutableForStatement(for_statement_pointer);
                for_statement.end_index_ = end_index;
                statements.push_back(parseForEndStatement(code, for_index));
            }
        }
        else {
            statements.push_back(parseNamedElement(code));
        }
        code.first = end(statements.back());
    }
    code = parseCharacter(code, '}');
    return makeDictionary(
        CodeRange{first, code.begin()}, Dictionary{statements}
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

Expression parseFunctionTuple(CodeRange code) {
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
    return makeFunctionTuple(
        CodeRange{first, code.begin()},
        {Expression{}, input_names, body}
    );
}

Expression parseAnyFunction(CodeRange code) {
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    if (startsWith(code, '{')) {return parseFunctionDictionary(code);}
    if (startsWith(code, '(')) {return parseFunctionTuple(code);}
    return parseFunction(code);
}

Expression parseStackNew(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '[');
    code = parseWhiteSpace(code);
    auto stack = makeEmptyStack({}, {});
    while (!::startsWith(code, ']')) {
        throwIfEmpty(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        stack = putStack(stack, expression);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ']');
    return reverseStack(CodeRange{first, code.first}, stack);
}

Expression parseTuple(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto expressions = std::vector<Expression>{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        expressions.push_back(expression);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    return makeTuple(CodeRange{first, code.first}, Tuple{expressions});
}

Expression parseTable(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '<');
    code = parseWhiteSpace(code);
    auto rows = std::vector<Row>{};
    while (!::startsWith(code, '>')) {
        throwIfEmpty(code);
        const auto key = parseExpression(code);
        code.first = end(key);
        code = parseWhiteSpace(code);
        code = parseCharacter(code, ':');
        code = parseWhiteSpace(code);
        const auto value = parseExpression(code);
        code.first = end(value);
        code = parseWhiteSpace(code);
        rows.push_back({key, value});
    }
    code = parseCharacter(code, '>');
    return makeTable(CodeRange{first, code.first}, Table{rows});
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

Expression parseNegInf(CodeRange code) {
    return makeNumber(
        parseKeyWordContent(code, "-inf"),
        {-std::numeric_limits<double>::infinity()}
    );
}

Expression parseDynamicExpression(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "dynamic");
    auto inner_expression = parseExpression(code);
    code.first = end(inner_expression);
    return makeDynamicExpression(
        CodeRange{first, code.begin()}, DynamicExpression{inner_expression}
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

} // namespace

Expression parseExpression(CodeRange code) {
    try {
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        const auto c = code.first->character;
        if (c == '[') {return parseStackNew(code);}
        if (c == '{') {return parseDictionary(code);}
        if (c == '(') {return parseTuple(code);}
        if (c == '<') {return parseTable(code);}
        if (c == '\'') {return parseCharacterExpression(code);}
        if (c == '\"') {return parseString(code);}
        if (isKeyword(code, "yes")) {return parseYes(code);}
        if (isKeyword(code, "no")) {return parseNo(code);}
        if (isKeyword(code, "-inf")) {return parseNegInf(code);}
        if (isKeyword(code, "if")) {return parseConditional(code);}
        if (isKeyword(code, "is")) {return parseIs(code);}
        if (isKeyword(code, "in")) {return parseAnyFunction(code);}
        if (isKeyword(code, "dynamic")) {return parseDynamicExpression(code);}
        if (isKeyword(code, "out")) {throwParseException(code);}
        if (isKeyword(code, "then")) {throwParseException(code);}
        if (isKeyword(code, "else")) {throwParseException(code);}
        if (isKeyword(code, "while")) {throwParseException(code);}
        if (isKeyword(code, "end")) {throwParseException(code);}
        if (isdigit(c) or c == '+' or c == '-') {return parseNumber(code);}
        if (isalpha(c) or c == '_') {return parseSubstitution(code);}
        throwParseException(code);
        return {};
    } catch (const std::runtime_error& e) {
        std::cout << "Exception while parsing: " << e.what();
        throw e;
    }
}
