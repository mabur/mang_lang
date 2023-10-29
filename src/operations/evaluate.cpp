#include "evaluate.h"

#include <cassert>
#include <iostream>
#include <string.h>

#include "../built_in_functions/container.h"
#include "../factory.h"
#include "serialize.h"

namespace {

template<typename Predicate, typename Getter>
bool allOfPairs(Expression left, Expression right, Predicate predicate, int empty_type, Getter getter) {
    while (left.type != empty_type && right.type != empty_type) {
        const auto left_container = getter(left);
        const auto right_container = getter(right);
        if (!predicate(left_container.top, right_container.top)) {
            return false;
        }
        left = left_container.rest;
        right = right_container.rest;
    }
    return left.type == empty_type && right.type == empty_type;
}

void checkTypes(Expression super, Expression sub, const std::string& description);

void checkTypesEvaluatedStack(Expression super, Expression sub, const std::string& description) {
    const auto stack_super = storage.evaluated_stacks.at(super.index).top;
    const auto stack_sub = storage.evaluated_stacks.at(sub.index).top;
    checkTypes(stack_super, stack_sub, description);
}

void checkTypesEvaluatedTable(Expression super, Expression sub, const std::string& description) {
    const auto& table_super = storage.evaluated_tables.at(super.index);
    const auto& table_sub = storage.evaluated_tables.at(sub.index);
    if (table_super.empty()) return;
    if (table_sub.empty()) return;
    const auto row_super = table_super.begin()->second;
    const auto row_sub = table_sub.begin()->second;
    checkTypes(row_super.key, row_sub.key, description);
    checkTypes(row_super.value, row_sub.value, description);
}

void checkTypesEvaluatedTuple(Expression super, Expression sub, const std::string& description) {
    const auto& tuple_super = storage.evaluated_tuples.at(super.index);
    const auto& tuple_sub = storage.evaluated_tuples.at(sub.index);
    if (tuple_super.expressions.size() != tuple_sub.expressions.size()) {
        throw std::runtime_error(
            "Static type error in " + description + ". Inconsistent tuple size."
        );
    }
    const auto count = tuple_super.expressions.size();
    for (size_t i = 0; i < count; ++i) {
        checkTypes(tuple_super.expressions.at(i), tuple_sub.expressions.at(i), description);
    }
}

void checkTypesEvaluatedDictionary(Expression super, Expression sub, const std::string& description) {
    const auto& dictionary_super = storage.evaluated_dictionaries.at(super.index);
    const auto& dictionary_sub = storage.evaluated_dictionaries.at(sub.index);
    for (const auto& definition_super : dictionary_super.definitions) {
        const auto name_super = definition_super.name;
        const auto value_sub = dictionary_sub.optionalLookup(name_super);
        if (value_sub) {
            checkTypes(definition_super.expression, *value_sub, description);
        }
        else {
            throw std::runtime_error(
                "Static type error in " + description +
                    ". Could not find name " + getName(name_super) +
                    " in dictionary" + describeLocation(sub.range) 
            );            
        }
    }
}

void checkTypes(Expression super, Expression sub, const std::string& description) {
    if (super.type == ANY || sub.type == ANY) return;
    
    if (super.type == NUMBER && sub.type == NUMBER) return;
    if (super.type == CHARACTER && sub.type == CHARACTER) return;
    
    if (super.type == NO && sub.type == NO) return;
    if (super.type == YES && sub.type == YES) return;
    if (super.type == YES && sub.type == NO) return;
    if (super.type == NO && sub.type == YES) return;

    if (super.type == FUNCTION && sub.type == FUNCTION) return;
    if (super.type == FUNCTION && sub.type == FUNCTION_DICTIONARY) return;
    if (super.type == FUNCTION && sub.type == FUNCTION_TUPLE) return;
    if (super.type == FUNCTION && sub.type == FUNCTION_BUILT_IN) return;
    if (super.type == FUNCTION_DICTIONARY && sub.type == FUNCTION) return;
    if (super.type == FUNCTION_TUPLE && sub.type == FUNCTION) return;
    if (super.type == FUNCTION_BUILT_IN && sub.type == FUNCTION) return;
    
    if (super.type == EMPTY_STRING && sub.type == EMPTY_STRING) return;
    if (super.type == EMPTY_STRING && sub.type == STRING) return;
    if (super.type == STRING && sub.type == EMPTY_STRING) return;
    if (super.type == STRING && sub.type == STRING) return;
    
    if (super.type == EMPTY_STACK && sub.type == EMPTY_STACK) return;
    if (super.type == EMPTY_STACK && sub.type == EVALUATED_STACK) return;
    if (super.type == EVALUATED_STACK && sub.type == EMPTY_STACK) return;
    if (super.type == EVALUATED_STACK && sub.type == EVALUATED_STACK) {
        checkTypesEvaluatedStack(super, sub, description);
        return;
    }
    if (super.type == EVALUATED_TABLE && sub.type == EVALUATED_TABLE) {
        checkTypesEvaluatedTable(super, sub, description);
        return;
    }
    if (super.type == EVALUATED_TUPLE && sub.type == EVALUATED_TUPLE) {
        checkTypesEvaluatedTuple(super, sub, description);
        return;
    }
    if (super.type == EVALUATED_DICTIONARY && sub.type == EVALUATED_DICTIONARY) {
        checkTypesEvaluatedDictionary(super, sub, description);
        return;
    }
    throw std::runtime_error(
        "Static type error in " + description +
        describeLocation(super.range) +
        ". " + NAMES[super.type] +
        " is not a supertype for " + NAMES[sub.type]
    );
}

template<typename Evaluator>
Expression evaluateStack(Evaluator evaluator,
    Expression stack, Expression environment
) {
    // Allocation:
    auto items = std::vector<Expression>{};
    while (stack.type != EMPTY_STACK) {
        if (stack.type != STACK) {
            throw std::runtime_error(
                std::string{"\n\nI have found a type error."} +
                "\nIt happens in evaluateStack. " +
                "\nInstead of a stack I got a " + NAMES[stack.type] +
                ".\n"
            );
        }
        const auto stack_struct = storage.stacks.at(stack.index);
        const auto& top = stack_struct.top;
        const auto& rest = stack_struct.rest;
        items.push_back(evaluator(top, environment));
        stack = rest;
    }
    std::reverse(items.begin(), items.end());
    auto evaluated_stack = Expression{EMPTY_STACK, 0, stack.range};
    for (const auto& item : items) {
        evaluated_stack = putEvaluatedStack(evaluated_stack, item);
    }
    return evaluated_stack;
}

template<typename Evaluator>
Expression evaluateTuple(
    Evaluator evaluator, Expression tuple, Expression environment
) {
    const auto tuple_struct = storage.tuples.at(tuple.index);
    auto evaluated_expressions = std::vector<Expression>{};
    evaluated_expressions.reserve(tuple_struct.expressions.size());
    for (const auto& expression : tuple_struct.expressions) {
        evaluated_expressions.push_back(evaluator(expression, environment));
    }
    const auto code = tuple.range;
    return makeEvaluatedTuple(code, EvaluatedTuple{evaluated_expressions});
}

template<typename Evaluator, typename Serializer>
Expression evaluateTable(
    Evaluator evaluator,
    Serializer serializer,
    Expression table,
    Expression environment
) {
    const auto table_struct = storage.tables.at(table.index);
    auto rows = std::map<std::string, Row>{};
    for (const auto& row : table_struct.rows) {
        const auto key = evaluator(row.key, environment);
        const auto value = evaluator(row.value, environment);
        std::string serialized_key;
        serializer(serialized_key, key);
        rows[serialized_key] = {key, value};
    }
    const auto code = table.range;
    return makeEvaluatedTable(code, EvaluatedTable{rows});
}

template<typename Evaluator>
Expression evaluateLookupChild(
    Evaluator evaluator, Expression lookup_child, Expression environment
) {
    const auto lookup_child_struct = storage.child_lookups.at(lookup_child.index);
    const auto child = evaluator(lookup_child_struct.child, environment);
    if (child.type != EVALUATED_DICTIONARY) {
        throw std::runtime_error(
            std::string{"\n\nI have found a type error."} +
                "\nIt happens when trying to lookup a child in a dictionary, " +
                "\nbut instead of a dictionary I got a" + NAMES[child.type] +
                ".\n"
        );
    }
    const auto dictionary = storage.evaluated_dictionaries.at(child.index);
    return dictionary.lookup(lookup_child_struct.name);
}

template<typename Evaluator>
void checkArgument(
    Evaluator evaluator, const Argument& a, Expression input, Expression environment
) {
    if (a.type.type == ANY) {
        return;
    }
    const auto type = evaluator(a.type, environment);
    checkTypes(input, type, "function call");
}

template<typename Evaluator>
Expression applyFunction(
    Evaluator evaluator,
    Expression function,
    Expression input
) {
    const auto function_struct = storage.functions.at(function.index);
    const auto argument = storage.arguments.at(function_struct.argument.index);
    checkArgument(evaluator, argument, input, function_struct.environment);
    const auto definitions = std::vector<Definition>{{argument.name, input, 0}};
    const auto middle = makeEvaluatedDictionary(input.range,
        EvaluatedDictionary{function_struct.environment, definitions}
    );
    return evaluator(function_struct.body, middle);
}

template<typename Evaluator>
Expression applyFunctionDictionary(
    Evaluator evaluator,
    Expression function,
    Expression input
) {
    if (input.type != EVALUATED_DICTIONARY) {
        throw std::runtime_error(
            std::string{"\n\nI have found a type error."} +
                "\nIt happens when calling a function that is expecting a dictionary as input. " +
                "\nBut now it got a" + NAMES[input.type] +
                ".\n"
        );
    }
    const auto function_struct = storage.dictionary_functions.at(function.index);
    const auto evaluated_dictionary = storage.evaluated_dictionaries.at(input.index);
    for (auto i = function_struct.first_argument.index; i < function_struct.last_argument.index; ++i) {
        const auto argument = storage.arguments.at(i);
        const auto expression = evaluated_dictionary.lookup(argument.name);
        checkArgument(evaluator, argument, expression, function_struct.environment);
    }
    // TODO: pass along environment? Is some use case missing now?
    return evaluator(function_struct.body, input);
}

template<typename Evaluator>
Expression applyFunctionTuple(
    Evaluator evaluator,
    Expression function,
    Expression input
) {
    if (input.type != EVALUATED_TUPLE) {
        throw std::runtime_error(
            std::string{"\n\nI have found a type error."} +
                "\nIt happens when trying to call a function that takes a tuple. " +
                "\nInstead of a tuple I got a" + NAMES[input.type] +
                ".\n"
        );
    }
    auto tuple = storage.evaluated_tuples.at(input.index);
    const auto function_struct = storage.tuple_functions.at(function.index);
    const auto first_argument = function_struct.first_argument;
    const auto last_argument = function_struct.last_argument;
    const auto num_inputs = last_argument.index - first_argument.index;
    
    if (num_inputs != tuple.expressions.size()) {
        throw std::runtime_error{"Wrong number of input to function_struct"};
    }

    auto argument_index = first_argument.index;
    auto definitions = std::vector<Definition>(num_inputs);
    for (size_t i = 0; i < num_inputs; ++i) {
        const auto argument = storage.arguments.at(argument_index + i);
        const auto expression = tuple.expressions.at(i);
        checkArgument(evaluator, argument, expression, function_struct.environment);
        definitions.at(i) = Definition{argument.name, expression, i};
    }
    const auto middle = makeEvaluatedDictionary(input.range,
        EvaluatedDictionary{function_struct.environment, definitions}
    );
    return evaluator(function_struct.body, middle);
}

Expression evaluateFunction(Expression function, Expression environment) {
    const auto function_struct = storage.functions.at(function.index);
    return makeFunction(function.range, {
        environment, function_struct.argument, function_struct.body
    });
}

Expression evaluateFunctionDictionary(
    Expression function_dictionary, Expression environment
) {
    const auto function_dictionary_struct = storage.dictionary_functions.at(function_dictionary.index);
    return makeFunctionDictionary(function_dictionary.range, {
        environment,
        function_dictionary_struct.first_argument,
        function_dictionary_struct.last_argument,
        function_dictionary_struct.body
    });
}

Expression evaluateFunctionTuple(
    Expression function_tuple, Expression environment
) {
    const auto function_tuple_struct = storage.tuple_functions.at(function_tuple.index);
    return makeFunctionTuple(function_tuple.range, {
        environment,
        function_tuple_struct.first_argument,
        function_tuple_struct.last_argument,
        function_tuple_struct.body
    });
}

Expression lookupDictionary(Expression name, Expression expression) {
    if (expression.type != EVALUATED_DICTIONARY) {
        throw MissingSymbol(getName(name), "environment of type " + NAMES[expression.type]);
    }
    const auto& dictionary = storage.evaluated_dictionaries.at(expression.index);
    const auto result = dictionary.optionalLookup(name);
    if (result) {
        return *result;
    }
    return lookupDictionary(name, dictionary.environment);
}

Expression applyFunctionBuiltIn(
    Expression function, Expression input
) {
    const auto function_struct = storage.built_in_functions.at(function.index);
    return function_struct.function(input);
}

void booleanTypes(Expression expression) {
    switch (expression.type) {
        case NUMBER: return;
        case YES: return;
        case NO: return;
        case EVALUATED_TABLE: return;
        case EVALUATED_STACK: return;
        case EMPTY_STACK: return;
        case STRING: return;
        case EMPTY_STRING: return;
        case ANY: return;
        default: throw std::runtime_error(
                std::string{"Static type error.\n"} +
                    "Cannot convert type " + NAMES[expression.type] + " to boolean."
            );
    }
}

bool boolean(Expression expression) {
    const auto type = expression.type;
    const auto index = expression.index;
    switch (type) {
        case EVALUATED_TABLE: return !storage.evaluated_tables.at(index).empty();
        case EVALUATED_TABLE_VIEW: return !storage.evaluated_table_views.at(index).empty();
        case NUMBER: return static_cast<bool>(storage.numbers.at(index));
        case YES: return true;
        case NO: return false;
        case EVALUATED_STACK: return true;
        case EMPTY_STACK: return false;
        case STRING: return true;
        case EMPTY_STRING: return false;
        default: throw UnexpectedExpression(type, "boolean operation");
    }
}

size_t getIndex(Number number) {
    if (number < 0) {
        using namespace std;
        throw runtime_error("Cannot have negative index: " + to_string(number));
    }
    return static_cast<size_t>(number);
}

Expression applyTupleIndexing(Expression tuple, Expression input) {
    const auto& tuple_struct = storage.evaluated_tuples.at(tuple.index);
    if (input.type != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a type error."} +
                "\nIt happens when indexing a tuple. " +
                "\nThe index is expected to be a " + NAMES[NUMBER] + "," +
                "\nbut now it is a" + NAMES[input.type] +
                ".\n"
        );
    }
    const auto number = storage.numbers.at(input.index);
    const auto i = getIndex(number);
    try {
        return tuple_struct.expressions.at(i);
    }
    catch (const std::out_of_range&) {
        throw std::runtime_error(
            "Tuple of size " + std::to_string(tuple_struct.expressions.size()) +
                " indexed with " + std::to_string(i)
        );
    }
}

