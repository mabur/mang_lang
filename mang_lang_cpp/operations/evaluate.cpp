#include "evaluate.h"

#include <cassert>

#include "../factory.h"
#include "serialize.h"
#include "../container.h"

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
        return getNumber(left).value == getNumber(right).value;
    }
    if (left_type == CHARACTER && right_type == CHARACTER) {
        return getCharacter(left).value == getCharacter(right).value;
    }
    if (left_type == BOOLEAN && right_type == BOOLEAN) {
        return getBoolean(left).value == getBoolean(right).value;
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
        case NUMBER: return static_cast<bool>(getNumber(expression).value);
        case BOOLEAN: return getBoolean(expression).value;
        case EVALUATED_STACK: return true;
        case EMPTY_STACK: return false;
        case STRING: return true;
        case EMPTY_STRING: return false;
        default: throw UnexpectedExpression(expression.type, "boolean operation");
    }
}

std::string getNameAsLabel(Expression name)
{
    return getName(name).value;
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

Expression evaluateDictionary(
    const Dictionary& dictionary, Expression environment
) {
    const auto result_environment = makeEvaluatedDictionary(
        CodeRange{}, EvaluatedDictionary{environment, {}}
    );
    auto i = size_t{0};
    while (i < dictionary.statements.size()) {
        const auto statement = dictionary.statements[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            const auto definition = getDefinition(statement);
            const auto& right_expression = definition.expression;
            const auto value = evaluate(right_expression, result_environment);
            const auto label = getNameAsLabel(definition.name);
            auto& result = getMutableEvaluatedDictionary(result_environment);
            result.definitions.add(label, value);
            i += 1;
        }
        else if (type == WHILE_STATEMENT) {
            const auto while_statement = getWileStatement(statement);
            if (boolean(evaluate(while_statement.expression, result_environment))) {
                i += 1;
            } else {
                i = while_statement.end_index_ + 1;
            }
        }
        else if (type == END_STATEMENT) {
            const auto end_statement = getEndStatement(statement);
            i = end_statement.while_index_;
        }
    }
    return result_environment;
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

Expression evaluateStack(
    Expression stack, Expression environment
) {
    const auto op = [&](Expression rest, Expression top) -> Expression {
        const auto evaluated_top = evaluate(top, environment);
        return putEvaluatedStack(rest, evaluated_top);
    };
    const auto code = CodeRange{};
    const auto init = makeEmptyStack(code, {});
    const auto output = leftFold(init, stack, op, EMPTY_STACK, getStack);
    return reverseEvaluatedStack(code, output);
}

Expression evaluateTuple(Tuple tuple, Expression environment) {
    auto evaluated_expressions = std::vector<Expression>{};
    evaluated_expressions.reserve(tuple.expressions.size());
    for (const auto& expression : tuple.expressions) {
        evaluated_expressions.push_back(evaluate(expression, environment));
    }
    const auto code = CodeRange{};
    return makeEvaluatedTuple(code, EvaluatedTuple{evaluated_expressions});
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

Expression lookupCurrentDictionary(Expression expression, const std::string& name) {
    if (expression.type != EVALUATED_DICTIONARY) {
        throw MissingSymbol(name, "environment of type " + NAMES[expression.type]);
    }
    const auto d = getEvaluatedDictionary(expression);
    return d.definitions.lookup(name);
}

Expression lookupDictionary(Expression expression, const std::string& name) {
    const auto result = lookupCurrentDictionary(expression, name);
    if (result.type != EMPTY) {
        return result;
    }
    const auto d = getEvaluatedDictionary(expression);
    return lookupDictionary(d.environment, name);
}

Expression lookupChildInEvaluatedDictionary(const EvaluatedDictionary& dictionary, Expression name) {
    const auto label = getNameAsLabel(name);
    const auto result = dictionary.definitions.lookup(label);
    if (result.type != EMPTY) {
        return result;
    }
    throw MissingSymbol(label, "dictionary");
}

Expression lookupChildInEvaluatedTuple(const EvaluatedTuple& stack, const std::string& name) {
    const auto& expressions = stack.expressions;
    if (name == "first") return expressions.at(0);
    if (name == "second") return expressions.at(1);
    if (name == "third") return expressions.at(2);
    if (name == "fourth") return expressions.at(3);
    if (name == "fifth") return expressions.at(4);
    if (name == "sixth") return expressions.at(5);
    if (name == "seventh") return expressions.at(6);
    if (name == "eighth") return expressions.at(7);
    if (name == "ninth") return expressions.at(8);
    if (name == "tenth") return expressions.at(9);
    throw MissingSymbol(name, "evaluated_tuple");
}

Expression evaluateLookupChild(
    const LookupChild& lookup_child, Expression environment
) {
    const auto child = evaluate(lookup_child.child, environment);
    const auto name = getName(lookup_child.name).value;
    switch (child.type) {
        case EVALUATED_DICTIONARY: return lookupChildInEvaluatedDictionary(getEvaluatedDictionary(child), lookup_child.name);
        case EVALUATED_TUPLE: return lookupChildInEvaluatedTuple(getEvaluatedTuple(child), name);
        default: throw UnexpectedExpression(child.type, "evaluateLookupChild");
    }
}

Expression applyFunction(const Function& function, Expression input) {

    auto definitions = Definitions{};
    const auto label = getNameAsLabel(function.input_name);
    definitions.add(label, input);
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{function.environment, definitions}
    );
    return evaluate(function.body, middle);
}

Expression applyFunctionBuiltIn(
    const FunctionBuiltIn& function_built_in, Expression input
) {
    return function_built_in.function(input);
}

Expression applyFunctionDictionary(
    const FunctionDictionary& function_dictionary, Expression input
) {
    // TODO: pass along environment? Is some use case missing now?
    return evaluate(function_dictionary.body, input);
}

Expression applyFunctionTuple(const FunctionTuple& function_stack, Expression input
) {
    auto tuple = getEvaluatedTuple(input);
    const auto& input_names = function_stack.input_names;
    if (input_names.size() != tuple.expressions.size()) {
        throw std::runtime_error{"Wrong number of input to function"};
    }
    auto definitions = Definitions{};
    const auto num_inputs = input_names.size();
    for (size_t i = 0; i < num_inputs; ++i) {
        const auto label = getNameAsLabel(input_names[i]);
        const auto expression = tuple.expressions[i];
        definitions.add(label, expression);
    }
    const auto middle = makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{function_stack.environment, definitions}
    );
    return evaluate(function_stack.body, middle);
}

Expression evaluateFunctionApplication(
    const FunctionApplication& function_application, Expression environment
) {
    const auto label = getNameAsLabel(function_application.name);
    const auto function = lookupDictionary(environment, label);
    const auto input = evaluate(function_application.child, environment);
    switch (function.type) {
        case FUNCTION: return applyFunction(getFunction(function), input);
        case FUNCTION_BUILT_IN: return applyFunctionBuiltIn(getFunctionBuiltIn(function), input);
        case FUNCTION_DICTIONARY: return applyFunctionDictionary(getFunctionDictionary(function), input);
        case FUNCTION_TUPLE: return applyFunctionTuple(
                getFunctionTuple(function), input);
        default: throw UnexpectedExpression(function.type, "evaluateFunctionApplication");
    }
}

Expression evaluateLookupSymbol(
    const LookupSymbol& lookup_symbol, Expression environment
) {
    return lookupDictionary(environment, getNameAsLabel(lookup_symbol.name));
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
        case STACK: return evaluateStack(expression, environment);
        case TUPLE: return evaluateTuple(getTuple(expression), environment);
        case TABLE: return evaluateTable(getTable(expression), environment);
        case LOOKUP_CHILD: return evaluateLookupChild(getLookupChild(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(getFunctionApplication(expression), environment);
        case LOOKUP_SYMBOL: return evaluateLookupSymbol(getLookupSymbol(expression), environment);
        default: throw UnexpectedExpression(expression.type, "evaluate operation");
    }
}
