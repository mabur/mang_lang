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

ExpressionPointer makeNumber(const ExpressionPointer& in, double x) {
    return makeNumber(std::make_shared<Number>(in->range, nullptr, x));
}

ExpressionPointer boolean(const ExpressionPointer& in) {
    return makeNumber(in, boolean(in.get()));
}

ExpressionPointer logic_not(const ExpressionPointer& in) {
    return makeNumber(in, !boolean(in.get()));
}

ExpressionPointer all(const ExpressionPointer& in) {
    const auto result = !findIf(list(in.get()), isFalse);
    return makeNumber(in, result);
}

ExpressionPointer any(const ExpressionPointer& in) {
    const auto result = !!findIf(list(in.get()), isTrue);
    return makeNumber(in, result);
}

ExpressionPointer none(const ExpressionPointer& in) {
    const auto result = !findIf(list(in.get()), isTrue);
    return makeNumber(in, result);
}

}