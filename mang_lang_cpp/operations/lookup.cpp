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
        case CHARACTER: return lookup(expression.character().environment, name);
        case DICTIONARY: return lookupDictionary(expression.dictionary(), name);
        case LIST: return lookupList(expression.list(), name);
        case STRING: return lookupString(expression.string(), name);
        case EMPTY: throw ParseException("Cannot find symbol " + name);
        case CONDITIONAL: return lookup(expression.conditional().environment, name);
        case FUNCTION: return lookup(expression.function().environment, name);
        case FUNCTION_BUILT_IN: return lookup(expression.functionBuiltIn().environment, name);
        case FUNCTION_DICTIONARY: return lookup(expression.functionDictionary().environment, name);
        case FUNCTION_LIST: return lookup(expression.functionList().environment, name);
        case LOOKUP_CHILD: return lookup(expression.lookupChild().environment, name);
        case LOOKUP_FUNCTION: return lookup(expression.lookupFunction().environment, name);
        case LOOKUP_SYMBOL: return lookup(expression.lookupSymbol().environment, name);
        case NAME: return lookup(expression.name().environment, name);
        case NUMBER: return lookup(expression.number().environment, name);
        case NAMED_ELEMENT: return lookup(expression.dictionaryElement().environment, name);
        case WHILE_ELEMENT: return lookup(expression.dictionaryElement().environment, name);
        case END_ELEMENT: return lookup(expression.dictionaryElement().environment, name);
    }
    throw std::runtime_error{"Did not recognize expression to lookup"};
}
