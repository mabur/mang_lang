#pragma once

#include <string>

struct Expression;

void serialize_types(std::string& s, Expression expression);
void serialize(std::string& s, Expression expression);
