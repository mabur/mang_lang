#include "end.h"

#include "../Expression.h"

const CodeCharacter* end(ExpressionPointer expression) {
    switch (expression.type) {
        case CHARACTER: return character(expression).range.last;
        case CONDITIONAL: return conditional(expression).range.last;
        case DICTIONARY: return dictionary(expression).range.last;
        case NAMED_ELEMENT: return namedElement(expression).range.last;
        case WHILE_ELEMENT: return whileElement(expression).range.last;
        case END_ELEMENT: return endElement(expression).range.last;
        case FUNCTION: return function(expression).range.last;
        case FUNCTION_DICTIONARY: return functionDictionary(expression).range.last;
        case FUNCTION_LIST: return functionList(expression).range.last;
        case LIST: return list2(expression).range.last;
        case LOOKUP_CHILD: return lookupChild(expression).range.last;
        case FUNCTION_APPLICATION: return functionApplication(expression).range.last;
        case LOOKUP_SYMBOL: return lookupSymbol(expression).range.last;
        case NAME: return name(expression).range.last;
        case NUMBER: return number2(expression).range.last;
        case STRING: return string(expression).range.last;
        default: throw std::runtime_error{"Missing end operation for : " + std::to_string(expression.type)};
    }
}
