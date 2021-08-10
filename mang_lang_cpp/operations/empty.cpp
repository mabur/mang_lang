#include "empty.h"

#include "../expressions/List.h"
#include "../expressions/String.h"

#include "../factory.h"

ExpressionPointer empty(ExpressionPointer expression) {
    switch (expression.type) {
        case LIST: return makeList(new List{CodeRange{}, ExpressionPointer{}, InternalList{}});
        case STRING: return makeString(std::make_shared<String>(String{CodeRange{}, ExpressionPointer{}, InternalList{}}));
        default: throw std::runtime_error{"Expected list"};
    }
}
