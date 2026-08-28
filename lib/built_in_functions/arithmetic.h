#pragma once

struct Expression;

namespace arithmetic {

Expression builtInAdd(Expression in);
Expression builtInAddTyped(Expression in);
Expression builtInMul(Expression in);
Expression builtInMulTyped(Expression in);
Expression builtInSub(Expression in);
Expression builtInSubTyped(Expression in);
Expression builtInDiv(Expression in);
Expression builtInDivTyped(Expression in);
Expression builtInMod(Expression in);
Expression builtInModTyped(Expression in);

Expression builtInLess(Expression in);
Expression builtInLessTyped(Expression in);

Expression builtInSqrt(Expression in);
Expression builtInSqrtTyped(Expression in);
Expression builtInRound(Expression in);
Expression builtInRoundTyped(Expression in);
Expression builtInRoundUp(Expression in);
Expression builtInRoundUpTyped(Expression in);
Expression builtInRoundDown(Expression in);
Expression builtInRoundDownTyped(Expression in);

Expression builtInAsciiNumber(Expression in);
Expression builtInAsciiNumberTyped(Expression in);
Expression builtInAsciiCharacter(Expression in);
Expression builtInAsciiCharacterTyped(Expression in);

}
