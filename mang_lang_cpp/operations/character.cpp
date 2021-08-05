#include "character.h"

#include "../expressions/Character.h"
#include "../expressions/Expression.h"

char character(const ExpressionPointer& expression_smart) {
    const auto type = expression_smart.type;
    switch (type) {
        case CHARACTER: return expression_smart.character().value;
        default: throw std::runtime_error{"Expected character"};
    }
}
