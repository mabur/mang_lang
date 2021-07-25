#include "Expression.h"

#include "../mang_lang.h"
#include "Character.h"
#include "Conditional.h"
#include "Dictionary.h"
#include "Function.h"
#include "FunctionDictionary.h"
#include "FunctionList.h"
#include "List.h"
#include "LookupChild.h"
#include "LookupFunction.h"
#include "LookupSymbol.h"
#include "Number.h"
#include "String.h"

#include "../operations/parse.h"
#include "../operations/lookup.h"

const CodeCharacter* Expression::begin() const {
    return range_.begin();
}

const CodeCharacter* Expression::end() const {
    return range_.end();
}

CodeRange Expression::range() const {
    return range_;
}

const Expression* Expression::environment() const {
    return environment_;
}
