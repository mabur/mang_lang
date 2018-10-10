from lexing import lexer
from parsing import parse_expression, TokenSlice
import global_environment


def interpret(code: str):
    tokens = lexer(code)
    expression = parse_expression(TokenSlice(tokens))[0]
    value = expression.evaluate(global_environment.ENVIRONMENT)
    return value
