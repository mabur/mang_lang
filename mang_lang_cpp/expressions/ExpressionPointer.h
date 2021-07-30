#pragma once

#include <memory>

struct Expression;

struct ExpressionPointer {
    std::shared_ptr<const Expression> inner;
    operator bool () const {return bool(inner);}
    const Expression* get() const {return inner.get();}
    const Expression* operator -> () const {return get();}
};
