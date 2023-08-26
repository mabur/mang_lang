#include "evaluate.h"

#include <cassert>
#include <iostream>
#include <string.h>

#include "../built_in_functions/container.h"
#include "../factory.h"
#include "serialize.h"

namespace {

void checkTypes(Expression super, Expression sub, const std::string& description);

void checkTypesEvaluatedStack(Expression super, Expression sub, const std::string& description) {
    const auto stack_super = getEvaluatedStack(super).top;
    const auto stack_sub = getEvaluatedStack(sub).top;
    checkTypes(stack_super, stack_sub, description);
}

void checkTypesEvaluatedTable(Expression super, Expression sub, const std::string& description) {
    const auto table_super = getEvaluatedTable(super);
    const auto table_sub = getEvaluatedTable(sub);
    if (table_super.empty()) return;
    if (table_sub.empty()) return;
    const auto row_super = table_super.begin()->second;
    const auto row_sub = table_sub.begin()->second;
    checkTypes(row_super.key, row_sub.key, description);
    checkTypes(row_super.value, row_sub.value, description);
}

void checkTypesEvaluatedTuple(Expression super, Expression sub, const std::string& description) {
    const auto tuple_super = getEvaluatedTuple(super);
    const auto tuple_sub = getEvaluatedTuple(sub);
    if (tuple_super.expressions.size() != tuple_sub.expressions.size()) {
        throw std::runtime_error(
            "Static type error in " + description + ". Inconsistent tuple size."
        );
    }
    const auto count = tuple_super.expressions.size();
    for (size_t i = 0; i < count; ++i) {
        checkTypes(tuple_super.expressions[i], tuple_sub.expressions[i], description);
    }
}

void checkTypesEvaluatedDictionary(Expression super, Expression sub, const std::string& description) {
    const auto definitions_super = getEvaluatedDictionary(super).definitions;
    const auto definitions_sub = getEvaluatedDictionary(sub).definitions;
    if (definitions_super.size() > definitions_sub.size()) {
        throw std::runtime_error(
            "Static type error in " + description + 
            ". Dictionary supertype is bigger than dictionary subtype."
        );
    }
    const auto count = definitions_super.size();
    for (size_t i = 0; i < count; ++i) {
        const auto& name_super = definitions_super[i].name;
        const auto& name_sub = definitions_sub[i].name;
        if (name_super.index != name_sub.index) {
            throw std::runtime_error(
                "Static type error in " + description +
                    ". Inconsistent dictionary names " +
                    getName(name_super) + " & " + getName(name_sub)
            );
        }
        checkTypes(definitions_super[i].expression, definitions_sub[i].expression, description);
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
        describeLocation(super.range.begin()) +
        ". " + NAMES[super.type] +
        " is not a supertype for " + NAMES[sub.type]
    );
}

template<typename Evaluator>
Expression evaluateStack(Evaluator evaluator,
    Expression stack, Expression environment
) {
    const auto op = [&](Expression rest, Expression top) -> Expression {
        const auto evaluated_top = evaluator(top, environment);
        return putEvaluatedStack(rest, evaluated_top);
    };
    const auto code = stack.range;
    const auto init = Expression{EMPTY_STACK, 0, code};
    const auto output = leftFold(init, stack, op, EMPTY_STACK, getStack);
    return reverseEvaluatedStack(code, output);
}

template<typename Evaluator>
Expression evaluateTuple(
    Evaluator evaluator, Expression tuple, Expression environment
) {
    const auto tuple_struct = getTuple(tuple);
    auto evaluated_expressions = std::vector<Expression>{};
    evaluated_expressions.reserve(tuple_struct.expressions.size());
    for (const auto& expression : tuple_struct.expressions) {
        evaluated_expressions.push_back(evaluator(expression, environment));
    }
    const auto code = CodeRange{};
    return makeEvaluatedTuple(code, EvaluatedTuple{evaluated_expressions});
}

template<typename Evaluator, typename Serializer>
Expression evaluateTable(
    Evaluator evaluator,
    Serializer serializer,
    Expression table,
    Expression environment
) {
    const auto table_struct = getTable(table);
    auto rows = std::map<std::string, Row>{};
    for (const auto& row : table_struct.rows) {
        const auto key = evaluator(row.key, environment);
        const auto value = evaluator(row.value, environment);
        const auto serialized_key = serializer(key);
        rows[serialized_key] = {key, value};
    }
    const auto code = CodeRange{};
    return makeEvaluatedTable(code, EvaluatedTable{rows});
}

template<typename Evaluator>
Expression evaluateLookupChild(
    Evaluator evaluator, Expression lookup_child, Expression environment
) {
    const auto lookup_child_struct = getLookupChild(lookup_child);
    const auto child = evaluator(lookup_child_struct.child, environment);
    const auto dictionary = getEvaluatedDictionary(child);
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
    const Function& function,
    Expression input
) {
    
    const auto argument = getArgument(function.input_name);
    checkArgument(evaluator, argument, input, function.environment);
    const auto definitions = std::vector<Definition>{{argument.name, input, 0}};
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{function.environment, definitions}
    );
    return evaluator(function.body, middle);
}

