#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "parse_utils.h"

struct Expression;
using ExpressionPointer = std::unique_ptr<Expression>;

struct Expression {
    Expression(const CodeCharacter* first, const CodeCharacter* last)
    : first{first}, last{last} {}
    const CodeCharacter* first;
    const CodeCharacter* last;
    const CodeCharacter* begin() const {return first;}
    const CodeCharacter* end() const {return last;}
    virtual std::string serialize() const = 0;
    virtual ~Expression() = default;
    virtual ExpressionPointer evaluate() const = 0;
    virtual bool isTrue() const {
        return false;
    }
};

struct Number : public Expression {
    Number(const CodeCharacter* first, const CodeCharacter* last, double value)
        : Expression{first, last}, value{value} {}
    double value;
    virtual std::string serialize() const {
        std::stringstream s;
        s << value;
        return s.str();
    };
    virtual ExpressionPointer evaluate() const {
        return std::make_unique<Number>(first, last, value);
    }
    virtual bool isTrue() const {
        return static_cast<bool>(value);
    }
};

struct String : public Expression {
    String(const CodeCharacter* first, const CodeCharacter* last, std::string value)
        : Expression{first, last}, value{value} {}
    std::string value;
    virtual std::string serialize() const {
        return "\"" + value + "\"";
    };
    virtual ExpressionPointer evaluate() const {
        return std::make_unique<String>(first, last, value);
    }
};

struct List : public Expression {
    List(
        const CodeCharacter* first,
        const CodeCharacter* last,
        std::vector<ExpressionPointer> elements
    ) : Expression{first, last}, elements{std::move(elements)}
    {}
    std::vector<ExpressionPointer> elements;
    virtual std::string serialize() const {
        auto result = std::string{};
        result += '[';
        for (const auto& element : elements) {
            result += element->serialize();
            result += ',';
        }
        if (elements.empty()) {
            result += ']';
        }
        else {
            result.back() = ']';
        }
        return result;
    }
    virtual ExpressionPointer evaluate() const {
        auto evaluated_elements = std::vector<ExpressionPointer>{};
        for (const auto& element : elements) {
            evaluated_elements.emplace_back(element->evaluate());
        }
        return std::make_unique<List>(first, last, std::move(evaluated_elements));
    }
};

struct Name : public Expression {
    Name(const CodeCharacter* first, const CodeCharacter* last, std::string value)
        : Expression{first, last}, value{value} {}
    std::string value;
    virtual std::string serialize() const {
        return value;
    };
    virtual ExpressionPointer evaluate() const {
        return std::make_unique<Name>(first, last, value);
    }
};

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
        std::vector<DictionaryElement> elements
    ) : Expression{first, last}, elements{std::move(elements)}
    {}
    std::vector<DictionaryElement> elements;
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
    virtual ExpressionPointer evaluate() const {
        auto evaluated_elements = std::vector<DictionaryElement>{};
        for (const auto& element : elements) {
            evaluated_elements.emplace_back(
                element.name, element.expression->evaluate());
        }
        return std::make_unique<Dictionary>(first, last, std::move(evaluated_elements));
    }
};

struct Conditional : public Expression {
    Conditional(
        const CodeCharacter* first,
        const CodeCharacter* last,
        ExpressionPointer expression_if,
        ExpressionPointer expression_then,
        ExpressionPointer expression_else
    )
        : Expression{first, last},
        expression_if{std::move(expression_if)},
        expression_then{std::move(expression_then)},
        expression_else{std::move(expression_else)}
    {}
    ExpressionPointer expression_if;
    ExpressionPointer expression_then;
    ExpressionPointer expression_else;
    virtual std::string serialize() const {
        return std::string{"if "} + expression_if->serialize()
            + " then " + expression_then->serialize()
            + " else " + expression_else->serialize();
    };
    virtual ExpressionPointer evaluate() const {
        if (expression_if->evaluate()->isTrue()) {
            return expression_then->evaluate();
        } else {
            return expression_else->evaluate();
        }
    }
};
