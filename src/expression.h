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
    ALTERNATIVE,
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
    ARGUMENT,
    DEFINITION,
    PUT_ASSIGNMENT,
    PUT_EACH_ASSIGNMENT,
    DROP_ASSIGNMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    FOR_SIMPLE_STATEMENT,
    WHILE_END_STATEMENT,
    FOR_END_STATEMENT,
    RETURN_STATEMENT,
    NUMBER,
    STRING,
    EMPTY_STRING,
    YES,
    NO,
    DYNAMIC_EXPRESSION,
    TYPED_EXPRESSION,
    ANY,
};

const auto NAMES = std::vector<std::string>{
    "CHARACTER",
    "CONDITIONAL",
    "IS",
    "ALTERNATIVE",
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
    "ARGUMENT",
    "DEFINITION",
    "PUT_ASSIGNMENT",
    "PUT_EACH_ASSIGNMENT",
    "DROP_ASSIGNMENT",
    "WHILE_STATEMENT",
    "FOR_STATEMENT",
    "FOR_SIMPLE_STATEMENT,",
    "WHILE_END_STATEMENT",
    "FOR_END_STATEMENT",
    "RETURN_STATEMENT",
    "NUMBER",
    "STRING",
    "EMPTY_STRING",
    "YES",
    "NO",
    "DYNAMIC_EXPRESSION",
    "TYPED_EXPRESSION",
    "ANY",
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

struct MissingKey : public std::runtime_error
{
    MissingKey(const std::string& key);
    using runtime_error::runtime_error;
};

// TODO: Pack tighter?
// Bit size           Current  Pack1  Pack2
// Expression::type        16      4      4
// Expression::index       32     32   32-4
// Expression::range       64   32-4      0
// Expression             128     64     32
struct Expression {
    ExpressionType type = ANY;
    size_t index = 0;
    CodeRange range;
};

using Number = double;
using Character = char;
using Name = std::string;

struct Argument {
    Expression type; // Optional
    size_t name;
};

// TODO: type alias instead of struct?
struct DynamicExpression {
    Expression expression;
};

struct TypedExpression {
    size_t type_name;
    Expression value;
};

struct Alternative {
    Expression left;
    Expression right;
};

struct Conditional {
    Expression alternative_first;
    Expression alternative_last;
    Expression expression_else;
};

struct IsExpression {
    Expression input;
    Expression alternative_first;
    Expression alternative_last;
    Expression expression_else;
};

struct Function {
    Expression environment;
    size_t argument;
    Expression body;
};

struct FunctionBuiltIn {
    std::function<Expression(Expression)> function;
};

struct FunctionDictionary {
    Expression environment; // TODO: use this.
    size_t first_argument;
    size_t last_argument; // Exclusive, to handle empty range
    Expression body;
};

struct FunctionTuple {
    Expression environment;
    size_t first_argument;
    size_t last_argument; // Exclusive, to handle empty range
    Expression body;
};

struct LookupChild {
    size_t name;
    Expression child;
};

struct FunctionApplication {
    size_t name;
    Expression child;
};

// TODO: type alias instead of struct?
struct LookupSymbol {
    size_t name;
};

struct String {
    Expression top;
    Expression rest;
};

// TODO: make cheaper to copy.
// Have multiple versions of Tuple each with a fixed size on the stack.
// 2 & 3 are the most common sizes.
struct Tuple {
    std::vector<Expression> expressions;
};

// TODO: make cheaper to copy.
// Have multiple versions of Tuple each with a fixed size on the stack.
// 2 & 3 are the most common sizes.
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

// STATEMENTS BEGIN

struct BoundLocalName {
    size_t global_index; // Index to this name in the global storage.
    size_t dictionary_index; // Index to this name and its data in the dictionary.
};

struct Definition {
    BoundLocalName name;
    Expression expression;
};

struct PutAssignment {
    BoundLocalName name;
    Expression expression;
};

struct PutEachAssignment {
    BoundLocalName name;
    Expression expression;
};

struct DropAssignment {
    BoundLocalName name;
};

struct WhileStatement {
    Expression expression;
    size_t end_index_;
};

struct ForStatement {
    size_t name_item;
    size_t name_container;
    size_t end_index_;
    size_t name_index_item;
    size_t name_index_container;
};

struct ForSimpleStatement {
    BoundLocalName container_name;
    size_t end_index_;
};

struct WhileEndStatement {
    size_t while_index_;
};

struct ForEndStatement {
    size_t for_index_;
};

// STATEMENTS END

// TODO: make cheaper to copy.
struct Dictionary {
    std::vector<Expression> statements;
    size_t definition_count;
};

// TODO: make cheaper to copy.
// Bottleneck.
// Use index range for definitions.
// But then need to know all members of the dictionary before it is evaluated.
// so that they are all added together, at least the first time.
// Alternatively, use unordered_map for fast lookup of all names
// and not just definitions.
// Bottleneck.
struct EvaluatedDictionary {
    Expression environment;
    std::vector<Definition> definitions;

    const Expression* optionalLookup(size_t name) const;
    Expression lookup(size_t name) const;
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
