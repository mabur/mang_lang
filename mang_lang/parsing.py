import copy
from typing import Any, Callable, Sequence, Tuple

from ast import Expression, Array, Number, String, VariableDefinition, \
    Dictionary, Function, Lookup, Conditional, ArrayComprehension
from error_handling import CodeFragment, print_syntax_error


def parse(code: str) -> Expression:
    code = CodeFragment(code)
    try:
        return _parse_expression(code)[0]
    except:
        print_syntax_error(code)
        raise


ARRAY_BEGIN = "["
ARRAY_END = "]"
DICTIONARY_BEGIN = "{"
DICTIONARY_END = "}"
EQUAL = "="
COMMA = ","
IF = "if"
THEN = "then"
ELSE = "else"
EACH = "each"
FOR = "for"
IN = "in"
FROM = "from"
TO = "to"
OF = "of"
STRING_BEGIN = "\""
STRING_END = "\""

DIGITS = '+-.1234567890'
LETTERS = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_'
WHITESPACE = ' \n'


def _parse_keyword(code: CodeFragment, keyword: str) -> Tuple[str, CodeFragment]:
    value = ''
    for expected in keyword:
        actual = code.pop()
        assert expected == actual
        value += actual
    code = _parse_optional_white_space(code)
    return value, code


def _parse_while(code: CodeFragment, predicate) -> Tuple[str, CodeFragment]:
    value = ''
    while code and predicate(code.front()):
        value += code.pop()
    return value, code


def _parse_optional_white_space(code: CodeFragment) -> CodeFragment:
    value, code = _parse_while(code, lambda c: c in WHITESPACE)
    return code


def _parse_symbol(code: CodeFragment) -> Tuple[str, CodeFragment]:
    value, code = _parse_while(code, lambda c: c in LETTERS)
    assert value
    code = _parse_optional_white_space(code)
    return value, code


def _parse_number(code: CodeFragment) -> Tuple[Number, CodeFragment]:
    section = copy.copy(code)
    value, code = _parse_while(code, lambda c: c in DIGITS)
    assert value
    code = _parse_optional_white_space(code)
    return Number(value, code=section), code


def _parse_string(code: CodeFragment) -> Tuple[String, CodeFragment]:
    section = copy.copy(code)
    value, code = _parse_keyword(code, STRING_BEGIN)
    body, code = _parse_while(code, lambda c: c != STRING_END)
    value += body
    value += code.pop()
    code = _parse_optional_white_space(code)
    return String(value, code=section), code


def _parse_array(code: CodeFragment) -> Tuple[Array, CodeFragment]:
    section = copy.copy(code)
    expressions = ()
    _, code = _parse_keyword(code, ARRAY_BEGIN)
    while not code.startswith(ARRAY_END):
        expression, code = _parse_expression(code)
        expressions += (expression,)
        if code.startswith(COMMA):
            _, code = _parse_keyword(code, COMMA)
    _, code = _parse_keyword(code, ARRAY_END)
    return Array(expressions=expressions, code=section), code


def _parse_dictionary(code: CodeFragment) -> Tuple[Dictionary, CodeFragment]:
    section = copy.copy(code)
    variable_definitions = []
    _, code = _parse_keyword(code, DICTIONARY_BEGIN)
    while not code.startswith(DICTIONARY_END):
        variable_definition, code = _parse_variable_definition(code)
        variable_definitions.append(variable_definition)
        if code.startswith(COMMA):
            _, code = _parse_keyword(code, COMMA)
    _, code = _parse_keyword(code, DICTIONARY_END)
    return Dictionary(expressions=variable_definitions, code=section), code


def _parse_lookup(code: CodeFragment) -> Tuple[Lookup, CodeFragment]:
    section = copy.copy(code)
    value, code = _parse_symbol(code)
    if not code.startswith(OF):
        return Lookup(left=value, right=None, code=section), code
    _, code = _parse_keyword(code, OF)
    right, code = _parse_expression(code)
    return Lookup(left=value, right=right, code=section), code


def _parse_variable_definition(code: CodeFragment) -> Tuple[VariableDefinition, CodeFragment]:
    section = copy.copy(code)
    value, code = _parse_symbol(code)
    _, code = _parse_keyword(code, EQUAL)
    expression, code = _parse_expression(code)
    return VariableDefinition(name=value, expression=expression, code=section), code


def _parse_function(code: CodeFragment) -> Tuple[Function, CodeFragment]:
    section = copy.copy(code)
    _, code = _parse_keyword(code, FROM)
    value, code = _parse_symbol(code)
    _, code = _parse_keyword(code, TO)
    expression, code = _parse_expression(code)
    return Function(argument_name=value, expression=expression, code=section), code


def _parse_conditional(code: CodeFragment) -> Tuple[Conditional, CodeFragment]:
    section = copy.copy(code)
    _, code = _parse_keyword(code, IF)
    condition, code = _parse_expression(code)
    _, code = _parse_keyword(code, THEN)
    then_expression, code = _parse_expression(code)
    _, code = _parse_keyword(code, ELSE)
    else_expression, code = _parse_expression(code)
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression, code=section), code)


def _parse_array_comprehension(code: CodeFragment)\
        -> Tuple[ArrayComprehension, CodeFragment]:
    section = copy.copy(code)
    _, code = _parse_keyword(code, EACH)
    all_expression, code = _parse_expression(code)
    _, code = _parse_keyword(code, FOR)
    for_expression, code = _parse_expression(code)
    _, code = _parse_keyword(code, IN)
    in_expression, code = _parse_expression(code)
    if_expression = None
    if code.startswith(IF):
        _, code = _parse_keyword(code, IF)
        if_expression, code = _parse_expression(code)
    return (ArrayComprehension(all_expression=all_expression,
                               for_expression=for_expression,
                               in_expression=in_expression,
                               if_expression=if_expression,
                               code=section), code)


def _parse_expression(code: CodeFragment) -> Tuple[Expression, CodeFragment]:
    code = _parse_optional_white_space(code)
    try:
        for sequence, parser in _PARSE_TABLE:
            if code.startswith(sequence):
                return parser(code)
    except KeyError:
        raise ValueError('Bad token pattern: {}'.format(code[0]))


def _make_parse_table()\
        -> Sequence[Tuple[str, Callable[[CodeFragment], Tuple[Any, CodeFragment]]]]:
    parser_from_token = [
        (ARRAY_BEGIN, _parse_array),
        (DICTIONARY_BEGIN, _parse_dictionary),
        (STRING_BEGIN, _parse_string),
    ]
    for char in WHITESPACE:
        parser_from_token.append((IF + char, _parse_conditional))
        parser_from_token.append((EACH + char, _parse_array_comprehension))
        parser_from_token.append((FROM + char, _parse_function))
    for char in DIGITS:
        parser_from_token.append((char, _parse_number))
    for char in LETTERS:
        parser_from_token.append((char, _parse_lookup))
    return parser_from_token


_PARSE_TABLE = _make_parse_table()
