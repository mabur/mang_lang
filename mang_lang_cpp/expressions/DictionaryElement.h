#pragma once
#include "Expression.h"
#include "Name.h"

struct DictionaryElement : Expression {
    DictionaryElement(
        CodeRange range,
        const Expression* parent,
        const Name& name,
        ExpressionPointer expression
    ) : Expression{range, parent}, name{name}, expression{std::move(expression)}
    {}
    Name name;
    ExpressionPointer expression;
    std::string serialize() const final {
        return name.serialize() + '=' + expression->serialize() + ',';
    }
    ExpressionPointer evaluate(const Expression* parent, std::ostream& log) const final {
        return std::make_shared<DictionaryElement>(
            range(),
            nullptr,
            name,
            expression->evaluate(parent, log)
        );
    }
    ExpressionPointer lookup(const std::string& s) const final {
        if (name.value == s) {
            return expression;
        }
        return nullptr;
    }
    static ExpressionPointer parse(CodeRange code) {
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

struct End : Expression {
    End(CodeRange range, const Expression* parent) : Expression(range, parent)
    {}
    std::string serialize() const final {
        return "end,";
    }
    ExpressionPointer evaluate(const Expression*, std::ostream&) const final {
        return nullptr;
    }
    static ExpressionPointer parse(CodeRange code) {
        auto first = code.begin();
        code = parseWhiteSpace(code);
        code = parseKeyword(code, "end");
        code = parseWhiteSpace(code);
        code = parseOptionalCharacter(code, ',');
        code = parseWhiteSpace(code);
        return std::make_shared<End>(CodeRange{first, code.begin()}, nullptr);
    }
    static bool startsWith(CodeRange code) {
        return isKeyword(code, "end");
    }
};

struct While : Expression {
    While(CodeRange range, const Expression* parent, ExpressionPointer predicate)
    : Expression(range, parent), predicate_(predicate)
    {}
    ExpressionPointer predicate_;
    std::string serialize() const final {
        return "while " + predicate_->serialize() + ",";
    }
    ExpressionPointer evaluate(const Expression*, std::ostream&) const final {
        return nullptr;
    }
    static ExpressionPointer parse(CodeRange code) {
        auto first = code.begin();
        code = parseWhiteSpace(code);
        code = parseKeyword(code, "while");
        code = parseWhiteSpace(code);
        auto predicate = Expression::parse(code);
        code.first = predicate->end();
        code = parseWhiteSpace(code);
        code = parseOptionalCharacter(code, ',');
        code = parseWhiteSpace(code);
        return std::make_shared<While>(
            CodeRange{first, code.begin()}, nullptr, predicate
        );
    }
    static bool startsWith(CodeRange code) {
        return isKeyword(code, "while");
    }
};
