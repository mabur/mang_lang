#include "list.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

InternalList list(ExpressionPointer expression) {
    switch (expression.type) {
        case LIST: return expression.list().elements;
        case STRING: return expression.string().elements;
        default: throw std::runtime_error{"Expected list"};
    }
}
