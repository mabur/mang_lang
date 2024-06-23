#include "built_in_functions.h"

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

static
Definition makeDefinitionBuiltIn(size_t i, const char* name, FunctionPointer function) {
    return Definition{
        {makeName(CodeRange{}, Name{name}).index, i},
        makeFunctionBuiltIn(CodeRange{}, {function}),
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
        makeDefinitionBuiltIn(i++, "clear",      container_functions::clearTyped),
        makeDefinitionBuiltIn(i++, "put",        container_functions::putTyped),
        makeDefinitionBuiltIn(i++, "take",       container_functions::takeTyped),
        makeDefinitionBuiltIn(i++, "drop",       container_functions::dropTyped),
        makeDefinitionBuiltIn(i++, "get",        container_functions::getTyped),
        makeDefinitionBuiltIn(i++, "add",        arithmetic::addTyped),
        makeDefinitionBuiltIn(i++, "mul",        arithmetic::mulTyped),
        makeDefinitionBuiltIn(i++, "sub",        arithmetic::subTyped),
        makeDefinitionBuiltIn(i++, "div",        arithmetic::divTyped),
        makeDefinitionBuiltIn(i++, "mod",        arithmetic::modTyped),
        makeDefinitionBuiltIn(i++, "less",       arithmetic::lessTyped),
        makeDefinitionBuiltIn(i++, "round",      arithmetic::roundTyped),
        makeDefinitionBuiltIn(i++, "round_up",   arithmetic::roundUpTyped),
        makeDefinitionBuiltIn(i++, "round_down", arithmetic::roundDownTyped),
        makeDefinitionBuiltIn(i++, "sqrt",       arithmetic::sqrtTyped),
        makeDefinitionBuiltIn(i++, "number",     arithmetic::asciiNumberTyped),
        makeDefinitionBuiltIn(i++, "character",  arithmetic::asciiCharacterTyped),
    };
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
