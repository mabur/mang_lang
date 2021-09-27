#include "logic.h"

#include "../factory.h"
#include "../operations/boolean.h"
#include "../new_string.h"

namespace logic {

bool isFalse(Expression x) {
    return !boolean(x);
}

Expression makeBoolean(double x) {
    return makeNumber(new Number{CodeRange{}, x});
}

Expression booleanExpression(Expression in) {
    return makeBoolean(boolean(in));
}

Expression logic_not(Expression in) {
    return makeBoolean(!boolean(in));
}

template<typename Predicate>
bool anyOf(Expression expression, Predicate predicate) {
    while (expression.type != EMPTY_LIST) {
        const auto list = getList(expression);
        if (predicate(list.first)) {
            return true;
        }
        expression = list.rest;
    }
    return false;
}

Expression all(Expression list) {
    return makeBoolean(!anyOf(list, isFalse));
}

Expression any(Expression list) {
    return makeBoolean(anyOf(list, boolean));
}

Expression none(Expression list) {
    return makeBoolean(!anyOf(list, boolean));
}

bool isEqual(Expression left, Expression right) {
    const auto left_type = left.type;
    const auto right_type = right.type;
    if (left_type == NUMBER && right_type == NUMBER) {
        return getNumber(left).value == getNumber(right).value;
    }
    if (left_type == CHARACTER && right_type == CHARACTER) {
        return getCharacter(left).value == getCharacter(right).value;
    }
    if (left_type == EMPTY_LIST && right_type == EMPTY_LIST) {
        return true;
    }
    if (left_type == LIST && right_type == LIST) {
        return new_list::allOfPairs(left, right, isEqual);
    }
    if (left_type == EMPTY_STRING && right_type == EMPTY_STRING) {
        return true;
    }
    if (left_type == STRING && right_type == STRING) {
        return new_string::allOfPairs(left, right, isEqual);
    }
    return false;
}

Expression equal(Expression in) {
    const auto left = new_list::first(in);
    const auto right = new_list::second(in);
    const auto value = isEqual(left, right);
    return makeBoolean(value);
}

Expression unequal(Expression in) {
    const auto left = new_list::first(in);
    const auto right = new_list::second(in);
    const auto value = !isEqual(left, right);
    return makeBoolean(value);
}

}
