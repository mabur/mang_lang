from lexing import lexer
from parsing import parse_expression
import global_environment
from value_array import ValueArray

def interpret(code: str) -> ValueArray:
    tokens = lexer(code)
    expression = parse_expression(tokens=tokens)
    value = expression.evaluate(global_environment)
    return value
