#pragma once

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
Expression roundUp(Expression in);
Expression roundUpTyped(Expression in);
Expression roundDown(Expression in);
Expression roundDownTyped(Expression in);

Expression asciiNumber(Expression in);
Expression asciiNumberTyped(Expression in);
Expression asciiCharacter(Expression in);
Expression asciiCharacterTyped(Expression in);

}
