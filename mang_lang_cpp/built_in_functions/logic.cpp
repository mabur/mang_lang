#include "logic.h"

#include "../factory.h"
#include "../operations/boolean.h"
#include "../new_string.h"

namespace logic {

bool isTrue(Expression x) {
    return ::boolean(x);
}

bool isFalse(Expression x) {
    return !::boolean(x);
}

Expression makeBoolean(double x) {
    return makeNumber(new Number{CodeRange{}, x});
}

Expression boolean(Expression in) {
    return makeBoolean(::boolean(in));
}

Expression logic_not(Expression in) {
    return makeBoolean(!::boolean(in));
}

Expression all(Expression in) {
    const auto result = new_list::findIf(in, isFalse).type == EMPTY_LIST;
    return makeBoolean(result);
}

Expression any(Expression in) {
    const auto result = new_list::findIf(in, isTrue).type == LIST;
    return makeBoolean(result);
}

Expression none(Expression in) {
    const auto result = new_list::findIf(in, isTrue).type == EMPTY_LIST;
    return makeBoolean(result);
}

bool isEqual(Expression left, Expression right);

bool isEqualString(Expression left, Expression right) {
    for (; ::boolean(left) && ::boolean(right); left = new_string::rest(left), right = new_string::rest(right)) {
        if (!isEqual(new_string::first(left), new_string::first(right))) {
            return false;
        }
    }
    return !::boolean(left) && !::boolean(right);
}

bool isEqualList(Expression left, Expression right) {
    for (; ::boolean(left) && ::boolean(right); left = new_list::rest(left), right = new_list::rest(right)) {
        if (!isEqual(new_list::first(left), new_list::first(right))) {
            return false;
        }
    }
    return !::boolean(left) && !::boolean(right);
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
