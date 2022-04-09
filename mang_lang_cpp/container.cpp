#include "container.h"

#include "factory.h"

// Note that we need code range since we use this during parsing.
CodeRange addCodeRanges(Expression rest, Expression first) {
    const auto first_character = std::min(rest.range.first, first.range.first);
    const auto last_character = std::max(rest.range.last, first.range.last);
    return CodeRange{first_character, last_character};
}

BinaryInput getBinaryInput(Expression expression) {
    const auto stack = getEvaluatedStack(expression);
    const auto rest = getEvaluatedStack(stack.rest);
    return BinaryInput{stack.first, rest.first};
}

Expression putString(Expression rest, Expression first) {
    return makeString(addCodeRanges(first, rest), String{first, rest});
}

Expression putStack(Expression rest, Expression first) {
    return makeStack(addCodeRanges(first, rest), Stack{first, rest});
}

Expression putEvaluatedStack(Expression rest, Expression first) {
    return makeEvaluatedStack(addCodeRanges(first, rest),
        EvaluatedStack{first, rest});
}

Expression reverseString(CodeRange code, Expression string) {
    return leftFold(makeEmptyString(code, EmptyString{}), string, putString, EMPTY_STRING, getString);
}

Expression reverseStack(CodeRange code, Expression stack) {
    return leftFold(makeEmptyStack(code, EmptyStack{}), stack, putStack,
        EMPTY_STACK, getStack);
}

Expression reverseEvaluatedStack(CodeRange code, Expression stack) {
    return leftFold(makeEmptyStack(code, EmptyStack{}), stack, putEvaluatedStack,
        EMPTY_STACK, getEvaluatedStack);
}
