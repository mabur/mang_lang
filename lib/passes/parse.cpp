#include "parse.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <carma/carma.h>

#include "../exceptions.h"
#include "../factory.h"
#include "../built_in_functions/arithmetic.h"
#include "../built_in_functions/container.h"
#include "../parsing.h"
#include "../mang_lang_string.h"

namespace {

BoundLocalName getUnboundLocalName(Expression name) {
    return BoundLocalName{name.index, 0};
}

struct BuiltInEntry {
    const char* name;
    FunctionPointer function;
    FunctionPointer function_types;
};

// Mirrors builtIns()/builtInsTypes() in built_in_functions.cpp exactly, so a
// function application to one of these names can be resolved directly at
// parse time, skipping the environment lookup entirely at evaluation time.
const BuiltInEntry BUILT_IN_ENTRIES[] = {
    {"clear",      container_functions::clear, container_functions::clearTyped},
    {"put",        container_functions::put,   container_functions::putTyped},
    {"take",       container_functions::take,  container_functions::takeTyped},
    {"drop",       container_functions::drop,  container_functions::dropTyped},
    {"get",        container_functions::get,   container_functions::getTyped},
    {"add",        arithmetic::add,            arithmetic::add},
    {"mul",        arithmetic::mul,            arithmetic::mul},
    {"sub",        arithmetic::sub,            arithmetic::sub},
    {"div",        arithmetic::div,            arithmetic::div},
    {"mod",        arithmetic::mod,            arithmetic::mod},
    {"less",       arithmetic::less,           arithmetic::less},
    {"round",      arithmetic::round,          arithmetic::round},
    {"round_up",   arithmetic::roundUp,        arithmetic::roundUp},
    {"round_down", arithmetic::roundDown,      arithmetic::roundDown},
    {"sqrt",       arithmetic::sqrt,           arithmetic::sqrt},
    {"number",     arithmetic::asciiNumber,    arithmetic::asciiNumber},
    {"character",  arithmetic::asciiCharacter, arithmetic::asciiCharacter},
};

const BuiltInEntry* findBuiltIn(size_t name_index) {
    const auto name_text = storage.names.data + name_index;
    for (const auto& entry : BUILT_IN_ENTRIES) {
        if (strcmp(name_text, entry.name) == 0) {
            return &entry;
        }
    }
    return nullptr;
}

Expression parseCharacterExpression(CodeRange code) {
    auto whole = code;
    if (code.count < 3) {
        return makeErrorExpression(code,
            "I found an error while parsing a character.\n"
            "It ends too early."
        );
    }
    if (!startsWith(code, '\'')) {
        return makeErrorExpression(code, "Parse error. Expected '");
    }
    code = parseCharacter(code);
    auto value = firstCharacter(code);
    code = parseCharacter(code);
    if (!startsWith(code, '\'')) {
        return makeErrorExpression(code, "Parse error. Expected '");
    }
    code = parseCharacter(code);
    return makeCharacter(firstPart(whole, code), value);
}

Expression parseAlternative(CodeRange code) {
    auto whole = code;
    auto left = parseExpression(code);
    code = lastPart(code, left.range);
    code = parseWhiteSpace(code);
    if (!isKeyword(code, "then")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'then'."
        );
    }
    if (!isKeyword(code, "then")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'then'."
        );
    }
    code = parseKeyword(code, "then");
    code = parseWhiteSpace(code);
    auto right = parseExpression(code);
    code = lastPart(code, right.range);
    code = parseWhiteSpace(code);
    return makeAlternative(firstPart(whole, code), Alternative{left, right});
}

Expression parseConditional(CodeRange code) {
    auto whole = code;
    if (!isKeyword(code, "if")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'if'."
        );
    }
    code = parseKeyword(code, "if");
    code = parseWhiteSpace(code);
    
    auto alternatives = Expressions{};

    while (!isKeyword(code, "else")) {
        APPEND(alternatives, parseAlternative(code));
        code = lastPart(code, LAST_ITEM(alternatives).range);
    }
    if (!isKeyword(code, "else")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'else'."
        );
    }
    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code = lastPart(code, expression_else.range);
    code = parseWhiteSpace(code);

    // TODO: verify parsing of nested alternatives. This looks suspicious.
    // TODO: make it more explicit that we require at least one alternative.
    auto first_index = FIRST_ITEM(alternatives).index;
    auto last_index = LAST_ITEM(alternatives).index;
    auto result = makeConditional(
        firstPart(whole, code),
        Conditional{Indices{first_index, last_index - first_index + 1}, expression_else}
    );
    FREE_DARRAY(alternatives);
    return result;
}

