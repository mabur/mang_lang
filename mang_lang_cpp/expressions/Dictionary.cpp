#include "Dictionary.h"
#include "../mang_lang.h"
#include <algorithm>
#include <cassert>
#include <iostream>

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

ExpressionPointer Dictionary::lookup(const std::string& name) const {
    for (const auto& element : elements) {
        if (element) {
            auto expression = element->lookup(name);
            if (expression) {
                return expression;
            }
        }
    }
    return Expression::lookup(name);
}

std::string Dictionary::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer Dictionary::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}

bool Dictionary::boolean() const {
    return !elements.empty();
}

bool Dictionary::startsWith(CodeRange code) {
    return ::startsWith(code, '{');
}
