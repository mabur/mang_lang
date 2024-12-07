#pragma once

#include <string>

struct Expression;
struct StringBuilder;

StringBuilder serialize_types(StringBuilder s, Expression expression);
StringBuilder serialize(StringBuilder s, Expression expression);
