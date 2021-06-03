#include "FunctionDictionary.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"

std::string FunctionDictionary::serialize() const {
    auto result = std::string{};
    result += "in ";
    result += "{";
    for (const auto& name : input_names) {
        result += name->serialize();
        result += ",";
    }
    if (input_names.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    result += " out " + body->serialize();
    return result;
}

ExpressionPointer FunctionDictionary::evaluate(const Expression* parent, std::ostream& log) const {
    auto result = std::make_shared<FunctionDictionary>(range(), parent, input_names, body);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer FunctionDictionary::apply(ExpressionPointer input, std::ostream& log) const {
    auto middle = input->evaluate(parent(), log);
    auto output = body->evaluate(middle.get(), log);
    return output;
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
        code = parseOptionalCharacter(code, ',');
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
