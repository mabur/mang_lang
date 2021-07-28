#include <fstream>

#include "mang_lang.h"
#include "expressions/Expression.h"
#include "built_in_functions/built_in_functions.h"
#include "built_in_functions/standard_library.h"
#include "operations/evaluate.h"
#include "operations/parse.h"
#include "operations/serialize.h"

auto nullStream() {
    std::ofstream stream;
    stream.setstate(std::ios_base::badbit);
    return stream;
}

ExpressionPointer parse(const std::string& string) {
    const auto result = makeCodeCharacters(string);
    return parseExpression({result.data(), result.data() + result.size()});
}

std::string reformat(std::string code) {
    return serialize(parse(code).get());
}

std::string evaluate(std::string code) {
    auto log = nullStream();
    const auto built_ins = builtIns();
    const auto standard_library = evaluate(parse(STANDARD_LIBRARY), built_ins.get(), log);
    return serialize(evaluate(parse(code), standard_library.get(), log).get());
    //return serialize(parse(code)->evaluate(nullptr, log).get());
}
