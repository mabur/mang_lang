#include "begin.h"

#include "../Expression.h"

const CodeCharacter* begin(ExpressionPointer expression) {
    switch (expression.type) {
        case CHARACTER: return character(expression).range.first;
        case CONDITIONAL: return conditional(expression).range.first;
        case DICTIONARY: return dictionary(expression).range.first;
        case NAMED_ELEMENT: return namedElement(expression).range.first;
        case WHILE_ELEMENT: return whileElement(expression).range.first;
        case END_ELEMENT: return endElement(expression).range.first;
        case FUNCTION: return function(expression).range.first;
        case FUNCTION_DICTIONARY: return functionDictionary(expression).range.first;
        case FUNCTION_LIST: return functionList(expression).range.first;
        case LIST: return list2(expression).range.first;
        case LOOKUP_CHILD: return lookupChild(expression).range.first;
        case FUNCTION_APPLICATION: return functionApplication(expression).range.first;
        case LOOKUP_SYMBOL: return lookupSymbol(expression).range.first;
        case NAME: return name(expression).range.first;
        case NUMBER: return number2(expression).range.first;
        case STRING: return string(expression).range.first;
        default: throw std::runtime_error{"Missing begin operation for : " + std::to_string(expression.type)};
    }
}
