#pragma once

#include "Expression.h"
#include "operations/boolean.h"
#include "operations/begin.h"
#include "operations/end.h"
#include "factory.h"

namespace new_string {

ExpressionPointer first(ExpressionPointer list);
ExpressionPointer rest(ExpressionPointer list);
ExpressionPointer second(ExpressionPointer list);

inline ExpressionPointer prepend(ExpressionPointer rest, ExpressionPointer first) {
    const auto first_character = std::min(begin(rest), begin(first));
    const auto last_character = std::max(end(rest), end(first));
    const auto code = CodeRange{first_character, last_character};
    return makeNewString(new NewString{code, first, rest});
}

template<typename Predicate>
ExpressionPointer findIf(ExpressionPointer list, Predicate predicate) {
    for (; boolean(list) && !predicate(first(list)); list = rest(list)) {
    }
    return list;
}

template<typename T, typename Operation>
T leftFold(T value, ExpressionPointer list, Operation operation) {
    for (; boolean(list); list = rest(list)) {
        value = operation(value, first(list));
    }
    return value;
}

inline ExpressionPointer reverse(CodeRange code, ExpressionPointer list) {
    return leftFold(makeNewEmptyString(new NewEmptyString{code}), list, prepend);
}

template<typename Function>
ExpressionPointer map(ExpressionPointer list, Function f) {
    const auto op = [&](ExpressionPointer new_list, ExpressionPointer x) -> ExpressionPointer {
        return prepend(new_list, f(x));
    };
    const auto code = CodeRange{};
    const auto output = leftFold(makeNewEmptyString(new NewEmptyString{}), list, op);
    return reverse(code, output);
}

} // namespace new_string
