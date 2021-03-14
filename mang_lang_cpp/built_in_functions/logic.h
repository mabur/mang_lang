#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

namespace logic {

bool isTrue(const ExpressionPointer& x) {
    return x->boolean();
}

bool isFalse(const ExpressionPointer& x) {
    return !x->boolean();
}

ExpressionPointer makeNumber(const Expression& in, double x) {
    return std::make_shared<Number>(in.begin(), in.end(), nullptr, x);
}

ExpressionPointer boolean(const Expression& in) {
    return makeNumber(in, in.boolean());
}

ExpressionPointer logic_not(const Expression& in) {
    return makeNumber(in, !in.boolean());
}

ExpressionPointer all(const Expression& in) {
    const auto result = !findIf(in.list(), isFalse);
    return makeNumber(in, result);
}

ExpressionPointer any(const Expression& in) {
    const auto result = !!findIf(in.list(), isTrue);
    return makeNumber(in, result);
}

ExpressionPointer none(const Expression& in) {
    const auto result = !findIf(in.list(), isTrue);
    return makeNumber(in, result);
}

}