#include "evaluate_types.h"

#include <cassert>

#include "../built_in_functions/stack.h"
#include "../factory.h"
#include "serialize.h"
#include "../container.h"

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

std::string getNameAsLabel(Expression name)
{
    return getName(name).value;
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
    for (size_t i = 0; i < dictionary.statements.size(); ++i) {
        const auto statement = dictionary.statements[i];
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
            const auto while_statement = getWileStatement(statement);
            boolean(evaluate_types(while_statement.expression, result_environment));
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
        const auto evaluated_top = evaluate_types(top, environment);
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
        evaluated_expressions.push_back(evaluate_types(expression, environment));
    }
    const auto code = CodeRange{};
    return makeEvaluatedTuple(code, EvaluatedTuple{evaluated_expressions});
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
    const auto child = evaluate_types(lookup_child.child, environment);
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
    return evaluate_types(function.body, middle);
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
    return evaluate_types(function_dictionary.body, input);
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
    return evaluate_types(function_stack.body, middle);
}

Expression evaluateFunctionApplication(
    const FunctionApplication& function_application, Expression environment
) {
    const auto label = getNameAsLabel(function_application.name);
    const auto function = lookupDictionary(environment, label);
    const auto input = evaluate_types(function_application.child, environment);
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
        case STACK: return evaluateStack(expression, environment);
        case TUPLE: return evaluateTuple(getTuple(expression), environment);
        case LOOKUP_CHILD: return evaluateLookupChild(getLookupChild(expression), environment);
        case FUNCTION_APPLICATION: return evaluateFunctionApplication(getFunctionApplication(expression), environment);
        case LOOKUP_SYMBOL: return evaluateLookupSymbol(getLookupSymbol(expression), environment);
        default: throw UnexpectedExpression(expression.type, "evaluate types operation");
    }
}