Expression parseIs(CodeRange code) {
    auto whole = code;
    if (!isKeyword(code, "is")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'is'."
        );
    }
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
    if (!isKeyword(code, "else")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'else'."
        );
    }
    code = parseKeyword(code, "else");
    code = parseWhiteSpace(code);
    auto expression_else = parseExpression(code);
    code = lastPart(code, expression_else.range);
    code = parseWhiteSpace(code);

    // TODO: verify parsing of nested alternatives. This looks suspicious.
    auto first_index = FIRST_ITEM(alternatives).index;
    auto last_index = LAST_ITEM(alternatives).index;
    auto result = makeIs(
        firstPart(whole, code),
        IsExpression{
            input,
            Indices{first_index, last_index - first_index + 1},
            expression_else
        }
    );
    FREE_DARRAY(alternatives);
    return result;
}

Expression parseName(CodeRange code) {
    auto whole = code;
    code = parseRawName(code);
    auto first_part = firstPart(whole, code);
    return makeName(
        first_part,
        storage.code_characters.data + first_part.data,
        first_part.count
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
        code = parseCharacter(code);
        code = parseWhiteSpace(code);
        auto expression = parseExpression(code);
        code = lastPart(code, expression.range);
        code = parseWhiteSpace(code);
        return makeDefinition(
            firstPart(whole, code),
            Definition{getUnboundLocalName(name), expression}
        );
    }
    else if (startsWithString(code, "--")) {
        code = parseKeyword(code, "--");
        code = parseWhiteSpace(code);
        return makeDropAssignment(
            firstPart(whole, code),
            DropAssignment{getUnboundLocalName(name)}
        );
    }
    else if (startsWithString(code, "+=")) {
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
    else if (startsWithString(code, "++=")) {
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
    return makeErrorExpression(code,
        "I found a parsing error. I do not recognize the statement."
    );
}

Expression parseWhileStatement(CodeRange code) {
    auto whole = code;
    if (!isKeyword(code, "while")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'while'."
        );
    }
    code = parseKeyword(code, "while");
    code = parseWhiteSpace(code);
    auto expression = parseExpression(code);
    code = lastPart(code, expression.range);
    code = parseWhiteSpace(code);
    return makeWhileStatement(firstPart(whole, code), {expression, 0});
}

Expression parseForStatement(CodeRange code) {
    const auto whole = code;
    if (!isKeyword(code, "for")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'for'."
        );
    }
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

Expression parseEndStatement(CodeRange code) {
    auto whole = code;
    if (!isKeyword(code, "end")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'end'."
        );
    }
    code = parseKeyword(code, "end");
    code = parseWhiteSpace(code);
    return Expression{0, firstPart(whole, code), END_STATEMENT};
}

Expression parseReturnStatement(CodeRange code) {
    auto whole = code;
    if (!isKeyword(code, "return")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'return'."
        );
    }
    code = parseKeyword(code, "return");
    code = parseWhiteSpace(code);
    return Expression{0, firstPart(whole, code), RETURN_STATEMENT};
}

Expression parseDictionary(CodeRange code) {
    auto whole = code;
    if (!startsWith(code, '{')) {
        return makeErrorExpression(code, "Parse error. Expected {");
    }
    code = parseCharacter(code);
    code = parseWhiteSpace(code);
    auto statements = Expressions{};
    auto loop_depth = size_t{0};
    while (!::startsWith(code, '}')) {
        code = parseWhiteSpace(code);
        if (IS_EMPTY(code)) {
            return makeErrorExpression(code,
                "I found an error while parsing a dictionary.\nIt ended too early."
            );
        }
        if (isKeyword(code, "while")) {
            ++loop_depth;
            APPEND(statements, parseWhileStatement(code));
        }
        else if (isKeyword(code, "for")) {
            ++loop_depth;
            APPEND(statements, parseForStatement(code));
        }
        else if (isKeyword(code, "end")) {
            if (loop_depth == 0) {
                return makeErrorExpression(code,
                    "I find a parsing error.\n"
                    "end is not matching a while or for");
            }
            --loop_depth;
            APPEND(statements, parseEndStatement(code));
        }
        else if (isKeyword(code, "return")) {
            APPEND(statements, parseReturnStatement(code));
        }
        else {
            APPEND(statements, parseNamedElement(code));
        }
        code = lastPart(code, LAST_ITEM(statements).range);
    }
    if (!startsWith(code, '}')) {
        return makeErrorExpression(code, "Parse error. Expected }");
    }
    code = parseCharacter(code);

    const auto statements_first = storage.statements.count;
    CONCAT(storage.statements, statements);
    const auto statements_last = storage.statements.count;

    FREE_DARRAY(statements);

    auto dictionary = Dictionary{Indices{statements_first, statements_last - statements_first}, 0};
    return makeDictionary(firstPart(whole, code), dictionary);
}

