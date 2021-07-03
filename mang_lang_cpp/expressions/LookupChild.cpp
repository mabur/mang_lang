#include "LookupChild.h"

#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string LookupChild::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer LookupChild::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}

bool LookupChild::startsWith(CodeRange code) {
    if (!Name::startsWith(code)) {
        return false;
    }
    code = parseWhile(code, isNameCharacter);
    code = parseWhiteSpace(code);
    return ::startsWith(code, '@');
}
