#pragma once

#include <memory>

struct Character;
struct Conditional;
struct Dictionary;
struct Function;
struct Number;

std::shared_ptr<const Number> makeNumber(
    std::shared_ptr<const Number> expression);

std::shared_ptr<const Character> makeCharacter(
    std::shared_ptr<const Character> expression);

std::shared_ptr<const Conditional> makeConditional(
    std::shared_ptr<const Conditional> expression);

std::shared_ptr<const Dictionary> makeDictionary(
    std::shared_ptr<const Dictionary> expression);

std::shared_ptr<const Function> makeFunction(
    std::shared_ptr<const Function> expression);
