import copy
from typing import Any, Callable, Sequence, Tuple

from ast import Expression, Array, Number, String, VariableDefinition, \
    Dictionary, Function, Lookup, Conditional, ArrayComprehension
from error_handling import Slice, print_syntax_error


def parse(code: str) -> Expression:
    text = Slice(code)
    try:
        return _parse_expression(text)[0]
    except:
        print_syntax_error(text)
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


def _parse_keyword(text: Slice, keyword: str) -> Tuple[str, Slice]:
    value = ''
    for expected in keyword:
        actual = text.pop()
        assert expected == actual
        value += actual
    text = _parse_optional_white_space(text)
    return value, text


def _parse_while(text: Slice, predicate) -> Tuple[str, Slice]:
    value = ''
    while text and predicate(text.front()):
        value += text.pop()
    return value, text


def _parse_optional_white_space(text: Slice) -> Slice:
    value, text = _parse_while(text, lambda c: c in WHITESPACE)
    return text


def _parse_symbol(text: Slice) -> Tuple[str, Slice]:
    value, text = _parse_while(text, lambda c: c in LETTERS)
    assert value
    text = _parse_optional_white_space(text)
    return value, text


def _parse_number(text: Slice) -> Tuple[Number, Slice]:
    section = copy.copy(text)
    value, text = _parse_while(text, lambda c: c in DIGITS)
    assert value
    text = _parse_optional_white_space(text)
    return Number(value, section=section), text


def _parse_string(text: Slice) -> Tuple[String, Slice]:
    section = copy.copy(text)
    value, text = _parse_keyword(text, STRING_BEGIN)
    body, text = _parse_while(text, lambda c: c != STRING_END)
    value += body
    value += text.pop()
    text = _parse_optional_white_space(text)
    return String(value, section=section), text


def _parse_array(text: Slice) -> Tuple[Array, Slice]:
    section = copy.copy(text)
    expressions = ()
    _, text = _parse_keyword(text, ARRAY_BEGIN)
    while not text.startswith(ARRAY_END):
        expression, text = _parse_expression(text)
        expressions += (expression,)
        if text.startswith(COMMA):
            _, text = _parse_keyword(text, COMMA)
    _, text = _parse_keyword(text, ARRAY_END)
    return Array(expressions=expressions, section=section), text


def _parse_dictionary(text: Slice) -> Tuple[Dictionary, Slice]:
    section = copy.copy(text)
    variable_definitions = []
    _, text = _parse_keyword(text, DICTIONARY_BEGIN)
    while not text.startswith(DICTIONARY_END):
        variable_definition, text = _parse_variable_definition(text)
        variable_definitions.append(variable_definition)
        if text.startswith(COMMA):
            _, text = _parse_keyword(text, COMMA)
    _, text = _parse_keyword(text, DICTIONARY_END)
    return Dictionary(expressions=variable_definitions, section=section), text


def _parse_lookup(text: Slice) -> Tuple[Lookup, Slice]:
    section = copy.copy(text)
    value, text = _parse_symbol(text)
    if not text.startswith(OF):
        return Lookup(left=value, right=None, section=section), text
    _, text = _parse_keyword(text, OF)
    right, text = _parse_expression(text)
    return Lookup(left=value, right=right, section=section), text


def _parse_variable_definition(text: Slice) -> Tuple[VariableDefinition, Slice]:
    section = copy.copy(text)
    value, text = _parse_symbol(text)
    _, text = _parse_keyword(text, EQUAL)
    expression, text = _parse_expression(text)
    return VariableDefinition(name=value, expression=expression, section=section), text


def _parse_function(text: Slice) -> Tuple[Function, Slice]:
    section = copy.copy(text)
    _, text = _parse_keyword(text, FROM)
    value, text = _parse_symbol(text)
    _, text = _parse_keyword(text, TO)
    expression, text = _parse_expression(text)
    return Function(argument_name=value, expression=expression, section=section), text


def _parse_conditional(text: Slice) -> Tuple[Conditional, Slice]:
    section = copy.copy(text)
    _, text = _parse_keyword(text, IF)
    condition, text = _parse_expression(text)
    _, text = _parse_keyword(text, THEN)
    then_expression, text = _parse_expression(text)
    _, text = _parse_keyword(text, ELSE)
    else_expression, text = _parse_expression(text)
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression, section=section), text)


def _parse_array_comprehension(text: Slice)\
        -> Tuple[ArrayComprehension, Slice]:
    section = copy.copy(text)
    _, text = _parse_keyword(text, EACH)
    all_expression, text = _parse_expression(text)
    _, text = _parse_keyword(text, FOR)
    for_expression, text = _parse_expression(text)
    _, text = _parse_keyword(text, IN)
    in_expression, text = _parse_expression(text)
    if_expression = None
    if text.startswith(IF):
        _, text = _parse_keyword(text, IF)
        if_expression, text = _parse_expression(text)
    return (ArrayComprehension(all_expression=all_expression,
                               for_expression=for_expression,
                               in_expression=in_expression,
                               if_expression=if_expression,
                               section=section), text)


def _parse_expression(text: Slice) -> Tuple[Expression, Slice]:
    text = _parse_optional_white_space(text)
    try:
        for sequence, parser in _PARSE_TABLE:
            if text.startswith(sequence):
                return parser(text)
    except KeyError:
        raise ValueError('Bad token pattern: {}'.format(text[0]))


def _make_parse_table()\
        -> Sequence[Tuple[str, Callable[[Slice], Tuple[Any, Slice]]]]:
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
