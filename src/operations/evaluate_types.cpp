#include "evaluate_types.h"

#include <cassert>

#include "../built_in_functions/stack.h"
#include "../container.h"
#include "../factory.h"
#include "evaluate_generic.h"
#include "serialize.h"
#include "serialize_types.h"

namespace {

bool areTypesConsistent(ExpressionType left, ExpressionType right) {
    if (left == ANY || right == ANY) return true;
    if (left == EMPTY_STACK && right == EVALUATED_STACK) return true;
    if (left == EVALUATED_STACK && right == EMPTY_STACK) return true;
    if (left == EMPTY_STRING && right == STRING) return true;
    if (left == STRING && right == EMPTY_STRING) return true;
    return left == right;
}
    
void boolean(Expression expression) {
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

Expression evaluateConditional(
    const Conditional& conditional, Expression environment
) {
    boolean(evaluate_types(conditional.expression_if, environment));
    const auto left = evaluate_types(conditional.expression_then, environment);
    const auto right = evaluate_types(conditional.expression_else, environment);
    if (left.type == ANY || left.type == EMPTY_STACK ||left.type == EMPTY_STRING) {
        return right;
    }
    if (right.type == ANY || right.type == EMPTY_STACK ||right.type == EMPTY_STRING) {
        return left;
    }
    if (left.type != right.type) {
        throw std::runtime_error(
            std::string{"Static type error.\n"} +
            "Cannot return two different types for if-then-else:\n" + 
            NAMES[left.type] + " and " + NAMES[right.type] 
        );
    }
    return left;
}

Expression evaluateIs(
    const IsExpression& is_expression, Expression environment
) {
    evaluate_types(is_expression.input, environment);
    for (const auto alternative : is_expression.alternatives) {
        evaluate_types(alternative.left, environment);
    }
    const auto else_expression = evaluate_types(is_expression.expression_else, environment);
    for (const auto alternative : is_expression.alternatives) {
        const auto alternative_expression = evaluate_types(alternative.right, environment);
        if (!areTypesConsistent(alternative_expression.type, else_expression.type)) {
            throw std::runtime_error(
                "Static type error. Different output types in is-alternatives " +
                NAMES[alternative_expression.type] + " & " +
                NAMES[else_expression.type]
            );
        }
    }
    return else_expression;
}

Expression evaluateDictionary(
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
            if (value.type != ANY) {
                result.definitions.add(name, value);
            }
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = getPutAssignment(statement);
            const auto& right_expression = put_assignment.expression;
            const auto value = evaluate_types(right_expression, result_environment);
            const auto name = getName(put_assignment.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto current = result.definitions.lookup(name);
            const auto tuple = makeEvaluatedTuple(
                {}, EvaluatedTuple{{value, current}}
            );
            const auto new_value = stack_functions::putTyped(tuple);
            result.definitions.add(name, new_value);
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWhileStatement(statement);
            boolean(evaluate_types(while_statement.expression, result_environment));
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = getForStatement(statement);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto name_container = getName(for_statement.name_container);
            const auto container = lookupDictionary(name_container, result_environment);
            boolean(container);
            const auto name_item = getName(for_statement.name_item);
            const auto value = stack_functions::takeTyped(container);
            result.definitions.add(name_item, value);
        }
    }
    return result_environment;
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

Expression evaluateFunctionApplication(
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
        
        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplication");
    }
}
    
} // namespace

Expression evaluate_types(Expression expression, Expression environment) {
    switch (expression.type) {
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

        case FUNCTION: return evaluateFunction(getFunction(expression), environment);
        case FUNCTION_TUPLE: return evaluateFunctionTuple(getFunctionTuple(expression), environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(getFunctionDictionary(expression), environment);

        case CONDITIONAL: return evaluateConditional(getConditional(expression), environment);
        case IS: return evaluateIs(getIs(expression), environment);
        case DICTIONARY: return evaluateDictionary(getDictionary(expression), environment);
        case STACK: return evaluateStack(evaluate_types, expression, environment);
        case TUPLE: return evaluateTuple(evaluate_types, getTuple(expression), environment);
        case TABLE: return evaluateTable(evaluate_types, serialize_types, getTable(expression), environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate_types, getLookupChild(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(getFunctionApplication(expression), environment);
        case LOOKUP_SYMBOL: return lookupDictionary(getName(getLookupSymbol(expression).name), environment);
        default: throw UnexpectedExpression(expression.type, "evaluate types operation");
    }
}
