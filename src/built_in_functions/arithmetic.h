#pragma once

#include <string>

struct Expression;

namespace arithmetic {

Expression add(Expression in);
Expression mul(Expression in);
Expression sub(Expression in);
Expression div(Expression in);
Expression mod(Expression in);
Expression less(Expression in);

Expression sqrt(Expression in);
Expression round(Expression in);
Expression round_up(Expression in);
Expression round_down(Expression in);
Expression ascii_number(Expression in);
Expression ascii_character(Expression in);

struct FunctionNumberToNumber {
    std::string name;
    Expression operator()(Expression in) const;
};

struct FunctionNumberToCharacter {
    std::string name;
    Expression operator()(Expression in) const;
};

struct FunctionCharacterToNumber {
    std::string name;
    Expression operator()(Expression in) const;
};

struct FunctionNumberNumberToNumber {
    std::string name;
    Expression operator()(Expression in) const;    
};

struct FunctionNumberNumberToBoolean {
    std::string name;
    Expression operator()(Expression in) const;
};

}