Expression applyTableIndexingTypes(Expression table) {
    const auto& table_struct = storage.evaluated_tables.at(table.index);
    if (table_struct.rows.empty()) {
        return Expression{ANY, 0, table.range};
    }
    return table_struct.begin()->second.value;
}

Expression applyStackIndexingTypes(Expression stack) {
    return getEvaluatedStack(stack).top;
}

Expression applyStringIndexingTypes(Expression string) {
    return getString(string).top;
}

template<typename Vector, typename Predicate>
bool allOfVectors(const Vector& left, const Vector& right, Predicate predicate) {
    if (left.size() != right.size()) {
        return false;
    }
    for (size_t i = 0; i < left.size(); ++i) {
        if (!predicate(left.at(i), right.at(i))) {
            return false;
        }
    }
    return true;
}

bool isEqual(Expression left, Expression right) {
    const auto left_type = left.type;
    const auto right_type = right.type;
    if (left_type == NUMBER && right_type == NUMBER) {
        return storage.numbers.at(left.index) == storage.numbers.at(right.index);
    }
    if (left_type == CHARACTER && right_type == CHARACTER) {
        return getCharacter(left) == getCharacter(right);
    }
    if (left_type == YES && right_type == YES) {
        return true;
    }
    if (left_type == NO && right_type == NO) {
        return true;
    }
    if (left_type == EMPTY_STACK && right_type == EMPTY_STACK) {
        return true;
    }
    if (left_type == EVALUATED_STACK && right_type == EVALUATED_STACK) {
        return allOfPairs(left, right, isEqual, EMPTY_STACK, getEvaluatedStack);
    }
    if (left_type == EMPTY_STRING && right_type == EMPTY_STRING) {
        return true;
    }
    if (left_type == STRING && right_type == STRING) {
        return allOfPairs(left, right, isEqual, EMPTY_STRING, getString);
    }
    if (left_type == EVALUATED_TUPLE && right_type == EVALUATED_TUPLE) {
        return allOfVectors(
            storage.evaluated_tuples.at(left.index).expressions,
            storage.evaluated_tuples.at(right.index).expressions,
            isEqual
        );
    }
    return false;
}

