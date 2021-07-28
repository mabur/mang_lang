#include "empty.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

ExpressionPointer empty(const ExpressionPointer& expression_smart) {
    const auto expression = expression_smart.get();
    if (expression->type_ == LIST) {
        return makeList(std::make_shared<List>(expression->range, nullptr, nullptr));
    }
    if (expression->type_ == STRING) {
        return makeString(std::make_shared<String>(expression->range, nullptr, nullptr));
    }
    throw std::runtime_error{"Expected list"};
}