Expression parseFunction(CodeRange code) {
    auto whole = code;
    auto argument = parseArgument(code);
    code = lastPart(code, argument.range);
    code = parseWhiteSpace(code);
    if (!isKeyword(code, "out")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'out'."
        );
    }
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
    if (!startsWith(code, '{')) {
        return makeErrorExpression(code, "Parse error. Expected {");
    }
    code = parseCharacter(code);
    code = parseWhiteSpace(code);
    
    const auto first_argument = Expression{
        storage.arguments.count, CodeRange{}, ARGUMENT
    };
    auto last_argument = first_argument;
    
    while (!::startsWith(code, '}')) {
        if (IS_EMPTY(code)) {
            return makeErrorExpression(code,
                "I found an error while parsing a function.\n"
                "The input had a starting '{' but no ending '}'."
            );
        }
        const auto argument = parseArgument(code);
        code = lastPart(code, argument.range);
        ++last_argument.index;
        code = parseWhiteSpace(code);
    }
    if (!startsWith(code, '}')) {
        return makeErrorExpression(code, "Parse error. Expected }");
    }
    code = parseCharacter(code);
    code = parseWhiteSpace(code);
    if (!isKeyword(code, "out")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'out'."
        );
    }
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code = lastPart(code, body.range);
    auto indices = Indices{first_argument.index, last_argument.index - first_argument.index};
    return makeFunctionDictionary(
        firstPart(whole, code),
        FunctionDictionary{
            Expression{},
            indices,
            body
        }
    );
}

Expression parseFunctionTuple(CodeRange code) {
    auto whole = code;
    if (!startsWith(code, '(')) {
        return makeErrorExpression(code, "Parse error. Expected (");
    }
    code = parseCharacter(code);
    code = parseWhiteSpace(code);

    const auto first_argument = Expression{
        storage.arguments.count, CodeRange{}, ARGUMENT
    };
    auto last_argument = first_argument;
    
    while (!::startsWith(code, ')')) {
        if (IS_EMPTY(code)) {
            return makeErrorExpression(code,
                "I found an error while parsing a function.\n"
                "The function definition ended too early."
            );
        }
        const auto name = parseArgument(code);
        code = lastPart(code, name.range);
        ++last_argument.index;
        code = parseWhiteSpace(code);
    }
    if (!startsWith(code, ')')) {
        return makeErrorExpression(code, "Parse error. Expected )");
    }
    code = parseCharacter(code);
    code = parseWhiteSpace(code);
    if (!isKeyword(code, "out")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'out'."
        );
    }
    code = parseKeyword(code, "out");
    auto body = parseExpression(code);
    code = lastPart(code, body.range);
    return makeFunctionTuple(
        firstPart(whole, code),
        {Expression{}, Indices{first_argument.index, last_argument.index - first_argument.index}, body}
    );
}

Expression parseAnyFunction(CodeRange code) {
    if (!isKeyword(code, "in")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'in'."
        );
    }
    if (!isKeyword(code, "in")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'in'."
        );
    }
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    if (startsWith(code, '{')) {return parseFunctionDictionary(code);}
    if (startsWith(code, '(')) {return parseFunctionTuple(code);}
    return parseFunction(code);
}

