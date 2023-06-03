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
        {"clear",      define(container_functions::clear)},
        {"put",        define(container_functions::put)},
        {"take",       define(container_functions::take)},
        {"drop",       define(container_functions::drop)},
        {"get",        define(container_functions::get)},
        {"add",        define(arithmetic::add)},
        {"mul",        define(arithmetic::mul)},
        {"sub",        define(arithmetic::sub)},
        {"div",        define(arithmetic::div)},
        {"mod",        define(arithmetic::mod)},
        {"less",       define(arithmetic::less)},
        {"round",      define(arithmetic::round)},
        {"round_up",   define(arithmetic::round_up)},
        {"round_down", define(arithmetic::round_down)},
        {"sqrt",       define(arithmetic::sqrt)},
        {"number",     define(arithmetic::ascii_number)},
        {"character",  define(arithmetic::ascii_character)},
    }};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    const auto definitions = Definitions{{
        {"clear",       define(container_functions::clearTyped)},
        {"put",         define(container_functions::putTyped)},
        {"take",        define(container_functions::takeTyped)},
        {"drop",        define(container_functions::dropTyped)},
        {"get",         define(container_functions::get)},
        {"add",         define(arithmetic::FunctionNumberNumberToNumber{"add"})},
        {"mul",         define(arithmetic::FunctionNumberNumberToNumber{"mul"})},
        {"sub",         define(arithmetic::FunctionNumberNumberToNumber{"sub"})},
        {"div",         define(arithmetic::FunctionNumberNumberToNumber{"div"})},
        {"mod",         define(arithmetic::FunctionNumberNumberToNumber{"mod"})},
        {"less",        define(arithmetic::FunctionNumberNumberToBoolean{"less"})},
        {"round",       define(arithmetic::FunctionNumberToNumber{"round"})},
        {"round_up",    define(arithmetic::FunctionNumberToNumber{"round_up"})},
        {"round_down",  define(arithmetic::FunctionNumberToNumber{"round_down"})},
        {"sqrt",        define(arithmetic::FunctionNumberToNumber{"sqrt"})},
        {"number",      define(arithmetic::FunctionCharacterToNumber{"number"})},
        {"character",   define(arithmetic::FunctionNumberToCharacter{"character"})},
    }};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