Expression evaluateDynamicExpressionTyped(Expression expression) {
    return Expression{ANY, 0, expression.range};
}

Expression evaluateDynamicExpression(Expression expression, Expression environment) {
    const auto inner_expression = storage.dynamic_expressions.at(expression.index).expression;
    return evaluate(inner_expression, environment);
}

Expression evaluateConditionalTypes(
    Expression conditional, Expression environment
) {
    const auto conditional_struct = storage.conditionals.at(conditional.index);
    for (auto alternative = conditional_struct.alternative_first;
        alternative.index <= conditional_struct.alternative_last.index;
        ++alternative.index
    ) {
        evaluate_types(storage.alternatives.at(alternative.index).left, environment);
    }
    const auto else_expression = evaluate_types(conditional_struct.expression_else, environment);
    for (auto a = conditional_struct.alternative_first;
        a.index <= conditional_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.at(a.index);
        const auto alternative_expression = evaluate_types(
            alternative.right, environment
        );
        checkTypes(else_expression, alternative_expression, "if");
    }
    return else_expression;
}

Expression evaluateConditional(Expression conditional, Expression environment) {
    const auto conditional_struct = storage.conditionals.at(conditional.index);
    for (auto a = conditional_struct.alternative_first;
        a.index <= conditional_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.at(a.index);
        const auto left_value = evaluate(alternative.left, environment);
        if (boolean(left_value)) {
            return evaluate(alternative.right, environment);
        }
    }
    return evaluate(conditional_struct.expression_else, environment);
}

