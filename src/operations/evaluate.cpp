#include "evaluate.h"

#include <cassert>

#include "../built_in_functions/stack.h"
#include "../container.h"
#include "../factory.h"
#include "serialize.h"

namespace {

void checkTypes(Expression left, Expression right, const std::string& description) {
    if (left.type == ANY || right.type == ANY) return;
    if (left.type == EMPTY_STACK && right.type == EVALUATED_STACK) return;
    if (left.type == EVALUATED_STACK && right.type == EMPTY_STACK) return;
    if (left.type == EMPTY_STRING && right.type == STRING) return;
    if (left.type == STRING && right.type == EMPTY_STRING) return;
    if (left.type == right.type) return;
    throw std::runtime_error(
        "Static type error in " + description +
        ". Expected " + NAMES[left.type] +
        " but got " + NAMES[right.type]
    );
}

// TODO: remove forward declaration.
Expression evaluateFunction(const Function& function, Expression environment);

Expression evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression environment
);

Expression evaluateFunctionTuple(
    const FunctionTuple& function_stack, Expression environment
);

template<typename Evaluator>
Expression evaluateStack(Evaluator evaluator,
    Expression stack, Expression environment
) {
    const auto op = [&](Expression rest, Expression top) -> Expression {
        const auto evaluated_top = evaluator(top, environment);
        return putEvaluatedStack(rest, evaluated_top);
    };
    const auto code = CodeRange{};
    const auto init = makeEmptyStack(code, {});
    const auto output = leftFold(init, stack, op, EMPTY_STACK, getStack);
    return reverseEvaluatedStack(code, output);
}

template<typename Evaluator>
Expression evaluateTuple(Evaluator evaluator, Tuple tuple, Expression environment) {
    auto evaluated_expressions = std::vector<Expression>{};
    evaluated_expressions.reserve(tuple.expressions.size());
    for (const auto& expression : tuple.expressions) {
        evaluated_expressions.push_back(evaluator(expression, environment));
    }
    const auto code = CodeRange{};
    return makeEvaluatedTuple(code, EvaluatedTuple{evaluated_expressions});
}

template<typename Evaluator, typename Serializer>
Expression evaluateTable(
    Evaluator evaluator,
    Serializer serializer,
    Table table,
    Expression environment
) {
    auto rows = std::map<std::string, Row>{};
    for (const auto& row : table.rows) {
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
    Evaluator evaluator, const LookupChild& lookup_child, Expression environment
) {
    const auto child = evaluator(lookup_child.child, environment);
    const auto name = getName(lookup_child.name);
    const auto dictionary = getEvaluatedDictionary(child);
    return dictionary.definitions.lookup(name);
}

Expression lookupDictionary(const Name& name, Expression expression);


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
    auto definitions = Definitions{};
    const auto argument = getArgument(function.input_name);
    checkArgument(evaluator, argument, input, function.environment);
    definitions.add(argument.name, input);
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{function.environment, definitions}
    );
    return evaluator(function.body, middle);
}

Expression applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, Expression input
);

template<typename Evaluator>
Expression applyFunctionDictionary(
    Evaluator evaluator,
    const FunctionDictionary& function_dictionary,
    Expression input
) {
    const auto evaluated_dictionary = getEvaluatedDictionary(input);
    const auto& definitions = evaluated_dictionary.definitions;
    for (const auto& name : function_dictionary.input_names) {
        const auto argument = getArgument(name);
        const auto expression = definitions.lookup(argument.name);
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
    auto definitions = Definitions{};
    const auto num_inputs = input_names.size();
    for (size_t i = 0; i < num_inputs; ++i) {
        const auto argument = getArgument(input_names[i]);
        const auto expression = tuple.expressions[i];
        checkArgument(evaluator, argument, expression, function.environment);
        definitions.add(argument.name, expression);
    }
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{function.environment, definitions}
    );
    return evaluator(function.body, middle);
}

Expression evaluateFunction(const Function& function, Expression environment) {
    return makeFunction(CodeRange{}, {
        environment, function.input_name, function.body
    });
}

