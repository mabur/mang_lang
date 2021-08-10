#include "empty.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

ExpressionPointer empty(ExpressionPointer expression) {
    switch (expression.type) {
        case LIST: return makeList(new List{CodeRange{}, InternalList{}});
        case STRING: return makeString(new String{CodeRange{}, InternalList{}});
        default: throw std::runtime_error{"Expected list"};
    }
}
