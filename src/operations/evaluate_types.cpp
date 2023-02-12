#include "evaluate_types.h"

#include <cassert>

#include "../built_in_functions/stack.h"
#include "../container.h"
#include "../factory.h"
#include "evaluate_generic.h"
#include "serialize_types.h"

namespace {

void boolean(Expression expression) {
    switch (expression.type) {
        case NUMBER: return;
        case BOOLEAN: return;
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
        if (else_expression.type != alternative_expression.type) {
            throw StaticTypeError(else_expression.type, "Different types in is-alternatives");
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
            result.definitions.add(name, value);
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
            const auto new_value = stack_functions::put(tuple);
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
            const auto value = stack_functions::take(container);
            result.definitions.add(name_item, value);
        }
    }
    return result_environment;
}

} // namespace

Expression evaluate_types(Expression expression, Expression environment) {
    switch (expression.type) {
        case EMPTY: return expression;
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
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(evaluate_types, getFunctionApplication(expression), environment);
        case LOOKUP_SYMBOL: return lookupDictionary(getName(getLookupSymbol(expression).name), environment);
        default: throw UnexpectedExpression(expression.type, "evaluate types operation");
    }
}
