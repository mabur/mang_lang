#pragma once

#include "../expressions/Expression.h"

struct Character;
struct Conditional;
struct Dictionary;
struct Function;
struct FunctionDictionary;
struct FunctionList;
struct List;
struct LookupChild;
struct LookupFunction;
struct LookupSymbol;
struct Number;
struct String;

ExpressionPointer evaluateCharacter(const Character& character, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateConditional(const Conditional& conditional, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateDictionary(const Dictionary& dictionary, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateFunction(const Function& function, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateFunctionDictionary(const FunctionDictionary& function_dictionary, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateFunctionList(const FunctionList& function_list, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateList(const List& list, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateLookupChild(const LookupChild& lookup_child, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateLookupFunction(const LookupFunction& lookup_function, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateLookupSymbol(const LookupSymbol& lookup_symbol, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateNumber(const Number& number, const Expression* environment, std::ostream& log);
ExpressionPointer evaluateString(const String& string, const Expression* environment, std::ostream& log);
