#include "lookup.h"

#include "../expressions/Dictionary.h"
#include "../expressions/List.h"
#include "../expressions/String.h"

ExpressionPointer lookupDictionary(const Dictionary& dictionary, const std::string& name) {
    for (const auto& element : dictionary.elements) {
        if (element) {
            const auto dictionary_element = element.dictionaryElement();
            if (dictionary_element.name->value == name) {
                return dictionary_element.expression;
            }
        }
    }
    return lookup(dictionary.environment, name);
}

ExpressionPointer lookupList(const List& list, const std::string& name) {
    if (name == "first") {
        return list.elements->first;
    }
    if (name == "rest") {
        return makeList(std::make_shared<List>(list.range, ExpressionPointer{}, list.elements->rest));
    }
    throw ParseException("List does not contain symbol " + name);
}

ExpressionPointer lookupString(const String& string, const std::string& name) {
    if (name == "first") {
        return string.elements->first;
    }
    if (name == "rest") {
        return makeString(std::make_shared<String>(string.range, ExpressionPointer{}, string.elements->rest));
    }
    throw ParseException("List does not contain symbol " + name);
}

ExpressionPointer lookup(ExpressionPointer expression, const std::string& name) {
    switch(expression.type) {
        case DICTIONARY: return lookupDictionary(expression.dictionary(), name);
        case LIST: return lookupList(expression.list(), name);
        case STRING: return lookupString(expression.string(), name);
        case EMPTY: throw ParseException("Cannot find symbol " + name);
        default: return lookup(expression->environment, name);
    }
}
