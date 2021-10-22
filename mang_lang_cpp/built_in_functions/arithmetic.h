#pragma once

struct Expression;

namespace arithmetic {

Expression add(Expression in);
Expression mul(Expression in);
Expression sub(Expression in);
Expression div(Expression in);
Expression sqrt(Expression in);
Expression are_all_less(Expression in);
Expression are_all_less_or_equal(Expression in);
Expression round(Expression in);
Expression round_up(Expression in);
Expression round_down(Expression in);
Expression ascii_number(Expression in);
Expression ascii_character(Expression in);

}
