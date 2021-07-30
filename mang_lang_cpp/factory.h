#pragma once

#include <memory>

#include <memory>

enum ExpressionType {
    CHARACTER,
    CONDITIONAL,
    DICTIONARY,
    FUNCTION,
    FUNCTION_BUILT_IN,
    FUNCTION_DICTIONARY,
    FUNCTION_LIST,
    LIST,
    LOOKUP_CHILD,
    LOOKUP_FUNCTION,
    LOOKUP_SYMBOL,
    NAME,
    NAMED_ELEMENT,
    WHILE_ELEMENT,
    END_ELEMENT,
    NUMBER,
    STRING,
};

struct Expression;

struct ExpressionPointer {
    std::shared_ptr<const Expression> inner;
    operator bool () const;
    const Expression* get() const;
    const Expression* operator -> () const;
};


struct Character;
struct Conditional;
struct Dictionary;
struct DictionaryElement;
struct Expression;
struct Function;
struct FunctionBuiltIn;
struct FunctionDictionary;
struct FunctionList;
struct List;
struct LookupChild;
struct LookupFunction;
struct LookupSymbol;
struct Name;
struct Number;
struct String;

ExpressionPointer makeNumber(std::shared_ptr<const Number> expression);
ExpressionPointer makeCharacter(std::shared_ptr<const Character> expression);
ExpressionPointer makeConditional(std::shared_ptr<const Conditional> expression);
ExpressionPointer makeDictionary(std::shared_ptr<const Dictionary> expression);
ExpressionPointer makeFunction(std::shared_ptr<const Function> expression);
ExpressionPointer makeFunctionBuiltIn(std::shared_ptr<const FunctionBuiltIn> expression);
ExpressionPointer makeFunctionDictionary(std::shared_ptr<const FunctionDictionary> expression);
ExpressionPointer makeFunctionList(std::shared_ptr<const FunctionList> expression);
ExpressionPointer makeList(std::shared_ptr<const List> expression);
ExpressionPointer makeLookupChild(std::shared_ptr<const LookupChild> expression);
ExpressionPointer makeLookupFunction(std::shared_ptr<const LookupFunction> expression);
ExpressionPointer makeLookupSymbol(std::shared_ptr<const LookupSymbol> expression);
ExpressionPointer makeName(std::shared_ptr<const Name> expression);
ExpressionPointer makeString(std::shared_ptr<const String> expression);

ExpressionPointer makeDictionaryElement(std::shared_ptr<const DictionaryElement> expression);