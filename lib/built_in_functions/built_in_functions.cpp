#include "built_in_functions.h"

#include <string.h>

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

const BuiltInEntry BUILT_IN_ENTRIES[] = {
    {"clear",      builtInClear, builtInClearTyped},
    {"put",        builtInPut,   builtInPutTyped},
    {"take",       builtInTake,  builtInTakeTyped},
    {"drop",       builtInDrop,  builtInDropTyped},
    {"get",        builtInGet,   builtInGetTyped},
    {"add",        arithmetic::builtInAdd,            arithmetic::builtInAdd},
    {"mul",        arithmetic::builtInMul,            arithmetic::builtInMul},
    {"sub",        arithmetic::builtInSub,            arithmetic::builtInSub},
    {"div",        arithmetic::builtInDiv,            arithmetic::builtInDiv},
    {"mod",        arithmetic::builtInMod,            arithmetic::builtInMod},
    {"less",       arithmetic::builtInLess,           arithmetic::builtInLess},
    {"round",      arithmetic::builtInRound,          arithmetic::builtInRound},
    {"round_up",   arithmetic::builtInRoundUp,        arithmetic::builtInRoundUp},
    {"round_down", arithmetic::builtInRoundDown,      arithmetic::builtInRoundDown},
    {"sqrt",       arithmetic::builtInSqrt,           arithmetic::builtInSqrt},
    {"number",     arithmetic::builtInAsciiNumber,    arithmetic::builtInAsciiNumber},
    {"character",  arithmetic::builtInAsciiCharacter, arithmetic::builtInAsciiCharacter},
};

const size_t BUILT_IN_ENTRIES_COUNT = sizeof(BUILT_IN_ENTRIES) / sizeof(BUILT_IN_ENTRIES[0]);

const BuiltInEntry* findBuiltIn(size_t name_index) {
    const auto name_text = storage.names.data + name_index;
    for (size_t i = 0; i < BUILT_IN_ENTRIES_COUNT; ++i) {
        if (strcmp(name_text, BUILT_IN_ENTRIES[i].name) == 0) {
            return &BUILT_IN_ENTRIES[i];
        }
    }
    return nullptr;
}

static
Definition makeDefinitionBuiltIn(size_t i, const char* name, FunctionPointer function) {
    return Definition{
        {makeName(CodeRange{}, name, strlen(name)).index, i},
        makeFunctionBuiltIn(CodeRange{}, {function}),
    };
}

Expression builtIns() {
    auto first = storage.definitions.count;
    for (size_t i = 0; i < BUILT_IN_ENTRIES_COUNT; ++i) {
        auto entry = BUILT_IN_ENTRIES[i];
        makeDefinition({}, makeDefinitionBuiltIn(i, entry.name, entry.function));
    }
    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}

Expression builtInsTypes() {
    auto first = storage.definitions.count;
    for (size_t i = 0; i < BUILT_IN_ENTRIES_COUNT; ++i) {
        auto entry = BUILT_IN_ENTRIES[i];
        makeDefinition({}, makeDefinitionBuiltIn(i, entry.name, entry.function_types));
    }
    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    return makeEvaluatedDictionary(CodeRange{},
        EvaluatedDictionary{Expression{}, definitions}
    );
}
