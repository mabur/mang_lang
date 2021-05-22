#pragma once
#include "Expression.h"
#include "Name.h"

struct DictionaryElementBase : Expression {
    DictionaryElementBase(CodeRange range, const Expression* parent)
        : Expression(range, parent) {
    }
    size_t index;
    virtual std::string name() const {return "";}
};

using DictionaryElementBasePointer = std::shared_ptr<DictionaryElementBase>;

struct DictionaryElement : DictionaryElementBase {
    DictionaryElement(
        CodeRange range,
        const Expression* parent,
        const Name& name,
        ExpressionPointer expression
    ) : DictionaryElementBase{range, parent}, name_{name}, expression{std::move(expression)}
    {}
    Name name_;
    ExpressionPointer expression;
    std::string name() const final {return name_.value;}
    std::string serialize() const final {
        return name_.serialize() + '=' + expression->serialize() + ',';
    }
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final {
        return std::make_shared<DictionaryElement>(
            range(),
            nullptr,
            name_,
            expression->evaluate(parent, log)
        );
    }
    ExpressionPointer lookup(const std::string& s) const final {
        if (name_.value == s) {
            return expression;
        }
        return nullptr;
    }
    static DictionaryElementBasePointer parse(CodeRange code) {
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
        return std::make_shared<DictionaryElement>(
            CodeRange{first, code.first}, nullptr, name, std::move(expression)
        );
    }
    static bool startsWith(CodeRange) {
        return false;
    }
};
