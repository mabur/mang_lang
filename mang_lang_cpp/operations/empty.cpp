#include "empty.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

ExpressionPointer empty(const ExpressionPointer& expression) {
    switch (expression.type) {
        case LIST: return makeList(std::make_shared<List>(expression->range, ExpressionPointer{}, InternalList{}));
        case STRING: return makeString(std::make_shared<String>(expression->range, ExpressionPointer{}, InternalList{}));
        default: throw std::runtime_error{"Expected list"};
    }
}
