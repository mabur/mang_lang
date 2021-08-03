#include "factory.h"

#include <vector>

#include "expressions/Character.h"
#include "expressions/Conditional.h"
#include "expressions/Dictionary.h"
#include "expressions/DictionaryElement.h"
#include "expressions/Expression.h"
#include "expressions/Function.h"
#include "expressions/FunctionBuiltIn.h"
#include "expressions/FunctionDictionary.h"
#include "expressions/FunctionList.h"
#include "expressions/List.h"
#include "expressions/LookupChild.h"
#include "expressions/LookupFunction.h"
#include "expressions/LookupSymbol.h"
#include "expressions/Name.h"
#include "expressions/Number.h"
#include "expressions/String.h"

std::vector<std::shared_ptr<const Character>> characters;
std::vector<std::shared_ptr<const Conditional>> conditionals;
std::vector<std::shared_ptr<const Dictionary>> dictionaries;
std::vector<std::shared_ptr<const Function>> functions;
std::vector<std::shared_ptr<const FunctionBuiltIn>> built_in_functions;
std::vector<std::shared_ptr<const FunctionDictionary>> dictionary_functions;
std::vector<std::shared_ptr<const FunctionList>> list_functions;
std::vector<std::shared_ptr<const List>> lists;
std::vector<std::shared_ptr<const LookupChild>> child_lookups;
std::vector<std::shared_ptr<const LookupFunction>> function_lookups;
std::vector<std::shared_ptr<const LookupSymbol>> symbol_lookups;
std::vector<std::shared_ptr<const Name>> names;
std::vector<std::shared_ptr<const Number>> numbers;
std::vector<std::shared_ptr<const String>> strings;

std::vector<std::shared_ptr<const DictionaryElement>> dictionary_elements;

ExpressionPointer makeNumber(std::shared_ptr<const Number> expression) {
    numbers.push_back(expression);
    return ExpressionPointer{numbers.back(), NUMBER, numbers.size() - 1};
}

ExpressionPointer makeCharacter(std::shared_ptr<const Character> expression) {
    characters.push_back(expression);
    return ExpressionPointer{characters.back(), CHARACTER, characters.size() - 1};
}

ExpressionPointer makeConditional(std::shared_ptr<const Conditional> expression) {
    conditionals.push_back(expression);
    return ExpressionPointer{conditionals.back(), CONDITIONAL, conditionals.size() - 1};
}

ExpressionPointer makeDictionary(std::shared_ptr<const Dictionary> expression) {
    dictionaries.push_back(expression);
    return ExpressionPointer{dictionaries.back(), DICTIONARY, dictionaries.size() - 1};
}

ExpressionPointer makeFunction(std::shared_ptr<const Function> expression) {
    functions.push_back(expression);
    return ExpressionPointer{functions.back(), FUNCTION, functions.size() - 1};
}

ExpressionPointer makeFunctionBuiltIn(std::shared_ptr<const FunctionBuiltIn> expression) {
    built_in_functions.push_back(expression);
    return ExpressionPointer{built_in_functions.back(), FUNCTION_BUILT_IN, built_in_functions.size() - 1};
}

ExpressionPointer makeFunctionDictionary(std::shared_ptr<const FunctionDictionary> expression) {
    dictionary_functions.push_back(expression);
    return ExpressionPointer{dictionary_functions.back(), FUNCTION_DICTIONARY, dictionary_functions.size() - 1};
}

ExpressionPointer makeFunctionList(std::shared_ptr<const FunctionList> expression) {
    list_functions.push_back(expression);
    return ExpressionPointer{list_functions.back(), FUNCTION_LIST, list_functions.size() - 1};
}

ExpressionPointer makeList(std::shared_ptr<const List> expression) {
    lists.push_back(expression);
    return ExpressionPointer{lists.back(), LIST, lists.size() - 1};
}

ExpressionPointer makeLookupChild(std::shared_ptr<const LookupChild> expression) {
    child_lookups.push_back(expression);
    return ExpressionPointer{child_lookups.back(), LOOKUP_CHILD, child_lookups.size() - 1};
}

ExpressionPointer makeLookupFunction(std::shared_ptr<const LookupFunction> expression) {
    function_lookups.push_back(expression);
    return ExpressionPointer{function_lookups.back(), LOOKUP_FUNCTION, function_lookups.size() - 1};
}

ExpressionPointer makeLookupSymbol(std::shared_ptr<const LookupSymbol> expression) {
    symbol_lookups.push_back(expression);
    return ExpressionPointer{symbol_lookups.back(), LOOKUP_SYMBOL, symbol_lookups.size() - 1};
}

ExpressionPointer makeName(std::shared_ptr<const Name> expression) {
    names.push_back(expression);
    return ExpressionPointer{names.back(), NAME, names.size() - 1};
}

ExpressionPointer makeString(std::shared_ptr<const String> expression) {
    strings.push_back(expression);
    return ExpressionPointer{strings.back(), STRING, strings.size() - 1};
}

ExpressionPointer makeDictionaryElement(std::shared_ptr<const DictionaryElement> expression) {
    dictionary_elements.push_back(expression);
    return ExpressionPointer{dictionary_elements.back(), expression->type_, dictionary_elements.size() - 1};
}

DictionaryElementPointer makeTypedDictionaryElement(std::shared_ptr<const DictionaryElement> expression) {
    dictionary_elements.push_back(expression);
    return DictionaryElementPointer{expression->type_, dictionary_elements.size() - 1};
}

ExpressionPointer::operator bool () const {
    return bool(get());
}

const Expression* ExpressionPointer::get() const {
    if (type == NUMBER) {
        return numbers.at(index).get();
    }
    if (type == CHARACTER) {
        return characters.at(index).get();
    }
    if (type == CONDITIONAL) {
        return conditionals.at(index).get();
    }
    if (type == DICTIONARY) {
        return dictionaries.at(index).get();
    }
    if (type == FUNCTION) {
        return functions.at(index).get();
    }
    if (type == FUNCTION_LIST) {
        return list_functions.at(index).get();
    }
    if (type == FUNCTION_DICTIONARY) {
        return dictionary_functions.at(index).get();
    }
    if (type == LIST) {
        return lists.at(index).get();
    }
    if (type == LOOKUP_SYMBOL) {
        return symbol_lookups.at(index).get();
    }
    if (type == LOOKUP_CHILD) {
        return child_lookups.at(index).get();
    }
    if (type == LOOKUP_FUNCTION) {
        return function_lookups.at(index).get();
    }
    if (type == NAME) {
        return names.at(index).get();
    }
    if (type == NAMED_ELEMENT) {
        return dictionary_elements.at(index).get();
    }
    if (type == END_ELEMENT) {
        return dictionary_elements.at(index).get();
    }
    if (type == WHILE_ELEMENT) {
        return dictionary_elements.at(index).get();
    }
    if (type == EMPTY) {
        return nullptr;
    }
    return inner.get();
}

const Expression* ExpressionPointer::operator -> () const {
    return get();
}

const DictionaryElement* DictionaryElementPointer::get() const {
    return dictionary_elements.at(index).get();
}
