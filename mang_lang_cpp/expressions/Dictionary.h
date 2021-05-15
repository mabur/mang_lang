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
