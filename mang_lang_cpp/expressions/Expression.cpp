#include "Expression.h"

#include "../mang_lang.h"
#include "Conditional.h"
#include "Dictionary.h"
#include "Function.h"
#include "List.h"
#include "LookupChild.h"
#include "LookupFunction.h"
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
    if (!parent()) {
        throw ParseException("Cannot find symbol " + name);
    }
    return parent()->lookup(name);
}

ExpressionPointer Expression::parse(
    const CodeCharacter* first, const CodeCharacter* last
) {
    auto it = first;
    it = parseWhiteSpace(it, last);
    verifyThisIsNotTheEnd(it, last);
    if (List::startsWith(it, last)) {return List::parse(it, last);}
    if (Dictionary::startsWith(it, last)) {return Dictionary::parse(it, last);}
    if (Number::startsWith(it, last)) {return Number::parse(it, last);}
    if (String::startsWith(it, last)) {return String::parse(it, last);}
    if (Conditional::startsWith(it, last)) {return Conditional::parse(it, last);}
    if (Function::startsWith(it, last)) {return Function::parse(it, last);}
    if (LookupChild::startsWith(it, last)) {return LookupChild::parse(it, last);}
    if (LookupFunction::startsWith(it, last)) {return LookupFunction::parse(it, last);}
    if (LookupSymbol::startsWith(it, last)) {return LookupSymbol::parse(it, last);}
    throwParseException(it, last);
    return {};
}

bool Expression::startsWith(const CodeCharacter* first, const CodeCharacter* last) {
    return
        List::startsWith(first, last) ||
        Dictionary::startsWith(first, last) ||
        Number::startsWith(first, last) ||
        String::startsWith(first, last) ||
        Conditional::startsWith(first, last) ||
        Function::startsWith(first, last) ||
        LookupChild::startsWith(first, last) ||
        LookupFunction::startsWith(first, last) ||
        LookupSymbol::startsWith(first, last);
}

ExpressionPointer Expression::apply(ExpressionPointer) const {
    throw std::runtime_error{"Expected function"};
}

bool Expression::isEqual(const Expression*) const {
    return false;
}

double Expression::number() const {
    throw std::runtime_error{"Expected number"};
}

bool Expression::boolean() const {
    throw std::runtime_error{"Expected boolean"};
}

const std::vector<ExpressionPointer>& Expression::list() const {
    throw std::runtime_error{"Expected list"};
}
