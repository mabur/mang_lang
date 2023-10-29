#include "built_in_functions.h"

#include <memory>

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

Expression define(std::function<Expression(Expression)> function) {
    return makeFunctionBuiltIn(CodeRange{}, {function});
}

Expression builtIns() {
    size_t i = 0;
    const auto definitions = std::vector<Definition>{
        {makeName({}, "clear"),      define(container_functions::clear), i++},
        {makeName({}, "put"),        define(container_functions::put), i++},
        {makeName({}, "take"),       define(container_functions::take), i++},
        {makeName({}, "drop"),       define(container_functions::drop), i++},
        {makeName({}, "get"),        define(container_functions::get), i++},
        {makeName({}, "add"),        define(arithmetic::add), i++},
        {makeName({}, "mul"),        define(arithmetic::mul), i++},
        {makeName({}, "sub"),        define(arithmetic::sub), i++},
        {makeName({}, "div"),        define(arithmetic::div), i++},
        {makeName({}, "mod"),        define(arithmetic::mod), i++},
        {makeName({}, "less"),       define(arithmetic::less), i++},
        {makeName({}, "round"),      define(arithmetic::round), i++},
        {makeName({}, "round_up"),   define(arithmetic::round_up), i++},
        {makeName({}, "round_down"), define(arithmetic::round_down), i++},
        {makeName({}, "sqrt"),       define(arithmetic::sqrt), i++},
        {makeName({}, "number"),     define(arithmetic::ascii_number), i++},
        {makeName({}, "character"),  define(arithmetic::ascii_character), i++},
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    size_t i = 0;
    const auto definitions = std::vector<Definition>{
        {makeName({}, "clear"),       define(container_functions::clearTyped), i++},
        {makeName({}, "put"),         define(container_functions::putTyped), i++},
        {makeName({}, "take"),        define(container_functions::takeTyped), i++},
        {makeName({}, "drop"),        define(container_functions::dropTyped), i++},
        {makeName({}, "get"),         define(container_functions::getTyped), i++},
        {makeName({}, "add"),         define(arithmetic::FunctionNumberNumberToNumber{"add"}), i++},
        {makeName({}, "mul"),         define(arithmetic::FunctionNumberNumberToNumber{"mul"}), i++},
        {makeName({}, "sub"),         define(arithmetic::FunctionNumberNumberToNumber{"sub"}), i++},
        {makeName({}, "div"),         define(arithmetic::FunctionNumberNumberToNumber{"div"}), i++},
        {makeName({}, "mod"),         define(arithmetic::FunctionNumberNumberToNumber{"mod"}), i++},
        {makeName({}, "less"),        define(arithmetic::FunctionNumberNumberToBoolean{"less"}), i++},
        {makeName({}, "round"),       define(arithmetic::FunctionNumberToNumber{"round"}), i++},
        {makeName({}, "round_up"),    define(arithmetic::FunctionNumberToNumber{"round_up"}), i++},
        {makeName({}, "round_down"),  define(arithmetic::FunctionNumberToNumber{"round_down"}), i++},
        {makeName({}, "sqrt"),        define(arithmetic::FunctionNumberToNumber{"sqrt"}), i++},
        {makeName({}, "number"),      define(arithmetic::FunctionCharacterToNumber{"number"}), i++},
        {makeName({}, "character"),   define(arithmetic::FunctionNumberToCharacter{"character"}), i++},
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
