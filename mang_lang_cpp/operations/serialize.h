#pragma once

#include "../expressions/Expression.h"

struct Character;
struct Conditional;
struct Dictionary;

std::string serialize(const Character& character);
std::string serialize(const Conditional& conditional);
std::string serialize(const Dictionary& dictionary);