Expression parseStack(CodeRange code) {
    auto whole = code;
    if (!startsWith(code, '[')) {
        return makeErrorExpression(code, "Parse error. Expected [");
    }
    code = parseCharacter(code);
    code = parseWhiteSpace(code);
    auto items = Expressions{};
    while (!::startsWith(code, ']')) {
        if (IS_EMPTY(code)) {
            return makeErrorExpression(code,
                "I found an error while parsing a stack.\n"
                "It is missing a closing ']'."
            );
        };
        auto item = parseExpression(code);
        code = lastPart(code, item.range);
        code = parseWhiteSpace(code);
        APPEND(items, item);
    }
    auto stack = Expression{0, CodeRange{}, EMPTY_STACK};
    FOR_EACH_BACKWARD(it, items){
        stack = putStack(stack, *it);
    }
    FREE_DARRAY(items);
    if (!startsWith(code, ']')) {
        return makeErrorExpression(code, "Parse error. Expected ]");
    }
    code = parseCharacter(code);
    stack.range = firstPart(whole, code);
    return stack;
}

Expression parseTuple(CodeRange code) {
    auto whole = code;
    if (!startsWith(code, '(')) {
        return makeErrorExpression(code, "Parse error. Expected (");
    }
    code = parseCharacter(code);
    code = parseWhiteSpace(code);
    auto expressions = Expressions{};
    while (!::startsWith(code, ')')) {
        if (IS_EMPTY(code)) {
            return makeErrorExpression(code,
                "I found an error while parsing a tuple.\n"
                "It is missing a closing ')'."
            );
        };
        auto expression = parseExpression(code);
        code = lastPart(code, expression.range);
        APPEND(expressions, expression);
        code = parseWhiteSpace(code);
    }
    const auto first_expression = storage.expressions.count;
    CONCAT(storage.expressions,  expressions);
    FREE_DARRAY(expressions);
    const auto last_expression = storage.expressions.count;
    if (!startsWith(code, ')')) {
        return makeErrorExpression(code, "Parse error. Expected )");
    }
    code = parseCharacter(code);
    return makeTuple(
        firstPart(whole, code),
        Tuple{Indices{first_expression, last_expression - first_expression}}
    );
}

struct Rows {
    Row* data;
    size_t count;
    size_t capacity;
};

Expression parseTable(CodeRange code) {
    auto whole = code;
    if (!startsWith(code, '<')) {
        return makeErrorExpression(code, "Parse error. Expected <");
    }
    code = parseCharacter(code);
    code = parseWhiteSpace(code);
    auto rows = Rows{};
    while (!::startsWith(code, '>')) {
        if (IS_EMPTY(code)) {
            return makeErrorExpression(code,
                "I found an error while parsing a table.\n"
                "It is missing a closing '>'."
            );
        }
        if (!startsWith(code, '(')) {
            return makeErrorExpression(code, "Parse error. Expected (");
        }
        code = parseCharacter(code);
        code = parseWhiteSpace(code);
        const auto key = parseExpression(code);
        code = lastPart(code, key.range);
        code = parseWhiteSpace(code);
        const auto value = parseExpression(code);
        code = lastPart(code, value.range);
        code = parseWhiteSpace(code);
        if (!startsWith(code, ')')) {
           return makeErrorExpression(code, "Parse error. Expected )");
        }
        code = parseCharacter(code);
        code = parseWhiteSpace(code);
        auto row = Row{key, value};
        APPEND(rows, row);
    }
    if (!startsWith(code, '>')) {
        return makeErrorExpression(code, "Parse error. Expected >");
    }
    code = parseCharacter(code);
    auto first = storage.rows.count;
    CONCAT(storage.rows, rows);
    FREE_DARRAY(rows);
    auto last = storage.rows.count;
    return makeTable(firstPart(whole, code), Table{Indices{first, last - first}});
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
        const auto built_in = findBuiltIn(name.index);
        if (built_in) {
            return makeFunctionApplicationBuiltIn(
                firstPart(whole, code),
                FunctionApplicationBuiltIn{name.index, built_in->function, built_in->function_types, child}
            );
        }
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
    if (IS_EMPTY(code)) {
        return makeErrorExpression(code, "Reached end of file when parsing number");
    }
    auto start = code;
    bool is_negative = false;
    if (startsWith(code, '+')) {
        DROP_FRONT(code);
    }
    else if (startsWith(code, '-')) {
        is_negative = true;
        DROP_FRONT(code);
    }
    if (IS_EMPTY(code)) {
        return makeErrorExpression(code, "Reached end of file when parsing number");
    }
    double integer_part = 0.0;
    while (startsWithDigit(code)) {
        integer_part = integer_part * 10 + parseDigitAsDouble(code);
        DROP_FRONT(code);
    }
    double fraction_part = 0.0;
    if (startsWith(code, '.')) {
        DROP_FRONT(code);
        if (IS_EMPTY(code)) {
            return makeErrorExpression(code, "Reached end of file when parsing number");
        }
        double divisor = 10.0;
        while (startsWithDigit(code)) {
            fraction_part += parseDigitAsDouble(code) / divisor;
            divisor *= 10;
            DROP_FRONT(code);
        }
    }
    double value = integer_part + fraction_part;
    if (is_negative) {
        value = -value;
    }
    return makeNumber(firstPart(start, code), value);
}

