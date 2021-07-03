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
    return parent_;
}

ExpressionPointer Expression::lookup(const std::string& name) const {
    if (!environment()) {
        // TODO: define evaluation exception.
        throw ParseException("Cannot find symbol " + name);
    }
    return environment()->lookup(name);
}

bool Expression::startsWith(CodeRange code) {
    return
        List::startsWith(code) ||
        Dictionary::startsWith(code) ||
        Number::startsWith(code) ||
        Character::startsWith(code) ||
        String::startsWith(code) ||
        Conditional::startsWith(code) ||
        FunctionDictionary::startsWith(code) ||
        Function::startsWith(code) ||
        LookupChild::startsWith(code) ||
        LookupFunction::startsWith(code) ||
        LookupSymbol::startsWith(code);
}

ExpressionPointer Expression::evaluate(const Expression*, std::ostream&) const {
    throw std::runtime_error{"Expected evaluatable expression"};
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

char Expression::character() const {
    throw std::runtime_error{"Expected character"};
}

bool Expression::boolean() const {
    throw std::runtime_error{"Expected boolean"};
}

const InternalList& Expression::list() const {
    throw std::runtime_error{"Expected list"};
}

ExpressionPointer Expression::empty() const {
    throw std::runtime_error{"Expected list"};
}

ExpressionPointer Expression::prepend(ExpressionPointer) const {
    throw std::runtime_error{"Expected list"};
}
