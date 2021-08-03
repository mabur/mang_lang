#include "lookup.h"

#include "../expressions/Dictionary.h"
#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

#include "list.h"

ExpressionPointer lookupExpression(const Expression* expression, const std::string& name) {
    if (!expression->environment) {
        // TODO: define evaluation exception.
        throw ParseException("Cannot find symbol " + name);
    }
    return lookup(expression->environment, name);
}

ExpressionPointer lookupDictionaryElement(const DictionaryElement* element, const std::string& name) {
    if (element->name->value == name) {
        return element->expression;
    }
    return ExpressionPointer{};
}

ExpressionPointer lookupDictionary(const Dictionary* dictionary, const std::string& name) {
    for (const auto& element : dictionary->elements) {
        if (element.get()) {
            auto expression = lookupDictionaryElement(element.getDictionaryElement(), name);
            if (expression) {
                return expression;
            }
        }
    }
    return lookupExpression(dictionary, name);
}

ExpressionPointer lookupList(const ExpressionPointer& list, const std::string& name) {
    if (name == "first") {
        return ::list(list)->first;
    }
    if (name == "rest") {
        return makeList(std::make_shared<List>(list->range, ExpressionPointer{}, ::list(list)->rest));
    }
    throw ParseException("List does not contain symbol " + name);
}

ExpressionPointer lookupString(const ExpressionPointer& list, const std::string& name) {
    if (name == "first") {
        return ::list(list)->first;
    }
    if (name == "rest") {
        return makeString(std::make_shared<String>(list->range, ExpressionPointer{}, ::list(list)->rest));
    }
    throw ParseException("List does not contain symbol " + name);
}

ExpressionPointer lookup(const ExpressionPointer& expression_smart, const std::string& name) {
    const auto type = expression_smart.type;
    const auto expression = expression_smart.get();
    if (type == DICTIONARY) {
        return lookupDictionary(dynamic_cast<const Dictionary *>(expression), name);
    }
    if (type == NAMED_ELEMENT) {
        return lookupDictionaryElement(dynamic_cast<const DictionaryElement *>(expression), name);
    }
    if (type == LIST) {
        return lookupList(expression_smart, name);
    }
    if (type == STRING) {
        return lookupString(expression_smart, name);
    }
    return lookupExpression(expression, name);
}
