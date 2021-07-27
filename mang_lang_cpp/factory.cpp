#include "factory.h"

#include <vector>

std::vector<std::shared_ptr<const Character>> characters;
std::vector<std::shared_ptr<const Number>> numbers;

std::shared_ptr<const Number> makeNumber(
    std::shared_ptr<const Number> expression
) {
    numbers.push_back(expression);
    return numbers.back();
}

std::shared_ptr<const Character> makeCharacter(
    std::shared_ptr<const Character> expression
) {
    characters.push_back(expression);
    return characters.back();
}
