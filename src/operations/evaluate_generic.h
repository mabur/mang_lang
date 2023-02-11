#pragma once

#include <string>

#include "../container.h"
#include "../expression.h"
#include "../factory.h"

std::string getNameAsLabel(Expression name);

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
