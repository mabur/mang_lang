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
struct FunctionList;
struct List;
struct LookupChild;
struct LookupFunction;
struct LookupSymbol;
struct Name;
struct Number;

std::string serialize(const Character& character);
std::string serialize(const Conditional& conditional);
std::string serialize(const Dictionary& dictionary);

std::string serialize(const NamedElement& element);
std::string serialize(const WhileElement& element);
std::string serialize(const EndElement& element);

std::string serialize(const Function& function);
std::string serialize(const FunctionDictionary& function_dictionary);
std::string serialize(const FunctionList& function_list);
std::string serialize(const List& list);
std::string serialize(const LookupChild& lookup_child);
std::string serialize(const LookupFunction& lookup_function);
std::string serialize(const LookupSymbol& lookup_symbol);
std::string serialize(const Name& name);
std::string serialize(const Number& number);
