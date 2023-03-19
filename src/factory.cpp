#include "factory.h"

#include <cassert>
#include <vector>

#include "operations/serialize.h"

std::vector<DynamicExpression> dynamic_expressions;
std::vector<EvaluatedDictionary> evaluated_dictionaries;
std::vector<Dictionary> dictionaries;
std::vector<Character> characters;
std::vector<Conditional> conditionals;
std::vector<IsExpression> is_expressions;
std::vector<Function> functions;
std::vector<FunctionBuiltIn> built_in_functions;
std::vector<FunctionDictionary> dictionary_functions;
std::vector<FunctionTuple> tuple_functions;
std::vector<Tuple> tuples;
std::vector<EvaluatedTuple> evaluated_tuples;
std::vector<Stack> stacks;
std::vector<EvaluatedStack> evaluated_stacks;
std::vector<EmptyStack> empty_stacks;
std::vector<Table> tables;
std::vector<EvaluatedTable> evaluated_tables;
std::vector<EvaluatedTableView> evaluated_table_views;
std::vector<LookupChild> child_lookups;
std::vector<FunctionApplication> function_applications;
std::vector<LookupSymbol> symbol_lookups;
std::vector<Name> names;
std::vector<Number> numbers;
std::vector<WhileStatement> while_statements;
std::vector<ForStatement> for_statements;
std::vector<WhileEndStatement> while_end_statements;
std::vector<ForEndStatement> for_end_statements;
std::vector<Definition> definitions;
std::vector<PutAssignment> put_assignments;
std::vector<String> strings;
std::vector<EmptyString> empty_strings;
std::vector<Boolean> booleans;
std::vector<Expression> expressions;

