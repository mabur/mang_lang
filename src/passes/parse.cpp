#include "parse.h"

#include <iostream>
#include <limits>
#include <unordered_map>

#include <carma/carma.h>

#include "../exceptions.h"
#include "../factory.h"
#include "../built_in_functions/container.h"
#include "../parsing.h"

namespace {

BoundLocalName getUnboundLocalName(Expression name) {
    return BoundLocalName{name.index, 0};
}

struct DictionaryNameIndexer {
    size_t size() const {
        return dictionary_index_from_global_index.size();
    }
    void bindName(BoundLocalName& name) {
        name.dictionary_index = getDictionaryIndex(name.global_index);
    }
private:
    size_t getDictionaryIndex(size_t global_index) {
        const auto it = dictionary_index_from_global_index.find(global_index);
        if (it != dictionary_index_from_global_index.end()) {
            return it->second;
        }
        else {
            const auto count = size();
            dictionary_index_from_global_index[global_index] = count;
            return count;
        }
    }
    std::unordered_map<size_t, size_t> dictionary_index_from_global_index;
};

Expression parseCharacterExpression(CodeRange code) {
    auto whole = code;
    code = parseCharacter(code, '\'');
    auto value = firstCharacter(code);
    code = parseCharacter(code);
    code = parseCharacter(code, '\'');
    return makeCharacter(firstPart(whole, code), value);
}

Expression parseAlternative(CodeRange code) {
    auto whole = code;
    auto left = parseExpression(code);
    code = lastPart(code, left.range);
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "then");
    code = parseWhiteSpace(code);
    auto right = parseExpression(code);
    code = lastPart(code, right.range);
    code = parseWhiteSpace(code);
    return makeAlternative(firstPart(whole, code), Alternative{left, right});
}

Expression parseConditional(CodeRange code) {
    auto whole = code;

    code = parseKeyword(code, "if");
    code = parseWhiteSpace(code);
    
    auto alternatives = Expressions{};

    while (!isKeyword(code, "else")) {
        APPEND(alternatives, parseAlternative(code));
        code = lastPart(code, LAST_ITEM(alternatives).range);
    }

    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code = lastPart(code, expression_else.range);
    code = parseWhiteSpace(code);

    // TODO: verify parsing of nested alternatives. This looks suspicious.
    // TODO: make it more explicit that we require at least one alternative.
    auto result = makeConditional(
        firstPart(whole, code),
        Conditional{FIRST_ITEM(alternatives), LAST_ITEM(alternatives), expression_else}
    );
    FREE_DARRAY(alternatives);
    return result;
}

Expression parseIs(CodeRange code) {
    auto whole = code;

    code = parseKeyword(code, "is");
    code = parseWhiteSpace(code);
    auto input = parseExpression(code);
    code = lastPart(code, input.range);
    code = parseWhiteSpace(code);
    
    auto alternatives = Expressions{};

    while (!isKeyword(code, "else")) {
        APPEND(alternatives, parseAlternative(code));
        code = lastPart(code, LAST_ITEM(alternatives).range);
    }

    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code = lastPart(code, expression_else.range);
    code = parseWhiteSpace(code);

    // TODO: verify parsing of nested alternatives. This looks suspicious.
    auto result = makeIs(
        firstPart(whole, code),
        IsExpression{input, FIRST_ITEM(alternatives), LAST_ITEM(alternatives), expression_else}
    );
    FREE_DARRAY(alternatives);
    return result;
}

Expression parseName(CodeRange code) {
    auto whole = code;
    code = parseWhile(code, isNameCharacter);
    return makeName(
        firstPart(whole, code),
        rawString(firstPart(whole, code))
    );
}

Expression parseArgument(CodeRange code) {
    auto whole = code;
    auto first_name = parseName(code);
    code = lastPart(code, first_name.range);
    code = parseWhiteSpace(code);
    if (startsWith(code, ':')) {
        code = parseCharacter(code);
        code = parseWhiteSpace(code);
        auto second_name = parseName(code);
        code = lastPart(code, second_name.range);
        const auto type = makeLookupSymbol(
            first_name.range, {first_name.index}
        );
        return makeArgument(
            firstPart(whole, code), Argument{type, second_name.index}
        );
    }
    else {
        return makeArgument(
            firstPart(whole, code), Argument{{}, first_name.index}
        );   
    }
}

