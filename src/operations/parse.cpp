#include "parse.h"

#include <iostream>
#include <limits>
#include <unordered_map>

#include "../factory.h"
#include "../built_in_functions/container.h"

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

Expression parseAlternative(CodeRange code) {
    auto first = code.begin();
    auto left = parseExpression(code);
    code.first = end(left);
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "then");
    code = parseWhiteSpace(code);
    auto right = parseExpression(code);
    code.first = end(right);
    code = parseWhiteSpace(code);
    return makeAlternative({first, code.first}, Alternative{left, right});
}

Expression parseConditional(CodeRange code) {
    auto first = code.begin();

    code = parseKeyword(code, "if");
    code = parseWhiteSpace(code);
    
    auto alternatives = std::vector<Expression>{};

    while (!isKeyword(code, "else")) {
        alternatives.push_back(parseAlternative(code));
        code.first = end(alternatives.back());
    }

    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code.first = end(expression_else);
    code = parseWhiteSpace(code);

    return makeConditional(
        CodeRange{first, code.begin()},
        Conditional{alternatives.front(), alternatives.back(), expression_else}
    );
}

Expression parseIs(CodeRange code) {
    auto first = code.begin();

    code = parseKeyword(code, "is");
    code = parseWhiteSpace(code);
    auto input = parseExpression(code);
    code.first = end(input);
    code = parseWhiteSpace(code);
    
    auto alternatives = std::vector<Expression>{};

    while (!isKeyword(code, "else")) {
        alternatives.push_back(parseAlternative(code));
        code.first = end(alternatives.back());
    }

    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code.first = end(expression_else);
    code = parseWhiteSpace(code);

    return makeIs(
        CodeRange{first, code.begin()},
        IsExpression{input, alternatives.front(), alternatives.back(), expression_else}
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

Expression parseArgument(CodeRange code) {
    auto first = code.begin();
    auto first_name = parseName(code);
    code.first = end(first_name);
    code = parseWhiteSpace(code);
    if (startsWith(code, ':')) {
        code = parseCharacter(code);
        code = parseWhiteSpace(code);
        auto second_name = parseName(code);
        code.first = end(second_name);
        const auto type = makeLookupSymbol(
            CodeRange{first, end(first_name)}, {first_name}
        );
        return makeArgument(
            CodeRange{first, code.first}, Argument{type, second_name}
        );
    }
    else {
        return makeArgument(
            CodeRange{first, code.first}, Argument{{}, first_name}
        );   
    }
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
            Definition{std::move(name), expression, 0}
        );
    }
    else if (startsWith(code, '-')) {
        code = parseKeyword(code, "--");
        code = parseWhiteSpace(code);
        return makeDropAssignment(
            CodeRange{first, code.first},
            DropAssignment{std::move(name), 0}
        );
    }
    else if (startsWith(code, "+=")) {
        code = parseKeyword(code, "+=");
        code = parseWhiteSpace(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        code = parseWhiteSpace(code);
        return makePutAssignment(
            CodeRange{first, code.first},
            PutAssignment{std::move(name), expression, 0}
        );
    }
    else {
        code = parseKeyword(code, "++=");
        code = parseWhiteSpace(code);
        auto expression = parseExpression(code);
        code.first = end(expression);
        code = parseWhiteSpace(code);
        return makePutEachAssignment(
            CodeRange{first, code.first},
            PutEachAssignment{std::move(name), expression, 0}
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
    auto first_name = parseName(code);
    code.first = end(first_name);
    code = parseWhiteSpace(code);
    if (isKeyword(code, "in")) {
        code = parseKeyword(code, "in");
        code = parseWhiteSpace(code);
        auto second_name = parseName(code);
        code.first = end(second_name);
        return makeForStatement(CodeRange{first, code.first},
            ForStatement{first_name, second_name, 0}
        );
    }
    else {
        return makeForSimpleStatement(CodeRange{first, code.first},
            ForSimpleStatement{first_name, 0, 0}
        );
    }
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

Expression parseReturnStatement(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "return");
    code = parseWhiteSpace(code);
    return Expression{RETURN_STATEMENT, 0, CodeRange{first, code.first}};
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
    auto statements = std::vector<Expression>{};
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
                if (for_statement_pointer.type == FOR_STATEMENT) {
                    auto& for_statement = getMutableForStatement(for_statement_pointer);
                    for_statement.end_index_ = end_index;
                }
                else if (for_statement_pointer.type == FOR_SIMPLE_STATEMENT) {
                    auto& for_statement = getMutableForSimpleStatement(for_statement_pointer);
                    for_statement.end_index_ = end_index;
                }
                statements.push_back(parseForEndStatement(code, for_index));
            }
        }
        else if (isKeyword(code, "return")) {
            statements.push_back(parseReturnStatement(code));
        }
        else {
            statements.push_back(parseNamedElement(code));
        }
        code.first = end(statements.back());
    }
    code = parseCharacter(code, '}');
    // Set indices for definitions:
    auto new_statements = std::vector<Expression>{};
    auto index_from_name = std::unordered_map<size_t, size_t>{};
    size_t i = 0;
    for (const auto statement : statements) {
        if (statement.type == DEFINITION) {
            auto assignment = getDefinition(statement);
            const auto name_index = assignment.name.index;
            const auto it = index_from_name.find(assignment.name.index);
            if (it != index_from_name.end()) {
                const auto index_in_dictionary = it->second;
                assignment.name_index = index_in_dictionary;
            }
            else {
                const auto index_in_dictionary = i++;
                index_from_name[name_index] = index_in_dictionary;
                assignment.name_index = index_in_dictionary;
            }
            new_statements.push_back(makeDefinition(statement.range, assignment));
        }
        else if (statement.type == PUT_ASSIGNMENT) {
            auto assignment = getPutAssignment(statement);
            const auto name_index = assignment.name.index;
            const auto it = index_from_name.find(assignment.name.index);
            if (it != index_from_name.end()) {
                const auto index_in_dictionary = it->second;
                assignment.name_index = index_in_dictionary;
            }
            else {
                const auto index_in_dictionary = i++;
                index_from_name[name_index] = index_in_dictionary;
                assignment.name_index = index_in_dictionary;
            }
            new_statements.push_back(makePutAssignment(statement.range, assignment));
        }
        else if (statement.type == PUT_EACH_ASSIGNMENT) {
            auto assignment = getPutEachAssignment(statement);
            const auto name_index = assignment.name.index;
            const auto it = index_from_name.find(assignment.name.index);
            if (it != index_from_name.end()) {
                const auto index_in_dictionary = it->second;
                assignment.name_index = index_in_dictionary;
            }
            else {
                const auto index_in_dictionary = i++;
                index_from_name[name_index] = index_in_dictionary;
                assignment.name_index = index_in_dictionary;
            }
            new_statements.push_back(makePutEachAssignment(statement.range, assignment));
        }
        else if (statement.type == DROP_ASSIGNMENT) {
            auto assignment = getDropAssignment(statement);
            const auto name_index = assignment.name.index;
            const auto it = index_from_name.find(assignment.name.index);
            if (it != index_from_name.end()) {
                const auto index_in_dictionary = it->second;
                assignment.name_index = index_in_dictionary;
            }
            else {
                const auto index_in_dictionary = i++;
                index_from_name[name_index] = index_in_dictionary;
                assignment.name_index = index_in_dictionary;
            }
            new_statements.push_back(makeDropAssignment(statement.range, assignment));
        }
        else if (statement.type == FOR_SIMPLE_STATEMENT) {
            auto assignment = getForSimpleStatement(statement);
            const auto name_index = assignment.name_container.index;
            const auto it = index_from_name.find(assignment.name_container.index);
            if (it != index_from_name.end()) {
                const auto index_in_dictionary = it->second;
                assignment.name_index = index_in_dictionary;
            }
            else {
                const auto index_in_dictionary = i++;
                index_from_name[name_index] = index_in_dictionary;
                assignment.name_index = index_in_dictionary;
            }
            new_statements.push_back(makeForSimpleStatement(statement.range, assignment));
        }
        else {
            new_statements.push_back(statement);
        }
    }
    return makeDictionary(
        CodeRange{first, code.begin()}, Dictionary{new_statements, index_from_name.size()}
    );
}

