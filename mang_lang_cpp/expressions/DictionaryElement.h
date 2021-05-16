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

struct End : DictionaryElementBase {
    End(CodeRange range, const Expression* parent) : DictionaryElementBase(range, parent)
    {}
    std::string serialize() const final {
        return "end,";
    }
    ExpressionPointer evaluate(const Expression*, std::ostream&) const final {
        return nullptr;
    }
    static DictionaryElementBasePointer parse(CodeRange code) {
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

struct While : DictionaryElementBase {
    While(CodeRange range, const Expression* parent, ExpressionPointer predicate)
    : DictionaryElementBase(range, parent), predicate_(predicate)
    {}
    ExpressionPointer predicate_;
    size_t end_index_;
    std::string serialize() const final {
        return "while " + predicate_->serialize() + ",";
    }
    ExpressionPointer evaluate(const Expression*, std::ostream&) const final {
        return nullptr;
    }
    static DictionaryElementBasePointer parse(CodeRange code) {
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
