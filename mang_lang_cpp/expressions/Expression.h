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
    const CodeCharacter* begin() const {return first_;}
    const CodeCharacter* end() const {return last_;}
    const Expression* parent() const {return parent_;}
    virtual ExpressionPointer lookup(const std::string& name) const {
        if (parent()) {
            return parent()->lookup(name);
        }
        return {};
    }
    virtual bool isTrue() const {return false;}
    virtual std::string serialize() const = 0;
    virtual ExpressionPointer evaluate(const Expression* parent) const = 0;
};