Expression evaluateIsTypes(
    Expression is, Expression environment
) {
    const auto is_struct = storage.is_expressions.at(is.index);
    evaluate_types(is_struct.input, environment);
    for (auto a = is_struct.alternative_first;
        a.index <= is_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.at(a.index);
        evaluate_types(alternative.left, environment);
    }
    const auto else_expression = evaluate_types(is_struct.expression_else, environment);
    for (auto a = is_struct.alternative_first;
        a.index <= is_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.at(a.index);
        const auto alternative_expression = evaluate_types(alternative.right, environment);
        checkTypes(else_expression, alternative_expression, "is");
    }
    return else_expression;
}

Expression evaluateIs(Expression is, Expression environment) {
    const auto is_struct = storage.is_expressions.at(is.index);
    const auto value = evaluate(is_struct.input, environment);
    for (auto a = is_struct.alternative_first;
        a.index <= is_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.at(a.index);
        const auto left_value = evaluate(alternative.left, environment);
        if (isEqual(value, left_value)) {
            return evaluate(alternative.right, environment);
        }
    }
    return evaluate(is_struct.expression_else, environment);
}

template<typename Evaluator>
Expression evaluateTypedExpression(
    Evaluator evaluator, Expression expression, Expression environment
) {
    const auto expression_struct = storage.typed_expressions.at(expression.index);
    const auto type = evaluator(expression_struct.type, environment);
    const auto value = evaluator(expression_struct.value, environment);
    checkTypes(type, value, "typed expression");
    return value;
}

