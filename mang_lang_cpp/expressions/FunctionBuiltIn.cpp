#include "FunctionBuiltIn.h"
#include <cassert>

ExpressionPointer FunctionBuiltIn::apply(ExpressionPointer input) const {
    return function(*input);
}

std::string FunctionBuiltIn::serialize() const {
    assert(false);
    return {};
}

ExpressionPointer FunctionBuiltIn::evaluate(const Expression*) const {
    assert(false);
    return {};
}

ExpressionPointer FunctionBuiltIn::parse(const CodeCharacter*, const CodeCharacter*) {
    assert(false);
    return {};
}

bool FunctionBuiltIn::startsWith(const CodeCharacter*, const CodeCharacter*) {
    assert(false);
    return {};
}
