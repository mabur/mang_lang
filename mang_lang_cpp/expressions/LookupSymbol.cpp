#include "LookupSymbol.h"
#include <algorithm>

#include "Name.h"

std::string LookupSymbol::serialize() const {
    return name->serialize();
}

ExpressionPointer LookupSymbol::evaluate(const Expression* environment, std::ostream& log) const {
    auto result = environment->lookup(name->value);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer LookupSymbol::parse(CodeRange code) {
    auto first = code.begin();
    auto name = Name::parse(code);
    return std::make_shared<LookupSymbol>(
        CodeRange{first, name->end()}, nullptr, name
    );
}

bool LookupSymbol::startsWith(CodeRange code) {
    return Name::startsWith(code);
}
