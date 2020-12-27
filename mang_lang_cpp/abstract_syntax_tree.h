#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "parse_utils.h"

struct Expression;
using ExpressionPointer = std::unique_ptr<Expression>;

struct Expression {
    Expression(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent
    ) : first_{first}, last_{last}, parent_{parent} {}
    virtual ~Expression() = default;
    const CodeCharacter* first_;
    const CodeCharacter* last_;
    const Expression* parent_;
    const CodeCharacter* begin() const {return first_;}
    const CodeCharacter* end() const {return last_;}
    const Expression* parent() const {return parent_;}
    virtual bool isTrue() const {return false;}
    virtual std::string serialize() const = 0;
    virtual ExpressionPointer evaluate() const = 0;
};

struct Number : public Expression {
    Number(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        double value
    ) : Expression{first, last, parent}, value{value} {}
    double value;
    virtual std::string serialize() const {
        std::stringstream s;
        s << value;
        return s.str();
    };
    virtual ExpressionPointer evaluate() const {
        return std::make_unique<Number>(begin(), end(), parent(), value);
    }
    virtual bool isTrue() const {
        return static_cast<bool>(value);
    }
};

struct String : public Expression {
    String(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        std::string value
    ) : Expression{first, last, parent}, value{value} {}
    std::string value;
    virtual std::string serialize() const {
        return "\"" + value + "\"";
    };
    virtual ExpressionPointer evaluate() const {
        return std::make_unique<String>(begin(), end(), parent(), value);
    }
};

struct List : public Expression {
    List(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        std::vector<ExpressionPointer> elements
    ) : Expression{first, last, parent}, elements{std::move(elements)}
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
        return std::make_unique<List>(begin(), end(), parent(), std::move(evaluated_elements));
    }
};

struct Name : public Expression {
    Name(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        std::string value
    ) : Expression{first, last, parent}, value{value} {}
    std::string value;
    virtual std::string serialize() const {
        return value;
    };
    virtual ExpressionPointer evaluate() const {
        return std::make_unique<Name>(begin(), end(), parent(), value);
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
        const Expression* parent,
        std::vector<DictionaryElement> elements
    ) : Expression{first, last, parent}, elements{std::move(elements)}
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
        return std::make_unique<Dictionary>(begin(), end(), parent(), std::move(evaluated_elements));
    }
};

struct Conditional : public Expression {
    Conditional(
        const CodeCharacter* first,
        const CodeCharacter* last,
        const Expression* parent,
        ExpressionPointer expression_if,
        ExpressionPointer expression_then,
        ExpressionPointer expression_else
    )
        : Expression{first, last, parent},
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
