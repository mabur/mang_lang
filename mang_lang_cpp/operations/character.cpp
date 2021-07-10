#include "character.h"

#include "../expressions/Character.h"
#include "../expressions/Expression.h"

char character(const Expression* expression) {
    if (expression->type_ == CHARACTER) {
        return dynamic_cast<const Character *>(expression)->value;
    }
    throw std::runtime_error{"Expected character"};
}
