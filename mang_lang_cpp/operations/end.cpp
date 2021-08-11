#include "end.h"

#include "../expressions/Dictionary.h"

const CodeCharacter* end(ExpressionPointer expression) {
    switch (expression.type) {
        case CHARACTER: return expression.character().range.last;
        case CONDITIONAL: return expression.conditional().range.last;
        case DICTIONARY: return expression.dictionary().range.last;
        case NAMED_ELEMENT: return expression.dictionaryElement().range.last;
        case WHILE_ELEMENT: return expression.dictionaryElement().range.last;
        case END_ELEMENT: return expression.dictionaryElement().range.last;
        case FUNCTION: return expression.function().range.last;
        case FUNCTION_DICTIONARY: return expression.functionDictionary().range.last;
        case FUNCTION_LIST: return expression.functionList().range.last;
        case LIST: return expression.list().range.last;
        case LOOKUP_CHILD: return expression.lookupChild().range.last;
        case LOOKUP_FUNCTION: return expression.lookupFunction().range.last;
        case LOOKUP_SYMBOL: return expression.lookupSymbol().range.last;
        case NAME: return expression.name().range.last;
        case NUMBER: return expression.number().range.last;
        case STRING: return expression.string().range.last;
        default: throw std::runtime_error{"Missing end operation for : " + std::to_string(expression.type)};
    }
}
