#include "factory.h"

#include <vector>

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
std::vector<std::shared_ptr<const Number>> numbers;

std::shared_ptr<const Number> makeNumber(
    std::shared_ptr<const Number> expression
) {
    numbers.push_back(expression);
    return numbers.back();
}

std::shared_ptr<const Character> makeCharacter(
    std::shared_ptr<const Character> expression
) {
    characters.push_back(expression);
    return characters.back();
}

std::shared_ptr<const Conditional> makeConditional(
    std::shared_ptr<const Conditional> expression
) {
    conditionals.push_back(expression);
    return conditionals.back();
}

std::shared_ptr<const Dictionary> makeDictionary(
    std::shared_ptr<const Dictionary> expression
) {
    dictionaries.push_back(expression);
    return dictionaries.back();
}

std::shared_ptr<const Function> makeFunction(
    std::shared_ptr<const Function> expression
) {
    functions.push_back(expression);
    return functions.back();
}

std::shared_ptr<const FunctionBuiltIn> makeFunctionBuiltIn(
    std::shared_ptr<const FunctionBuiltIn> expression
) {
    built_in_functions.push_back(expression);
    return built_in_functions.back();
}

std::shared_ptr<const FunctionDictionary> makeFunctionDictionary(
    std::shared_ptr<const FunctionDictionary> expression
) {
    dictionary_functions.push_back(expression);
    return dictionary_functions.back();
}

std::shared_ptr<const FunctionList> makeFunctionList(
    std::shared_ptr<const FunctionList> expression
) {
    list_functions.push_back(expression);
    return list_functions.back();
}

std::shared_ptr<const List> makeList(
    std::shared_ptr<const List> expression
) {
    lists.push_back(expression);
    return lists.back();
}

std::shared_ptr<const LookupChild> makeLookupChild(
    std::shared_ptr<const LookupChild> expression
) {
    child_lookups.push_back(expression);
    return child_lookups.back();
}

std::shared_ptr<const LookupFunction> makeLookupFunction(
    std::shared_ptr<const LookupFunction> expression
) {
    function_lookups.push_back(expression);
    return function_lookups.back();
}

std::shared_ptr<const LookupSymbol> makeLookupSymbol(
    std::shared_ptr<const LookupSymbol> expression
) {
    symbol_lookups.push_back(expression);
    return symbol_lookups.back();
}
