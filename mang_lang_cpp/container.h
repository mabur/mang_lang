#pragma once

#include "expression.h"
#include "operations/boolean.h"
#include "operations/begin.h"
#include "operations/end.h"
#include "factory.h"

// Note that we need code range since we use this during parsing.
inline CodeRange addCodeRanges(Expression rest, Expression first) {
    const auto first_character = std::min(begin(rest), begin(first));
    const auto last_character = std::max(end(rest), end(first));
    return CodeRange{first_character, last_character};
}

namespace new_string {

inline Expression prepend(Expression rest, Expression first) {
    return makeString(new String{addCodeRanges(first, rest), first, rest});
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

struct BinaryInput {
    Expression left;
    Expression right;
};

BinaryInput getBinaryInput(Expression list);

inline Expression prepend(Expression rest, Expression first) {
    return makeList(new List{addCodeRanges(first, rest), first, rest});
}

template<typename T, typename Operation>
T leftFold(T value, Expression expression, Operation operation) {
    while (expression.type != EMPTY_LIST) {
        const auto list = getList(expression);
        value = operation(value, list.first);
        expression = list.rest;
    }
    return value;
}

inline Expression reverse(CodeRange code, Expression list) {
    return leftFold(makeEmptyList(new EmptyList{code}), list, prepend);
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

template<typename Predicate>
bool allOfNeighbours(Expression in, Predicate predicate) {
    if (in.type == EMPTY_LIST) {
        return true;
    }
    auto current_list = getList(in);
    auto next = current_list.rest;
    while (next.type != EMPTY_LIST) {
        const auto left = current_list.first;
        const auto next_list = getList(next);
        const auto right = next_list.first;
        if (!predicate(left, right)) {
            return false;
        }
        current_list = next_list;
        next = next_list.rest;
    }
    return true;
}

} // namespace new_list
