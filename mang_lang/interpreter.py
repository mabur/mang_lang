from parsing import parse
import ast
import built_in


def create_abstract_syntax_tree_json(code: str):
    return parse(code).to_json()


def interpret(code: str):
    standard_library = parse('import "standard_library.ml"').evaluate(built_in.ENVIRONMENT)
    assert isinstance(standard_library, ast.Dictionary)
    standard_library_environment = standard_library.make_environment()
    environment = {**built_in.ENVIRONMENT, **standard_library_environment}
    return parse(code).evaluate(environment).to_json()
