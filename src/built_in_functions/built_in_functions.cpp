#include "built_in_functions.h"

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

Definition makeDefinitionBuiltIn(size_t i, const char* name, FunctionPointer function) {
    return Definition{
        {makeName(CodeRange{}, Name{name}).index, i},
        makeFunctionBuiltIn(CodeRange{}, {function}),
    };
}

Definition makeDefinitionBuiltInTyped(size_t i, const char* name, FunctionPointer function) {
    return Definition{
        {makeName(CodeRange{}, Name{name}).index, i},
        makeFunctionBuiltIn(CodeRange{}, {function})
    };
}

Expression builtIns() {
    size_t i  = 0;
    const auto definitions = std::vector<Definition>{
        makeDefinitionBuiltIn(i++, "clear",      container_functions::clear),
        makeDefinitionBuiltIn(i++, "put",        container_functions::put),
        makeDefinitionBuiltIn(i++, "take",       container_functions::take),
        makeDefinitionBuiltIn(i++, "drop",       container_functions::drop),
        makeDefinitionBuiltIn(i++, "get",        container_functions::get),
        makeDefinitionBuiltIn(i++, "add",        arithmetic::add),
        makeDefinitionBuiltIn(i++, "mul",        arithmetic::mul),
        makeDefinitionBuiltIn(i++, "sub",        arithmetic::sub),
        makeDefinitionBuiltIn(i++, "div",        arithmetic::div),
        makeDefinitionBuiltIn(i++, "mod",        arithmetic::mod),
        makeDefinitionBuiltIn(i++, "less",       arithmetic::less),
        makeDefinitionBuiltIn(i++, "round",      arithmetic::round),
        makeDefinitionBuiltIn(i++, "round_up",   arithmetic::roundUp),
        makeDefinitionBuiltIn(i++, "round_down", arithmetic::roundDown),
        makeDefinitionBuiltIn(i++, "sqrt",       arithmetic::sqrt),
        makeDefinitionBuiltIn(i++, "number",     arithmetic::asciiNumber),
        makeDefinitionBuiltIn(i++, "character",  arithmetic::asciiCharacter),
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    size_t i = 0;
    const auto definitions = std::vector<Definition>{
        makeDefinitionBuiltInTyped(i++, "clear",      container_functions::clearTyped),
        makeDefinitionBuiltInTyped(i++, "put",        container_functions::putTyped),
        makeDefinitionBuiltInTyped(i++, "take",       container_functions::takeTyped),
        makeDefinitionBuiltInTyped(i++, "drop",       container_functions::dropTyped),
        makeDefinitionBuiltInTyped(i++, "get",        container_functions::getTyped),
        makeDefinitionBuiltInTyped(i++, "add",        arithmetic::addTyped),
        makeDefinitionBuiltInTyped(i++, "mul",        arithmetic::mulTyped),
        makeDefinitionBuiltInTyped(i++, "sub",        arithmetic::subTyped),
        makeDefinitionBuiltInTyped(i++, "div",        arithmetic::divTyped),
        makeDefinitionBuiltInTyped(i++, "mod",        arithmetic::modTyped),
        makeDefinitionBuiltInTyped(i++, "less",       arithmetic::lessTyped),
        makeDefinitionBuiltInTyped(i++, "round",      arithmetic::roundTyped),
        makeDefinitionBuiltInTyped(i++, "round_up",   arithmetic::roundUpTyped),
        makeDefinitionBuiltInTyped(i++, "round_down", arithmetic::roundDownTyped),
        makeDefinitionBuiltInTyped(i++, "sqrt",       arithmetic::sqrtTyped),
        makeDefinitionBuiltInTyped(i++, "number",     arithmetic::asciiNumberTyped),
        makeDefinitionBuiltInTyped(i++, "character",  arithmetic::asciiCharacterTyped),
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
