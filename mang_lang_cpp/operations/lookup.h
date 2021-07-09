#pragma once

#include "../expressions/Expression.h"

struct Dictionary;
struct NamedElement;

ExpressionPointer lookupDictionary(const Dictionary& dictionary, const std::string& name);
ExpressionPointer lookupNamedElement(const NamedElement& named_element, const std::string& name);
ExpressionPointer lookupExpression(const Expression& expression, const std::string& name);

template<typename List>
ExpressionPointer lookupList(const List& list, const std::string& name) {
    if (name == "first") {
        return list.list()->first;
    }
    if (name == "rest") {
        return std::make_shared<typename List::value_type>(list.range(), nullptr, list.list()->rest);
    }
    throw ParseException("List does not contain symbol " + name);
}
