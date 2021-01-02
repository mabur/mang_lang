#include "Expression.h"
#include "../mang_lang.h"
#include "Conditional.h"
#include "Dictionary.h"
#include "List.h"
#include "LookupSymbol.h"
#include "Number.h"
#include "String.h"

const CodeCharacter* Expression::begin() const {
    return first_;
}

const CodeCharacter* Expression::end() const {
    return last_;
}

const Expression* Expression::parent() const {
    return parent_;
}

ExpressionPointer Expression::lookup(const std::string& name) const {
    if (parent()) {
        return parent()->lookup(name);
    }
    return {};
}

bool Expression::isTrue() const {
    return false;
}

ExpressionPointer Expression::parse(
    const CodeCharacter* first, const CodeCharacter* last
) {
    auto it = first;
    it = parseWhiteSpace(it, last);
    if (it == last) {throw ParseException("could not parse expression");}
    if (List::isList(*it)) {return List::parse(it, last);}
    if (Dictionary::isDictionary(*it)) {return Dictionary::parse(it, last);}
    if (Number::isNumber(*it)) {return Number::parse(it, last);}
    if (String::isStringSeparator(*it)) {return String::parse(it, last);}
    if (Conditional::isConditional(it)) {return Conditional::parse(it, last);}
    if (LookupSymbol::isSymbol(*it)) {return LookupSymbol::parse(it, last);}
    throw ParseException("could not parse expression");
}
