#include "begin.h"

#include "../factory.h"

const CodeCharacter* begin(Expression expression) {
    return expression.range.first;
}
