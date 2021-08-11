#include "character.h"

#include "../expressions/Expression.h"

char character(ExpressionPointer expression) {
    switch (expression.type) {
        case CHARACTER: return expression.character().value;
        default: throw std::runtime_error{"Expected character"};
    }
}
