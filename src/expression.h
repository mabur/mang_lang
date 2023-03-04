#pragma once

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "parsing.h"

enum ExpressionType {
    CHARACTER,
    CONDITIONAL,
    IS,
    TABLE,
    EVALUATED_TABLE,
    EVALUATED_TABLE_VIEW,
    DICTIONARY,
    EVALUATED_DICTIONARY,
    TUPLE,
    EVALUATED_TUPLE,
    FUNCTION,
    FUNCTION_BUILT_IN,
    FUNCTION_DICTIONARY,
    FUNCTION_TUPLE,
    STACK,
    EVALUATED_STACK,
    EMPTY_STACK,
    LOOKUP_CHILD,
    FUNCTION_APPLICATION,
    LOOKUP_SYMBOL,
    NAME,
    DEFINITION,
    PUT_ASSIGNMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    WHILE_END_STATEMENT,
    FOR_END_STATEMENT,
    NUMBER,
    STRING,
    EMPTY_STRING,
    BOOLEAN,
    ANY,
    EMPTY,
};

const auto NAMES = std::vector<std::string>{
    "CHARACTER",
    "CONDITIONAL",
    "IS",
    "TABLE",
    "EVALUATED_TABLE",
    "EVALUATED_TABLE_VIEW",
    "DICTIONARY",
    "EVALUATED_DICTIONARY",
    "TUPLE",
    "EVALUATED_TUPLE",
    "FUNCTION",
    "FUNCTION_BUILT_IN",
    "FUNCTION_DICTIONARY",
    "FUNCTION_TUPLE",
    "STACK",
    "EVALUATED_STACK",
    "EMPTY_STACK",
    "LOOKUP_CHILD",
    "FUNCTION_APPLICATION",
    "LOOKUP_SYMBOL",
    "NAME",
    "DEFINITION",
    "PUT_ASSIGNMENT",
    "WHILE_STATEMENT",
    "FOR_STATEMENT",
    "WHILE_END_STATEMENT",
    "FOR_END_STATEMENT",
    "NUMBER",
    "STRING",
    "EMPTY_STRING",
    "BOOLEAN",
    "ANY",
    "EMPTY",
};

struct StaticTypeError : public std::runtime_error
{
    StaticTypeError(ExpressionType type, const std::string& location);
    using runtime_error::runtime_error;
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

using Number = double;
using Boolean = bool;
using Character = char;
// TODO: Consider pointer for cheap copy.
// Unlike the other atoms Names are just created during parsing.
// After that the std::vector of Names is static, which makes it easier to pass
// around references or pointers.
using Name = std::string; 

struct Conditional {
    Expression expression_if;
    Expression expression_then;
    Expression expression_else;
};

struct Alternative {
    Expression left;
    Expression right;
};

// TODO: make cheaper to copy or pass by reference or pointer?
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

// TODO: make cheaper to copy or pass by reference or pointer?
struct FunctionTuple {
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

struct String {
    Expression top;
    Expression rest;
};

struct EmptyString {
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct Tuple {
    std::vector<Expression> expressions;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct EvaluatedTuple {
    std::vector<Expression> expressions;
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

struct PutAssignment {
    Expression name;
    Expression expression;
};

struct WhileStatement {
    Expression expression;
    size_t end_index_;
};

struct ForStatement {
    Expression name_item;
    Expression name_container;
    size_t end_index_;
};

struct WhileEndStatement {
    size_t while_index_;
};

struct ForEndStatement {
    size_t for_index_;
};

using Statements = std::vector<Expression>;

// TODO: make cheaper to copy or pass by reference or pointer?
class Definitions {
private:
    std::vector<std::pair<std::string, Expression>> definitions;
public:
    bool empty() const;
    void add(const std::string& key, Expression value);
    bool has(const std::string& key) const;
    Expression lookup(const std::string& key) const;
    std::vector<std::pair<std::string, Expression>> sorted() const;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct Dictionary {
    Statements statements;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct EvaluatedDictionary {
    Expression environment;
    Definitions definitions;
};

struct Row {
    Expression key;
    Expression value;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct Table {
    std::vector<Row> rows;
};

// TODO: make cheaper to copy or pass by reference or pointer?
struct EvaluatedTable {
    using Iterator = std::map<std::string, Row>::const_iterator;
    std::map<std::string, Row> rows;
    Iterator begin() const {return rows.begin();}
    Iterator end() const {return rows.end();}
    bool empty() const {return rows.empty();}
};

struct EvaluatedTableView {
    using Iterator = std::map<std::string, Row>::const_iterator;
    Iterator first;
    Iterator last;
    Iterator begin() const {return first;}
    Iterator end() const {return last;}
    bool empty() const {return first == last;}
};