Expression evaluateFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression environment
) {
    return makeFunctionDictionary(CodeRange{}, {
        environment,
        function_dictionary.input_names,
        function_dictionary.body
    });
}

Expression evaluateFunctionTuple(
    const FunctionTuple& function_stack, Expression environment
) {
    return makeFunctionTuple(CodeRange{}, {
        environment, function_stack.input_names, function_stack.body
    });
}

Expression lookupDictionary(const Name& name, Expression expression) {
    if (expression.type != EVALUATED_DICTIONARY) {
        throw MissingSymbol(name, "environment of type " + NAMES[expression.type]);
    }
    const auto dictionary = getEvaluatedDictionary(expression);
    if (!dictionary.definitions.has(name)) {
        return lookupDictionary(name, dictionary.environment);
    }
    return dictionary.definitions.lookup(name);
}

Expression applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, Expression input
) {
    return function_built_in.function(input);
}

void booleanTypes(Expression expression) {
    switch (expression.type) {
        case NUMBER: return;
        case BOOLEAN: return;
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
        case BOOLEAN: return getBoolean(expression);
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
    if (left_type == BOOLEAN && right_type == BOOLEAN) {
        return getBoolean(left) == getBoolean(right);
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

Expression evaluateConditionalTypes(
    const Conditional& conditional, Expression environment
) {
    for (const auto alternative : conditional.alternatives) {
        evaluate_types(alternative.left, environment);
    }
    const auto else_expression = evaluate_types(conditional.expression_else, environment);
    for (const auto alternative : conditional.alternatives) {
        const auto alternative_expression = evaluate_types(alternative.right, environment);
        checkTypes(alternative_expression, else_expression, "if");
    }
    return else_expression;
}

Expression evaluateConditional(
    const Conditional& conditional, Expression environment
) {
    for (const auto alternative : conditional.alternatives) {
        const auto left_value = evaluate(alternative.left, environment);
        if (boolean(left_value)) {
            return evaluate(alternative.right, environment);
        }
    }
    return evaluate(conditional.expression_else, environment);
}

Expression evaluateIsTypes(
    const IsExpression& is_expression, Expression environment
) {
    evaluate_types(is_expression.input, environment);
    for (const auto alternative : is_expression.alternatives) {
        evaluate_types(alternative.left, environment);
    }
    const auto else_expression = evaluate_types(is_expression.expression_else, environment);
    for (const auto alternative : is_expression.alternatives) {
        const auto alternative_expression = evaluate_types(alternative.right, environment);
        checkTypes(alternative_expression, else_expression, "is");
    }
    return else_expression;
}

Expression evaluateIs(
    const IsExpression& is_expression, Expression environment
) {
    const auto value = evaluate(is_expression.input, environment);
    for (const auto alternative : is_expression.alternatives) {
        const auto left_value = evaluate(alternative.left, environment);
        if (isEqual(value, left_value)) {
            return evaluate(alternative.right, environment);
        }
    }
    return evaluate(is_expression.expression_else, environment);
}

template<typename Evaluator>
Expression evaluateTypedExpression(
    Evaluator evaluator, TypedExpression expression, Expression environment
) {
    const auto type = evaluator(expression.type, environment);
    const auto value = evaluator(expression.value, environment);
    checkTypes(type, value, "typed expression");
    return value;
}

Expression evaluateDictionaryTypes(
    const Dictionary& dictionary, Expression environment
) {
    const auto result_environment = makeEvaluatedDictionary(
        CodeRange{}, EvaluatedDictionary{environment, {}}
    );
    const auto& statements = dictionary.statements;
    for (size_t i = 0; i < dictionary.statements.size(); ++i) {
        const auto statement = statements[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = getDefinition(statement);
            const auto name = getName(definition.name);
            const auto& right_expression = definition.expression;
            const auto value = evaluate_types(right_expression, result_environment);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            // TODO: is this a principled approach?
            if (value.type != ANY || !result.definitions.has(name)) {
                result.definitions.add(name, value);
            }
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = getPutAssignment(statement);
            const auto& right_expression = put_assignment.expression;
            const auto value = evaluate_types(right_expression, result_environment);
            const auto name = getName(put_assignment.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto current = lookupDictionary(name, result_environment);
            const auto tuple = makeEvaluatedTuple(
                {}, EvaluatedTuple{{value, current}}
            );
            const auto new_value = stack_functions::putTyped(tuple);
            result.definitions.add(name, new_value);
        }
        else if (type == DROP_ASSIGNMENT) {
            const auto drop_assignment = getDropAssignment(statement);
            const auto name = getName(drop_assignment.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto current = lookupDictionary(name, result_environment);
            const auto new_value = stack_functions::dropTyped(current);
            result.definitions.add(name, new_value);
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWhileStatement(statement);
            booleanTypes(evaluate_types(while_statement.expression, result_environment));
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = getForStatement(statement);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto name_container = getName(for_statement.name_container);
            const auto container = lookupDictionary(name_container, result_environment);
            booleanTypes(container);
            const auto name_item = getName(for_statement.name_item);
            const auto value = stack_functions::takeTyped(container);
            result.definitions.add(name_item, value);
        }
    }
    return result_environment;
}

Expression evaluateDictionary(
    const Dictionary& dictionary, Expression environment
) {
    const auto result_environment = makeEvaluatedDictionary(
        CodeRange{}, EvaluatedDictionary{environment, {}}
    );
    const auto& statements = dictionary.statements;
    auto i = size_t{0};
    while (i < statements.size()) {
        const auto statement = statements[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = getDefinition(statement);
            const auto& right_expression = definition.expression;
            const auto value = evaluate(right_expression, result_environment);
            const auto name = getName(definition.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            result.definitions.add(name, value);
            i += 1;
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = getPutAssignment(statement);
            const auto& right_expression = put_assignment.expression;
            const auto value = evaluate(right_expression, result_environment);
            const auto name = getName(put_assignment.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto current = lookupDictionary(name, result_environment);
            const auto tuple = makeEvaluatedTuple(
                {}, EvaluatedTuple{{value, current}}
            );
            const auto new_value = stack_functions::put(tuple);
            result.definitions.add(name, new_value);
            i += 1;
        }
        else if (type == DROP_ASSIGNMENT) {
            const auto drop_assignment = getDropAssignment(statement);
            const auto name = getName(drop_assignment.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto current = lookupDictionary(name, result_environment);
            const auto new_value = stack_functions::drop(current);
            result.definitions.add(name, new_value);
            i += 1;
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWhileStatement(statement);
            if (boolean(evaluate(while_statement.expression, result_environment))) {
                i += 1;
            } else {
                i = while_statement.end_index_ + 1;
            }
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = getForStatement(statement);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto name_container = getName(for_statement.name_container);
            const auto container = lookupDictionary(name_container, result_environment);
            if (boolean(container)) {
                const auto name_item = getName(for_statement.name_item);
                const auto value = stack_functions::take(container);
                result.definitions.add(name_item, value);
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
            const auto for_statement = getForStatement(statements.at(i));
            const auto name = getName(for_statement.name_container);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto old_container = lookupDictionary(name, result_environment);
            const auto new_container = stack_functions::drop(old_container);
            result.definitions.add(name, new_container);
        }
    }
    return result_environment;
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
    const FunctionApplication& function_application,
    Expression environment
) {
    const auto function = lookupDictionary(getName(function_application.name), environment);
    const auto input = evaluate_types(function_application.child, environment);
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
        case EMPTY_STRING: return Expression{CHARACTER, {}, {}};

        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplicationTypes");
    }
}

Expression evaluateFunctionApplication(
    const FunctionApplication& function_application,
    Expression environment
) {
    const auto function = lookupDictionary(getName(function_application.name), environment);
    const auto input = evaluate(function_application.child, environment);
    switch (function.type) {
        case FUNCTION: return applyFunction(evaluate, getFunction(function), input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(getFunctionBuiltIn(function), input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(evaluate, getFunctionDictionary(function), input);
        case FUNCTION_TUPLE: return applyFunctionTuple(evaluate, getFunctionTuple(function), input);
        
        case EVALUATED_TABLE: return applyTableIndexing(getEvaluatedTable(function), input);
        case EVALUATED_TUPLE: return applyTupleIndexing(getEvaluatedTuple(function), getNumber(input));
        case EVALUATED_STACK: return applyStackIndexing(getEvaluatedStack(function), getNumber(input));
        case STRING: return applyStringIndexing(getString(function), getNumber(input));
        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplicationTypes");
    }
}

} // namespace

Expression evaluate_types(Expression expression, Expression environment) {
    switch (expression.type) {
        // These are the same for types and values, and just pass through:
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case BOOLEAN: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_STACK: return expression;
        case EVALUATED_STACK: return expression;
        case EVALUATED_DICTIONARY: return expression;
        case EVALUATED_TUPLE: return expression;
        case EVALUATED_TABLE: return expression;
        case EVALUATED_TABLE_VIEW: return expression;

        // These are the same for types and values:
        case FUNCTION: return evaluateFunction(getFunction(expression), environment);
        case FUNCTION_TUPLE: return evaluateFunctionTuple(getFunctionTuple(expression), environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(getFunctionDictionary(expression), environment);
        case LOOKUP_SYMBOL: return lookupDictionary(getName(getLookupSymbol(expression).name), environment);

        // These are different for types and values, but templated:
        case STACK: return evaluateStack(evaluate_types, expression, environment);
        case TUPLE: return evaluateTuple(evaluate_types, getTuple(expression), environment);
        case TABLE: return evaluateTable(evaluate_types, serialize_types, getTable(expression), environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate_types, getLookupChild(expression), environment);
        case TYPED_EXPRESSION: return evaluateTypedExpression(evaluate_types, getTypedExpression(expression), environment);

        // These are different for types and values:
        case DYNAMIC_EXPRESSION: return Expression{};
        case CONDITIONAL: return evaluateConditionalTypes(getConditional(expression), environment);
        case IS: return evaluateIsTypes(getIs(expression), environment);
        case DICTIONARY: return evaluateDictionaryTypes(getDictionary(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplicationTypes(getFunctionApplication(expression), environment);
        
        default: throw UnexpectedExpression(expression.type, "evaluate types operation");
    }
}

Expression evaluate(Expression expression, Expression environment) {
    switch (expression.type) {
        // These are the same for types and values, and just pass through:
        case NUMBER: return expression;
        case CHARACTER: return expression;
        case BOOLEAN: return expression;
        case EMPTY_STRING: return expression;
        case STRING: return expression;
        case EMPTY_STACK: return expression;
        case EVALUATED_STACK: return expression;
        case EVALUATED_DICTIONARY: return expression;
        case EVALUATED_TUPLE: return expression;
        case EVALUATED_TABLE: return expression;
        case EVALUATED_TABLE_VIEW: return expression;

        // These are the same for types and values:
        case FUNCTION: return evaluateFunction(getFunction(expression), environment);
        case FUNCTION_TUPLE: return evaluateFunctionTuple(getFunctionTuple(expression), environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(getFunctionDictionary(expression), environment);
        case LOOKUP_SYMBOL: return lookupDictionary(getName(getLookupSymbol(expression).name), environment);

        // These are different for types and values, but templated:
        case STACK: return evaluateStack(evaluate, expression, environment);
        case TUPLE: return evaluateTuple(evaluate, getTuple(expression), environment);
        case TABLE: return evaluateTable(evaluate, serialize, getTable(expression), environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate, getLookupChild(expression), environment);
        case TYPED_EXPRESSION: return evaluateTypedExpression(evaluate, getTypedExpression(expression), environment);

        // These are different for types and values:
        case DYNAMIC_EXPRESSION: return evaluate(getDynamicExpression(expression).expression, environment);
        case CONDITIONAL: return evaluateConditional(getConditional(expression), environment);
        case IS: return evaluateIs(getIs(expression), environment);
        case DICTIONARY: return evaluateDictionary(getDictionary(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(getFunctionApplication(expression), environment);
        
        default: throw UnexpectedExpression(expression.type, "evaluate operation");
    }
}
