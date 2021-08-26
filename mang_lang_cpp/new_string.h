#pragma once

#include "Expression.h"
#include "operations/boolean.h"
#include "operations/begin.h"
#include "operations/end.h"
#include "factory.h"

namespace new_string {

Expression first(Expression list);
Expression rest(Expression list);
Expression second(Expression list);

inline Expression prepend(Expression rest, Expression first) {
    const auto first_character = std::min(begin(rest), begin(first));
    const auto last_character = std::max(end(rest), end(first));
    const auto code = CodeRange{first_character, last_character};
    return makeNewString(new String{code, first, rest});
}

template<typename Predicate>
Expression findIf(Expression list, Predicate predicate) {
    for (; boolean(list) && !predicate(first(list)); list = rest(list)) {
    }
    return list;
}

template<typename T, typename Operation>
T leftFold(T value, Expression list, Operation operation) {
    for (; boolean(list); list = rest(list)) {
        value = operation(value, first(list));
    }
    return value;
}

inline Expression reverse(CodeRange code, Expression list) {
    return leftFold(makeNewEmptyString(new NewEmptyString{code}), list, prepend);
}

template<typename Function>
Expression map(Expression list, Function f) {
    const auto op = [&](Expression new_list, Expression x) -> Expression {
        return prepend(new_list, f(x));
    };
    const auto code = CodeRange{};
    const auto output = leftFold(makeNewEmptyString(new NewEmptyString{}), list, op);
    return reverse(code, output);
}

} // namespace new_string
