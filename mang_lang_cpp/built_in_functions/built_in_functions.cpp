#include "built_in_functions.h"

#include <memory>

#include "../factory.h"
#include "arithmetic.h"
#include "dictionary.h"
#include "stack.h"

Expression define(std::function<Expression(Expression)> function) {
    return makeFunctionBuiltIn(CodeRange{}, {function});
}

Expression builtIns() {
    auto definitions = Definitions{};
    definitions.add("'clear'", define(stack_functions::clear));
    definitions.add("'put'", define(stack_functions::put));
    definitions.add("'get'", define(dictionary_functions::get));
    definitions.add("'get_names'", define(dictionary_functions::get_names));
    definitions.add("'add'", define(arithmetic::add));
    definitions.add("'mul'", define(arithmetic::mul));
    definitions.add("'sub'", define(arithmetic::sub));
    definitions.add("'div'", define(arithmetic::div));
    definitions.add("'less'", define(arithmetic::are_all_less));
    definitions.add("'round'", define(arithmetic::round));
    definitions.add("'round_up'", define(arithmetic::round_up));
    definitions.add("'round_down'", define(arithmetic::round_down));
    definitions.add("'sqrt'", define(arithmetic::sqrt));
    definitions.add("'number'", define(arithmetic::ascii_number));
    definitions.add("'character'", define(arithmetic::ascii_character));
    return makeEvaluatedDictionary(CodeRange{},
        new EvaluatedDictionary{Expression{}, definitions}
    );
}
