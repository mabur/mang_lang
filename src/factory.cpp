#include "factory.h"

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "passes/serialize.h"

Storage storage;

namespace {

template<typename ElementType, typename ArrayType>
Expression makeExpression(
    CodeRange code,
    ElementType&& expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(std::move(expression));
    return Expression{type, array.size() - 1, code};
}

} // namespace

void clearMemory() {
    storage = Storage{};
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
    return makeExpression(code, std::move(expression), DICTIONARY, storage.dictionaries);
}

Expression makeEvaluatedDictionary(CodeRange code, EvaluatedDictionary expression) {
    return makeExpression(code, expression, EVALUATED_DICTIONARY, storage.evaluated_dictionaries);
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
    const auto first = storage.expressions.size();
    storage.expressions.push_back(a);
    storage.expressions.push_back(b);
    const auto last = storage.expressions.size();
    return makeEvaluatedTuple({}, EvaluatedTuple{first, last});
}

Expression makeStack(CodeRange code, Stack expression) {
    return makeExpression(code, expression, STACK, storage.stacks);
}

Expression makeEvaluatedStack(CodeRange code, EvaluatedStack expression) {
    return makeExpression(code, expression, EVALUATED_STACK, storage.evaluated_stacks);
}

Expression makeTable(CodeRange code, Table expression) {
    return makeExpression(code, expression, TABLE, storage.tables);
}

Expression makeEvaluatedTable(CodeRange code, EvaluatedTable expression) {
    return makeExpression(code, expression, EVALUATED_TABLE, storage.evaluated_tables);
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

Expression makeName(CodeRange code, Name expression) {
    const auto name_index = storage.name_indices.find(expression);
    if (name_index == storage.name_indices.end()) {
        storage.name_indices[expression] = storage.names.size();
        return makeExpression(code, expression, NAME, storage.names);
    }
    else {
        return Expression{NAME, name_index->second, code};
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
        throw std::runtime_error{
            "I found an error while retrieving a character. "
            "A character should have an ASCII value in the range 0-127. "
            "But I found one with the ASCII value " + std::to_string(expression.index)
        };
    }
    return static_cast<Character>(expression.index);
}

Number getNumber(Expression expression) {
    static_assert(sizeof(Number) == sizeof(size_t), "");
    Number result;
    memcpy(&result, &expression.index, sizeof(Number));
    return result;
}
