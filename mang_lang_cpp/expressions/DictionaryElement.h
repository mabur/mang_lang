#pragma once
#include "Expression.h"
#include "Name.h"

struct DictionaryElement : Expression {
    DictionaryElement(
        CodeRange range,
        const Expression* parent,
        const Name& name,
        ExpressionPointer expression
    )
        : Expression{range, parent}, name{name}, expression{std::move(expression)}
    {}
    Name name;
    ExpressionPointer expression;
    std::string serialize() const final {
        return name.serialize() + '=' + expression->serialize() + ',';
    }
    ExpressionPointer evaluate(const Expression*, std::ostream&) const final {
        return {};
    }
    ExpressionPointer lookup(const std::string& s) const final {
        if (name.value == s) {
            return expression;
        }
        return nullptr;
    };
    static DictionaryElement parse(CodeRange code) {
        auto first = code.begin();
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        const auto name = Name::parse(code);
        code.first = name.end();
        code = parseWhiteSpace(code);
        code = parseCharacter(code, '=');
        code = parseWhiteSpace(code);
        auto expression = Expression::parse(code);
        code.first = expression->end();
        code = parseWhiteSpace(code);
        code = parseOptionalCharacter(code, ',');
        return DictionaryElement(CodeRange{first, code.first}, nullptr, name, std::move(expression));
    }
    static bool startsWith(CodeRange) {
        return false;
    }
};
