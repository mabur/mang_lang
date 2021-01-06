#pragma once

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

namespace logic {

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
    for (const auto& element : in.list()) {
        if (!element->boolean()) {
            return makeNumber(in, false);
        }
    }
    return makeNumber(in, true);
}

ExpressionPointer any(const Expression& in) {
    for (const auto& element : in.list()) {
        if (element->boolean()) {
            return makeNumber(in, true);
        }
    }
    return makeNumber(in, false);
}

ExpressionPointer none(const Expression& in) {
    for (const auto& element : in.list()) {
        if (element->boolean()) {
            return makeNumber(in, false);
        }
    }
    return makeNumber(in, true);
}

}
