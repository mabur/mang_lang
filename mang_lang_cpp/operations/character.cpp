#include "character.h"

#include "../expressions/Character.h"
#include "../expressions/Expression.h"

char character(const ExpressionPointer& expression_smart) {
    const auto type = expression_smart.type;
    const auto expression = expression_smart.get();
    if (type == CHARACTER) {
        return dynamic_cast<const Character *>(expression)->value;
    }
    throw std::runtime_error{"Expected character"};
}
