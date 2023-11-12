#include "parse.h"

#include <iostream>
#include <limits>
#include <unordered_map>

#include "../factory.h"
#include "../built_in_functions/container.h"

namespace {

struct DictionaryNameIndexer {
    std::unordered_map<size_t, size_t> index_from_name;
    size_t count = 0;

    size_t getIndex(size_t name) {
        const auto it = index_from_name.find(name);
        if (it != index_from_name.end()) {
            return it->second;
        }
        else {
            index_from_name[name] = count;
            return count++;
        }
    }
};

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
            CodeRange{first, end(first_name)}, {first_name.index}
        );
        return makeArgument(
            CodeRange{first, code.first}, Argument{type, second_name.index}
        );
    }
    else {
        return makeArgument(
            CodeRange{first, code.first}, Argument{{}, first_name.index}
        );   
    }
}

Expression parseNamedElement(CodeRange code, DictionaryNameIndexer& indexer) {
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
            Definition{{name.index, indexer.getIndex(name.index)}, expression}
        );
    }
    else if (startsWith(code, '-')) {
        code = parseKeyword(code, "--");
        code = parseWhiteSpace(code);
        return makeDropAssignment(
            CodeRange{first, code.first},
            DropAssignment{name.index, indexer.getIndex(name.index)}
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
            PutAssignment{{name.index, indexer.getIndex(name.index)}, expression}
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
            PutEachAssignment{name.index, expression, indexer.getIndex(name.index)}
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

Expression parseForStatement(CodeRange code, DictionaryNameIndexer& indexer) {
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
        const auto first_name_index = indexer.getIndex(first_name.index);
        const auto second_name_index = indexer.getIndex(second_name.index);
        return makeForStatement(CodeRange{first, code.first},
            ForStatement{first_name.index, second_name.index, 0, first_name_index, second_name_index}
        );
    }
    else {
        return makeForSimpleStatement(CodeRange{first, code.first},
            ForSimpleStatement{first_name.index, 0, indexer.getIndex(first_name.index)}
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

Expression parseDictionary(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto statements = std::vector<Expression>{};
    auto loop_start_indices = std::vector<size_t>{};
    auto indexer = DictionaryNameIndexer{};
    while (!::startsWith(code, '}')) {
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        if (isKeyword(code, "while")) {
            loop_start_indices.push_back(statements.size());
            statements.push_back(parseWhileStatement(code));
        }
        else if (isKeyword(code, "for")) {
            loop_start_indices.push_back(statements.size());
            statements.push_back(parseForStatement(code, indexer));
        }
        else if (isKeyword(code, "end")) {
            const auto loop_end_index = statements.size();
            if (loop_start_indices.empty()) {
                throw ParseException("end not matching while or for", code);
            }
            const auto loop_start_index = loop_start_indices.back();
            loop_start_indices.pop_back();
            const auto start_expression = statements.at(loop_start_index);
            if (start_expression.type == WHILE_STATEMENT) {
                storage.while_statements.at(start_expression.index).end_index_ = loop_end_index;
                statements.push_back(parseWhileEndStatement(code, loop_start_index));
            } else if (start_expression.type == FOR_STATEMENT) {
                storage.for_statements.at(start_expression.index).end_index_ = loop_end_index;
                statements.push_back(parseForEndStatement(code, loop_start_index));
            } else if (start_expression.type == FOR_SIMPLE_STATEMENT) {
                storage.for_simple_statements.at(start_expression.index).end_index_ = loop_end_index;
                statements.push_back(parseForEndStatement(code, loop_start_index));
            } else {
                throw ParseException("Unexpected start type for loop", code);
            }
        }
        else if (isKeyword(code, "return")) {
            statements.push_back(parseReturnStatement(code));
        }
        else {
            statements.push_back(parseNamedElement(code, indexer));
        }
        code.first = end(statements.back());
    }
    code = parseCharacter(code, '}');
    return makeDictionary(
        CodeRange{first, code.begin()}, Dictionary{statements, indexer.count}
    );
}

Expression parseFunction(CodeRange code) {
    auto first = code.begin();
    auto argument = parseArgument(code);
    code.first = end(argument);
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunction(
        CodeRange{first, code.begin()},
        {Expression{}, argument.index, body}
    );
}

Expression parseFunctionDictionary(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    
    const auto first_argument = Expression{
        ARGUMENT, storage.arguments.size(), CodeRange{}
    };
    auto last_argument = first_argument;
    
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        const auto argument = parseArgument(code);
        code.first = end(argument);
        ++last_argument.index;
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, '}');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunctionDictionary(
        CodeRange{first, code.begin()},
        {Expression{}, first_argument.index, last_argument.index, body}
    );
}

Expression parseFunctionTuple(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);

    const auto first_argument = Expression{
        ARGUMENT, storage.arguments.size(), CodeRange{}
    };
    auto last_argument = first_argument;
    
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        const auto name = parseArgument(code);
        code.first = end(name);
        ++last_argument.index;
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code.first = end(body);
    return makeFunctionTuple(
        CodeRange{first, code.begin()},
        {Expression{}, first_argument.index, last_argument.index, body}
    );
}

Expression parseAnyFunction(CodeRange code) {
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    if (startsWith(code, '{')) {return parseFunctionDictionary(code);}
    if (startsWith(code, '(')) {return parseFunctionTuple(code);}
    return parseFunction(code);
}

Expression parseStack(CodeRange code) {
    auto first = code.begin();
    code = parseCharacter(code, '[');
    code = parseWhiteSpace(code);
    auto items = std::vector<Expression>{};
    while (!::startsWith(code, ']')) {
        throwIfEmpty(code);
        auto item = parseExpression(code);
        code.first = end(item);
        code = parseWhiteSpace(code);
        items.push_back(item);
    }
    std::reverse(items.begin(), items.end());
    auto stack = Expression{EMPTY_STACK, 0, CodeRange{}};
    for (const auto& item : items) {
        stack = putStack(stack, item);
    }
    code = parseCharacter(code, ']');
    stack.range = CodeRange{first, code.first};
    return stack;
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
        return makeLookupChild(CodeRange{first, code.first}, {name.index, child});
    }
    if (startsWith(code, '!') || startsWith(code, '?')) {
        code = parseCharacter(code);
        auto child = parseExpression(code);
        code.first = end(child);
        return makeFunctionApplication(CodeRange{first, code.first}, {name.index, child});
    }
    if (startsWith(code, ':')) {
        code = parseCharacter(code);
        auto value = parseExpression(code);
        code.first = end(value);
        return makeTypedExpression(CodeRange{first, code.first}, {name.index, value});
    }
    return makeLookupSymbol(CodeRange{first, end(name)}, {name.index});
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
    auto characters = std::vector<Expression>{};
    for (; code.first->character != '"'; ++code.first) {
        auto character = makeCharacter(
            CodeRange{code.first, code.first + 1},
            code.first->character
        );
        characters.push_back(character);
    }
    std::reverse(characters.begin(), characters.end());
    auto string = Expression{EMPTY_STRING, 0, {first, first + 1}};
    for (const auto& character : characters) {
        string = putString(string, character);
    }
    code = parseCharacter(code, '"');
    const auto last = code.begin();
    string.range = CodeRange{first, last};
    return string;
}

} // namespace

Expression parseExpression(CodeRange code) {
    try {
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        const auto c = code.first->character;
        if (c == '[') {return parseStack(code);}
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
