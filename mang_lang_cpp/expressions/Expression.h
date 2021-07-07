#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "../parsing.h"
#include "../SinglyLinkedList.h"

struct Expression;
using ExpressionPointer = std::shared_ptr<const Expression>;
using InternalList = SinglyLinkedList<ExpressionPointer>;

enum ExpressionType {
    CHARACTER,
    CONDITIONAL,
    DICTIONARY,
    DICTIONARY_ELEMENT,
    FUNCTION,
    FUNCTION_BUILT_IN,
    FUNCTION_DICTIONARY,
    FUNCTION_LIST,
    LIST,
    LIST_BASE,
    LOOKUP_CHILD,
    LOOKUP_FUNCTION,
    LOOKUP_SYMBOL,
    NAME,
    NUMBER,
    STRING,
};

struct Expression {
    Expression(CodeRange range, const Expression* environment, ExpressionType type)
        : range_{range}, parent_{environment}, type_{type} {}
    virtual ~Expression() = default;

    CodeRange range_;
    const Expression* parent_;
    ExpressionType type_;

    CodeRange range() const;
    const CodeCharacter* begin() const;
    const CodeCharacter* end() const;
    const Expression* environment() const;

    virtual std::string serialize() const = 0;
    virtual ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const;
    virtual ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const;
    virtual ExpressionPointer lookup(const std::string& name) const;
    virtual bool isEqual(const Expression* expression) const;
    virtual double number() const;
    virtual char character() const;
    virtual bool boolean() const;
    virtual const InternalList& list() const;

    virtual ExpressionPointer empty() const;
    virtual ExpressionPointer prepend(ExpressionPointer item) const;
};
