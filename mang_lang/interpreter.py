from parsing import lex_and_parse
import global_environment


def create_abstract_syntax_tree_json(code: str):
    return lex_and_parse(code).to_json()


def interpret(code: str):
    return lex_and_parse(code).evaluate(global_environment.ENVIRONMENT).to_json()
