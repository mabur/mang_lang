#include "number.h"

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

double number(const ExpressionPointer& expression) {
    switch (expression.type) {
        case NUMBER: return expression.number().value;
        default: throw std::runtime_error{"Expected number"};
    }
}
