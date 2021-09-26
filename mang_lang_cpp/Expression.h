#pragma once

#include <functional>
#include <string>
#include <vector>

#include "parsing.h"

enum ExpressionType {
    CHARACTER,
    CONDITIONAL,
    DICTIONARY,
    FUNCTION,
    FUNCTION_BUILT_IN,
    FUNCTION_DICTIONARY,
    FUNCTION_LIST,
    LIST,
    EMPTY_LIST,
    LOOKUP_CHILD,
    FUNCTION_APPLICATION,
    LOOKUP_SYMBOL,
    NAME,
    DEFINITION,
    WHILE_STATEMENT,
    END_STATEMENT,
    NUMBER,
    STRING,
    EMPTY_STRING,
    EMPTY,
};

struct Expression {
    ExpressionType type = EMPTY;
    size_t index = 0;
};

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
    Expression input_name;
    Expression body;
};

struct FunctionBuiltIn {
    CodeRange range;
    std::function<Expression(Expression)> function;
};

struct FunctionDictionary {
    CodeRange range;
    Expression environment; // TODO: use this.
    std::vector<Expression> input_names;
    Expression body;
};

struct FunctionList {
    CodeRange range;
    Expression environment;
    std::vector<Expression> input_names;
    Expression body;
};

struct LookupChild {
    CodeRange range;
    Expression name;
    Expression child;
};

struct FunctionApplication {
    CodeRange range;
    Expression name;
    Expression child;
};

struct LookupSymbol {
    CodeRange range;
    Expression name;
};

struct Number {
    CodeRange range;
    double value;
};

struct String {
    CodeRange range;
    Expression first;
    Expression rest;
};

struct EmptyString {
    CodeRange range;
};

struct List {
    CodeRange range;
    Expression first;
    Expression rest;
};

struct EmptyList {
    CodeRange range;
};

struct Definition {
    CodeRange range;
    Expression name;
    Expression expression;
    size_t name_index_;
};

struct WhileStatement {
    CodeRange range;
    Expression expression;
    size_t end_index_;
};

struct EndStatement {
    CodeRange range;
    size_t while_index_;
};

using Statements = std::vector<Expression>;

struct Dictionary {
    CodeRange range;
    Expression environment;
    Statements statements;
};
