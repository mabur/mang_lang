#include "logic.h"

#include "../factory.h"
#include "../operations/boolean.h"
#include "../new_string.h"

namespace logic {

InternalList list(Expression expression) {
    switch (expression.type) {
        case LIST: return getList(expression).elements;
        default: throw std::runtime_error{"Expected list"};
    }
}

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
    const auto result = !findIf(list(in), isFalse);
    return makeBoolean(result);
}

Expression any(Expression in) {
    const auto result = !!findIf(list(in), isTrue);
    return makeBoolean(result);
}

Expression none(Expression in) {
    const auto result = !findIf(list(in), isTrue);
    return makeBoolean(result);
}

bool isEqual(Expression left, Expression right);

bool isEqualList(Expression left_smart, Expression right_smart) {
    auto left = list(left_smart);
    auto right = list(right_smart);
    for (; left && right; left = left->rest, right = right->rest) {
        if (!isEqual(left->first, right->first)) {
            return false;
        }
    }
    return !left && !right;
}

bool isEqualNewString(Expression left, Expression right) {
    for (; ::boolean(left) && ::boolean(right); left = new_string::rest(left), right = new_string::rest(right)) {
        if (!isEqual(new_string::first(left), new_string::first(right))) {
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
    if (left_type == LIST && right_type == LIST) {
        return isEqualList(left, right);
    }
    if (left_type == EMPTY_STRING && right_type == EMPTY_STRING) {
        return true;
    }
    if (left_type == STRING && right_type == STRING) {
        return isEqualNewString(left, right);
    }
    return false;
}

Expression equal(Expression in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto value = isEqual(left, right);
    return makeBoolean(value);
}

Expression unequal(Expression in) {
    const auto& elements = list(in);
    const auto& left = first(elements);
    const auto& right = second(elements);
    const auto value = !isEqual(left, right);
    return makeBoolean(value);
}

}
