#include "number.h"

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

double number(const ExpressionPointer& expression_smart) {
    const auto type = expression_smart.type;
    const auto expression = expression_smart.get();
    if (type == NUMBER) {
        return dynamic_cast<const Number *>(expression)->value;
    }
    throw std::runtime_error{"Expected number"};
}
