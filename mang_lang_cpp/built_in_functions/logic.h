#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

#include "../operations/boolean.h"
#include "../operations/list.h"

#include "../factory.h"

namespace logic {

bool isTrue(const ExpressionPointer& x) {
    return ::boolean(x);
}

bool isFalse(const ExpressionPointer& x) {
    return !::boolean(x);
}

ExpressionPointer makeNumber(double x) {
    return makeNumber(std::make_shared<Number>(CodeRange{}, ExpressionPointer{}, x));
}

ExpressionPointer boolean(const ExpressionPointer& in) {
    return makeNumber(::boolean(in));
}

ExpressionPointer logic_not(const ExpressionPointer& in) {
    return makeNumber(!::boolean(in));
}

ExpressionPointer all(const ExpressionPointer& in) {
    const auto result = !findIf(list(in), isFalse);
    return makeNumber(result);
}

ExpressionPointer any(const ExpressionPointer& in) {
    const auto result = !!findIf(list(in), isTrue);
    return makeNumber(result);
}

ExpressionPointer none(const ExpressionPointer& in) {
    const auto result = !findIf(list(in), isTrue);
    return makeNumber(result);
}

}
