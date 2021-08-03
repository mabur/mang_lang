#include "empty.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

ExpressionPointer empty(const ExpressionPointer& expression_smart) {
    const auto type = expression_smart.type;
    const auto expression = expression_smart.get();
    if (type == LIST) {
        return makeList(std::make_shared<List>(expression->range, ExpressionPointer{}, InternalList{}));
    }
    if (type == STRING) {
        return makeString(std::make_shared<String>(expression->range, ExpressionPointer{}, InternalList{}));
    }
    throw std::runtime_error{"Expected list"};
}