std::vector<Definition> initializeDefinitions(const Dictionary& dictionary) {
    auto definitions = std::vector<Definition>(dictionary.definition_count);
    for (const auto& statement : dictionary.statements) {
        const auto type = statement.type;
        if (type == DEFINITION) {
            auto definition = storage.definitions.at(statement.index);
            definition.expression = Expression{ANY, 0, statement.range};
            definitions.at(definition.name_index) = definition;
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = storage.for_statements.at(statement.index);
            definitions.at(for_statement.name_index_item) = Definition{
                for_statement.name_item,
                Expression{ANY, 0, statement.range},
                for_statement.name_index_item
            };
        }
    }
    return definitions;
}

Expression evaluateDictionaryTypes(
    Expression dictionary, Expression environment
) {
    const auto dictionary_struct = storage.dictionaries.at(dictionary.index);
    const auto initial_definitions = initializeDefinitions(dictionary_struct);
    const auto result = makeEvaluatedDictionary(
        dictionary.range, EvaluatedDictionary{environment, initial_definitions}
    );
    for (const auto& statement : dictionary_struct.statements) {
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = storage.definitions.at(statement.index);
            const auto right_expression = definition.expression;
            const auto value = evaluate_types(right_expression, result);
            // TODO: is this a principled approach?
            if (value.type != ANY) {
                setDictionaryDefinition(result, definition.name_index, value);
            }
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = storage.put_assignments.at(statement.index);
            const auto right_expression = put_assignment.expression;
            const auto value = evaluate_types(right_expression, result);
            const auto current = getDictionaryDefinition(result, put_assignment.name_index);
            const auto tuple = makeEvaluatedTuple(
                {}, EvaluatedTuple{{value, current}}
            );
            const auto new_value = container_functions::putTyped(tuple);
            setDictionaryDefinition(result, put_assignment.name_index, new_value);
        }
        else if (type == PUT_EACH_ASSIGNMENT) {
            const auto put_each_assignment = storage.put_each_assignments.at(statement.index);
            const auto right_expression = put_each_assignment.expression;
            auto container = evaluate_types(right_expression, result);
            {
                const auto current = getDictionaryDefinition(result, put_each_assignment.name_index);
                const auto value = container_functions::takeTyped(container);
                const auto tuple = makeEvaluatedTuple(
                    {}, EvaluatedTuple{{value, current}}
                );
                const auto new_value = container_functions::putTyped(tuple);
                setDictionaryDefinition(result, put_each_assignment.name_index, new_value);
            }
        }
        else if (type == DROP_ASSIGNMENT) {
            const auto drop_assignment = storage.drop_assignments.at(statement.index);
            const auto current = getDictionaryDefinition(result, drop_assignment.name_index);
            const auto new_value = container_functions::dropTyped(current);
            setDictionaryDefinition(result, drop_assignment.name_index, new_value);
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = storage.while_statements.at(statement.index);
            booleanTypes(evaluate_types(while_statement.expression, result));
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = storage.for_statements.at(statement.index);
            const auto container = getDictionaryDefinition(result, for_statement.name_index_container);
            booleanTypes(container);
            const auto value = container_functions::takeTyped(container);
            setDictionaryDefinition(result, for_statement.name_index_item, value);
        }
        else if (type == FOR_SIMPLE_STATEMENT) {
            const auto for_statement = storage.for_simple_statements.at(statement.index);
            const auto container = getDictionaryDefinition(result, for_statement.name_index);
            booleanTypes(container);
        }
        else if (type == RETURN_STATEMENT) {
        }
    }
    return result;
}