Expression parseNamedElement(CodeRange code) {
    auto whole = code;
    auto name = parseName(code);
    code = lastPart(code, name.range);
    code = parseWhiteSpace(code);
    
    if (startsWith(code, '=')) {
        code = parseCharacter(code, '=');
        code = parseWhiteSpace(code);
        auto expression = parseExpression(code);
        code = lastPart(code, expression.range);
        code = parseWhiteSpace(code);
        return makeDefinition(
            firstPart(whole, code),
            Definition{getUnboundLocalName(name), expression}
        );
    }
    else if (startsWith(code, '-')) {
        code = parseKeyword(code, "--");
        code = parseWhiteSpace(code);
        return makeDropAssignment(
            firstPart(whole, code),
            DropAssignment{getUnboundLocalName(name)}
        );
    }
    else if (startsWith(code, "+=")) {
        code = parseKeyword(code, "+=");
        code = parseWhiteSpace(code);
        auto expression = parseExpression(code);
        code = lastPart(code, expression.range);
        code = parseWhiteSpace(code);
        return makePutAssignment(
            firstPart(whole, code),
            PutAssignment{getUnboundLocalName(name), expression}
        );
    }
    else {
        code = parseKeyword(code, "++=");
        code = parseWhiteSpace(code);
        auto expression = parseExpression(code);
        code = lastPart(code, expression.range);
        code = parseWhiteSpace(code);
        return makePutEachAssignment(
            firstPart(whole, code),
            PutEachAssignment{getUnboundLocalName(name), expression}
        );
    }
}

Expression parseWhileStatement(CodeRange code) {
    auto whole = code;
    code = parseKeyword(code, "while");
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code = lastPart(code, expression.range);
    code = parseWhiteSpace(code);
    return makeWhileStatement(firstPart(whole, code), {expression, 0});
}

Expression parseForStatement(CodeRange code) {
    const auto whole = code;
    code = parseKeyword(code, "for");
    code = parseWhiteSpace(code);
    const auto first_name = parseName(code);
    code = lastPart(code, first_name.range);
    code = parseWhiteSpace(code);
    if (isKeyword(code, "in")) {
        code = parseKeyword(code, "in");
        code = parseWhiteSpace(code);
        auto second_name = parseName(code);
        code = lastPart(code, second_name.range);
        return makeForStatement(
            firstPart(whole, code),
            ForStatement{
                getUnboundLocalName(first_name),
                getUnboundLocalName(second_name),
                0,
            }
        );
    }
    else {
        return makeForSimpleStatement(firstPart(whole, code),
            ForSimpleStatement{getUnboundLocalName(first_name), 0}
        );
    }
}

Expression parseWhileEndStatement(CodeRange code, size_t start_index) {
    auto whole = code;
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return makeWhileEndStatement(firstPart(whole, code), {start_index});
}

Expression parseForEndStatement(CodeRange code, size_t start_index) {
    auto whole = code;
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return makeForEndStatement(firstPart(whole, code), {start_index});
}

Expression parseForSimpleEndStatement(CodeRange code, size_t start_index) {
    auto whole = code;
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return makeForSimpleEndStatement(firstPart(whole, code), {start_index});
}

Expression parseReturnStatement(CodeRange code) {
    auto whole = code;
    code = parseKeyword(code, "return");
    code = parseWhiteSpace(code);
    return Expression{RETURN_STATEMENT, 0, firstPart(whole, code)};
}

void bindDictionaryNames(Dictionary& dictionary_struct) {
    auto indexer = DictionaryNameIndexer{};
    
    for (size_t i = dictionary_struct.statement_first; i < dictionary_struct.statement_last; ++i) {
        const auto statement = storage.statements.data[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            auto& definition = storage.definitions.data[statement.index];
            indexer.bindName(definition.name);
        }
        else if (type == PUT_ASSIGNMENT) {
            auto& put_assignment = storage.put_assignments.data[statement.index];
            indexer.bindName(put_assignment.name);
        }
        else if (type == PUT_EACH_ASSIGNMENT) {
            auto& put_each_assignment = storage.put_each_assignments.data[statement.index];
            indexer.bindName(put_each_assignment.name);
        }
        else if (type == DROP_ASSIGNMENT) {
            auto& drop_assignment = storage.drop_assignments.data[statement.index];
            indexer.bindName(drop_assignment.name);
        }
        else if (type == FOR_STATEMENT) {
            auto& for_statement = storage.for_statements.data[statement.index];
            indexer.bindName(for_statement.item_name);
            indexer.bindName(for_statement.container_name);
        }
        else if (type == FOR_SIMPLE_STATEMENT) {
            auto& for_statement = storage.for_simple_statements.data[statement.index];
            indexer.bindName(for_statement.container_name);
        }
    }
    dictionary_struct.definition_count = indexer.size();
}

