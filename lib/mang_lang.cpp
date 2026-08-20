#include "mang_lang.h"
#include "factory.h"
#include "built_in_functions/built_in_functions.h"
#include "built_in_functions/standard_library.h"
#include "passes/evaluate.h"
#include "passes/parse.h"
#include "passes/resolve.h"
#include "passes/serialize.h"
#include "mang_lang_string.h"

#include <carma/carma.h>


static
StringBuilder serializeAndClearMemory(Expression expression) {
    auto buffer = StringBuilder{};
    buffer = serialize(buffer, expression);
    clearMemory();
    return buffer;
}

StringBuilder reformat(const char* code) {
    auto code_characters = makeCodeCharacters(code);
    return serializeAndClearMemory(resolve(parseExpression(code_characters)));
}

StringBuilder evaluate_types(const char* code) {
    const auto built_ins = builtInsTypes();
    const auto code_character_standard_library = makeCodeCharacters(STANDARD_LIBRARY.c_str());
    const auto code_characters_program = makeCodeCharacters(code);
    
    // N.B. Need to create all code characters before parsing for stable string interning.
    const auto std_ast = resolve(parseExpression(code_character_standard_library));
    const auto code_ast = resolve(parseExpression(code_characters_program));

    // bind(std_ast, built_ins);
    // bind(code_ast, std_ast);
    const auto standard_library = evaluate_types(std_ast, built_ins);
    auto buffer = StringBuilder{};
    buffer = serialize_types(buffer, evaluate_types(code_ast, standard_library));
    clearMemory();
    return buffer;
}

StringBuilder evaluate_all(const char* code) {
    const auto built_ins = builtIns();
    const auto built_ins_types = builtInsTypes();
    
    auto code_character_standard_library = makeCodeCharacters(STANDARD_LIBRARY.c_str());
    auto code_characters_program = makeCodeCharacters(code);
    // N.B. Need to create all code characters before parsing for stable string interning.
    
    const auto std_ast = resolve(parseExpression(code_character_standard_library));
    if (std_ast.type == ERROR_EXPRESSION) {
        return serializeAndClearMemory(std_ast);
    }
    const auto code_ast = resolve(parseExpression(code_characters_program));
    if (code_ast.type == ERROR_EXPRESSION) {
        return serializeAndClearMemory(code_ast);
    }
    // bind(std_ast, built_ins_types);
    // bind(code_ast, std_ast);
    const auto std_checked = evaluate_types(std_ast, built_ins_types);
    if (std_checked.type == ERROR_EXPRESSION) {
        return serializeAndClearMemory(std_checked);
    }
    const auto code_checked = evaluate_types(code_ast, std_checked);
    if (code_checked.type == ERROR_EXPRESSION) {
        return serializeAndClearMemory(code_checked);
    }
    const auto std_evaluated = evaluate(std_ast, built_ins);
    if (std_evaluated.type == ERROR_EXPRESSION) {
        return serializeAndClearMemory(std_evaluated);
    }
    const auto code_evaluated = evaluate(code_ast, std_evaluated);
    return serializeAndClearMemory(code_evaluated);
}
