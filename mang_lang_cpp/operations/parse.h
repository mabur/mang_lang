#pragma once

#include "../expressions/Expression.h"
#include "../expressions/DictionaryElement.h"
#include "../expressions/Name.h"

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
struct String;

ExpressionPointer parseCharacterExpression(CodeRange code);
ExpressionPointer parseConditional(CodeRange code);
ExpressionPointer parseDictionary(CodeRange code);

DictionaryElementPointer parseDictionaryElement(CodeRange code);
DictionaryElementPointer parseNamedElement(CodeRange code);
DictionaryElementPointer parseWhileElement(CodeRange code);
DictionaryElementPointer parseEndElement(CodeRange code);

ExpressionPointer parseFunction(CodeRange code);
ExpressionPointer parseFunctionDictionary(CodeRange code);
ExpressionPointer parseFunctionList(CodeRange code);
ExpressionPointer parseList(CodeRange code);
ExpressionPointer parseLookupChild(CodeRange code);
ExpressionPointer parseLookupFunction(CodeRange code);
ExpressionPointer parseLookupSymbol(CodeRange code);
NamePointer parseName(CodeRange code);
ExpressionPointer parseNumber(CodeRange code);
ExpressionPointer parseString(CodeRange code);
