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

ExpressionPointer evaluate(const Character& character, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Conditional& conditional, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Dictionary& dictionary, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Function& function, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const FunctionDictionary& function_dictionary, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const FunctionList& function_list, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const List& list, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const LookupChild& lookup_child, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const LookupFunction& lookup_function, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const LookupSymbol& lookup_symbol, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const Number& number, const Expression* environment, std::ostream& log);
ExpressionPointer evaluate(const String& string, const Expression* environment, std::ostream& log);
