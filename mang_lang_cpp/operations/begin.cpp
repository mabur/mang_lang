#include "begin.h"

#include "../expressions/Character.h"
#include "../expressions/Conditional.h"
#include "../expressions/Dictionary.h"
#include "../expressions/Function.h"
#include "../expressions/FunctionDictionary.h"
#include "../expressions/FunctionList.h"
#include "../expressions/List.h"
#include "../expressions/LookupChild.h"
#include "../expressions/LookupFunction.h"
#include "../expressions/LookupSymbol.h"
#include "../expressions/Number.h"
#include "../expressions/String.h"

const CodeCharacter* begin(ExpressionPointer expression) {
    switch (expression.type) {
        case CHARACTER: return expression.character().range.first;
        case CONDITIONAL: return expression.conditional().range.first;
        case DICTIONARY: return expression.dictionary().range.first;
        case NAMED_ELEMENT: return expression.dictionaryElement().range.first;
        case WHILE_ELEMENT: return expression.dictionaryElement().range.first;
        case END_ELEMENT: return expression.dictionaryElement().range.first;
        case FUNCTION: return expression.function().range.first;
        case FUNCTION_DICTIONARY: return expression.functionDictionary().range.first;
        case FUNCTION_LIST: return expression.functionList().range.first;
        case LIST: return expression.list().range.first;
        case LOOKUP_CHILD: return expression.lookupChild().range.first;
        case LOOKUP_FUNCTION: return expression.lookupFunction().range.first;
        case LOOKUP_SYMBOL: return expression.lookupSymbol().range.first;
        case NAME: return expression.name().range.first;
        case NUMBER: return expression.number().range.first;
        case STRING: return expression.string().range.first;
        default: throw std::runtime_error{"Missing begin operation for : " + std::to_string(expression.type)};
    }
}
