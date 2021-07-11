#include "FunctionBuiltIn.h"
#include <cassert>

#include "../operations/apply.h"

ExpressionPointer FunctionBuiltIn::apply(ExpressionPointer input, std::ostream& log) const {
    return ::apply(this, input, log);
}

ExpressionPointer FunctionBuiltIn::evaluate(const Expression*, std::ostream&) const {
    assert(false);
    return {};
}