struct Indices {
    size_t* data;
    size_t count;
    size_t capacity;
};

Expression parseDictionary(CodeRange code) {
    auto whole = code;
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto statements = Expressions{};
    auto loop_start_indices = Indices{};
    while (!::startsWith(code, '}')) {
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        if (isKeyword(code, "while")) {
            APPEND(loop_start_indices, statements.count);
            APPEND(statements, parseWhileStatement(code));
        }
        else if (isKeyword(code, "for")) {
            APPEND(loop_start_indices, statements.count);
            APPEND(statements, parseForStatement(code));
        }
        else if (isKeyword(code, "end")) {
            const auto loop_end_index = statements.count;
            if (IS_EMPTY(loop_start_indices)) {
                throwException(
                    "I find a parsing error.\n"
                    "end is not matching a while or for%s",
                    describeLocation(code)
                );
            }
            const auto loop_start_index = LAST_ITEM(loop_start_indices);
            DROP_BACK(loop_start_indices);
            const auto start_expression = statements.data[loop_start_index];
            if (start_expression.type == WHILE_STATEMENT) {
                storage.while_statements.data[start_expression.index].end_index = loop_end_index;
                APPEND(statements, parseWhileEndStatement(code, loop_start_index));
            } else if (start_expression.type == FOR_STATEMENT) {
                storage.for_statements.data[start_expression.index].end_index = loop_end_index;
                APPEND(statements, parseForEndStatement(code, loop_start_index));
            } else if (start_expression.type == FOR_SIMPLE_STATEMENT) {
                storage.for_simple_statements.data[start_expression.index].end_index = loop_end_index;
                APPEND(statements, parseForSimpleEndStatement(code, loop_start_index));
            } else {
                throwException(
                    "Unexpected start type for loop%s",
                    describeLocation(code)
                );
            }
        }
        else if (isKeyword(code, "return")) {
            APPEND(statements, parseReturnStatement(code));
        }
        else {
            APPEND(statements, parseNamedElement(code));
        }
        code = lastPart(code, LAST_ITEM(statements).range);
    }
    code = parseCharacter(code, '}');
    
    const auto statements_first = storage.statements.count;
    CONCAT(storage.statements, statements);
    const auto statements_last = storage.statements.count;

    FREE_DARRAY(statements);
    FREE_DARRAY(loop_start_indices);
    
    auto dictionary = Dictionary{statements_first, statements_last, 0};
    bindDictionaryNames(dictionary);
    return makeDictionary(firstPart(whole, code), dictionary);
}

Expression parseFunction(CodeRange code) {
    auto whole = code;
    auto argument = parseArgument(code);
    code = lastPart(code, argument.range);
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code = lastPart(code, body.range);
    return makeFunction(
        firstPart(whole, code),
        {Expression{}, argument.index, body}
    );
}

Expression parseFunctionDictionary(CodeRange code) {
    auto whole = code;
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    
    const auto first_argument = Expression{
        ARGUMENT, storage.arguments.count, CodeRange{}
    };
    auto last_argument = first_argument;
    
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        const auto argument = parseArgument(code);
        code = lastPart(code, argument.range);
        ++last_argument.index;
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, '}');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code = lastPart(code, body.range);
    return makeFunctionDictionary(
        firstPart(whole, code),
        {Expression{}, first_argument.index, last_argument.index, body}
    );
}

Expression parseFunctionTuple(CodeRange code) {
    auto whole = code;
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);

    const auto first_argument = Expression{
        ARGUMENT, storage.arguments.count, CodeRange{}
    };
    auto last_argument = first_argument;
    
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        const auto name = parseArgument(code);
        code = lastPart(code, name.range);
        ++last_argument.index;
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code = lastPart(code, body.range);
    return makeFunctionTuple(
        firstPart(whole, code),
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
    auto whole = code;
    code = parseCharacter(code, '[');
    code = parseWhiteSpace(code);
    auto items = Expressions{};
    while (!::startsWith(code, ']')) {
        throwIfEmpty(code);
        auto item = parseExpression(code);
        code = lastPart(code, item.range);
        code = parseWhiteSpace(code);
        APPEND(items, item);
    }
    auto stack = Expression{EMPTY_STACK, 0, CodeRange{}};
    FOR_EACH_BACKWARD(it, items){
        stack = putStack(stack, *it);
    }
    FREE_DARRAY(items);
    code = parseCharacter(code, ']');
    stack.range = firstPart(whole, code);
    return stack;
}

