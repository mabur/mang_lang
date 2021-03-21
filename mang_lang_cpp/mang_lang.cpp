#include <fstream>

#include "mang_lang.h"
#include "expressions/Expression.h"
#include "built_in_functions/built_in_functions.h"

auto nullStream() {
    std::ofstream stream;
    stream.setstate(std::ios_base::badbit);
    return stream;
}

ExpressionPointer parse(const std::string& string) {
    const auto result = makeCodeCharacters(string);
    return Expression::parse(result.data(), result.data() + result.size());
}

std::string reformat(std::string code) {
    return parse(code)->serialize();
}

std::string evaluate(std::string code) {
    auto log = nullStream();
    const auto built_ins = builtIns();
    return parse(code)->evaluate(built_ins.get(), log)->serialize();
}
