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

// Builds the built-in environment: a dictionary value with one slot per
// entry, holding either the value function or the type function of each entry.
static
Expression makeBuiltInEnvironment(bool types) {
    auto slot_values = Indices{storage.slot_values.count, BUILT_IN_ENTRIES_COUNT};
    auto first = storage.dictionary_names.count;
    for (size_t i = 0; i < BUILT_IN_ENTRIES_COUNT; ++i) {
        auto entry = BUILT_IN_ENTRIES[i];
        auto function = types ? entry.function_types : entry.function;
        APPEND(storage.slot_values, makeFunctionBuiltInValue(CodeRange{}, {function}));
        APPEND(storage.dictionary_names, makeName(CodeRange{}, entry.name, strlen(entry.name)).index);
    }
    auto names = Indices{first, BUILT_IN_ENTRIES_COUNT};
    return makeDictionaryValue(CodeRange{}, DictionaryValue{Expression{}, slot_values, names});
}

Expression builtIns() {
    return makeBuiltInEnvironment(false);
}

Expression builtInsTypes() {
    return makeBuiltInEnvironment(true);
}
