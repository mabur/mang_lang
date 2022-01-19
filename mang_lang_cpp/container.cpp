#include "container.h"

#include "factory.h"

BinaryInput getBinaryInput(Expression expression) {
    const auto list = getList(expression);
    const auto rest = getList(list.rest);
    return BinaryInput{list.first, rest.first};
}
