#pragma once

#include <memory>

enum ExpressionType {
    CHARACTER,
    CONDITIONAL,
    DICTIONARY,
    FUNCTION,
    FUNCTION_BUILT_IN,
    FUNCTION_DICTIONARY,
    FUNCTION_LIST,
    LIST,
    LOOKUP_CHILD,
    LOOKUP_FUNCTION,
    LOOKUP_SYMBOL,
    NAME,
    NAMED_ELEMENT,
    WHILE_ELEMENT,
    END_ELEMENT,
    NUMBER,
    STRING,
};

struct Expression;

struct ExpressionPointer {
    std::shared_ptr<const Expression> inner;
    operator bool () const {return bool(inner);}
    const Expression* get() const {return inner.get();}
    const Expression* operator -> () const {return get();}
};
