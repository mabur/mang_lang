#pragma once

#include "expression.h"

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

template<typename Predicate, typename Getter>
bool allOfNeighbours(Expression in, Predicate predicate, int empty_type, Getter getter) {
    if (in.type == empty_type) {
        return true;
    }
    auto current_list = getter(in);
    auto next = current_list.rest;
    while (next.type != empty_type) {
        const auto left = current_list.top;
        const auto next_list = getter(next);
        const auto right = next_list.top;
        if (!predicate(left, right)) {
            return false;
        }
        current_list = next_list;
        next = next_list.rest;
    }
    return true;
}

struct BinaryInput {
    Expression left;
    Expression right;
};

BinaryInput getBinaryInput(Expression list);

Expression putString(Expression rest, Expression top);
Expression putStack(Expression rest, Expression top);
Expression putEvaluatedStack(Expression rest, Expression top);

Expression reverseString(CodeRange code, Expression string);
Expression reverseStack(CodeRange code, Expression stack);
Expression reverseEvaluatedStack(CodeRange code, Expression stack);
