#include "built_in_functions.h"

#include <memory>

#include "../factory.h"
#include "arithmetic.h"
#include "list.h"
#include "logic.h"

Expression define(std::string name, std::function<Expression(Expression)> function) {
    return makeDefinition(
        new Definition{
            CodeRange{},
            makeName(new Name{CodeRange{}, name}),
            makeFunctionBuiltIn(new FunctionBuiltIn{{}, function}),
            0
        }
    );
}

Expression builtIns() {
    auto definitions = Statements{};
    definitions.push_back(define("min", arithmetic::min));
    definitions.push_back(define("max", arithmetic::max));
    definitions.push_back(define("add", arithmetic::add));
    definitions.push_back(define("mul", arithmetic::mul));
    definitions.push_back(define("sub", arithmetic::sub));
    definitions.push_back(define("div", arithmetic::div));
    definitions.push_back(define("less", arithmetic::less));
    definitions.push_back(define("less_or_equal", arithmetic::less_or_equal));
    definitions.push_back(define("round", arithmetic::round));
    definitions.push_back(define("round_up", arithmetic::round_up));
    definitions.push_back(define("round_down", arithmetic::round_down));
    definitions.push_back(define("abs", arithmetic::abs));
    definitions.push_back(define("sqrt", arithmetic::sqrt));
    definitions.push_back(define("number", arithmetic::ascii_number));
    definitions.push_back(define("character", arithmetic::ascii_character));
    definitions.push_back(define("boolean", logic::booleanExpression));
    definitions.push_back(define("not", logic::logic_not));
    definitions.push_back(define("all", logic::all));
    definitions.push_back(define("any", logic::any));
    definitions.push_back(define("none", logic::none));
    definitions.push_back(define("equal", logic::equal));
    definitions.push_back(define("unequal", logic::unequal));
    definitions.push_back(define("new", list_functions::empty));
    definitions.push_back(define("prepend", list_functions::prepend));
    return makeDictionary(
        new Dictionary{CodeRange{}, Expression{}, setContext(definitions)}
    );
}
