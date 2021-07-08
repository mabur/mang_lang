#include "FunctionBuiltIn.h"
#include <cassert>

#include "../operations/apply.h"

ExpressionPointer FunctionBuiltIn::apply(ExpressionPointer input, std::ostream& log) const {
    return ::applyFunctionBuiltIn(*this, input, log);
}

std::string FunctionBuiltIn::serialize() const {
    assert(false);
    return {};
}

ExpressionPointer FunctionBuiltIn::evaluate(const Expression*, std::ostream&) const {
    assert(false);
    return {};
}
