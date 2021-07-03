#include "FunctionDictionary.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"

#include "../operations/apply.h"
#include "../operations/evaluate.h"
#include "../operations/serialize.h"

std::string FunctionDictionary::serialize() const {
    return ::serialize(*this);
}

ExpressionPointer FunctionDictionary::evaluate(const Expression* environment, std::ostream& log) const {
    return ::evaluate(*this, environment, log);
}

ExpressionPointer FunctionDictionary::apply(ExpressionPointer input, std::ostream& log) const {
    return ::apply(*this, input, log);
}

ExpressionPointer FunctionDictionary::parse(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '{');
    code = parseWhiteSpace(code);
    auto input_names = std::vector<NamePointer>{};
    while (!::startsWith(code, '}')) {
        throwIfEmpty(code);
        const auto name = Name::parse(code);
        code.first = name->end();
        input_names.push_back(name);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, '}');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = Expression::parse(code);
    code.first = body->end();
    return std::make_shared<FunctionDictionary>(
        CodeRange{first, code.begin()}, nullptr, input_names, body
    );
}

bool FunctionDictionary::startsWith(CodeRange code) {
    if (!isKeyword(code, "in")) {
        return false;
    }
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    return ::startsWith(code, '{');
}
