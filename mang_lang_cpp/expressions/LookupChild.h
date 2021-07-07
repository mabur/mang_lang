#pragma once
#include "Expression.h"
#include "Name.h"

struct LookupChild : public Expression {
    LookupChild(
        CodeRange range,
        const Expression* environment,
        NamePointer name,
        ExpressionPointer child
    ) : Expression{range, environment, LOOKUP_CHILD}, name{std::move(name)}, child{std::move(child)} {}
    NamePointer name;
    ExpressionPointer child;
    std::string serialize() const final;
    ExpressionPointer evaluate(const Expression* environment, std::ostream& log) const final;
};
