#pragma once

#include <memory>

struct Number;

std::shared_ptr<const Number> makeNumber(
    std::shared_ptr<const Number> expression);
