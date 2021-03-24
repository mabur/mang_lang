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

ExpressionPointer Expression::parse(CodeRange code) {
    code = parseWhiteSpace(code);
    throwIfEmpty(code);
    if (List::startsWith(code)) {return List::parse(code);}
    if (Dictionary::startsWith(code)) {return Dictionary::parse(code);}
    if (Number::startsWith(code)) {return Number::parse(code);}
    if (String::startsWith(code)) {return String::parse(code);}
    if (Conditional::startsWith(code)) {return Conditional::parse(code);}
    if (Function::startsWith(code)) {return Function::parse(code);}
    if (LookupChild::startsWith(code)) {return LookupChild::parse(code);}
    if (LookupFunction::startsWith(code)) {return LookupFunction::parse(code);}
    if (LookupSymbol::startsWith(code)) {return LookupSymbol::parse(code);}
    throwParseException(code);
    return {};
}

bool Expression::startsWith(CodeRange code) {
    return
        List::startsWith(code) ||
        Dictionary::startsWith(code) ||
        Number::startsWith(code) ||
        String::startsWith(code) ||
        Conditional::startsWith(code) ||
        Function::startsWith(code) ||
        LookupChild::startsWith(code) ||
        LookupFunction::startsWith(code) ||
        LookupSymbol::startsWith(code);
}

ExpressionPointer Expression::apply(ExpressionPointer, std::ostream&) const {
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

const InternalList& Expression::list() const {
    throw std::runtime_error{"Expected list"};
}
