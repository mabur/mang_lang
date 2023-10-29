#pragma once

#include "../expression.h"

template<typename T, typename Operation, typename Getter>
T leftFold(T value, Expression expression, Operation operation, int empty_type, Getter getter) {
    while (expression.type != empty_type) {
        const auto container = getter(expression);
        value = operation(value, container.top);
        expression = container.rest;
    }
    return value;
}

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

Expression putString(Expression rest, Expression top);
Expression putStack(Expression rest, Expression top);
Expression putEvaluatedStack(Expression rest, Expression top);

Expression reverseEvaluatedStack(CodeRange code, Expression stack);

namespace container_functions {

Expression clear(Expression in);
Expression clearTyped(Expression in);
Expression put(Expression in);
Expression putTyped(Expression in);
Expression take(Expression in);
Expression takeTyped(Expression in);
Expression drop(Expression in);
Expression dropTyped(Expression in);
Expression get(Expression in);
Expression getTyped(Expression in);

}
