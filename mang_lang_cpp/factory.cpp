#include "factory.h"

#include <vector>

std::vector<std::shared_ptr<const Character>> characters;
std::vector<std::shared_ptr<const Conditional>> conditionals;
std::vector<std::shared_ptr<const Dictionary>> dictionaries;
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

std::shared_ptr<const Conditional> makeConditional(
    std::shared_ptr<const Conditional> expression
) {
    conditionals.push_back(expression);
    return conditionals.back();
}

std::shared_ptr<const Dictionary> makeDictionary(
    std::shared_ptr<const Dictionary> expression
) {
    dictionaries.push_back(expression);
    return dictionaries.back();
}