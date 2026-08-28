#pragma once

#include "../expression.h"

Expression putString(Expression rest, Expression top);
Expression putStack(Expression rest, Expression top);
Expression putEvaluatedStack(Expression rest, Expression top);

namespace container_functions {

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

}
