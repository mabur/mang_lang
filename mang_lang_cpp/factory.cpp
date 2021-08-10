#include "factory.h"

#include <cassert>
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

ExpressionPointer makeNumber(const Number* expression) {
    numbers.emplace_back(expression);
    return ExpressionPointer{NUMBER, numbers.size() - 1};
}

ExpressionPointer makeCharacter(const Character* expression) {
    characters.emplace_back(expression);
    return ExpressionPointer{CHARACTER, characters.size() - 1};
}

ExpressionPointer makeConditional(const Conditional* expression) {
    conditionals.emplace_back(expression);
    return ExpressionPointer{CONDITIONAL, conditionals.size() - 1};
}

ExpressionPointer makeDictionary(const Dictionary* expression) {
    dictionaries.emplace_back(expression);
    return ExpressionPointer{DICTIONARY, dictionaries.size() - 1};
}

ExpressionPointer makeFunction(const Function* expression) {
    functions.emplace_back(expression);
    return ExpressionPointer{FUNCTION, functions.size() - 1};
}

ExpressionPointer makeFunctionBuiltIn(const FunctionBuiltIn* expression) {
    built_in_functions.emplace_back(expression);
    return ExpressionPointer{FUNCTION_BUILT_IN, built_in_functions.size() - 1};
}

ExpressionPointer makeFunctionDictionary(const FunctionDictionary* expression) {
    dictionary_functions.emplace_back(expression);
    return ExpressionPointer{FUNCTION_DICTIONARY, dictionary_functions.size() - 1};
}

ExpressionPointer makeFunctionList(const FunctionList* expression) {
    list_functions.emplace_back(expression);
    return ExpressionPointer{FUNCTION_LIST, list_functions.size() - 1};
}

ExpressionPointer makeList(const List* expression) {
    lists.emplace_back(expression);
    return ExpressionPointer{LIST, lists.size() - 1};
}

ExpressionPointer makeLookupChild(const LookupChild* expression) {
    child_lookups.emplace_back(expression);
    return ExpressionPointer{LOOKUP_CHILD, child_lookups.size() - 1};
}

ExpressionPointer makeLookupFunction(const LookupFunction* expression) {
    function_lookups.emplace_back(expression);
    return ExpressionPointer{LOOKUP_FUNCTION, function_lookups.size() - 1};
}

ExpressionPointer makeLookupSymbol(const LookupSymbol* expression) {
    symbol_lookups.emplace_back(expression);
    return ExpressionPointer{LOOKUP_SYMBOL, symbol_lookups.size() - 1};
}

ExpressionPointer makeName(const Name* expression) {
    names.emplace_back(expression);
    return ExpressionPointer{NAME, names.size() - 1};
}

ExpressionPointer makeString(std::shared_ptr<const String> expression) {
    strings.push_back(expression);
    return ExpressionPointer{STRING, strings.size() - 1};
}

ExpressionPointer makeTypedDictionaryElement(
    std::shared_ptr<const DictionaryElement> expression,
    ExpressionType type
) {
    dictionary_elements.push_back(expression);
    return ExpressionPointer{type, dictionary_elements.size() - 1};
}

ExpressionPointer::operator bool () const {
    return type != EMPTY;
}

DictionaryElement ExpressionPointer::dictionaryElement() const {
    assert(type == NAMED_ELEMENT || type == WHILE_ELEMENT || type == END_ELEMENT);
    return *dictionary_elements.at(index).get();
}

Number ExpressionPointer::number() const {
    assert(type == NUMBER);
    return *numbers.at(index).get();
}

Character ExpressionPointer::character() const {
    assert(type == CHARACTER);
    return *characters.at(index).get();
}

Conditional ExpressionPointer::conditional() const {
    assert(type == CONDITIONAL);
    return *conditionals.at(index).get();
}

Dictionary ExpressionPointer::dictionary() const {
    assert(type == DICTIONARY);
    return *dictionaries.at(index).get();
}

Function ExpressionPointer::function() const {
    assert(type == FUNCTION);
    return *functions.at(index).get();
}

FunctionBuiltIn ExpressionPointer::functionBuiltIn() const {
    assert(type == FUNCTION_BUILT_IN);
    return *built_in_functions.at(index).get();
}

FunctionDictionary ExpressionPointer::functionDictionary() const {
    assert(type == FUNCTION_DICTIONARY);
    return *dictionary_functions.at(index).get();
}

FunctionList ExpressionPointer::functionList() const {
    assert(type == FUNCTION_LIST);
    return *list_functions.at(index).get();
}

List ExpressionPointer::list() const {
    assert(type == LIST);
    return *lists.at(index).get();
}

LookupChild ExpressionPointer::lookupChild() const {
    assert(type == LOOKUP_CHILD);
    return *child_lookups.at(index).get();
}

LookupFunction ExpressionPointer::lookupFunction() const {
    assert(type == LOOKUP_FUNCTION);
    return *function_lookups.at(index).get();
}

LookupSymbol ExpressionPointer::lookupSymbol() const {
    assert(type == LOOKUP_SYMBOL);
    return *symbol_lookups.at(index).get();
}

Name ExpressionPointer::name() const {
    assert(type == NAME);
    return *names.at(index).get();
}

String ExpressionPointer::string() const {
    assert(type == STRING);
    return *strings.at(index).get();
}
