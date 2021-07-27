#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

#include "../operations/boolean.h"
#include "../operations/list.h"

#include "../factory.h"

namespace logic {

bool isTrue(const ExpressionPointer& x) {
    return boolean(x.get());
}

bool isFalse(const ExpressionPointer& x) {
    return !boolean(x.get());
}

ExpressionPointer makeNumber(const Expression& in, double x) {
    return makeNumber(std::make_shared<Number>(in.range, nullptr, x));
}

ExpressionPointer boolean(const Expression& in) {
    return makeNumber(in, boolean(&in));
}

ExpressionPointer logic_not(const Expression& in) {
    return makeNumber(in, !boolean(&in));
}

ExpressionPointer all(const Expression& in) {
    const auto result = !findIf(list(&in), isFalse);
    return makeNumber(in, result);
}

ExpressionPointer any(const Expression& in) {
    const auto result = !!findIf(list(&in), isTrue);
    return makeNumber(in, result);
}

ExpressionPointer none(const Expression& in) {
    const auto result = !findIf(list(&in), isTrue);
    return makeNumber(in, result);
}

}