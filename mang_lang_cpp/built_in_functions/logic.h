#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

#include "../operations/boolean.h"
#include "../operations/list.h"

#include "../factory.h"

namespace logic {

bool isTrue(ExpressionPointer x) {
    return ::boolean(x);
}

bool isFalse(ExpressionPointer x) {
    return !::boolean(x);
}

ExpressionPointer makeNumber(double x) {
    return makeNumber(std::make_shared<Number>(Number{CodeRange{}, ExpressionPointer{}, x}));
}

ExpressionPointer boolean(ExpressionPointer in) {
    return makeNumber(::boolean(in));
}

ExpressionPointer logic_not(ExpressionPointer in) {
    return makeNumber(!::boolean(in));
}

ExpressionPointer all(ExpressionPointer in) {
    const auto result = !findIf(list(in), isFalse);
    return makeNumber(result);
}

ExpressionPointer any(ExpressionPointer in) {
    const auto result = !!findIf(list(in), isTrue);
    return makeNumber(result);
}

ExpressionPointer none(ExpressionPointer in) {
    const auto result = !findIf(list(in), isTrue);
    return makeNumber(result);
}

}
