#include "Expression.h"

const CodeCharacter* Expression::begin() const {
    return range.begin();
}

const CodeCharacter* Expression::end() const {
    return range.end();
}