CodeRange parseKeyWordContent(CodeRange code, const char* keyword) {
    auto tail = parseKeyword(code, keyword);
    return firstPart(code, tail);
}

Expression parseYes(CodeRange code) {
    return Expression{0, parseKeyWordContent(code, "yes"), YES};
}

Expression parseNo(CodeRange code) {
    return Expression{0, parseKeyWordContent(code, "no"), NO};
}

Expression parseNegInf(CodeRange code) {
    return makeNumber(parseKeyWordContent(code, "-inf"), -INFINITY);
}

Expression parseDynamicExpression(CodeRange code) {
    auto whole = code;
    if (!isKeyword(code, "dynamic")) {
        return makeErrorExpression(code,
            "I found a parsing error. I was expecting the keyword 'dynamic'."
        );
    }
    code = parseKeyword(code, "dynamic");
    auto inner_expression = parseExpression(code);
    code = lastPart(code, inner_expression.range);
    return makeDynamicExpression(
        firstPart(whole, code), DynamicExpression{inner_expression}
    );
}

CodeRange rangeOfFirst(CodeRange code) {
    return CodeRange{code.data, 1};
}

Expression parseString(CodeRange code) {
    auto whole = code;
    if (!startsWith(code, '"')) {
        return makeErrorExpression(code, "Parse error. Expected \"");
    }
    code = parseCharacter(code);
    auto characters = Expressions{};
    for (;;) {
        auto c = firstCharacter(code);
        if (c == '"') {
            break;
        }
        auto character = makeCharacter(rangeOfFirst(code), c);
        APPEND(characters, character);
        DROP_FRONT(code);
    }
    auto string = Expression{0, rangeOfFirst(whole), EMPTY_STRING};
    FOR_EACH_BACKWARD(it, characters) {
        string = putString(string, *it);
    }
    FREE_DARRAY(characters);
    if (!startsWith(code, '"')) {
        return makeErrorExpression(code, "Parse error. Expected \"");
    }
    code = parseCharacter(code);
    string.range = firstPart(whole, code);
    return string;
}

} // namespace

Expression parseExpression(CodeRange code) {
    code = parseWhiteSpace(code);
    if (IS_EMPTY(code)) {
        return makeErrorExpression(code,
            "I did not find any expression to parse."
        );
    }
    const auto c = firstCharacter(code);
    if (c == '[') return parseStack(code);
    if (c == '{') return parseDictionary(code);
    if (c == '(') return parseTuple(code);
    if (c == '<') return parseTable(code);
    if (c == '\'') return parseCharacterExpression(code);
    if (c == '\"') return parseString(code);
    if (isKeyword(code, "yes")) return parseYes(code);
    if (isKeyword(code, "no")) return parseNo(code);
    if (isKeyword(code, "-inf")) return parseNegInf(code);
    if (isKeyword(code, "if")) return parseConditional(code);
    if (isKeyword(code, "is")) return parseIs(code);
    if (isKeyword(code, "in")) return parseAnyFunction(code);
    if (isKeyword(code, "dynamic")) return parseDynamicExpression(code);
    if (isKeyword(code, "out")) return makeErrorExpression(code, "Parse error. 'out' is a reserved keyword.");
    if (isKeyword(code, "then")) return makeErrorExpression(code, "Parse error. 'then' is a reserved keyword.");
    if (isKeyword(code, "else")) return makeErrorExpression(code, "Parse error. 'else' is a reserved keyword.");
    if (isKeyword(code, "while")) return makeErrorExpression(code, "Parse error. 'while' is a reserved keyword.");
    if (isKeyword(code, "end")) return makeErrorExpression(code, "Parse error. 'end' is a reserved keyword.");
    if (isdigit(c) || c == '+' || c == '-') return parseNumber(code);
    if (isalpha(c) || c == '_') return parseSubstitution(code);
    return makeErrorExpression(code, "I did not recognize the expression to parse %s");
}
