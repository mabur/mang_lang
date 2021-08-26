#pragma once

struct Expression;

namespace logic {

Expression boolean(Expression in);
Expression logic_not(Expression in) ;
Expression all(Expression in) ;
Expression any(Expression in);
Expression none(Expression in);
Expression equal(Expression in);
Expression unequal(Expression in);

}
