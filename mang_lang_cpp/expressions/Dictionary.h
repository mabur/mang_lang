#pragma once
#include "DictionaryElement.h"
#include "Expression.h"
#include "Name.h"

struct Dictionary : public Expression {
    Dictionary(CodeRange range, const Expression* environment)
        : Expression{range, environment}
    {}
    std::vector<DictionaryElementPointer> elements;
    ExpressionPointer lookup(const std::string& name) const final;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
    bool boolean() const final;
};