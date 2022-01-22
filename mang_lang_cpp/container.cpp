#include "container.h"

#include "factory.h"

// Note that we need code range since we use this during parsing.
CodeRange addCodeRanges(Expression rest, Expression first) {
    const auto first_character = std::min(rest.range.first, first.range.first);
    const auto last_character = std::max(rest.range.last, first.range.last);
    return CodeRange{first_character, last_character};
}

BinaryInput getBinaryInput(Expression expression) {
    const auto list = getEvaluatedList(expression);
    const auto rest = getEvaluatedList(list.rest);
    return BinaryInput{list.first, rest.first};
}

Expression putString(Expression rest, Expression first) {
    return makeString(addCodeRanges(first, rest), String{first, rest});
}

Expression putList(Expression rest, Expression first) {
    return makeList(addCodeRanges(first, rest), List{first, rest});
}

Expression putEvaluatedList(Expression rest, Expression first) {
    return makeEvaluatedList(addCodeRanges(first, rest), EvaluatedList{first, rest});
}

Expression reverseString(CodeRange code, Expression string) {
    return leftFold(makeEmptyString(code, EmptyString{}), string, putString, EMPTY_STRING, getString);
}

Expression reverseList(CodeRange code, Expression list) {
    return leftFold(makeEmptyList(code, EmptyList{}), list, putList, EMPTY_LIST, getList);
}

Expression reverseEvaluatedList(CodeRange code, Expression list) {
    return leftFold(makeEmptyList(code, EmptyList{}), list, putEvaluatedList, EMPTY_LIST, getEvaluatedList);
}
