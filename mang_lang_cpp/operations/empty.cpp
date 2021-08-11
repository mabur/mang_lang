#include "empty.h"

#include "../factory.h"

ExpressionPointer empty(ExpressionPointer expression) {
    switch (expression.type) {
        case LIST: return makeList(new List{});
        case STRING: return makeString(new String{});
        default: throw std::runtime_error{"Expected list"};
    }
}