Expression parseFunction(CodeRange code) {
    auto first = code.begin();
    auto input_name = parseArgument(code);
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
        const auto name = parseArgument(code);
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
        const auto name = parseArgument(code);
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
    auto stack = Expression{EMPTY_STACK, 0, CodeRange{}};
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
        code = parseCharacter(code, '(');
        code = parseWhiteSpace(code);
        const auto key = parseExpression(code);
        code.first = end(key);
        code = parseWhiteSpace(code);
        const auto value = parseExpression(code);
        code.first = end(value);
        code = parseWhiteSpace(code);
        code = parseCharacter(code, ')');
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
    if (startsWith(code, '!') || startsWith(code, '?')) {
        code = parseCharacter(code);
        auto child = parseExpression(code);
        code.first = end(child);
        return makeFunctionApplication(CodeRange{first, code.first}, {name, child});
    }
    if (startsWith(code, ':')) {
        code = parseCharacter(code);
        auto value = parseExpression(code);
        code.first = end(value);
        const auto type = makeLookupSymbol(CodeRange{first, end(name)}, {name});
        return makeTypedExpression(CodeRange{first, code.first}, {type, value});
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
    return Expression{YES, 0, parseKeyWordContent(code, "yes")};
}

Expression parseNo(CodeRange code) {
    return Expression{NO, 0, parseKeyWordContent(code, "no")};
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
    auto value = Expression{EMPTY_STRING, 0, {first, first + 1}};
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
        if (isdigit(c) || c == '+' || c == '-') {return parseNumber(code);}
        if (isalpha(c) || c == '_') {return parseSubstitution(code);}
        throwParseException(code);
        return {};
    } catch (const std::runtime_error& e) {
        std::cout << "Exception while parsing: " << e.what();
        throw e;
    }
}
