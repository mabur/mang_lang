#pragma once

#include <memory>
#include <vector>
#include "Expression.h"
#include "Name.h"

struct DictionaryElement;

using DictionaryElementPointer = std::shared_ptr<DictionaryElement>;
using DictionaryElements = std::vector<DictionaryElementPointer>;

struct DictionaryElement : Expression {
    DictionaryElement(
        CodeRange range,
        const Expression* parent,
        NamePointer name,
        ExpressionPointer expression,
        size_t dictionary_index
    );
    NamePointer name;
    ExpressionPointer expression;
    size_t jump_true = 1;
    size_t jump_false = 0;
    size_t dictionary_index_;

    static DictionaryElementPointer parse(CodeRange code);
    virtual void mutate(const Expression* parent, std::ostream& log,
        DictionaryElements& elements) const = 0;
    virtual size_t jump(const Expression* parent, std::ostream& log) const = 0;
    virtual bool isWhile() const;
    virtual bool isEnd() const;
    virtual bool isSymbolDefinition() const;
};

struct NamedElement : DictionaryElement {
    NamedElement(
        CodeRange range,
        const Expression* parent,
        NamePointer name,
        ExpressionPointer expression,
        size_t dictionary_index
    );
    std::string serialize() const final;
    ExpressionPointer lookup(const std::string& s) const final;
    static DictionaryElementPointer parse(CodeRange code);
    void mutate(const Expression* parent, std::ostream& log,
        DictionaryElements& elements) const final;
    size_t jump(const Expression* parent, std::ostream& log) const final;
};

struct WhileElement : DictionaryElement {
    WhileElement(
        CodeRange range,
        const Expression* parent,
        ExpressionPointer expression
    );
    std::string serialize() const final;
    static DictionaryElementPointer parse(CodeRange code);
    void mutate(const Expression* parent, std::ostream& log,
        DictionaryElements& elements) const final;
    size_t jump(const Expression* parent, std::ostream& log) const final;
};

struct EndElement : DictionaryElement {
    EndElement(CodeRange range, const Expression* parent);
    std::string serialize() const final;
    static DictionaryElementPointer parse(CodeRange code);
    void mutate(const Expression* parent, std::ostream& log,
        DictionaryElements& elements) const final;
    size_t jump(const Expression* parent, std::ostream& log) const final;
};

void setContext(DictionaryElements& elements);

size_t numNames(const DictionaryElements& elements);
