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

bool isEqual(Expression left, Expression right);

bool isEqualString(Expression left, Expression right) {
    while (left.type != EMPTY_STRING && right.type != EMPTY_STRING) {
        const auto left_string = getString(left);
        const auto right_string = getString(right);
        if (!isEqual(left_string.first, right_string.first)) {
            return false;
        }
        left = left_string.rest;
        right = right_string.rest;
    }
    return left.type == EMPTY_STRING && right.type == EMPTY_STRING;
}

bool isEqualList(Expression left, Expression right) {
    while (left.type != EMPTY_LIST && right.type != EMPTY_LIST) {
        const auto left_list = getList(left);
        const auto right_list = getList(right);
        if (!isEqual(left_list.first, right_list.first)) {
            return false;
        }
        left = left_list.rest;
        right = right_list.rest;
    }
    return left.type == EMPTY_LIST && right.type == EMPTY_LIST;
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
        return isEqualList(left, right);
    }
    if (left_type == EMPTY_STRING && right_type == EMPTY_STRING) {
        return true;
    }
    if (left_type == STRING && right_type == STRING) {
        return isEqualString(left, right);
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
