from lexing import lexer
from parsing import parse_expression, TokenSlice
import global_environment


def interpret(code: str):
    tokens = lexer(code)
    expression = parse_expression(TokenSlice(tokens=tokens))
    value = expression.evaluate(global_environment.ENVIRONMENT)
    return value
