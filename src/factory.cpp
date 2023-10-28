#include "factory.h"

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "operations/serialize.h"

Storage storage;

namespace {

template<typename ElementType, typename ArrayType>
Expression makeExpression(
    CodeRange code,
    ElementType expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(expression);
    storage.expressions.push_back(Expression{type, array.size() - 1, code});
    return storage.expressions.back();
}

template<typename ArrayType>
typename ArrayType::value_type& getMutableExpressionReference(
    Expression expression,
    ExpressionType type,
    ArrayType& array
) {
    if (expression.type != type) {
        throw std::runtime_error{
            "getMutableExpressionReference expected " + NAMES[type]
            + " got " + NAMES[expression.type]
        };
    }
    assert(expression.type == type);
    return array.at(expression.index);
}

template<typename ArrayType>
const typename ArrayType::value_type& getImmutableExpressionReference(
    Expression expression,
    ExpressionType type,
    ArrayType& array
) {
    if (expression.type != type) {
        throw std::runtime_error{
            "getMutableExpressionReference expected " + NAMES[type]
                + " got " + NAMES[expression.type]
        };
    }
    assert(expression.type == type);
    return array.at(expression.index);
}

template<typename ArrayType>
typename ArrayType::value_type getExpression(
    Expression expression,
    ExpressionType type,
    const ArrayType& array
) {
    if (expression.type != type) {
        throw std::runtime_error{
            "getExpression expected " + NAMES[type]
            + " got " + NAMES[expression.type]
        };
    }
    assert(expression.type == type);
    return array.at(expression.index);
}

} // namespace

BinaryTuple getBinaryTuple(Expression in) {
    const auto tuple = getEvaluatedTuple(in);
    const auto left = tuple.expressions.at(0);
    const auto right = tuple.expressions.at(1);
    return BinaryTuple{left, right};
}

void clearMemory() {
    storage = Storage{};
}

std::string getLog() {
    std::string log;
    for (const auto expression : storage.expressions) {
        serialize(log, expression);
        log.append("\n");
    }
    return log;
}

// MAKERS:

