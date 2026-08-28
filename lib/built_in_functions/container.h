#pragma once

#include "../expression.h"

Expression containerPutString(Expression rest, Expression top);
Expression containerPutStack(Expression rest, Expression top);
Expression containerPutEvaluatedStack(Expression rest, Expression top);

Expression containerClear(Expression in);
Expression containerClearTyped(Expression in);
Expression containerPut(Expression in);
Expression containerPutTyped(Expression in);
Expression containerTake(Expression in);
Expression containerTakeTyped(Expression in);
Expression containerDrop(Expression in);
Expression containerDropTyped(Expression in);
Expression containerGet(Expression in);
Expression containerGetTyped(Expression in);