size_t getContainerNameIndex(Expression expression) {
    switch (expression.type) {
        case FOR_STATEMENT: return storage.for_statements.at(expression.index).name_index_container;
        case FOR_SIMPLE_STATEMENT: return storage.for_simple_statements.at(expression.index).name_index;
        default: throw UnexpectedExpression(expression.type, "getContainerNameIndex");
    }
}

Expression evaluateDictionary(Expression dictionary, Expression environment) {
    const auto dictionary_struct = storage.dictionaries.at(dictionary.index);
    const auto initial_definitions = initializeDefinitions(dictionary_struct);
    const auto result = makeEvaluatedDictionary(
        dictionary.range, EvaluatedDictionary{environment, initial_definitions}
    );
    const auto& statements = dictionary_struct.statements;
    auto i = size_t{0};
    while (i < statements.size()) {
        const auto statement = statements.at(i);
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = storage.definitions.at(statement.index);
            const auto right_expression = definition.expression;
            const auto value = evaluate(right_expression, result);
            setDictionaryDefinition(result, definition.name_index, value);
            i += 1;
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = storage.put_assignments.at(statement.index);
            const auto right_expression = put_assignment.expression;
            const auto value = evaluate(right_expression, result);
            const auto current = getDictionaryDefinition(result, put_assignment.name_index);
            const auto tuple = makeEvaluatedTuple(
                {}, EvaluatedTuple{{value, current}}
            );
            const auto new_value = container_functions::put(tuple);
            setDictionaryDefinition(result, put_assignment.name_index, new_value);
            i += 1;
        }
        else if (type == PUT_EACH_ASSIGNMENT) {
            const auto put_each_assignment = storage.put_each_assignments.at(statement.index);
            const auto right_expression = put_each_assignment.expression;
            for (
                auto container = evaluate(right_expression, result);
                boolean(container);
                container = container_functions::drop(container)
            ) {
                const auto current = getDictionaryDefinition(result, put_each_assignment.name_index);
                const auto value = container_functions::take(container);
                const auto tuple = makeEvaluatedTuple(
                    {}, EvaluatedTuple{{value, current}}
                );
                const auto new_value = container_functions::put(tuple);
                setDictionaryDefinition(result, put_each_assignment.name_index, new_value);
            }
            i += 1;
        }
        else if (type == DROP_ASSIGNMENT) {
            const auto drop_assignment = storage.drop_assignments.at(statement.index);
            const auto current = getDictionaryDefinition(result, drop_assignment.name_index);
            const auto new_value = container_functions::drop(current);
            setDictionaryDefinition(result, drop_assignment.name_index, new_value);
            i += 1;
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = storage.while_statements.at(statement.index);
            if (boolean(evaluate(while_statement.expression, result))) {
                i += 1;
            } else {
                i = while_statement.end_index_ + 1;
            }
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = storage.for_statements.at(statement.index);
            const auto container = getDictionaryDefinition(result, for_statement.name_index_container);
            if (boolean(container)) {
                const auto value = container_functions::take(container);
                setDictionaryDefinition(result, for_statement.name_index_item, value);
                i += 1;
            } else {
                i = for_statement.end_index_ + 1;
            }
        }
        else if (type == FOR_SIMPLE_STATEMENT) {
            const auto for_statement = storage.for_simple_statements.at(statement.index);
            const auto container = getDictionaryDefinition(result, for_statement.name_index);
            if (boolean(container)) {
                i += 1;
            } else {
                i = for_statement.end_index_ + 1;
            }
        }
        else if (type == WHILE_END_STATEMENT) {
            const auto end_statement = storage.while_end_statements.at(statement.index);
            i = end_statement.while_index_;
        }
        else if (type == FOR_END_STATEMENT) {
            const auto end_statement = storage.for_end_statements.at(statement.index);
            i = end_statement.for_index_;
            const auto name_index = getContainerNameIndex(statements.at(i));
            const auto old_container = getDictionaryDefinition(result, name_index);
            const auto new_container = container_functions::drop(old_container);
            setDictionaryDefinition(result, name_index, new_container);
        }
        else if (type == RETURN_STATEMENT) {
            break;
        }
    }
    return result;
}

