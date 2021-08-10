#pragma once

#include <memory>

#include <memory>

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
    EMPTY,
};

struct ExpressionPointer {
    ExpressionType type = EMPTY;
    size_t index = 0;
    operator bool () const;
    DictionaryElement dictionaryElement() const;
    Number number() const;
    Character character() const;
    Conditional conditional() const;
    Dictionary dictionary() const;
    Function function() const;
    FunctionBuiltIn functionBuiltIn() const;
    FunctionDictionary functionDictionary() const;
    FunctionList functionList() const;
    List list() const;
    LookupChild lookupChild() const;
    LookupFunction lookupFunction() const;
    LookupSymbol lookupSymbol() const;
    Name name() const;
    String string() const;
};

ExpressionPointer makeNumber(const Number* expression);
ExpressionPointer makeCharacter(const Character* expression);
ExpressionPointer makeConditional(const Conditional* expression);
ExpressionPointer makeDictionary(const Dictionary* expression);
ExpressionPointer makeFunction(const Function* expression);
ExpressionPointer makeFunctionBuiltIn(const FunctionBuiltIn* expression);
ExpressionPointer makeFunctionDictionary(const FunctionDictionary* expression);
ExpressionPointer makeFunctionList(const FunctionList* expression);
ExpressionPointer makeList(const List* expression);
ExpressionPointer makeLookupChild(const LookupChild* expression);
ExpressionPointer makeLookupFunction(std::shared_ptr<const LookupFunction> expression);
ExpressionPointer makeLookupSymbol(std::shared_ptr<const LookupSymbol> expression);
ExpressionPointer makeName(std::shared_ptr<const Name> expression);
ExpressionPointer makeString(std::shared_ptr<const String> expression);

ExpressionPointer makeTypedDictionaryElement(
    std::shared_ptr<const DictionaryElement> expression,
    ExpressionType type
);
