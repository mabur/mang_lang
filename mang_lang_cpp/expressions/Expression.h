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
    Expression(CodeRange range, ExpressionPointer environment)
        : range{range}, environment{environment} {}
    virtual ~Expression() = default;

    CodeRange range;
    ExpressionPointer environment;
};
