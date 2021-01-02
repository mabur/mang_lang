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
    void add(DictionaryElement element) {
        elements.push_back(std::move(element));
    }
    virtual ExpressionPointer lookup(const std::string& name) const {
        for (const auto& element : elements) {
            if (element.name.value == name) {
                return element.expression;
            }
        }
        return Expression::lookup(name);
    }
    virtual std::string serialize() const {
        auto result = std::string{};
        result += '{';
        for (const auto& element : elements) {
            result += element.name.serialize();
            result += '=';
            result += element.expression->serialize();
            result += ',';
        }
        if (elements.empty()) {
            result += '}';
        }
        else {
            result.back() = '}';
        }
        return result;
    }
    virtual ExpressionPointer evaluate(const Expression* parent) const {
        auto result = std::make_shared<Dictionary>(
            begin(), end(), parent);
        for (const auto& element : elements) {
            result->add(DictionaryElement{
                element.name,
                element.expression->evaluate(result.get())
            });
        }
        return result;
    }
};
