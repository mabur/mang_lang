#include "serialize.h"

#include "../expressions/Character.h"

std::string serialize(const Character& character) {
    return "\'" + std::string{character.value} + "\'";
}
