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

    std::string serialize() const final;
    ExpressionPointer lookup(const std::string& s) const final;
    static DictionaryElementPointer parse(CodeRange code);
    void mutate(const Expression* parent, std::ostream& log,
        DictionaryElements& elements) const;
    size_t jump(const Expression* parent, std::ostream& log) const;
    bool isWhile() const;
    bool isEnd() const;
    bool isSymbolDefinition() const;
};

void setContext(DictionaryElements& elements);

size_t numNames(const DictionaryElements& elements);
