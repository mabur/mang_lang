#include "begin.h"

#include "../Expression.h"

const CodeCharacter* begin(ExpressionPointer expression) {
    switch (expression.type) {
        case CHARACTER: return expression.character().range.first;
        case CONDITIONAL: return expression.conditional().range.first;
        case DICTIONARY: return expression.dictionary().range.first;
        case NAMED_ELEMENT: return expression.namedElement().range.first;
        case WHILE_ELEMENT: return expression.whileElement().range.first;
        case END_ELEMENT: return expression.endElement().range.first;
        case FUNCTION: return expression.function().range.first;
        case FUNCTION_DICTIONARY: return expression.functionDictionary().range.first;
        case FUNCTION_LIST: return expression.functionList().range.first;
        case LIST: return expression.list().range.first;
        case LOOKUP_CHILD: return expression.lookupChild().range.first;
        case FUNCTION_APPLICATION: return expression.functionApplication().range.first;
        case LOOKUP_SYMBOL: return expression.lookupSymbol().range.first;
        case NAME: return expression.name().range.first;
        case NUMBER: return expression.number().range.first;
        case STRING: return expression.string().range.first;
        default: throw std::runtime_error{"Missing begin operation for : " + std::to_string(expression.type)};
    }
}
