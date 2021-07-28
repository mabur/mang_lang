#pragma once

#include <memory>

struct Character;
struct Conditional;
struct Dictionary;
struct Function;
struct FunctionBuiltIn;
struct FunctionDictionary;
struct FunctionList;
struct List;
struct Number;

std::shared_ptr<const Number> makeNumber(
    std::shared_ptr<const Number> expression);

std::shared_ptr<const Character> makeCharacter(
    std::shared_ptr<const Character> expression);

std::shared_ptr<const Conditional> makeConditional(
    std::shared_ptr<const Conditional> expression);

std::shared_ptr<const Dictionary> makeDictionary(
    std::shared_ptr<const Dictionary> expression);

std::shared_ptr<const Function> makeFunction(
    std::shared_ptr<const Function> expression);

std::shared_ptr<const FunctionBuiltIn> makeFunctionBuiltIn(
    std::shared_ptr<const FunctionBuiltIn> expression);

std::shared_ptr<const FunctionDictionary> makeFunctionDictionary(
    std::shared_ptr<const FunctionDictionary> expression);

std::shared_ptr<const FunctionList> makeFunctionList(
    std::shared_ptr<const FunctionList> expression);

std::shared_ptr<const List> makeList(
    std::shared_ptr<const List> expression);