Expression applyTableIndexing(Expression table, Expression key) {
    const auto& table_struct = storage.evaluated_tables.at(table.index);
    std::string k;
    serialize(k, key);
    try {
        return table_struct.rows.at(k).value;
    }
    catch (const std::out_of_range&) {
        throw MissingKey(k);
    }
}

Expression applyStackIndexing(Expression stack, Expression input) {
    if (input.type != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens when indexing a stack. " +
                "\nThe index is expected to be a " + NAMES[NUMBER] + "," +
                "\nbut now it is a" + NAMES[input.type] +
                ".\n"
        );
    }
    const auto number = storage.numbers.at(input.index);
    const auto index = getIndex(number);
    auto stack_struct = storage.evaluated_stacks.at(stack.index);
    for (size_t i = 0; i < index; ++i) {
        if (stack_struct.rest.type == EMPTY_STACK) {
            throw std::runtime_error("Stack index out of range");
        }
        stack_struct = getEvaluatedStack(stack_struct.rest);
    }
    return stack_struct.top;
}

Expression applyStringIndexing(Expression string, Expression input) {
    if (input.type != NUMBER) {
        throw std::runtime_error(
            std::string{"\n\nI have found a dynamic type error."} +
                "\nIt happens when indexing a string. " +
                "\nThe index is expected to be a " + NAMES[NUMBER] + "," +
                "\nbut now it is a" + NAMES[input.type] +
                ".\n"
        );
    }
    const auto number = storage.numbers.at(input.index);
    const auto index = getIndex(number);
    auto string_struct = storage.strings.at(string.index);
    for (size_t i = 0; i < index; ++i) {
        if (string_struct.rest.type == EMPTY_STACK) {
            throw std::runtime_error("String index out of range");
        }
        string_struct = getString(string_struct.rest);
    }
    return string_struct.top;
}

