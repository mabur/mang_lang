#pragma once
#include "Expression.h"
#include "Name.h"

struct DictionaryElement {
    DictionaryElement(const Name& name, ExpressionPointer&& expression)
        : name{name}, expression{std::move(expression)}
    {}
    Name name;
    ExpressionPointer expression;
};

struct Dictionary : public Expression {
    Dictionary(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent
    ) : Expression{first, last, parent}
    {}
    std::vector<DictionaryElement> elements;
    void add(DictionaryElement element);
    virtual ExpressionPointer lookup(const std::string& name) const;
    virtual std::string serialize() const;
    virtual ExpressionPointer evaluate(const Expression* parent) const;
    static ExpressionPointer parse(const CodeCharacter* first, const CodeCharacter* last);
    static bool startsWith(const CodeCharacter* first, const CodeCharacter* last);
    virtual bool boolean() const;
};
