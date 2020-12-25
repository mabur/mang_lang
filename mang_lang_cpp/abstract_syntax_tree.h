#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "parse_utils.h"

struct Expression {
    Expression(const CodeCharacter* first, const CodeCharacter* last)
    : first{first}, last{last} {}
    const CodeCharacter* first;
    const CodeCharacter* last;
    const CodeCharacter* begin() const {return first;}
    const CodeCharacter* end() const {return last;}
    virtual std::string serialize() const = 0;
    virtual ~Expression() = default;
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
};

struct String : public Expression {
    String(const CodeCharacter* first, const CodeCharacter* last, std::string value)
        : Expression{first, last}, value{value} {}
    std::string value;
    virtual std::string serialize() const {
        return "\"" + value + "\"";
    };
};

struct Name : public Expression {
    Name(const CodeCharacter* first, const CodeCharacter* last, std::string value)
        : Expression{first, last}, value{value} {}
    std::string value;
    virtual std::string serialize() const {
        return value;
    };
};

struct List : public Expression {
    List(
        const CodeCharacter* first,
        const CodeCharacter* last,
        std::vector<std::unique_ptr<Expression>> elements
    ) : Expression{first, last}, elements{std::move(elements)}
    {}
    std::vector<std::unique_ptr<Expression>> elements;
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
};

struct DictionaryElement {
    DictionaryElement(const Name& name, std::unique_ptr<Expression>&& expression)
        : name{name}, expression{std::move(expression)}
    {}
    Name name;
    std::unique_ptr<Expression> expression;
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
};
