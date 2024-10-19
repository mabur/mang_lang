#include "factory.h"

#include <cstring>
#include <unordered_map>
#include <vector>

#include <carma/carma.h>

#include "exceptions.h"
#include "passes/serialize.h"
#include "string.h"

Storage storage;

namespace {

template<typename ElementType, typename ArrayType>
Expression makeExpression(
    CodeRange code,
    ElementType&& expression,
    ExpressionType type,
    ArrayType& array
) {
    APPEND(array, expression);
    return Expression{type, array.count - 1, code};
}

} // namespace

void clearMemory() {
    FREE_DARRAY(storage.code_characters);
    
    FREE_DARRAY(storage.dynamic_expressions);
    FREE_DARRAY(storage.typed_expressions);
    FREE_DARRAY(storage.dictionaries);
    FREE_DARRAY(storage.conditionals);
    FREE_DARRAY(storage.is_expressions);
    FREE_DARRAY(storage.alternatives);
    FREE_DARRAY(storage.functions);
    FREE_DARRAY(storage.built_in_functions);
    FREE_DARRAY(storage.dictionary_functions);
    FREE_DARRAY(storage.tuple_functions);
    FREE_DARRAY(storage.tuples);
    FREE_DARRAY(storage.evaluated_tuples);
    FREE_DARRAY(storage.stacks);
    FREE_DARRAY(storage.evaluated_stacks);
    FREE_DARRAY(storage.evaluated_table_views);
    FREE_DARRAY(storage.child_lookups);
    FREE_DARRAY(storage.function_applications);
    FREE_DARRAY(storage.symbol_lookups);
    FREE_DARRAY(storage.arguments);
    FREE_DARRAY(storage.while_statements);
    FREE_DARRAY(storage.for_statements);
    FREE_DARRAY(storage.for_simple_statements);
    FREE_DARRAY(storage.while_end_statements);
    FREE_DARRAY(storage.for_end_statements);
    FREE_DARRAY(storage.for_simple_end_statements);
    FREE_DARRAY(storage.definitions);
    FREE_DARRAY(storage.put_assignments);
    FREE_DARRAY(storage.put_each_assignments);
    FREE_DARRAY(storage.drop_assignments);
    FREE_DARRAY(storage.statements);
    FREE_DARRAY(storage.expressions);
    FREE_DARRAY(storage.strings);
    
    storage.names.clear();
    storage.name_indices.clear();
    storage.evaluated_dictionaries.clear();
    storage.tables.clear();
    storage.evaluated_tables.clear();
}

// MAKERS:

Expression makeNumber(CodeRange code, Number expression) {
    static_assert(sizeof(Number) == sizeof(size_t), "");
    auto result = Expression{};
    result.type = NUMBER;
    result.range = code;
    memcpy(&result.index, &expression, sizeof(Number));
    return result;
}

Expression makeCharacter(CodeRange code, Character expression) {
    return Expression{CHARACTER, static_cast<size_t>(expression), code};
}

Expression makeDynamicExpression(CodeRange code, DynamicExpression expression) {
    return makeExpression(code, expression, DYNAMIC_EXPRESSION, storage.dynamic_expressions);
}

Expression makeTypedExpression(CodeRange code, TypedExpression expression) {
    return makeExpression(code, expression, TYPED_EXPRESSION, storage.typed_expressions);
}

Expression makeConditional(CodeRange code, Conditional expression) {
    return makeExpression(code, expression, CONDITIONAL, storage.conditionals);
}

Expression makeIs(CodeRange code, IsExpression expression) {
    return makeExpression(code, expression, IS, storage.is_expressions);
}

Expression makeAlternative(CodeRange code, Alternative expression) {
    return makeExpression(code, expression, ALTERNATIVE, storage.alternatives);
}

Expression makeDictionary(CodeRange code, Dictionary expression) {
    return makeExpression(code, expression, DICTIONARY, storage.dictionaries);
}

Expression makeEvaluatedDictionary(CodeRange code, EvaluatedDictionary expression) {
    storage.evaluated_dictionaries.emplace_back(std::move(expression));
    return Expression{EVALUATED_DICTIONARY, storage.evaluated_dictionaries.size() - 1, code};
}

Expression makeFunction(CodeRange code, Function expression) {
    return makeExpression(code, expression, FUNCTION, storage.functions);
}

Expression makeFunctionBuiltIn(CodeRange code, FunctionBuiltIn expression) {
    return makeExpression(code, expression, FUNCTION_BUILT_IN, storage.built_in_functions);
}

Expression makeFunctionDictionary(CodeRange code, FunctionDictionary expression) {
    return makeExpression(code, expression, FUNCTION_DICTIONARY, storage.dictionary_functions);
}

Expression makeFunctionTuple(CodeRange code, FunctionTuple expression) {
    return makeExpression(code, expression, FUNCTION_TUPLE, storage.tuple_functions);
}

Expression makeTuple(CodeRange code, Tuple expression) {
    return makeExpression(code, expression, TUPLE, storage.tuples);
}

Expression makeEvaluatedTuple(CodeRange code, EvaluatedTuple expression) {
    return makeExpression(code, expression, EVALUATED_TUPLE, storage.evaluated_tuples);
}

Expression makeEvaluatedTuple2(Expression a, Expression b) {
    const auto first = storage.expressions.count;
    APPEND(storage.expressions, a);
    APPEND(storage.expressions, b);
    const auto last = storage.expressions.count;
    return makeEvaluatedTuple(CodeRange{}, EvaluatedTuple{first, last});
}

Expression makeStack(CodeRange code, Stack expression) {
    return makeExpression(code, expression, STACK, storage.stacks);
}