template<typename Evaluator>
Expression applyFunctionDictionary(
    Evaluator evaluator,
    const FunctionDictionary& function_dictionary,
    Expression input
) {
    const auto evaluated_dictionary = getEvaluatedDictionary(input);
    for (const auto& name : function_dictionary.input_names) {
        const auto argument = getArgument(name);
        const auto expression = evaluated_dictionary.lookup(argument.name);
        checkArgument(evaluator, argument, expression, function_dictionary.environment);
    }
    // TODO: pass along environment? Is some use case missing now?
    return evaluator(function_dictionary.body, input);
}

template<typename Evaluator>
Expression applyFunctionTuple(
    Evaluator evaluator,
    const FunctionTuple& function,
    Expression input
) {
    auto tuple = getEvaluatedTuple(input);
    const auto& input_names = function.input_names;
    if (input_names.size() != tuple.expressions.size()) {
        throw std::runtime_error{"Wrong number of input to function"};
    }
    
    const auto num_inputs = input_names.size();
    auto definitions = std::vector<Definition>(num_inputs);
    for (size_t i = 0; i < num_inputs; ++i) {
        const auto argument = getArgument(input_names[i]);
        const auto expression = tuple.expressions[i];
        checkArgument(evaluator, argument, expression, function.environment);
        definitions[i] = Definition{argument.name, expression, i};
    }
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{function.environment, definitions}
    );
    return evaluator(function.body, middle);
}

Expression evaluateFunction(Expression function, Expression environment) {
    const auto function_struct = getFunction(function);
    return makeFunction(CodeRange{}, {
        environment, function_struct.input_name, function_struct.body
    });
}

Expression evaluateFunctionDictionary(
    Expression function_dictionary, Expression environment
) {
    const auto function_dictionary_struct = getFunctionDictionary(function_dictionary);
    return makeFunctionDictionary(CodeRange{}, {
        environment,
        function_dictionary_struct.input_names,
        function_dictionary_struct.body
    });
}

Expression evaluateFunctionTuple(
    Expression function_tuple, Expression environment
) {
    const auto function_tuple_struct = getFunctionTuple(function_tuple);
    return makeFunctionTuple(CodeRange{}, {
        environment, function_tuple_struct.input_names, function_tuple_struct.body
    });
}

Expression lookupDictionary(Expression name, Expression expression) {
    if (expression.type != EVALUATED_DICTIONARY) {
        throw MissingSymbol(getName(name), "environment of type " + NAMES[expression.type]);
    }
    const auto dictionary = getEvaluatedDictionary(expression);
    const auto result = dictionary.optionalLookup(name);
    if (result) {
        return *result;
    }
    return lookupDictionary(name, dictionary.environment);
}

