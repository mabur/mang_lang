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
        ExpressionPointer environment,
        ExpressionType type,
        NamePointer name,
        ExpressionPointer expression,
        size_t dictionary_index
    );
    NamePointer name;
    ExpressionPointer expression;
    size_t jump_true = 1;
    size_t jump_false = 0;
    size_t dictionary_index_;

    virtual DictionaryElements mutate(
        const ExpressionPointer& environment,
        std::ostream& log,
        const DictionaryElements& elements) const = 0;
    virtual size_t jump(const ExpressionPointer& environment, std::ostream& log) const = 0;
    virtual bool isWhile() const;
    virtual bool isEnd() const;
    virtual bool isSymbolDefinition() const;
};

struct NamedElement : DictionaryElement {
    NamedElement(
        CodeRange range,
        ExpressionPointer environment,
        NamePointer name,
        ExpressionPointer expression,
        size_t dictionary_index
    );
    DictionaryElements mutate(
        const ExpressionPointer& environment,
        std::ostream& log,
        const DictionaryElements& elements) const final;
    size_t jump(const ExpressionPointer& environment, std::ostream& log) const final;
};

struct WhileElement : DictionaryElement {
    WhileElement(
        CodeRange range,
        ExpressionPointer environment,
        ExpressionPointer expression
    );
    DictionaryElements mutate(
        const ExpressionPointer& environment,
        std::ostream& log,
        const DictionaryElements& elements) const final;
    size_t jump(const ExpressionPointer& environment, std::ostream& log) const final;
};

struct EndElement : DictionaryElement {
    EndElement(CodeRange range, ExpressionPointer environment);
    DictionaryElements mutate(
        const ExpressionPointer& environment,
        std::ostream& log,
        const DictionaryElements& elements) const final;
    size_t jump(const ExpressionPointer& environment, std::ostream& log) const final;
};

void setContext(DictionaryElements& elements);

size_t numNames(const DictionaryElements& elements);
