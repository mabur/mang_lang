#include "FunctionDictionary.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"

#include "../operations/apply.h"
#include "../operations/evaluate.h"
#include "../operations/serialize.h"

ExpressionPointer FunctionDictionary::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}
