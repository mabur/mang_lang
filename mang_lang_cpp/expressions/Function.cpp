#include "Function.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"
#include "DictionaryElement.h"

#include "../operations/apply.h"
#include "../operations/evaluate.h"
#include "../operations/serialize.h"

ExpressionPointer Function::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}

ExpressionPointer Function::apply(ExpressionPointer input, std::ostream& log) const {
    return ::apply(this, input, log);
}
