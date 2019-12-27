from parsing import parse
import global_environment


def create_abstract_syntax_tree_json(code: str):
    return parse(code).to_json()


def interpret(code: str):
    return parse(code).evaluate(global_environment.ENVIRONMENT).to_json()