Expression evaluateFunctionApplicationTypes(
    Expression function_application, Expression environment
) {
    const auto function_application_struct = storage.function_applications.at(function_application.index);
    const auto function = lookupDictionary(function_application_struct.name, environment);
    const auto input = evaluate_types(function_application_struct.child, environment);
    switch (function.type) {
        case FUNCTION: return applyFunction(evaluate_types, function, input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(function, input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(evaluate_types, function, input);
        case FUNCTION_TUPLE: return applyFunctionTuple(evaluate_types, function, input);

        case EVALUATED_TABLE: return applyTableIndexingTypes(function);
        case EVALUATED_TUPLE: return applyTupleIndexing(function, input);
        case EVALUATED_STACK: return applyStackIndexingTypes(function);
        case STRING: return applyStringIndexingTypes(function);

        case EMPTY_STACK: return Expression{ANY, 0, function_application.range};
        case EMPTY_STRING: return Expression{CHARACTER, 0, function_application.range};

        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplicationTypes");
    }
}

Expression evaluateFunctionApplication(
    Expression function_application, Expression environment
) {
    const auto function_application_struct = storage.function_applications.at(function_application.index);
    const auto function = lookupDictionary(function_application_struct.name, environment);
    const auto input = evaluate(function_application_struct.child, environment);
    switch (function.type) {
        case FUNCTION: return applyFunction(evaluate, function, input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(function, input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(evaluate, function, input);
        case FUNCTION_TUPLE: return applyFunctionTuple(evaluate, function, input);
        
        case EVALUATED_TABLE: return applyTableIndexing(function, input);
        case EVALUATED_TUPLE: return applyTupleIndexing(function, input);
        case EVALUATED_STACK: return applyStackIndexing(function, input);
        case STRING: return applyStringIndexing(function, input);
        
        case EMPTY_STACK: throw std::runtime_error("I caught a run-time error when trying to index an empty stack.");
        case EMPTY_STRING: throw std::runtime_error("I caught a run-time error when trying to index an empty string.");
        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplication");
    }
}

} // namespace

Expression evaluate_types(Expression expression, Expression environment) {
    switch (expression.type) {
        // These are the same for types and values, and just pass through:
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case YES: return expression;
        case NO: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_STACK: return expression;
        case EVALUATED_STACK: return expression;
        case EVALUATED_DICTIONARY: return expression;
        case EVALUATED_TUPLE: return expression;
        case EVALUATED_TABLE: return expression;
        case EVALUATED_TABLE_VIEW: return expression;

        // These are the same for types and values:
        case FUNCTION: return evaluateFunction(expression, environment);
        case FUNCTION_TUPLE: return evaluateFunctionTuple(expression, environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(expression, environment);
        case LOOKUP_SYMBOL: return lookupDictionary(storage.symbol_lookups.at(expression.index).name, environment);

        // These are different for types and values, but templated:
        case STACK: return evaluateStack(evaluate_types, expression, environment);
        case TUPLE: return evaluateTuple(evaluate_types, expression, environment);
        case TABLE: return evaluateTable(evaluate_types, serialize_types, expression, environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate_types, expression, environment);
        case TYPED_EXPRESSION: return evaluateTypedExpression(evaluate_types, expression, environment);

        // These are different for types and values:
        case DYNAMIC_EXPRESSION: return evaluateDynamicExpressionTyped(expression);
        case CONDITIONAL: return evaluateConditionalTypes(expression, environment);
        case IS: return evaluateIsTypes(expression, environment);
        case DICTIONARY: return evaluateDictionaryTypes(expression, environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplicationTypes(expression, environment);
        
        default: throw UnexpectedExpression(expression.type, "evaluate types operation");
    }
}

Expression evaluate(Expression expression, Expression environment) {
    switch (expression.type) {
        // These are the same for types and values, and just pass through:
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case YES: return expression;
        case NO: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_STACK: return expression;
        case EVALUATED_STACK: return expression;
        case EVALUATED_DICTIONARY: return expression;
        case EVALUATED_TUPLE: return expression;
        case EVALUATED_TABLE: return expression;
        case EVALUATED_TABLE_VIEW: return expression;

        // These are the same for types and values:
        case FUNCTION: return evaluateFunction(expression, environment);
        case FUNCTION_TUPLE: return evaluateFunctionTuple(expression, environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(expression, environment);
        case LOOKUP_SYMBOL: return lookupDictionary(storage.symbol_lookups.at(expression.index).name, environment);

        // These are different for types and values, but templated:
        case STACK: return evaluateStack(evaluate, expression, environment);
        case TUPLE: return evaluateTuple(evaluate, expression, environment);
        case TABLE: return evaluateTable(evaluate, serialize, expression, environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate, expression, environment);
        case TYPED_EXPRESSION: return evaluateTypedExpression(evaluate, expression, environment);

        // These are different for types and values:
        case DYNAMIC_EXPRESSION: return evaluateDynamicExpression(expression, environment);
        case CONDITIONAL: return evaluateConditional(expression, environment);
        case IS: return evaluateIs(expression, environment);
        case DICTIONARY: return evaluateDictionary(expression, environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(expression, environment);
        
        default: throw UnexpectedExpression(expression.type, "evaluate operation");
    }
}
