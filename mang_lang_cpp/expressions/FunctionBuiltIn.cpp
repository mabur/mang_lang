#include "FunctionBuiltIn.h"
#include <cassert>

ExpressionPointer FunctionBuiltIn::apply(ExpressionPointer input) const {
    return function(*input);
}

std::string FunctionBuiltIn::serialize() const {
    assert(false);
}

ExpressionPointer FunctionBuiltIn::evaluate(const Expression* parent) const {
    assert(false);
}

ExpressionPointer FunctionBuiltIn::parse(const CodeCharacter* first, const CodeCharacter* last) {
    assert(false);
}

bool FunctionBuiltIn::startsWith(const CodeCharacter* first, const CodeCharacter* last) {
    assert(false);
}
