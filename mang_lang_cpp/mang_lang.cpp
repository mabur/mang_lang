#include "mang_lang.h"
#include "factory.h"
#include "built_in_functions/built_in_functions.h"
#include "built_in_functions/standard_library.h"
#include "operations/evaluate.h"
#include "operations/parse.h"
#include "operations/serialize.h"

Expression parse(const std::string& string) {
    const auto result = makeCodeCharacters(string);
    return parseExpression({result.data(), result.data() + result.size()});
}

std::string reformat(std::string code) {
    const auto result = serialize(parse(code));
    clearMemory();
    return result;
}

std::string evaluate(std::string code) {
    const auto built_ins = builtIns();
    const auto standard_library = evaluate(parse(STANDARD_LIBRARY), built_ins);
    const auto result = serialize(evaluate(parse(code), standard_library));
    clearMemory();
    return result;
}
