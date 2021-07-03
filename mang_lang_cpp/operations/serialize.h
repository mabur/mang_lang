#pragma once

#include "../expressions/Expression.h"

struct Character;
struct Conditional;

std::string serialize(const Character& character);
std::string serialize(const Conditional& conditional);
