#pragma once

#include <memory>

struct Character;
struct Conditional;
struct Number;

std::shared_ptr<const Number> makeNumber(
    std::shared_ptr<const Number> expression);

std::shared_ptr<const Character> makeCharacter(
    std::shared_ptr<const Character> expression);

std::shared_ptr<const Conditional> makeConditional(
    std::shared_ptr<const Conditional> expression);
