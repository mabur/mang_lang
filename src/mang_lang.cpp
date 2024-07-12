#include "mang_lang.h"
#include "factory.h"
#include "built_in_functions/built_in_functions.h"
#include "built_in_functions/standard_library.h"
#include "passes/bind.h"
#include "passes/evaluate.h"
#include "passes/parse.h"
#include "passes/serialize.h"
#include "string.h"

#include <carma/carma.h>

static
Expression parse(const char* string) {
    auto c = makeCodeCharacters(string);
    auto expression = parseExpression(c);
    FREE_RANGE(c);
    return expression;
}

DynamicString reformat(const char* code) {
    auto buffer = DynamicString{};
    buffer = serialize(buffer, parse(code));
    clearMemory();
    return buffer;
}

DynamicString evaluate_types(const char* code) {
    const auto built_ins = builtInsTypes();
    const auto std_ast = parse(STANDARD_LIBRARY.c_str());
    const auto code_ast = parse(code);
    // bind(std_ast, built_ins);
    // bind(code_ast, std_ast);
    const auto standard_library = evaluate_types(std_ast, built_ins);
    auto buffer = DynamicString{};
    buffer = serialize_types(buffer, evaluate_types(code_ast, standard_library));
    clearMemory();
    return buffer;
}

DynamicString evaluate_all(const char* code) {
    const auto built_ins = builtIns();
    const auto built_ins_types = builtInsTypes();
    const auto std_ast = parse(STANDARD_LIBRARY.c_str());
    const auto code_ast = parse(code);
    // bind(std_ast, built_ins_types);
    // bind(code_ast, std_ast);
    const auto std_checked = evaluate_types(std_ast, built_ins_types);
    const auto code_checked = evaluate_types(code_ast, std_checked);
    const auto std_evaluated = evaluate(std_ast, built_ins);
    const auto code_evaluated = evaluate(code_ast, std_evaluated);
    std::ignore = code_checked;
    auto buffer = DynamicString{};
    buffer = serialize(buffer, code_evaluated);
    clearMemory();
    return buffer;
}
