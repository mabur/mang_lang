#pragma once

struct Expression;

Expression builtInAdd(Expression in);
Expression builtInMul(Expression in);
Expression builtInSub(Expression in);
Expression builtInDiv(Expression in);
Expression builtInMod(Expression in);

Expression builtInLess(Expression in);

Expression builtInSqrt(Expression in);
Expression builtInRound(Expression in);
Expression builtInRoundUp(Expression in);
Expression builtInRoundDown(Expression in);

Expression builtInAsciiNumber(Expression in);
Expression builtInAsciiCharacter(Expression in);
