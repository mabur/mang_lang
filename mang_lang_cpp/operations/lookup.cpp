#include "lookup.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"
#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionBuiltIn.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"
#include "../expressions/List.h"
#include "../expressions/LookupChild.h"
#include "../expressions/LookupFunction.h"
#include "../expressions/LookupSymbol.h"
#include "../expressions/Number.h"
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
    throw std::runtime_error("Dictionary does not contain symbol " + name);
}

ExpressionPointer lookupList(const List& list, const std::string& name) {
    if (name == "first") {
        return list.elements->first;
    }
    if (name == "rest") {
        return makeList(std::make_shared<List>(List{list.range, ExpressionPointer{}, list.elements->rest}));
    }
    throw std::runtime_error("List does not contain symbol " + name);
}

ExpressionPointer lookupString(const String& string, const std::string& name) {
    if (name == "first") {
        return string.elements->first;
    }
    if (name == "rest") {
        return makeString(std::make_shared<String>(String{string.range, ExpressionPointer{}, string.elements->rest}));
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
