from lexing import lexer
from parsing import parse_expression
import global_environment


def interpret(code: str):
    tokens = lexer(code)
    expression = parse_expression(tokens=tokens)
    value = expression.evaluate(global_environment.ENVIRONMENT)
    return value
