#include "FunctionList.h"
#include <algorithm>
#include "../parsing.h"
#include "Dictionary.h"
#include "DictionaryElement.h"

std::string FunctionList::serialize() const {
    auto result = std::string{};
    result += "in ";
    result += "(";
    for (const auto& name : input_names) {
        result += name->serialize();
        result += " ";
    }
    if (input_names.empty()) {
        result += ')';
    }
    else {
        result.back() = ')';
    }
    result += " out " + body->serialize();
    return result;
}

ExpressionPointer FunctionList::evaluate(const Expression* environment, std::ostream& log) const {
    auto result = std::make_shared<FunctionList>(range(), environment, input_names, body);
    log << result->serialize() << std::endl;
    return result;
}

ExpressionPointer FunctionList::apply(ExpressionPointer input, std::ostream& log) const {
    auto middle = Dictionary(range(), environment());
    auto i = 0;
    for (auto list = input->list(); list; list = list->rest, ++i) {
        middle.elements.push_back(
            std::make_shared<NamedElement>(
                range(), &middle, input_names[i], list->first, i
            )
        );
    }
    auto output = body->evaluate(&middle, log);
    return output;
}

ExpressionPointer FunctionList::parse(CodeRange code) {
    auto first = code.begin();
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    code = parseCharacter(code, '(');
    code = parseWhiteSpace(code);
    auto input_names = std::vector<NamePointer>{};
    while (!::startsWith(code, ')')) {
        throwIfEmpty(code);
        const auto name = Name::parse(code);
        code.first = name->end();
        input_names.push_back(name);
        code = parseWhiteSpace(code);
    }
    code = parseCharacter(code, ')');
    code = parseWhiteSpace(code);
    code = parseKeyword(code, "out");
    auto body = Expression::parse(code);
    code.first = body->end();
    return std::make_shared<FunctionList>(
        CodeRange{first, code.begin()}, nullptr, input_names, body
    );
}

bool FunctionList::startsWith(CodeRange code) {
    if (!isKeyword(code, "in")) {
        return false;
    }
    code = parseKeyword(code, "in");
    code = parseWhiteSpace(code);
    return ::startsWith(code, '(');
}
