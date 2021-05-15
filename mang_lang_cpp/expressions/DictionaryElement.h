#pragma once
#include "Expression.h"
#include "Name.h"

struct DictionaryElement : Expression {
    DictionaryElement(
        CodeRange range,
        const Expression* parent,
        const Name& name,
        ExpressionPointer expression
    );
    Name name;
    ExpressionPointer expression;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression*, std::ostream&) const final;
    ExpressionPointer lookup(const std::string& s) const final;
    static DictionaryElement parse(CodeRange code);
    static bool startsWith(CodeRange);
};
