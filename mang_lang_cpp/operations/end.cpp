#include "end.h"

#include "../factory.h"

const CodeCharacter* end(Expression expression) {
    return expression.range.last;
}