Expression applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, Expression input
) {
    return function_built_in.function(input);
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
    switch (expression.type) {
        case EVALUATED_TABLE: return !getEvaluatedTable(expression).empty();
        case EVALUATED_TABLE_VIEW: return !getEvaluatedTableView(expression).empty();
        case NUMBER: return static_cast<bool>(getNumber(expression));
        case YES: return true;
        case NO: return false;
        case EVALUATED_STACK: return true;
        case EMPTY_STACK: return false;
        case STRING: return true;
        case EMPTY_STRING: return false;
        default: throw UnexpectedExpression(expression.type, "boolean operation");
    }
}

size_t getIndex(Number number) {
    if (number < 0) {
        using namespace std;
        throw runtime_error("Cannot have negative index: " + to_string(number));
    }
    return static_cast<size_t>(number);
}

Expression applyTupleIndexing(const EvaluatedTuple& tuple, Number number) {
    const auto i = getIndex(number);
    try {
        return tuple.expressions.at(i);
    }
    catch (const std::out_of_range&) {
        throw std::runtime_error(
            "Tuple of size " + std::to_string(tuple.expressions.size()) +
                " indexed with " + std::to_string(i)
        );
    }
}

Expression applyTableIndexing(const EvaluatedTable& table) {
    if (table.rows.empty()) {
        return Expression{};
    }
    return table.begin()->second.value;
}

Expression applyStackIndexing(EvaluatedStack stack) {
    return stack.top;
}

Expression applyStringIndexing(String string) {
    return string.top;
}

template<typename Vector, typename Predicate>
bool allOfVectors(const Vector& left, const Vector& right, Predicate predicate) {
    if (left.size() != right.size()) {
        return false;
    }
    for (size_t i = 0; i < left.size(); ++i) {
        if (!predicate(left[i], right[i])) {
            return false;
        }
    }
    return true;
}

bool isEqual(Expression left, Expression right) {
    const auto left_type = left.type;
    const auto right_type = right.type;
    if (left_type == NUMBER && right_type == NUMBER) {
        return getNumber(left) == getNumber(right);
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
            getEvaluatedTuple(left).expressions,
            getEvaluatedTuple(right).expressions,
            isEqual
        );
    }
    return false;
}

Expression evaluateDynamicExpressionTyped(Expression expression) {
    return Expression{ANY, 0, expression.range};
}

Expression evaluateDynamicExpression(Expression expression, Expression environment) {
    return evaluate(getDynamicExpression(expression).expression, environment);
}

Expression evaluateConditionalTypes(
    Expression conditional, Expression environment
) {
    const auto conditional_struct = getConditional(conditional);
    for (auto alternative = conditional_struct.alternative_first;
        alternative.index <= conditional_struct.alternative_last.index;
        ++alternative.index
    ) {
        evaluate_types(getAlternative(alternative).left, environment);
    }
    const auto else_expression = evaluate_types(conditional_struct.expression_else, environment);
    for (auto a = conditional_struct.alternative_first;
        a.index <= conditional_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = getAlternative(a);
        const auto alternative_expression = evaluate_types(
            alternative.right, environment
        );
        checkTypes(else_expression, alternative_expression, "if");
    }
    return else_expression;
}

