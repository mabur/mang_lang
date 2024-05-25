#pragma once

#include <string>

struct Expression;

using SerializedString = std::string&;

void serialize_types(SerializedString s, Expression expression);
void serialize(SerializedString s, Expression expression);
