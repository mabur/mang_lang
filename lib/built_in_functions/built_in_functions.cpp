#include "built_in_functions.h"

#include <string.h>

#include <carma/carma.h>

#include "../factory.h"
#include "arithmetic.h"
#include "container.h"

const BuiltInEntry BUILT_IN_ENTRIES[] = {
    {"clear",      builtInClear, builtInClearTyped},
    {"put",        builtInPut,   builtInPutTyped},
    {"take",       builtInTake,  builtInTakeTyped},
    {"drop",       builtInDrop,  builtInDropTyped},
    {"get",        builtInGet,   builtInGetTyped},
    {"add",        builtInAdd,            builtInAdd},
    {"mul",        builtInMul,            builtInMul},
    {"sub",        builtInSub,            builtInSub},
    {"div",        builtInDiv,            builtInDiv},
    {"mod",        builtInMod,            builtInMod},
    {"less",       builtInLess,           builtInLess},
    {"round",      builtInRound,          builtInRound},
    {"round_up",   builtInRoundUp,        builtInRoundUp},
    {"round_down", builtInRoundDown,      builtInRoundDown},
    {"sqrt",       builtInSqrt,           builtInSqrt},
    {"number",     builtInAsciiNumber,    builtInAsciiNumber},
    {"character",  builtInAsciiCharacter, builtInAsciiCharacter},
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
        makeFunctionBuiltInValue(CodeRange{}, {function}),
    };
}

// Persists the built-in names in slot order, in the same shared array that
// dictionary and function expressions use for their slot names.
static
Indices appendBuiltInNames() {
    const auto first = storage.dictionary_names.count;
    for (size_t i = 0; i < BUILT_IN_ENTRIES_COUNT; ++i) {
        const auto name = BUILT_IN_ENTRIES[i].name;
        APPEND(storage.dictionary_names, makeName(CodeRange{}, name, strlen(name)).index);
    }
    return Indices{first, BUILT_IN_ENTRIES_COUNT};
}

Expression builtIns() {
    auto first = storage.definitions.count;
    for (size_t i = 0; i < BUILT_IN_ENTRIES_COUNT; ++i) {
        auto entry = BUILT_IN_ENTRIES[i];
        makeDefinition({}, makeDefinitionBuiltIn(i, entry.name, entry.function));
    }
    auto last = storage.definitions.count;
    auto definitions = Indices{first, last - first};
    return makeDictionaryValue(CodeRange{},
        DictionaryValue{Expression{}, definitions, appendBuiltInNames()}
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
    return makeDictionaryValue(CodeRange{},
        DictionaryValue{Expression{}, definitions, appendBuiltInNames()}
    );
}
