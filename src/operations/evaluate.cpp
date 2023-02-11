#include "evaluate.h"

#include <cassert>

#include "../built_in_functions/stack.h"
#include "../container.h"
#include "../factory.h"
#include "evaluate_generic.h"
#include "serialize.h"

namespace {

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
    if (left_type == EMPTY && right_type == EMPTY) {
        return true;
    }
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

bool boolean(Expression expression) {
    switch (expression.type) {
        case EVALUATED_DICTIONARY: return !getEvaluatedDictionary(expression).definitions.empty();
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

Expression evaluateConditional(
    const Conditional& conditional, Expression environment
) {
    return
        boolean(evaluate(conditional.expression_if, environment)) ?
        evaluate(conditional.expression_then, environment) :
        evaluate(conditional.expression_else, environment);
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

Expression evaluateTable(Table table, Expression environment) {
    auto rows = std::map<std::string, Row>{};
    for (const auto& row : table.rows) {
        const auto key = evaluate(row.key, environment);
        const auto value = evaluate(row.value, environment);
        const auto serialized_key = serialize(key);
        rows[serialized_key] = {key, value};
    }
    const auto code = CodeRange{};
    return makeEvaluatedTable(code, EvaluatedTable{rows});
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
            const auto label = getNameAsString(definition.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            result.definitions.add(label, value);
            i += 1;
        }
        else if (type == PUT_ASSIGNMENT) {
            const auto put_assignment = getPutAssignment(statement);
            const auto& right_expression = put_assignment.expression;
            const auto value = evaluate(right_expression, result_environment);
            const auto label = getNameAsString(put_assignment.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto current = result.definitions.lookup(label);
            const auto tuple = makeEvaluatedTuple(
                {}, EvaluatedTuple{{value, current}}
            );
            const auto new_value = stack_functions::put(tuple);
            result.definitions.add(label, new_value);
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
            const auto container = lookupDictionary(for_statement.name_container, result_environment);
            if (boolean(container)) {
                const auto label_item = getNameAsString(for_statement.name_item);
                const auto item = stack_functions::take(container);
                result.definitions.add(label_item, item);
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
            const auto label_container = getNameAsString(
                for_statement.name_container);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            const auto old_container = lookupDictionary(for_statement.name_container, result_environment);
            const auto new_container = stack_functions::drop(old_container);
            result.definitions.add(label_container, new_container);
        }
    }
    return result_environment;
}

} // namespace

Expression evaluate(Expression expression, Expression environment) {
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
        case FUNCTION_TUPLE: return evaluateFunctionTuple(
                getFunctionTuple(expression), environment);
        case FUNCTION_DICTIONARY: return evaluateFunctionDictionary(getFunctionDictionary(expression), environment);

        case CONDITIONAL: return evaluateConditional(getConditional(expression), environment);
        case IS: return evaluateIs(getIs(expression), environment);
        case DICTIONARY: return evaluateDictionary(getDictionary(expression), environment);
        case STACK: return evaluateStack(evaluate, expression, environment);
        case TUPLE: return evaluateTuple(evaluate, getTuple(expression), environment);
        case TABLE: return evaluateTable(getTable(expression), environment);
        case LOOKUP_CHILD: return evaluateLookupChild(evaluate, getLookupChild(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(evaluate, getFunctionApplication(expression), environment);
        case LOOKUP_SYMBOL: return lookupDictionary(getLookupSymbol(expression).name, environment);
        default: throw UnexpectedExpression(expression.type, "evaluate operation");
    }
}
