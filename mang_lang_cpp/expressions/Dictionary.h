#pragma once
#include "DictionaryElement.h"
#include "Expression.h"
#include "Name.h"

struct Dictionary : public Expression {
    Dictionary(CodeRange range, const Expression* parent)
        : Expression{range, parent}
    {}
    std::vector<DictionaryElement> elements;
    void add(DictionaryElement element);
    ExpressionPointer lookup(const std::string& name) const final;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final;
    static ExpressionPointer parse(CodeRange code);
    static bool startsWith(CodeRange code);
    bool boolean() const final;
};
