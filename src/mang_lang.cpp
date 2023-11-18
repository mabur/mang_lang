#include "mang_lang.h"
#include "factory.h"
#include "built_in_functions/built_in_functions.h"
#include "built_in_functions/standard_library.h"
#include "passes/bind.h"
#include "passes/evaluate.h"
#include "passes/parse.h"
#include "passes/serialize.h"

static
Expression parse(const std::string& string) {
    const auto result = makeCodeCharacters(string);
    return parseExpression({result.data(), result.data() + result.size()});
}

std::string reformat(std::string code) {
    std::string result;
    serialize(result, parse(code));
    clearMemory();
    return result;
}

std::string evaluate_types(std::string code) {
    const auto built_ins = builtInsTypes();
    const auto std_ast = parse(STANDARD_LIBRARY);
    const auto code_ast = parse(code);
    // bind(std_ast, built_ins);
    // bind(code_ast, std_ast);
    const auto standard_library = evaluate_types(std_ast, built_ins);
    std::string result;
    serialize_types(result, evaluate_types(code_ast, standard_library));
    clearMemory();
    return result;
}

std::string evaluate_all(std::string code) {
    const auto built_ins = builtIns();
    const auto built_ins_types = builtInsTypes();
    const auto std_ast = parse(STANDARD_LIBRARY);
    const auto code_ast = parse(code);
    // bind(std_ast, built_ins_types);
    // bind(code_ast, std_ast);
    const auto std_checked = evaluate_types(std_ast, built_ins_types);
    const auto code_checked = evaluate_types(code_ast, std_checked);
    const auto std_evaluated = evaluate(std_ast, built_ins);
    const auto code_evaluated = evaluate(code_ast, std_evaluated);
    std::string result;
    serialize(result, code_evaluated);
    clearMemory();
    std::ignore = code_checked;
    return result;
}