Expression parseTuple(CodeRange code) {
    auto whole = code;
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto expressions = Expressions{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        auto expression = parseExpression(code);
        code = lastPart(code, expression.range);
        APPEND(expressions, expression);
        code = parseWhiteSpace(code);
    }
    const auto first_expression = storage.expressions.count;
    CONCAT(storage.expressions,  expressions);
    FREE_DARRAY(expressions);
    const auto last_expression = storage.expressions.count;
    code = parseCharacter(code, ')');
    return makeTuple(firstPart(whole, code), Tuple{first_expression, last_expression});
}

Expression parseTable(CodeRange code) {
    auto whole = code;
    code = parseCharacter(code, '<');
    code = parseWhiteSpace(code);
    auto rows = std::vector<Row>{};
    while (!::startsWith(code, '>')) {
        throwIfEmpty(code);
        code = parseCharacter(code, '(');
        code = parseWhiteSpace(code);
        const auto key = parseExpression(code);
        code = lastPart(code, key.range);
        code = parseWhiteSpace(code);
        const auto value = parseExpression(code);
        code = lastPart(code, value.range);
        code = parseWhiteSpace(code);
        code = parseCharacter(code, ')');
        code = parseWhiteSpace(code);
        rows.push_back({key, value});
    }
    code = parseCharacter(code, '>');
    return makeTable(firstPart(whole, code), Table{rows});
}

Expression parseSubstitution(CodeRange code) {
    auto whole = code;
    auto name = parseName(code);
    code = lastPart(code, name.range);
    code = parseWhiteSpace(code);
    if (startsWith(code, '@')) {
        code = parseCharacter(code);
        code = parseWhiteSpace(code);
        auto child = parseExpression(code);
        code = lastPart(code, child.range);
        return makeLookupChild(firstPart(whole, code), {name.index, child});
    }
    if (startsWith(code, '!') || startsWith(code, '?')) {
        code = parseCharacter(code);
        auto child = parseExpression(code);
        code = lastPart(code, child.range);
        return makeFunctionApplication(
            firstPart(whole, code), {BoundGlobalName{name.index}, child}
        );
    }
    if (startsWith(code, ':')) {
        code = parseCharacter(code);
        auto value = parseExpression(code);
        code = lastPart(code, value.range);
        return makeTypedExpression(
            firstPart(whole, code), {BoundGlobalName{name.index}, value}
        );
    }
    return makeLookupSymbol(name.range, {name.index});
}

Expression parseNumber(CodeRange code) {
    code = parseRawNumber(code);
    const auto value = std::stod(rawString(code));
    return makeNumber(code, value);
}

CodeRange parseKeyWordContent(CodeRange code, const char* keyword) {
    auto tail = parseKeyword(code, keyword);
    return firstPart(code, tail);
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
        -std::numeric_limits<double>::infinity()
    );
}

Expression parseDynamicExpression(CodeRange code) {
    auto whole = code;
    code = parseKeyword(code, "dynamic");
    auto inner_expression = parseExpression(code);
    code = lastPart(code, inner_expression.range);
    return makeDynamicExpression(
        firstPart(whole, code), DynamicExpression{inner_expression}
    );
}

Expression parseString(CodeRange code) {
    auto whole = code;
    code = parseCharacter(code, '"');
    auto characters = Expressions{};
    for (;;) {
        auto c = firstCharacter(code);
        if (c == '"') {
            break;
        }
        auto character = makeCharacter(CodeRange{code.data, 1}, c);
        APPEND(characters, character);
        DROP_FRONT(code);
    }
    auto string = Expression{EMPTY_STRING, 0, CodeRange{whole.data, 1}};
    FOR_EACH_BACKWARD(it, characters) {
        string = putString(string, *it);
    }
    FREE_DARRAY(characters);
    code = parseCharacter(code, '"');
    string.range = firstPart(whole, code);
    return string;
}

} // namespace

Expression parseExpression(CodeRange code) {
    try {
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        const auto c = firstCharacter(code);
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
        return Expression{};
    } catch (const std::runtime_error& e) {
        std::cout << "Exception while parsing: " << e.what();
        throw e;
    }
}
