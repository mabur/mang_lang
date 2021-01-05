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

}
