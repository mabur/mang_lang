#pragma once

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "../parsing.h"
#include "../SinglyLinkedList.h"

#include "../factory.h"

struct Expression;
//using ExpressionPointer = std::shared_ptr<const Expression>;
using InternalList = SinglyLinkedList<ExpressionPointer>;

struct Expression {
    Expression(CodeRange range, ExpressionPointer environment, ExpressionType type)
        : range{range}, environment{environment}, type_{type} {}
    virtual ~Expression() = default;

    CodeRange range;
    ExpressionPointer environment;
    ExpressionType type_;

    const CodeCharacter* begin() const;
    const CodeCharacter* end() const;
};
