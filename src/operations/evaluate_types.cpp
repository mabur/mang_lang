#include "evaluate_types.h"

#include <cassert>

#include "../built_in_functions/stack.h"
#include "../container.h"
#include "../factory.h"
#include "evaluate_generic.h"

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
        default: throw StaticTypeError(expression.type, "expected type that can be evaluated to boolean");
    }
}

Expression evaluateConditional(
    const Conditional& conditional, Expression environment
) {
    boolean(evaluate_types(conditional.expression_if, environment));
    const auto left = evaluate_types(conditional.expression_then, environment);
    const auto right = evaluate_types(conditional.expression_else, environment);
    if (left.type == ANY) {
        return right;
    }
    if (right.type == ANY) {
        return left;
    }
    if (left.type != right.type) {
        throw StaticTypeError(left.type, "Different types in conditional branches");
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
            const auto& right_expression = definition.expression;
            const auto value = evaluate_types(right_expression, result_environment);
            const auto label = getNameAsLabel(definition.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            result.definitions.add(label, value);
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = getPutAssignment(statement);
            const auto& right_expression = put_assignment.expression;
            const auto value = evaluate_types(right_expression, result_environment);
            const auto label = getNameAsLabel(put_assignment.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto current = result.definitions.lookup(label);
            const auto tuple = makeEvaluatedTuple(
                {}, EvaluatedTuple{{value, current}}
            );
            const auto new_value = stack_functions::put(tuple);
            result.definitions.add(label, new_value);
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWhileStatement(statement);
            boolean(evaluate_types(while_statement.expression, result_environment));
        }
        else if (type == FOR_STATEMENT) {
            const auto for_statement = getForStatement(statement);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto container = lookupDictionary(for_statement.name_container, result_environment);
            boolean(container);
            const auto label_item = getNameAsLabel(for_statement.name_item);
            const auto item = stack_functions::take(container);
            result.definitions.add(label_item, item);
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

        case FUNCTION: return evaluateFunction(getFunction(expression), environment);
        case FUNCTION_TUPLE: return evaluateFunctionTuple(getFunctionTuple(expression), environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(getFunctionDictionary(expression), environment);

        case CONDITIONAL: return evaluateConditional(getConditional(expression), environment);
        case IS: return evaluateIs(getIs(expression), environment);
        case DICTIONARY: return evaluateDictionary(getDictionary(expression), environment);
        case STACK: return evaluateStack(evaluate_types, expression, environment);
        case TUPLE: return evaluateTuple(evaluate_types, getTuple(expression), environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate_types, getLookupChild(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(evaluate_types, getFunctionApplication(expression), environment);
        case LOOKUP_SYMBOL: return lookupDictionary(getLookupSymbol(expression).name, environment);
        default: throw UnexpectedExpression(expression.type, "evaluate types operation");
    }
}
