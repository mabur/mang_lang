from parsing import Expression
import global_environment
from value_array import ValueArray

def evaluate(expression: Expression) -> ValueArray:
    return expression.evaluate(global_environment)
