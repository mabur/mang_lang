#include "number.h"

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

double number(const ExpressionPointer& expression_smart) {
    const auto expression = expression_smart.get();
    if (expression->type_ == NUMBER) {
        return dynamic_cast<const Number *>(expression)->value;
    }
    throw std::runtime_error{"Expected number"};
}
