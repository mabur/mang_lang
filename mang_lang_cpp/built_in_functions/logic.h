#pragma once

struct Expression;

namespace logic {

Expression booleanExpression(Expression in);
Expression logic_not(Expression in) ;
Expression equal(Expression in);
Expression unequal(Expression in);

}
