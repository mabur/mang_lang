#include "number.h"

#include "../expressions/Expression.h"
#include "../expressions/Number.h"

double number(const Expression* expression) {
    if (expression->type_ == NUMBER) {
        return dynamic_cast<const Number *>(expression)->value;
    }
    throw std::runtime_error{"Expected number"};
}
