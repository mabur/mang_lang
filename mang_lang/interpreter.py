from lexing import lexer
from parsing import Expression, parse_expression
from token_slice import TokenSlice
import global_environment


def create_abstract_syntax_tree_json(code: str):
    return _lex_and_parse(code).to_json()


def interpret(code: str):
    return _lex_and_parse(code).evaluate(global_environment.ENVIRONMENT).to_json()


def _lex_and_parse(code: str) -> Expression:
    tokens = lexer(code)
    expression, _ = parse_expression(TokenSlice(tokens))
    return expression
