#include "parser.h"

#include <memory>

ExpressionPointer parse(const std::string& string) {
    const auto result = makeCodeCharacters(string);
    return Expression::parse(result.data(), result.data() + result.size());
}
