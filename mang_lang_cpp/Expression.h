#pragma once

#include <memory>
#include <string>
#include <vector>

#include "parsing.h"
#include "SinglyLinkedList.h"

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
    FUNCTION_APPLICATION,
    LOOKUP_SYMBOL,
    NAME,
    NAMED_ELEMENT,
    WHILE_ELEMENT,
    END_ELEMENT,
    NUMBER,
    NEW_STRING,
    NEW_EMPTY_STRING,
    EMPTY,
};

struct Expression {
    ExpressionType type = EMPTY;
    size_t index = 0;
};

using InternalList = SinglyLinkedList<Expression>;

struct Name;
using NamePointer = std::shared_ptr<const Name>;

struct Name {
    CodeRange range;
    std::string value;
};

struct Character {
    CodeRange range;
    char value;
};

struct Conditional {
    CodeRange range;
    Expression expression_if;
    Expression expression_then;
    Expression expression_else;
};

struct Function {
    CodeRange range;
    Expression environment;
    NamePointer input_name;
    Expression body;
};

struct FunctionBuiltIn {
    CodeRange range;
    std::function<Expression(Expression)> function;
};

struct FunctionDictionary {
    CodeRange range;
    Expression environment; // TODO: use this.
    std::vector<NamePointer> input_names;
    Expression body;
};

struct FunctionList {
    CodeRange range;
    Expression environment;
    std::vector<NamePointer> input_names;
    Expression body;
};

struct List {
    CodeRange range;
    InternalList elements;
};

struct LookupChild {
    CodeRange range;
    NamePointer name;
    Expression child;
};

struct FunctionApplication {
    CodeRange range;
    NamePointer name;
    Expression child;
};

struct LookupSymbol {
    CodeRange range;
    NamePointer name;
};

struct Number {
    CodeRange range;
    double value;
};

struct NewString {
    CodeRange range;
    Expression first;
    Expression rest;
};

struct NewEmptyString {
    CodeRange range;
};

struct NamedElement {
    CodeRange range;
    NamePointer name;
    Expression expression;
    size_t name_index_;
};

struct WhileElement {
    CodeRange range;
    Expression expression;
    size_t end_index_;
};

struct EndElement {
    CodeRange range;
    size_t while_index_;
};

using DictionaryElements = std::vector<Expression>;

struct Dictionary {
    CodeRange range;
    Expression environment;
    DictionaryElements elements;
};
