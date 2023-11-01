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
        {makeName({}, "clear").index,      define(container_functions::clear), i++},
        {makeName({}, "put").index,        define(container_functions::put), i++},
        {makeName({}, "take").index,       define(container_functions::take), i++},
        {makeName({}, "drop").index,       define(container_functions::drop), i++},
        {makeName({}, "get").index,        define(container_functions::get), i++},
        {makeName({}, "add").index,        define(arithmetic::add), i++},
        {makeName({}, "mul").index,        define(arithmetic::mul), i++},
        {makeName({}, "sub").index,        define(arithmetic::sub), i++},
        {makeName({}, "div").index,        define(arithmetic::div), i++},
        {makeName({}, "mod").index,        define(arithmetic::mod), i++},
        {makeName({}, "less").index,       define(arithmetic::less), i++},
        {makeName({}, "round").index,      define(arithmetic::round), i++},
        {makeName({}, "round_up").index,   define(arithmetic::round_up), i++},
        {makeName({}, "round_down").index, define(arithmetic::round_down), i++},
        {makeName({}, "sqrt").index,       define(arithmetic::sqrt), i++},
        {makeName({}, "number").index,     define(arithmetic::ascii_number), i++},
        {makeName({}, "character").index,  define(arithmetic::ascii_character), i++},
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    size_t i = 0;
    const auto definitions = std::vector<Definition>{
        {makeName({}, "clear").index,       define(container_functions::clearTyped), i++},
        {makeName({}, "put").index,         define(container_functions::putTyped), i++},
        {makeName({}, "take").index,        define(container_functions::takeTyped), i++},
        {makeName({}, "drop").index,        define(container_functions::dropTyped), i++},
        {makeName({}, "get").index,         define(container_functions::getTyped), i++},
        {makeName({}, "add").index,         define(arithmetic::FunctionNumberNumberToNumber{"add"}), i++},
        {makeName({}, "mul").index,         define(arithmetic::FunctionNumberNumberToNumber{"mul"}), i++},
        {makeName({}, "sub").index,         define(arithmetic::FunctionNumberNumberToNumber{"sub"}), i++},
        {makeName({}, "div").index,         define(arithmetic::FunctionNumberNumberToNumber{"div"}), i++},
        {makeName({}, "mod").index,         define(arithmetic::FunctionNumberNumberToNumber{"mod"}), i++},
        {makeName({}, "less").index,        define(arithmetic::FunctionNumberNumberToBoolean{"less"}), i++},
        {makeName({}, "round").index,       define(arithmetic::FunctionNumberToNumber{"round"}), i++},
        {makeName({}, "round_up").index,    define(arithmetic::FunctionNumberToNumber{"round_up"}), i++},
        {makeName({}, "round_down").index,  define(arithmetic::FunctionNumberToNumber{"round_down"}), i++},
        {makeName({}, "sqrt").index,        define(arithmetic::FunctionNumberToNumber{"sqrt"}), i++},
        {makeName({}, "number").index,      define(arithmetic::FunctionCharacterToNumber{"number"}), i++},
        {makeName({}, "character").index,   define(arithmetic::FunctionNumberToCharacter{"character"}), i++},
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
