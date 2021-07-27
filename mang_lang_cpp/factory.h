#pragma once

#include <memory>

struct Character;
struct Number;

std::shared_ptr<const Number> makeNumber(
    std::shared_ptr<const Number> expression);

std::shared_ptr<const Character> makeCharacter(
    std::shared_ptr<const Character> expression);
