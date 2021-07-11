#include "Dictionary.h"
#include "../mang_lang.h"
#include <algorithm>
#include <cassert>
#include <iostream>

#include "../operations/boolean.h"
#include "../operations/evaluate.h"
#include "../operations/lookup.h"
#include "../operations/serialize.h"

ExpressionPointer Dictionary::lookup(const std::string& name) const {
    return ::lookup(this, name);
}

std::string Dictionary::serialize() const {
    return ::serialize(this);
}

ExpressionPointer Dictionary::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}

bool Dictionary::boolean() const {
    return ::boolean(this);
}
