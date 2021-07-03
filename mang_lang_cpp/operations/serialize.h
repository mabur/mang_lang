#pragma once

#include "../expressions/Expression.h"

struct Character;
struct Conditional;
struct Dictionary;

struct NamedElement;
struct WhileElement;
struct EndElement;

struct Function;
struct FunctionDictionary;

std::string serialize(const Character& character);
std::string serialize(const Conditional& conditional);
std::string serialize(const Dictionary& dictionary);

std::string serialize(const NamedElement& element);
std::string serialize(const WhileElement& element);
std::string serialize(const EndElement& element);

std::string serialize(const Function& function);
std::string serialize(const FunctionDictionary& function_dictionary);
