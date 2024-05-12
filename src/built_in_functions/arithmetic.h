#pragma once

#include <string>

struct Expression;

namespace arithmetic {

Expression add(Expression in);
Expression addTyped(Expression in);
Expression mul(Expression in);
Expression mulTyped(Expression in);
Expression sub(Expression in);
Expression subTyped(Expression in);
Expression div(Expression in);
Expression divTyped(Expression in);
Expression mod(Expression in);
Expression modTyped(Expression in);

Expression less(Expression in);
Expression lessTyped(Expression in);

Expression sqrt(Expression in);
Expression sqrtTyped(Expression in);
Expression round(Expression in);
Expression roundTyped(Expression in);
Expression round_up(Expression in);
Expression round_upTyped(Expression in);
Expression round_down(Expression in);
Expression round_downTyped(Expression in);

Expression ascii_number(Expression in);
Expression ascii_character(Expression in);

struct FunctionNumberToCharacter {
    std::string name;
    Expression operator()(Expression in) const;
};

struct FunctionCharacterToNumber {
    std::string name;
    Expression operator()(Expression in) const;
};

}
