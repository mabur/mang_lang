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

const auto KEYWORDS = std::vector<std::string>{"if", "then", "else", "from", "to"};

struct Expression {
    Expression(CodeRange range, const Expression* parent)
        : range_{range}, parent_{parent} {}
    virtual ~Expression() = default;

    CodeRange range_;
    const Expression* parent_;

    CodeRange range() const;
    const CodeCharacter* begin() const;
    const CodeCharacter* end() const;
    const Expression* parent() const;

    static bool startsWith(CodeRange code);
    static ExpressionPointer parse(CodeRange code);

    virtual std::string serialize() const = 0;
    virtual ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const;
    virtual ExpressionPointer apply(ExpressionPointer input, std::ostream& log) const;
    virtual ExpressionPointer lookup(const std::string& name) const;
    virtual bool isEqual(const Expression* expression) const;
    virtual double number() const;
    virtual char character() const;
    virtual bool boolean() const;
    virtual const InternalList& list() const;

    virtual ExpressionPointer first() const;
    virtual ExpressionPointer rest() const;
    virtual ExpressionPointer reverse() const;
    virtual ExpressionPointer prepend(ExpressionPointer item) const;
};
