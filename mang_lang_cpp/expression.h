#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "parsing.h"

enum ExpressionType {
    CHARACTER,
    CONDITIONAL,
    IS,
    DICTIONARY,
    EVALUATED_DICTIONARY,
    FUNCTION,
    FUNCTION_BUILT_IN,
    FUNCTION_DICTIONARY,
    FUNCTION_STACK,
    STACK,
    EVALUATED_STACK,
    EMPTY_STACK,
    LOOKUP_CHILD,
    FUNCTION_APPLICATION,
    LOOKUP_SYMBOL,
    DYNAMIC_LOOKUP_SYMBOL,
    NAME,
    LABEL,
    DEFINITION,
    DYNAMIC_DEFINITION,
    WHILE_STATEMENT,
    END_STATEMENT,
    NUMBER,
    STRING,
    EMPTY_STRING,
    BOOLEAN,
    EMPTY,
};

const auto NAMES = std::vector<std::string>{
    "CHARACTER",
    "CONDITIONAL",
    "IS",
    "DICTIONARY",
    "EVALUATED_DICTIONARY",
    "FUNCTION",
    "FUNCTION_BUILT_IN",
    "FUNCTION_DICTIONARY",
    "FUNCTION_STACK",
    "STACK",
    "EVALUATED_STACK",
    "EMPTY_STACK",
    "LOOKUP_CHILD",
    "FUNCTION_APPLICATION",
    "LOOKUP_SYMBOL",
    "DYNAMIC_LOOKUP_SYMBOL",
    "NAME",
    "LABEL",
    "DEFINITION",
    "DYNAMIC_DEFINITION",
    "WHILE_STATEMENT",
    "END_STATEMENT",
    "NUMBER",
    "STRING",
    "EMPTY_STRING",
    "BOOLEAN",
    "EMPTY",
};

struct UnexpectedExpression : public std::runtime_error
{
    UnexpectedExpression(ExpressionType type, const std::string& location);
    using runtime_error::runtime_error;
};

struct MissingSymbol : public std::runtime_error
{
    MissingSymbol(const std::string& symbol, const std::string& location);
    using runtime_error::runtime_error;
};

struct Expression {
    ExpressionType type = EMPTY;
    size_t index = 0;
    CodeRange range;
};

struct Name {
    std::string value;
};

struct Label {
    std::string value;
};

struct Character {
    char value;
};

struct Conditional {
    Expression expression_if;
    Expression expression_then;
    Expression expression_else;
};

struct Alternative {
    Expression left;
    Expression right;
};

struct IsExpression {
    Expression input;
    std::vector<Alternative> alternatives;
    Expression expression_else;
};

struct Function {
    Expression environment;
    Expression input_name;
    Expression body;
};

struct FunctionBuiltIn {
    std::function<Expression(Expression)> function;
};

struct FunctionDictionary {
    Expression environment; // TODO: use this.
    std::vector<Expression> input_names;
    Expression body;
};

struct FunctionStack {
    Expression environment;
    std::vector<Expression> input_names;
    Expression body;
};

struct LookupChild {
    Expression name;
    Expression child;
};

struct FunctionApplication {
    Expression name;
    Expression child;
};

struct LookupSymbol {
    Expression name;
};

struct DynamicLookupSymbol {
    Expression expression;
};

struct Number {
    double value;
};

struct Boolean {
    bool value;
};

struct String {
    Expression top;
    Expression rest;
};

struct EmptyString {
};

struct Stack {
    Expression top;
    Expression rest;
};

struct EvaluatedStack {
    Expression top;
    Expression rest;
};

struct EmptyStack {
};

struct Definition {
    Expression name;
    Expression expression;
};

struct DynamicDefinition {
    Expression dynamic_name;
    Expression expression;
};

struct WhileStatement {
    Expression expression;
    size_t end_index_;
};

struct EndStatement {
    size_t while_index_;
};

using Statements = std::vector<Expression>;

class Definitions {
private:
    std::unordered_map<std::string, Expression> definitions;
    std::unordered_map<std::string, size_t> order;
public:
    bool empty() const;
    void add(const std::string& key, Expression value);
    Expression lookup(const std::string& key) const;
    std::vector<std::pair<std::string, Expression>> sorted() const;
};

struct Dictionary {
    Statements statements;
};

struct EvaluatedDictionary {
    Expression environment;
    Definitions definitions;
};
