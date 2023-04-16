#include "algorithm.h"

#include "factory.h"

// Note that we need code range since we use this during parsing.
CodeRange addCodeRanges(Expression rest, Expression top) {
    const auto first_character = std::min(rest.range.first, top.range.first);
    const auto last_character = std::max(rest.range.last, top.range.last);
    return CodeRange{first_character, last_character};
}

Expression putString(Expression rest, Expression top) {
    return makeString(addCodeRanges(top, rest), String{top, rest});
}

Expression putStack(Expression rest, Expression top) {
    return makeStack(addCodeRanges(top, rest), Stack{top, rest});
}

Expression putEvaluatedStack(Expression rest, Expression top) {
    return makeEvaluatedStack(addCodeRanges(top, rest),
        EvaluatedStack{top, rest});
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