Expression evaluateConditional(Expression conditional, Expression environment) {
    const auto conditional_struct = getConditional(conditional);
    for (auto a = conditional_struct.alternative_first;
        a.index <= conditional_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = getAlternative(a);
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
    const auto is_struct = getIs(is);
    evaluate_types(is_struct.input, environment);
    for (auto a = is_struct.alternative_first;
        a.index <= is_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = getAlternative(a);
        evaluate_types(alternative.left, environment);
    }
    const auto else_expression = evaluate_types(is_struct.expression_else, environment);
    for (auto a = is_struct.alternative_first;
        a.index <= is_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = getAlternative(a);
        const auto alternative_expression = evaluate_types(alternative.right, environment);
        checkTypes(else_expression, alternative_expression, "is");
    }
    return else_expression;
}

Expression evaluateIs(Expression is, Expression environment) {
    const auto is_struct = getIs(is);
    const auto value = evaluate(is_struct.input, environment);
    for (auto a = is_struct.alternative_first;
        a.index <= is_struct.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = getAlternative(a);
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
    const auto expression_struct = getTypedExpression(expression);
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
            auto definition = getDefinition(statement);
            definition.expression = Expression{};
            definitions[definition.name_index] = definition;
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = getForStatement(statement);
            definitions[for_statement.name_index_item] = Definition{
                for_statement.name_item,
                Expression{},
                for_statement.name_index_item
            };
        }
    }
    return definitions;
}

Expression evaluateDictionaryTypes(
    Expression dictionary, Expression environment
) {
    const auto dictionary_struct = getDictionary(dictionary);
    const auto initial_definitions = initializeDefinitions(dictionary_struct);
    const auto result = makeEvaluatedDictionary(
        CodeRange{}, EvaluatedDictionary{environment, initial_definitions}
    );
    for (const auto& statement : dictionary_struct.statements) {
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = getDefinition(statement);
            const auto right_expression = definition.expression;
            const auto value = evaluate_types(right_expression, result);
            // TODO: is this a principled approach?
            if (value.type != ANY) {
                setDictionaryDefinition(result, definition.name_index, value);
            }
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = getPutAssignment(statement);
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
            const auto put_each_assignment = getPutEachAssignment(statement);
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
            const auto drop_assignment = getDropAssignment(statement);
            const auto current = getDictionaryDefinition(result, drop_assignment.name_index);
            const auto new_value = container_functions::dropTyped(current);
            setDictionaryDefinition(result, drop_assignment.name_index, new_value);
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWhileStatement(statement);
            booleanTypes(evaluate_types(while_statement.expression, result));
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = getForStatement(statement);
            const auto container = getDictionaryDefinition(result, for_statement.name_index_container);
            booleanTypes(container);
            const auto value = container_functions::takeTyped(container);
            setDictionaryDefinition(result, for_statement.name_index_item, value);
        }
        else if (type == FOR_SIMPLE_STATEMENT) {
            const auto for_statement = getForSimpleStatement(statement);
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
        case FOR_STATEMENT: return getForStatement(expression).name_index_container;
        case FOR_SIMPLE_STATEMENT: return getForSimpleStatement(expression).name_index;
        default: throw UnexpectedExpression(expression.type, "getContainerNameIndex");
    }
}

