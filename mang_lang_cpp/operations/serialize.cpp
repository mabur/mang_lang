#include "serialize.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"

std::string serialize(const Character& character) {
    return "\'" + std::string{character.value} + "\'";
}

std::string serialize(const Conditional& conditional) {
    return "if " + conditional.expression_if->serialize() +
        " then " + conditional.expression_then->serialize() +
        " else " + conditional.expression_else->serialize();
}
