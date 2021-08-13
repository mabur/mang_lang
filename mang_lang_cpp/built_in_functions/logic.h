#pragma once

struct ExpressionPointer;

namespace logic {

ExpressionPointer boolean(ExpressionPointer in);
ExpressionPointer logic_not(ExpressionPointer in) ;
ExpressionPointer all(ExpressionPointer in) ;
ExpressionPointer any(ExpressionPointer in);
ExpressionPointer none(ExpressionPointer in);
ExpressionPointer equal(ExpressionPointer in);
ExpressionPointer unequal(ExpressionPointer in);

}
