#include "FunctionBuiltIn.h"
#include <cassert>

#include "../operations/apply.h"

ExpressionPointer FunctionBuiltIn::evaluate(const Expression*, std::ostream&) const {
    assert(false);
    return {};
}
