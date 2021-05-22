#pragma once

#include <memory>
#include "Expression.h"
#include "Name.h"

struct DictionaryElement;

using DictionaryElementPointer = std::shared_ptr<DictionaryElement>;

struct DictionaryElement : Expression {
    DictionaryElement(
        CodeRange range,
        const Expression* parent,
        NamePointer name,
        ExpressionPointer expression
    ) : Expression{range, parent}, name_{std::move(name)}, expression{std::move(expression)}
    {}
    NamePointer name_;
    ExpressionPointer expression;
    size_t jump_true = 1;
    size_t jump_false = 0;
    size_t dictionary_index_ = 0;

    std::string name() const {return name_->value;}
    std::string serialize() const final {
        if (isWhile()) {
            return "while " + expression->serialize() + ',';
        }
        if (isEnd()) {
            return "end,";
        }
        return name_->serialize() + '=' + expression->serialize() + ',';
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
        if (name_->value == s) {
            return expression;
        }
        return nullptr;
    }
    static DictionaryElementPointer parse(CodeRange code) {
        auto first = code.begin();
        code = parseWhiteSpace(code);
        throwIfEmpty(code);
        if (isKeyword(code, "end")) {
            code = parseKeyword(code, "end");
            code = parseWhiteSpace(code);
            code = parseOptionalCharacter(code, ',');
            return std::make_shared<DictionaryElement>(
                CodeRange{first, code.first}, nullptr, nullptr, nullptr
            );
        }
        if (isKeyword(code, "while")) {
            code = parseKeyword(code, "while");
            code = parseWhiteSpace(code);
            auto expression = Expression::parse(code);
            code.first = expression->end();
            code = parseWhiteSpace(code);
            code = parseOptionalCharacter(code, ',');
            return std::make_shared<DictionaryElement>(
                CodeRange{first, code.first}, nullptr, nullptr, std::move(expression)
            );
        }
        auto name = Name::parse(code);
        code.first = name->end();
        code = parseWhiteSpace(code);
        code = parseCharacter(code, '=');
        code = parseWhiteSpace(code);
        auto expression = Expression::parse(code);
        code.first = expression->end();
        code = parseWhiteSpace(code);
        code = parseOptionalCharacter(code, ',');
        return std::make_shared<DictionaryElement>(
            CodeRange{first, code.first}, nullptr, std::move(name), std::move(expression)
        );
    }
    static bool startsWith(CodeRange) {
        return false;
    }
    bool isWhile() const {return !name_ && expression;}
    bool isEnd() const {return !name_ && !expression;}
    bool isSymbolDefinition() const {return name_ && expression;}
};
