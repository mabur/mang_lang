#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../parse_utils.h"

struct Expression;
using ExpressionPointer = std::shared_ptr<const Expression>;

struct Expression {
    Expression(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent
    ) : first_{first}, last_{last}, parent_{parent} {}
    virtual ~Expression() = default;
    const CodeCharacter* first_;
    const CodeCharacter* last_;
    const Expression* parent_;
    const CodeCharacter* begin() const;
    const CodeCharacter* end() const;
    const Expression* parent() const;
    virtual ExpressionPointer apply(const Expression& input) const;
    virtual ExpressionPointer lookup(const std::string& name) const;
    virtual bool isTrue() const;
    virtual std::string serialize() const = 0;
    virtual ExpressionPointer evaluate(const Expression* parent) const = 0;
    static ExpressionPointer parse(
        const CodeCharacter* first, const CodeCharacter* last
    );
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);

    virtual double number() const;
    virtual const std::vector<ExpressionPointer>& list() const;
};
