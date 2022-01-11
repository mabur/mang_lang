#include "built_in_functions.h"

#include <memory>

#include "../factory.h"
#include "arithmetic.h"
#include "list.h"

Expression define(std::string name, std::function<Expression(Expression)> function) {
    return makeDefinition(CodeRange{}, {
        makeName(CodeRange{}, {name}),
        makeFunctionBuiltIn(CodeRange{}, {function}),
        0
    });
}

Expression builtIns() {
    auto definitions = Statements{};
    definitions.push_back(define("clear", list_functions::empty));
    definitions.push_back(define("put", list_functions::prepend));
    definitions.push_back(define("add", arithmetic::add));
    definitions.push_back(define("mul", arithmetic::mul));
    definitions.push_back(define("sub", arithmetic::sub));
    definitions.push_back(define("div", arithmetic::div));
    definitions.push_back(define("less", arithmetic::are_all_less));
    definitions.push_back(define("less_or_equal", arithmetic::are_all_less_or_equal));
    definitions.push_back(define("round", arithmetic::round));
    definitions.push_back(define("round_up", arithmetic::round_up));
    definitions.push_back(define("round_down", arithmetic::round_down));
    definitions.push_back(define("sqrt", arithmetic::sqrt));
    definitions.push_back(define("number", arithmetic::ascii_number));
    definitions.push_back(define("character", arithmetic::ascii_character));
    return makeEvaluatedDictionary(CodeRange{},
        new EvaluatedDictionary{Expression{}, setContext(definitions)}
    );
}
