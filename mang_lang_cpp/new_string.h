#pragma once

#include "Expression.h"
#include "operations/boolean.h"
#include "operations/begin.h"
#include "operations/end.h"
#include "factory.h"

namespace new_string {

Expression first(Expression list);
Expression rest(Expression list);
Expression second(Expression list);

inline Expression prepend(Expression rest, Expression first) {
    const auto first_character = std::min(begin(rest), begin(first));
    const auto last_character = std::max(end(rest), end(first));
    const auto code = CodeRange{first_character, last_character};
    return makeString(new String{code, first, rest});
}

template<typename T, typename Operation>
T leftFold(T value, Expression expression, Operation operation) {
    while (expression.type != EMPTY_STRING) {
        const auto string = getString(expression);
        value = operation(value, string.first);
        expression = string.rest;
    }
    return value;
}

inline Expression reverse(CodeRange code, Expression list) {
    return leftFold(makeEmptyString(new EmptyString{code}), list, prepend);
}

template<typename Function>
Expression map(Expression list, Function f) {
    const auto op = [&](Expression new_list, Expression x) -> Expression {
        return prepend(new_list, f(x));
    };
    const auto code = CodeRange{};
    const auto output = leftFold(makeEmptyString(new EmptyString{}), list, op);
    return reverse(code, output);
}

template<typename Predicate>
bool allOfPairs(Expression left, Expression right, Predicate predicate) {
    while (left.type != EMPTY_STRING && right.type != EMPTY_STRING) {
        const auto left_string = getString(left);
        const auto right_string = getString(right);
        if (!predicate(left_string.first, right_string.first)) {
            return false;
        }
        left = left_string.rest;
        right = right_string.rest;
    }
    return left.type == EMPTY_STRING && right.type == EMPTY_STRING;
}

} // namespace new_string

namespace new_list {

Expression first(Expression list);
Expression rest(Expression list);
Expression second(Expression list);

inline Expression prepend(Expression rest, Expression first) {
    const auto first_character = std::min(begin(rest), begin(first));
    const auto last_character = std::max(end(rest), end(first));
    const auto code = CodeRange{first_character, last_character};
    return makeList(new List{code, first, rest});
}

template<typename T, typename Operation>
T leftFold(T value, Expression list, Operation operation) {
    for (; boolean(list); list = rest(list)) {
        value = operation(value, first(list));
    }
    return value;
}

inline Expression reverse(CodeRange code, Expression list) {
    return leftFold(makeEmptyList(new EmptyList{code}), list, prepend);
}

template<typename Function>
Expression map(Expression list, Function f) {
    const auto op = [&](Expression new_list, Expression x) -> Expression {
        return prepend(new_list, f(x));
    };
    const auto code = CodeRange{};
    const auto output = leftFold(makeEmptyList(new EmptyList{}), list, op);
    return reverse(code, output);
}

template<typename Predicate>
bool allOf(Expression expression, Predicate predicate) {
    while (expression.type != EMPTY_LIST) {
        const auto list = getList(expression);
        if (!predicate(list.first)) {
            return false;
        }
        expression = list.rest;
    }
    return true;
}

template<typename Predicate>
bool allOfPairs(Expression left, Expression right, Predicate predicate) {
    while (left.type != EMPTY_LIST && right.type != EMPTY_LIST) {
        const auto left_string = getList(left);
        const auto right_string = getList(right);
        if (!predicate(left_string.first, right_string.first)) {
            return false;
        }
        left = left_string.rest;
        right = right_string.rest;
    }
    return left.type == EMPTY_LIST && right.type == EMPTY_LIST;
}

} // namespace new_list
