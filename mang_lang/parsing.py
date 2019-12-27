from typing import Tuple

from ast import Expression, Array, Number, String, VariableDefinition, \
    Dictionary, Function, Lookup, Conditional, ArrayComprehension
from slice import Slice

ARRAY_BEGIN = "["
ARRAY_END = "]"
DICTIONARY_BEGIN = "{"
DICTIONARY_END = "}"
EQUAL = "="
COMMA = ","
IF = "if "
THEN = "then "
ELSE = "else "
EACH = "each "
FOR = "for "
IN = "in "
FROM = "from "
TO = "to "
OF = "of "
STRING_BEGIN = "\""
STRING_END = "\""


def _parse_sequence(text: Slice, sequence) -> Tuple[str, Slice]:
    value = ''
    for _ in sequence:
        value += text.pop()
    text = _parse_optional_white_space(text)
    return value, text


def _parse_while(text: Slice, predicate) -> Tuple[str, Slice]:
    value = ''
    while text and predicate(text.front()):
        value += text.pop()
    return value, text


def _parse_optional_white_space(text: Slice) -> Slice:
    value, text = _parse_while(text, lambda c: c == ' ' or c == '\n')
    return text


def _parse_symbol(text: Slice) -> Tuple[str, Slice]:
    value, text = _parse_while(text, lambda c: c.isalnum() or c == '_')
    assert value
    text = _parse_optional_white_space(text)
    return value, text


def _parse_number(text: Slice) -> Tuple[Number, Slice]:
    value, text = _parse_while(text, lambda c: c in '-+.1234567890')
    assert value
    text = _parse_optional_white_space(text)
    return Number(value), text


def _parse_string(text: Slice) -> Tuple[String, Slice]:
    assert text.front() == STRING_BEGIN
    value = text.pop()
    body, text = _parse_while(text, lambda c: c != STRING_END)
    value += body
    value += text.pop()
    text = _parse_optional_white_space(text)
    return String(value), text


def _parse_array(text: Slice) -> Tuple[Array, Slice]:
    expressions = ()
    _, text = _parse_sequence(text, ARRAY_BEGIN)
    while not text.startswith(ARRAY_END):
        expression, text = parse_expression(text)
        expressions += (expression,)
        if text.startswith(COMMA):
            _, text = _parse_sequence(text, COMMA)
    _, text = _parse_sequence(text, ARRAY_END)
    return Array(expressions=expressions), text


def _parse_dictionary(text: Slice) -> Tuple[Dictionary, Slice]:
    variable_definitions = []
    _, text = _parse_sequence(text, DICTIONARY_BEGIN)
    while not text.startswith(DICTIONARY_END):
        variable_definition, text = _parse_variable_definition(text)
        variable_definitions.append(variable_definition)
        if text.startswith(COMMA):
            _, text = _parse_sequence(text, COMMA)
    _, text = _parse_sequence(text, DICTIONARY_END)
    return Dictionary(expressions=variable_definitions), text


def _parse_lookup(text: Slice) -> Tuple[Lookup, Slice]:
    value, text = _parse_symbol(text)
    left = value
    if not text.startswith(OF):
        return Lookup(left=left, right=None), text
    _, text = _parse_sequence(text, OF)
    right, text = parse_expression(text)
    return Lookup(left=left, right=right), text


def _parse_variable_definition(text: Slice) -> Tuple[VariableDefinition, Slice]:
    value, text = _parse_symbol(text)
    name = value
    _, text = _parse_sequence(text, EQUAL)
    expression, text = parse_expression(text)
    return VariableDefinition(name=name, expression=expression), text


def _parse_function(text: Slice) -> Tuple[Function, Slice]:
    _, text = _parse_sequence(text, FROM)
    value, text = _parse_symbol(text)
    argument_name = value
    _, text = _parse_sequence(text, TO)
    expression, text = parse_expression(text)
    return Function(argument_name=argument_name, expression=expression), text


def _parse_conditional(text: Slice) -> Tuple[Conditional, Slice]:
    _, text = _parse_sequence(text, IF)
    condition, text = parse_expression(text)
    _, text = _parse_sequence(text, THEN)
    then_expression, text = parse_expression(text)
    _, text = _parse_sequence(text, ELSE)
    else_expression, text = parse_expression(text)
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression), text)


def _parse_array_comprehension(text: Slice)\
        -> Tuple[ArrayComprehension, Slice]:
    _, text = _parse_sequence(text, EACH)
    all_expression, text = parse_expression(text)
    _, text = _parse_sequence(text, FOR)
    for_expression, text = parse_expression(text)
    _, text = _parse_sequence(text, IN)
    in_expression, text = parse_expression(text)
    if_expression = None
    if text.startswith(IF):
        _, text = _parse_sequence(text, IF)
        if_expression, text = parse_expression(text)
    return (ArrayComprehension(all_expression=all_expression,
                               for_expression=for_expression,
                               in_expression=in_expression,
                               if_expression=if_expression), text)


parser_from_token = {
    ARRAY_BEGIN: _parse_array,
    DICTIONARY_BEGIN: _parse_dictionary,
    IF: _parse_conditional,
    EACH: _parse_array_comprehension,
    FROM: _parse_function,
    STRING_BEGIN: _parse_string,
}

for char in '+-.1234567890':
    parser_from_token[char] = _parse_number

for char in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_':
    parser_from_token[char] = _parse_lookup

def parse_expression(slice: Slice) -> Tuple[Expression, Slice]:
    try:
        for sequence, parser in parser_from_token.items():
            if slice.startswith(sequence):
                return parser(slice)
    except KeyError:
        raise ValueError('Bad token pattern: {}'.format(slice[0]))


def lex_and_parse(code: str) -> Expression:
    expression, _ = parse_expression(_parse_optional_white_space(Slice(code)))
    return expression
