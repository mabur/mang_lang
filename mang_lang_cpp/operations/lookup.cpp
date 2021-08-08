#include "lookup.h"

#include "../expressions/Dictionary.h"
#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

#include "list.h"

ExpressionPointer lookupDictionaryElement(const DictionaryElement& element, const std::string& name) {
    if (element.name->value == name) {
        return element.expression;
    }
    return ExpressionPointer{};
}

ExpressionPointer lookupDictionary(const Dictionary& dictionary, const std::string& name) {
    for (const auto& element : dictionary.elements) {
        if (element.get()) {
            auto expression = lookupDictionaryElement(
                element.dictionaryElement(), name);
            if (expression) {
                return expression;
            }
        }
    }
    return lookup(dictionary.environment, name);
}

ExpressionPointer lookupList(ExpressionPointer list, const std::string& name) {
    if (name == "first") {
        return ::list(list)->first;
    }
    if (name == "rest") {
        return makeList(std::make_shared<List>(list->range, ExpressionPointer{}, ::list(list)->rest));
    }
    throw ParseException("List does not contain symbol " + name);
}

ExpressionPointer lookupString(ExpressionPointer list, const std::string& name) {
    if (name == "first") {
        return ::list(list)->first;
    }
    if (name == "rest") {
        return makeString(std::make_shared<String>(list->range, ExpressionPointer{}, ::list(list)->rest));
    }
    throw ParseException("List does not contain symbol " + name);
}

ExpressionPointer lookup(ExpressionPointer expression, const std::string& name) {
    switch(expression.type) {
        case DICTIONARY: return lookupDictionary(expression.dictionary(), name);
        case LIST: return lookupList(expression, name);
        case STRING: return lookupString(expression, name);
        case EMPTY: throw ParseException("Cannot find symbol " + name);
        default: return lookup(expression->environment, name);
    }
}