namespace {

template<typename ElementType, typename ArrayType>
Expression makeExpression(
    CodeRange code,
    ElementType expression,
    ExpressionType type,
    ArrayType& array
) {
    array.emplace_back(expression);
    expressions.push_back(Expression{type, array.size() - 1, code});
    return expressions.back();
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
    evaluated_dictionaries.clear();
    dictionaries.clear();
    characters.clear();
    conditionals.clear();
    is_expressions.clear();
    functions.clear();
    built_in_functions.clear();
    dictionary_functions.clear();
    tuple_functions.clear();
    tuples.clear();
    evaluated_tuples.clear();
    stacks.clear();
    evaluated_stacks.clear();
    empty_stacks.clear();
    tables.clear();
    evaluated_tables.clear();
    evaluated_table_views.clear();
    child_lookups.clear();
    function_applications.clear();
    symbol_lookups.clear();
    names.clear();
    numbers.clear();
    while_statements.clear();
    for_statements.clear();
    while_end_statements.clear();
    for_end_statements.clear();
    definitions.clear();
    put_assignments.clear();
    strings.clear();
    empty_strings.clear();
    booleans.clear();
    expressions.clear();
}

std::string getLog() {
    auto log = std::string{};
    for (const auto expression : expressions) {
        log += serialize(expression) + '\n';
    }
    return log;
}

// MAKERS:

Expression makeNumber(CodeRange code, Number expression) {
    return makeExpression(code, expression, NUMBER, numbers);
}

Expression makeCharacter(CodeRange code, Character expression) {
    return makeExpression(code, expression, CHARACTER, characters);
}

Expression makeDynamicExpression(CodeRange code, DynamicExpression expression) {
    return makeExpression(code, expression, DYNAMIC_EXPRESSION, dynamic_expressions);
}

Expression makeConditional(CodeRange code, Conditional expression) {
    return makeExpression(code, expression, CONDITIONAL, conditionals);
}

Expression makeIs(CodeRange code, IsExpression expression) {
    return makeExpression(code, expression, IS, is_expressions);
}

Expression makeDictionary(CodeRange code, Dictionary expression) {
    return makeExpression(code, expression, DICTIONARY, dictionaries);
}

Expression makeEvaluatedDictionary(CodeRange code, EvaluatedDictionary expression) {
    return makeExpression(code, expression, EVALUATED_DICTIONARY, evaluated_dictionaries);
}

Expression makeFunction(CodeRange code, Function expression) {
    return makeExpression(code, expression, FUNCTION, functions);
}

Expression makeFunctionBuiltIn(CodeRange code, FunctionBuiltIn expression) {
    return makeExpression(code, expression, FUNCTION_BUILT_IN, built_in_functions);
}

Expression makeFunctionDictionary(CodeRange code, FunctionDictionary expression) {
    return makeExpression(code, expression, FUNCTION_DICTIONARY, dictionary_functions);
}

Expression makeFunctionTuple(CodeRange code, FunctionTuple expression) {
    return makeExpression(code, expression, FUNCTION_TUPLE, tuple_functions);
}

Expression makeTuple(CodeRange code, Tuple expression) {
    return makeExpression(code, expression, TUPLE, tuples);
}

Expression makeEvaluatedTuple(CodeRange code, EvaluatedTuple expression) {
    return makeExpression(code, expression, EVALUATED_TUPLE, evaluated_tuples);
}

Expression makeStack(CodeRange code, Stack expression) {
    return makeExpression(code, expression, STACK, stacks);
}

Expression makeEvaluatedStack(CodeRange code, EvaluatedStack expression) {
    return makeExpression(code, expression, EVALUATED_STACK, evaluated_stacks);
}

Expression makeEmptyStack(CodeRange code, EmptyStack expression) {
    return makeExpression(code, expression, EMPTY_STACK, empty_stacks);
}

Expression makeTable(CodeRange code, Table expression) {
    return makeExpression(code, expression, TABLE, tables);
}

Expression makeEvaluatedTable(CodeRange code, EvaluatedTable expression) {
    return makeExpression(code, expression, EVALUATED_TABLE, evaluated_tables);
}

Expression makeEvaluatedTableView(CodeRange code, EvaluatedTableView expression) {
    return makeExpression(code, expression, EVALUATED_TABLE_VIEW, evaluated_table_views);
}

Expression makeLookupChild(CodeRange code, LookupChild expression) {
    return makeExpression(code, expression, LOOKUP_CHILD, child_lookups);
}

Expression makeFunctionApplication(CodeRange code, FunctionApplication expression) {
    return makeExpression(code, expression, FUNCTION_APPLICATION, function_applications);
}

Expression makeLookupSymbol(CodeRange code, LookupSymbol expression) {
    return makeExpression(code, expression, LOOKUP_SYMBOL, symbol_lookups);
}

Expression makeName(CodeRange code, Name expression) {
    return makeExpression(code, expression, NAME, names);
}

Expression makeDefinition(CodeRange code, Definition expression) {
    return makeExpression(code, expression, DEFINITION, definitions);
}

Expression makePutAssignment(CodeRange code, PutAssignment expression) {
    return makeExpression(code, expression, PUT_ASSIGNMENT, put_assignments);
}

Expression makeWhileStatement(CodeRange code, WhileStatement expression) {
    return makeExpression(code, expression, WHILE_STATEMENT, while_statements);
}

Expression makeForStatement(CodeRange code, ForStatement expression) {
    return makeExpression(code, expression, FOR_STATEMENT, for_statements);
}

Expression makeWhileEndStatement(CodeRange code, WhileEndStatement expression) {
    return makeExpression(code, expression, WHILE_END_STATEMENT, while_end_statements);
}

Expression makeForEndStatement(CodeRange code, ForEndStatement expression) {
    return makeExpression(code, expression, FOR_END_STATEMENT, for_end_statements);
}

Expression makeString(CodeRange code, String expression) {
    return makeExpression(code, expression, STRING, strings);
}

Expression makeEmptyString(CodeRange code, EmptyString expression) {
    return makeExpression(code, expression, EMPTY_STRING, empty_strings);
}

Expression makeBoolean(CodeRange code, Boolean expression) {
    return makeExpression(code, expression, BOOLEAN, booleans);
}

// GETTERS

DynamicExpression getDynamicExpression(Expression expression) {
    return getExpression(expression, DYNAMIC_EXPRESSION, dynamic_expressions);
}

Definition getDefinition(Expression expression) {
    return getExpression(expression, DEFINITION, definitions);
}

PutAssignment getPutAssignment(Expression expression) {
    return getExpression(expression, PUT_ASSIGNMENT, put_assignments);
}

WhileStatement getWhileStatement(Expression expression) {
    return getExpression(expression, WHILE_STATEMENT, while_statements);
}

WhileStatement& getMutableWhileStatement(Expression expression) {
    return getMutableExpressionReference(expression, WHILE_STATEMENT, while_statements);
}

ForStatement getForStatement(Expression expression) {
    return getExpression(expression, FOR_STATEMENT, for_statements);
}

ForStatement& getMutableForStatement(Expression expression) {
    return getMutableExpressionReference(expression, FOR_STATEMENT, for_statements);
}

WhileEndStatement getWhileEndStatement(Expression expression) {
    return getExpression(expression, WHILE_END_STATEMENT, while_end_statements);
}

ForEndStatement getForEndStatement(Expression expression) {
    return getExpression(expression, FOR_END_STATEMENT, for_end_statements);
}

Number getNumber(Expression expression) {
    return getExpression(expression, NUMBER, numbers);
}

Character getCharacter(Expression expression) {
    return getExpression(expression, CHARACTER, characters);
}

Conditional getConditional(Expression expression) {
    return getExpression(expression, CONDITIONAL, conditionals);
}

IsExpression getIs(Expression expression) {
    return getExpression(expression, IS, is_expressions);
}

Dictionary getDictionary(Expression expression) {
    return getExpression(expression, DICTIONARY, dictionaries);
}

EvaluatedDictionary getEvaluatedDictionary(Expression expression) {
    return getExpression(expression, EVALUATED_DICTIONARY, evaluated_dictionaries);
}

EvaluatedDictionary& getMutableEvaluatedDictionary(Expression expression) {
    return getMutableExpressionReference(expression, EVALUATED_DICTIONARY,
        evaluated_dictionaries);
}

Table getTable(Expression expression) {
    return getExpression(expression, TABLE, tables);
}

const EvaluatedTable& getEvaluatedTable(Expression expression) {
    return getImmutableExpressionReference(expression, EVALUATED_TABLE, evaluated_tables);
}

EvaluatedTable& getMutableEvaluatedTable(Expression expression) {
    return getMutableExpressionReference(expression, EVALUATED_TABLE, evaluated_tables);
}

EvaluatedTableView getEvaluatedTableView(Expression expression) {
    return getExpression(expression, EVALUATED_TABLE_VIEW, evaluated_table_views);
}

Tuple getTuple(Expression expression) {
    return getExpression(expression, TUPLE, tuples);
}

EvaluatedTuple getEvaluatedTuple(Expression expression) {
    return getExpression(expression, EVALUATED_TUPLE, evaluated_tuples);
}

Function getFunction(Expression expression) {
    return getExpression(expression, FUNCTION, functions);
}

FunctionBuiltIn getFunctionBuiltIn(Expression expression) {
    return getExpression(expression, FUNCTION_BUILT_IN, built_in_functions);
}

FunctionDictionary getFunctionDictionary(Expression expression) {
    return getExpression(expression, FUNCTION_DICTIONARY, dictionary_functions);
}

FunctionTuple getFunctionTuple(Expression expression) {
    return getExpression(expression, FUNCTION_TUPLE, tuple_functions);
}

Stack getStack(Expression expression) {
    return getExpression(expression, STACK, stacks);
}

EvaluatedStack getEvaluatedStack(Expression expression) {
    return getExpression(expression, EVALUATED_STACK, evaluated_stacks);
}

EmptyStack getEmptyStack(Expression expression) {
    return getExpression(expression, EMPTY_STACK, empty_stacks);
}

LookupChild getLookupChild(Expression expression) {
    return getExpression(expression, LOOKUP_CHILD, child_lookups);
}

FunctionApplication getFunctionApplication(Expression expression) {
    return getExpression(expression, FUNCTION_APPLICATION,
        function_applications);
}

LookupSymbol getLookupSymbol(Expression expression) {
    return getExpression(expression, LOOKUP_SYMBOL, symbol_lookups);
}

Name getName(Expression expression) {
    return getExpression(expression, NAME, names);
}

String getString(Expression expression) {
    return getExpression(expression, STRING, strings);
}

EmptyString getEmptyString(Expression expression) {
    return getExpression(expression, EMPTY_STRING, empty_strings);
}

Boolean getBoolean(Expression expression) {
    return getExpression(expression, BOOLEAN, booleans);
}
