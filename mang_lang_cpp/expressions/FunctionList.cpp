#include "FunctionList.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"
#include "DictionaryElement.h"

#include "../operations/apply.h"
#include "../operations/evaluate.h"
#include "../operations/serialize.h"

ExpressionPointer FunctionList::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(this, environment, log);
}