Expression makeNumber(CodeRange code, Number expression) {
    return makeExpression(code, expression, NUMBER, storage.numbers);
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
        storage.expressions.push_back(Expression{NAME, name_index->second, code});
        return storage.expressions.back();
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

Expression makeString(CodeRange code, String expression) {
    return makeExpression(code, expression, STRING, storage.strings);
}

// GETTERS

WhileStatement& getMutableWhileStatement(Expression expression) {
    return getMutableExpressionReference(expression, WHILE_STATEMENT, storage.while_statements);
}

ForSimpleStatement getForSimpleStatement(Expression expression) {
    return getExpression(expression, FOR_SIMPLE_STATEMENT, storage.for_simple_statements);
}

ForStatement& getMutableForStatement(Expression expression) {
    return getMutableExpressionReference(expression, FOR_STATEMENT, storage.for_statements);
}

ForSimpleStatement& getMutableForSimpleStatement(Expression expression) {
    return getMutableExpressionReference(expression, FOR_SIMPLE_STATEMENT, storage.for_simple_statements);
}

WhileEndStatement getWhileEndStatement(Expression expression) {
    return getExpression(expression, WHILE_END_STATEMENT, storage.while_end_statements);
}

ForEndStatement getForEndStatement(Expression expression) {
    return getExpression(expression, FOR_END_STATEMENT, storage.for_end_statements);
}

Number getNumber(Expression expression) {
    return getExpression(expression, NUMBER, storage.numbers);
}

Character getCharacter(Expression expression) {
    if (expression.type != CHARACTER) {
        throw std::runtime_error{
            "getExpression expected " + NAMES[CHARACTER]
                + " got " + NAMES[expression.type]
        };
    }
    return static_cast<Character>(expression.index);
}

Conditional getConditional(Expression expression) {
    return getExpression(expression, CONDITIONAL, storage.conditionals);
}

IsExpression getIs(Expression expression) {
    return getExpression(expression, IS, storage.is_expressions);
}

Alternative getAlternative(Expression expression) {
    return getExpression(expression, ALTERNATIVE, storage.alternatives);
}

Dictionary getDictionary(Expression expression) {
    return getExpression(expression, DICTIONARY, storage.dictionaries);
}

EvaluatedDictionary getEvaluatedDictionary(Expression expression) {
    return getExpression(expression, EVALUATED_DICTIONARY, storage.evaluated_dictionaries);
}

Table getTable(Expression expression) {
    return getExpression(expression, TABLE, storage.tables);
}

const EvaluatedTable& getEvaluatedTable(Expression expression) {
    return getImmutableExpressionReference(expression, EVALUATED_TABLE, storage.evaluated_tables);
}

EvaluatedTable& getMutableEvaluatedTable(Expression expression) {
    return getMutableExpressionReference(expression, EVALUATED_TABLE, storage.evaluated_tables);
}

EvaluatedTableView getEvaluatedTableView(Expression expression) {
    return getExpression(expression, EVALUATED_TABLE_VIEW, storage.evaluated_table_views);
}

Tuple getTuple(Expression expression) {
    return getExpression(expression, TUPLE, storage.tuples);
}

EvaluatedTuple getEvaluatedTuple(Expression expression) {
    return getExpression(expression, EVALUATED_TUPLE, storage.evaluated_tuples);
}

Function getFunction(Expression expression) {
    return getExpression(expression, FUNCTION, storage.functions);
}

FunctionBuiltIn getFunctionBuiltIn(Expression expression) {
    return getExpression(expression, FUNCTION_BUILT_IN, storage.built_in_functions);
}

FunctionDictionary getFunctionDictionary(Expression expression) {
    return getExpression(expression, FUNCTION_DICTIONARY, storage.dictionary_functions);
}

FunctionTuple getFunctionTuple(Expression expression) {
    return getExpression(expression, FUNCTION_TUPLE, storage.tuple_functions);
}

Stack getStack(Expression expression) {
    return getExpression(expression, STACK, storage.stacks);
}

EvaluatedStack getEvaluatedStack(Expression expression) {
    return getExpression(expression, EVALUATED_STACK, storage.evaluated_stacks);
}

LookupChild getLookupChild(Expression expression) {
    return getExpression(expression, LOOKUP_CHILD, storage.child_lookups);
}

FunctionApplication getFunctionApplication(Expression expression) {
    return getExpression(expression, FUNCTION_APPLICATION,
        storage.function_applications);
}

LookupSymbol getLookupSymbol(Expression expression) {
    return getExpression(expression, LOOKUP_SYMBOL, storage.symbol_lookups);
}

Name getName(Expression expression) {
    return getExpression(expression, NAME, storage.names);
}

Argument getArgument(Expression expression) {
    return getExpression(expression, ARGUMENT, storage.arguments);
}

String getString(Expression expression) {
    return getExpression(expression, STRING, storage.strings);
}

void setDictionaryDefinition(
    Expression evaluated_dictionary, size_t name_index, Expression value
) {
    if (evaluated_dictionary.type != EVALUATED_DICTIONARY) {
        throw std::runtime_error{
            "setDictionaryDefinition expected " + NAMES[EVALUATED_DICTIONARY]
            + " got " + NAMES[evaluated_dictionary.type]
        };
    }
    storage.evaluated_dictionaries.at(evaluated_dictionary.index).definitions[name_index].expression = value;
}

Expression getDictionaryDefinition(
    Expression evaluated_dictionary, size_t name_index
) {
    if (evaluated_dictionary.type != EVALUATED_DICTIONARY) {
        throw std::runtime_error{
            "getDictionaryDefinition expected " + NAMES[EVALUATED_DICTIONARY]
            + " got " + NAMES[evaluated_dictionary.type]
        };
    }
    return storage.evaluated_dictionaries.at(evaluated_dictionary.index).definitions.at(name_index).expression;
}
