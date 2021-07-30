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
    return ExpressionPointer{numbers.back()};
}

ExpressionPointer makeCharacter(std::shared_ptr<const Character> expression) {
    characters.push_back(expression);
    return ExpressionPointer{characters.back()};
}

ExpressionPointer makeConditional(std::shared_ptr<const Conditional> expression) {
    conditionals.push_back(expression);
    return ExpressionPointer{conditionals.back()};
}

ExpressionPointer makeDictionary(std::shared_ptr<const Dictionary> expression) {
    dictionaries.push_back(expression);
    return ExpressionPointer{dictionaries.back()};
}

ExpressionPointer makeFunction(std::shared_ptr<const Function> expression) {
    functions.push_back(expression);
    return ExpressionPointer{functions.back()};
}

ExpressionPointer makeFunctionBuiltIn(std::shared_ptr<const FunctionBuiltIn> expression) {
    built_in_functions.push_back(expression);
    return ExpressionPointer{built_in_functions.back()};
}

ExpressionPointer makeFunctionDictionary(std::shared_ptr<const FunctionDictionary> expression) {
    dictionary_functions.push_back(expression);
    return ExpressionPointer{dictionary_functions.back()};
}

ExpressionPointer makeFunctionList(std::shared_ptr<const FunctionList> expression) {
    list_functions.push_back(expression);
    return ExpressionPointer{list_functions.back()};
}

ExpressionPointer makeList(std::shared_ptr<const List> expression) {
    lists.push_back(expression);
    return ExpressionPointer{lists.back()};
}

ExpressionPointer makeLookupChild(std::shared_ptr<const LookupChild> expression) {
    child_lookups.push_back(expression);
    return ExpressionPointer{child_lookups.back()};
}

ExpressionPointer makeLookupFunction(std::shared_ptr<const LookupFunction> expression) {
    function_lookups.push_back(expression);
    return ExpressionPointer{function_lookups.back()};
}

ExpressionPointer makeLookupSymbol(std::shared_ptr<const LookupSymbol> expression) {
    symbol_lookups.push_back(expression);
    return ExpressionPointer{symbol_lookups.back()};
}

ExpressionPointer makeName(std::shared_ptr<const Name> expression) {
    names.push_back(expression);
    return ExpressionPointer{names.back()};
}

ExpressionPointer makeString(std::shared_ptr<const String> expression) {
    strings.push_back(expression);
    return ExpressionPointer{strings.back()};
}

ExpressionPointer makeDictionaryElement(std::shared_ptr<const DictionaryElement> expression) {
    dictionary_elements.push_back(expression);
    return ExpressionPointer{dictionary_elements.back()};
}

ExpressionPointer::operator bool () const {
    return bool(inner);
}

const Expression* ExpressionPointer::get() const {
    return inner.get();
}

const Expression* ExpressionPointer::operator -> () const {
    return get();
}
