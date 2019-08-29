from lexing import lexer
from parsing import parse_expression
from token_slice import TokenSlice
import global_environment


def create_abstract_syntax_tree_json(code: str):
    tokens = lexer(code)
    expression, _ = parse_expression(TokenSlice(tokens))
    return expression.to_json()


def interpret(code: str):
    tokens = lexer(code)
    expression, _ = parse_expression(TokenSlice(tokens))
    value = expression.evaluate(global_environment.ENVIRONMENT).to_json()
    return value
