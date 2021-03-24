#include "FunctionBuiltIn.h"
#include <cassert>

ExpressionPointer FunctionBuiltIn::apply(ExpressionPointer input, std::ostream&) const {
    return function(*input);
}

std::string FunctionBuiltIn::serialize() const {
    assert(false);
    return {};
}

ExpressionPointer FunctionBuiltIn::evaluate(const Expression*, std::ostream&) const {
    assert(false);
    return {};
}

ExpressionPointer FunctionBuiltIn::parse(CodeRange) {
    assert(false);
    return {};
}

bool FunctionBuiltIn::startsWith(CodeRange) {
    assert(false);
    return {};
}