Expression makeEvaluatedStack(CodeRange code, EvaluatedStack expression) {
    return makeExpression(code, expression, EVALUATED_STACK, storage.evaluated_stacks);
}

Expression makeTable(CodeRange code, Table expression) {
    storage.tables.emplace_back(std::move(expression));
    return Expression{TABLE, storage.tables.size() - 1, code};
}

Expression makeEvaluatedTable(CodeRange code, EvaluatedTable expression) {
    storage.evaluated_tables.emplace_back(std::move(expression));
    return Expression{EVALUATED_TABLE, storage.evaluated_tables.size() - 1, code};
}

Expression makeEvaluatedTableView(CodeRange code, EvaluatedTableView expression) {
    return makeExpression(code, expression, EVALUATED_TABLE_VIEW, storage.evaluated_table_views);
}

Expression makeLookupChild(CodeRange code, LookupChild expression) {
    return makeExpression(code, expression, LOOKUP_CHILD, storage.child_lookups);
}

Expression makeFunctionApplication(CodeRange code, FunctionApplication expression) {
    return makeExpression(code, expression, FUNCTION_APPLICATION, storage.function_applications);
}

Expression makeLookupSymbol(CodeRange code, LookupSymbol expression) {
    return makeExpression(code, expression, LOOKUP_SYMBOL, storage.symbol_lookups);
}

size_t findNameIndex(Name name) {
    const auto name_index = storage.name_indices.find(name);
    return name_index == storage.name_indices.end() ? SIZE_MAX : name_index->second;
}

Expression makeName(CodeRange code, Name expression) {
    const auto name_index = findNameIndex(expression);
    if (name_index == SIZE_MAX) {
        storage.name_indices[expression] = storage.names.size();
        storage.names.emplace_back(std::string(expression.data(), expression.size()));
        return Expression{NAME, storage.names.size() - 1, code};
    }
    else {
        return Expression{NAME, name_index, code};
    }
}

Expression makeArgument(CodeRange code, Argument expression) {
    return makeExpression(code, expression, ARGUMENT, storage.arguments);
}

Expression makeDefinition(CodeRange code, Definition expression) {
    return makeExpression(code, expression, DEFINITION, storage.definitions);
}

Expression makePutAssignment(CodeRange code, PutAssignment expression) {
    return makeExpression(code, expression, PUT_ASSIGNMENT, storage.put_assignments);
}

Expression makePutEachAssignment(CodeRange code, PutEachAssignment expression) {
    return makeExpression(code, expression, PUT_EACH_ASSIGNMENT, storage.put_each_assignments);
}

Expression makeDropAssignment(CodeRange code, DropAssignment expression) {
    return makeExpression(code, expression, DROP_ASSIGNMENT, storage.drop_assignments);
}

Expression makeWhileStatement(CodeRange code, WhileStatement expression) {
    return makeExpression(code, expression, WHILE_STATEMENT, storage.while_statements);
}

Expression makeForStatement(CodeRange code, ForStatement expression) {
    return makeExpression(code, expression, FOR_STATEMENT, storage.for_statements);
}

Expression makeForSimpleStatement(CodeRange code, ForSimpleStatement expression) {
    return makeExpression(code, expression, FOR_SIMPLE_STATEMENT, storage.for_simple_statements);
}

Expression makeWhileEndStatement(CodeRange code, WhileEndStatement expression) {
    return makeExpression(code, expression, WHILE_END_STATEMENT, storage.while_end_statements);
}

Expression makeForEndStatement(CodeRange code, ForEndStatement expression) {
    return makeExpression(code, expression, FOR_END_STATEMENT, storage.for_end_statements);
}

Expression makeForSimpleEndStatement(CodeRange code, ForSimpleEndStatement expression) {
    return makeExpression(code, expression, FOR_SIMPLE_END_STATEMENT, storage.for_simple_end_statements);
}

Expression makeString(CodeRange code, String expression) {
    return makeExpression(code, expression, STRING, storage.strings);
}

// GETTERS

Character getCharacter(Expression expression) {
    if (expression.index > 127) {
        throwException(
            "I found an error while retrieving a character.\n"
            "A character should have an ASCII value in the range 0-127.\n"
            "But I found one with the ASCII value %zu.",
            expression.index
        );
    }
    return static_cast<Character>(expression.index);
}

Number getNumber(Expression expression) {
    static_assert(sizeof(Number) == sizeof(size_t), "");
    Number result;
    memcpy(&result, &expression.index, sizeof(Number));
    return result;
}

CodeRange makeCodeCharacters(const char* s) {
    auto string = makeStaticString(s);
    auto result = CodeRange{};
    INIT_RANGE(result, string.count);

    auto column = size_t{0};
    auto row = size_t{0};
    auto index = size_t{0};
    FOR_EACH(character, string) {
        result.data[index] = CodeCharacter{*character, row, column};
        ++index;
        ++column;
        if (*character == '\n') {
            ++row;
            column = 0;
        }
    }
    return result;
}

char firstCharacter(CodeRange code) {
    return FIRST_ITEM(code).character;
}

size_t firstColumn(CodeRange code) {
    return FIRST_ITEM(code).column + 1;
}

size_t firstRow(CodeRange code) {
    return FIRST_ITEM(code).row + 1;
}

char lastCharacter(CodeRange code) {
    return LAST_ITEM(code).character;
}

size_t lastColumn(CodeRange code) {
    return LAST_ITEM(code).column + 1;
}

size_t lastRow(CodeRange code) {
    return LAST_ITEM(code).row + 1;
}
