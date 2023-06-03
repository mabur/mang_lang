#include "built_in_functions.h"

#include <memory>

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

Expression define(std::function<Expression(Expression)> function) {
    return makeFunctionBuiltIn(CodeRange{}, {function});
}

Expression builtIns() {
    const auto definitions = Definitions{{
        {makeName({}, "clear"),      define(container_functions::clear)},
        {makeName({}, "put"),        define(container_functions::put)},
        {makeName({}, "take"),       define(container_functions::take)},
        {makeName({}, "drop"),       define(container_functions::drop)},
        {makeName({}, "get"),        define(container_functions::get)},
        {makeName({}, "add"),        define(arithmetic::add)},
        {makeName({}, "mul"),        define(arithmetic::mul)},
        {makeName({}, "sub"),        define(arithmetic::sub)},
        {makeName({}, "div"),        define(arithmetic::div)},
        {makeName({}, "mod"),        define(arithmetic::mod)},
        {makeName({}, "less"),       define(arithmetic::less)},
        {makeName({}, "round"),      define(arithmetic::round)},
        {makeName({}, "round_up"),   define(arithmetic::round_up)},
        {makeName({}, "round_down"), define(arithmetic::round_down)},
        {makeName({}, "sqrt"),       define(arithmetic::sqrt)},
        {makeName({}, "number"),     define(arithmetic::ascii_number)},
        {makeName({}, "character"),  define(arithmetic::ascii_character)},
    }};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    const auto definitions = Definitions{{
        {makeName({}, "clear"),       define(container_functions::clearTyped)},
        {makeName({}, "put"),         define(container_functions::putTyped)},
        {makeName({}, "take"),        define(container_functions::takeTyped)},
        {makeName({}, "drop"),        define(container_functions::dropTyped)},
        {makeName({}, "get"),         define(container_functions::get)},
        {makeName({}, "add"),         define(arithmetic::FunctionNumberNumberToNumber{"add"})},
        {makeName({}, "mul"),         define(arithmetic::FunctionNumberNumberToNumber{"mul"})},
        {makeName({}, "sub"),         define(arithmetic::FunctionNumberNumberToNumber{"sub"})},
        {makeName({}, "div"),         define(arithmetic::FunctionNumberNumberToNumber{"div"})},
        {makeName({}, "mod"),         define(arithmetic::FunctionNumberNumberToNumber{"mod"})},
        {makeName({}, "less"),        define(arithmetic::FunctionNumberNumberToBoolean{"less"})},
        {makeName({}, "round"),       define(arithmetic::FunctionNumberToNumber{"round"})},
        {makeName({}, "round_up"),    define(arithmetic::FunctionNumberToNumber{"round_up"})},
        {makeName({}, "round_down"),  define(arithmetic::FunctionNumberToNumber{"round_down"})},
        {makeName({}, "sqrt"),        define(arithmetic::FunctionNumberToNumber{"sqrt"})},
        {makeName({}, "number"),      define(arithmetic::FunctionCharacterToNumber{"number"})},
        {makeName({}, "character"),   define(arithmetic::FunctionNumberToCharacter{"character"})},
    }};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
