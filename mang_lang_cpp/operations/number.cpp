#include "number.h"

double number(ExpressionPointer expression) {
    switch (expression.type) {
        case NUMBER: return expression.number().value;
        default: throw std::runtime_error{"Expected number"};
    }
}
