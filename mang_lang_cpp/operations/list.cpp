#include "list.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

const InternalList& list(const ExpressionPointer& expression_smart) {
    const auto expression = expression_smart.get();
    if (expression->type_ == LIST) {
        return dynamic_cast<const List *>(expression)->elements;
    }
    if (expression->type_ == STRING) {
        return dynamic_cast<const String *>(expression)->elements;
    }
    throw std::runtime_error{"Expected list"};
}
