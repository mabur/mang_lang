#include "lookup.h"

ExpressionPointer lookupDictionary(const Dictionary& dictionary, const std::string& name) {
    for (const auto& element : dictionary.elements) {
        if (element.type == NAMED_ELEMENT) {
            const auto dictionary_element = element.namedElement();
            if (dictionary_element.name->value == name) {
                return dictionary_element.expression;
            }
        }
    }
    throw std::runtime_error("Dictionary does not contain symbol " + name);
}

ExpressionPointer lookupList(const List& list, const std::string& name) {
    if (name == "first") {
        return list.elements->first;
    }
    if (name == "rest") {
        return makeList(new List{list.range, list.elements->rest});
    }
    throw std::runtime_error("List does not contain symbol " + name);
}

ExpressionPointer lookupString(const String& string, const std::string& name) {
    if (name == "first") {
        return string.elements->first;
    }
    if (name == "rest") {
        return makeString(new String{string.range, string.elements->rest});
    }
    throw std::runtime_error("String does not contain symbol " + name);
}

ExpressionPointer lookup(ExpressionPointer expression, const std::string& name) {
    switch(expression.type) {
        case DICTIONARY: return lookupDictionary(expression.dictionary(), name);
        case LIST: return lookupList(expression.list(), name);
        case STRING: return lookupString(expression.string(), name);
        default: throw std::runtime_error{"Cannot lookup expression of type " + std::to_string(expression.type)};
    }
}
