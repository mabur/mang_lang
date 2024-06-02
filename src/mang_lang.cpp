#include "mang_lang.h"
#include "factory.h"
#include "built_in_functions/built_in_functions.h"
#include "built_in_functions/standard_library.h"
#include "passes/bind.h"
#include "passes/evaluate.h"
#include "passes/parse.h"
#include "passes/serialize.h"

#include <carma/carma.h>

static
Expression parse(const std::string& string) {
    auto c = makeCodeCharacters(string);
    auto expression = parseExpression(makeCodeRange(c.data, c.data + c.count));
    FREE_RANGE(c);
    return expression;
}

std::string reformat(const std::string& code) {
    auto buffer = SerializedString{};
    auto result = makeStdString(serialize(buffer, parse(code)));
    clearMemory();
    return result;
}

std::string evaluate_types(const std::string& code) {
    const auto built_ins = builtInsTypes();
    const auto std_ast = parse(STANDARD_LIBRARY);
    const auto code_ast = parse(code);
    // bind(std_ast, built_ins);
    // bind(code_ast, std_ast);
    const auto standard_library = evaluate_types(std_ast, built_ins);
    auto buffer = SerializedString{};
    auto result = makeStdString(serialize_types(buffer, evaluate_types(code_ast, standard_library)));
    clearMemory();
    return result;
}

std::string evaluate_all(const std::string& code) {
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
    auto buffer = SerializedString{};
    auto result = makeStdString(serialize(buffer, code_evaluated));
    clearMemory();
    std::ignore = code_checked;
    return result;
}
