#pragma once

struct ExpressionPointer;

namespace arithmetic {

ExpressionPointer min(ExpressionPointer in);
ExpressionPointer max(ExpressionPointer in);
ExpressionPointer add(ExpressionPointer in);
ExpressionPointer mul(ExpressionPointer in);
ExpressionPointer sub(ExpressionPointer in);
ExpressionPointer div(ExpressionPointer in);
ExpressionPointer abs(ExpressionPointer in);
ExpressionPointer sqrt(ExpressionPointer in);
ExpressionPointer less(ExpressionPointer in);
ExpressionPointer less_or_equal(ExpressionPointer in);
ExpressionPointer round(ExpressionPointer in);
ExpressionPointer round_up(ExpressionPointer in);
ExpressionPointer round_down(ExpressionPointer in);

}