Expression evaluateDictionary(Expression dictionary, Expression environment) {
    const auto dictionary_struct = getDictionary(dictionary);
    const auto initial_definitions = initializeDefinitions(dictionary_struct);
    const auto result = makeEvaluatedDictionary(
        CodeRange{}, EvaluatedDictionary{environment, initial_definitions}
    );
    const auto& statements = dictionary_struct.statements;
    auto i = size_t{0};
    while (i < statements.size()) {
        const auto statement = statements[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = getDefinition(statement);
            const auto right_expression = definition.expression;
            const auto value = evaluate(right_expression, result);
            setDictionaryDefinition(result, definition.name_index, value);
            i += 1;
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = getPutAssignment(statement);
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
            const auto put_each_assignment = getPutEachAssignment(statement);
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
            const auto drop_assignment = getDropAssignment(statement);
            const auto current = getDictionaryDefinition(result, drop_assignment.name_index);
            const auto new_value = container_functions::drop(current);
            setDictionaryDefinition(result, drop_assignment.name_index, new_value);
            i += 1;
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWhileStatement(statement);
            if (boolean(evaluate(while_statement.expression, result))) {
                i += 1;
            } else {
                i = while_statement.end_index_ + 1;
            }
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = getForStatement(statement);
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
            const auto for_statement = getForSimpleStatement(statement);
            const auto container = getDictionaryDefinition(result, for_statement.name_index);
            if (boolean(container)) {
                i += 1;
            } else {
                i = for_statement.end_index_ + 1;
            }
        }
        else if (type == WHILE_END_STATEMENT) {
            const auto end_statement = getWhileEndStatement(statement);
            i = end_statement.while_index_;
        }
        else if (type == FOR_END_STATEMENT) {
            const auto end_statement = getForEndStatement(statement);
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

Expression applyTableIndexing(const EvaluatedTable& table, Expression key) {
    const auto k = serialize(key);
    try {
        return table.rows.at(k).value;
    }
    catch (const std::out_of_range&) {
        throw MissingKey(k);
    }
}

Expression applyStackIndexing(EvaluatedStack stack, Number number) {
    const auto index = getIndex(number);
    for (size_t i = 0; i < index; ++i) {
        if (stack.rest.type == EMPTY_STACK) {
            throw std::runtime_error("Stack index out of range");
        }
        stack = getEvaluatedStack(stack.rest);
    }
    return stack.top;
}

Expression applyStringIndexing(String string, Number number) {
    const auto index = getIndex(number);
    for (size_t i = 0; i < index; ++i) {
        if (string.rest.type == EMPTY_STACK) {
            throw std::runtime_error("String index out of range");
        }
        string = getString(string.rest);
    }
    return string.top;
}

Expression evaluateFunctionApplicationTypes(
    Expression function_application, Expression environment
) {
    const auto function_application_struct = getFunctionApplication(function_application);
    const auto function = lookupDictionary(function_application_struct.name, environment);
    const auto input = evaluate_types(function_application_struct.child, environment);
    switch (function.type) {
        case FUNCTION: return applyFunction(evaluate_types, getFunction(function), input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(getFunctionBuiltIn(function), input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(evaluate_types, getFunctionDictionary(function), input);
        case FUNCTION_TUPLE: return applyFunctionTuple(evaluate_types, getFunctionTuple(function), input);

        case EVALUATED_TABLE: return applyTableIndexing(getEvaluatedTable(function));
        case EVALUATED_TUPLE: return applyTupleIndexing(getEvaluatedTuple(function), getNumber(input));
        case EVALUATED_STACK: return applyStackIndexing(getEvaluatedStack(function));
        case STRING: return applyStringIndexing(getString(function));

        case EMPTY_STACK: return Expression{};
        case EMPTY_STRING: return Expression{CHARACTER, {}, CodeRange{}};

        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplicationTypes");
    }
}

Expression evaluateFunctionApplication(
    Expression function_application, Expression environment
) {
    const auto function_application_struct = getFunctionApplication(function_application);
    const auto function = lookupDictionary(function_application_struct.name, environment);
    const auto input = evaluate(function_application_struct.child, environment);
    switch (function.type) {
        case FUNCTION: return applyFunction(evaluate, getFunction(function), input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(getFunctionBuiltIn(function), input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(evaluate, getFunctionDictionary(function), input);
        case FUNCTION_TUPLE: return applyFunctionTuple(evaluate, getFunctionTuple(function), input);
        
        case EVALUATED_TABLE: return applyTableIndexing(getEvaluatedTable(function), input);
        case EVALUATED_TUPLE: return applyTupleIndexing(getEvaluatedTuple(function), getNumber(input));
        case EVALUATED_STACK: return applyStackIndexing(getEvaluatedStack(function), getNumber(input));
        case STRING: return applyStringIndexing(getString(function), getNumber(input));
        
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
        case LOOKUP_SYMBOL: return lookupDictionary(getLookupSymbol(expression).name, environment);

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
        case LOOKUP_SYMBOL: return lookupDictionary(getLookupSymbol(expression).name, environment);

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
