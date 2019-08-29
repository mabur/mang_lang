from typing import Union

from lexing import lexer
from parsing import ExpressionTuple, Number, parse_expression, String
from token_slice import TokenSlice


def _add(x: ExpressionTuple):
    return Number(_value_left(x) + _value_right(x))


def _mul(x: ExpressionTuple):
    return Number(_value_left(x) * _value_right(x))


def _sub(x: ExpressionTuple):
    return Number(_value_left(x) - _value_right(x))


def _div(x: ExpressionTuple):
    return Number(_value_left(x) / _value_right(x))


def _and(x: ExpressionTuple):
    return Number(str(float(_value_left(x) != 0 and _value_right(x) != 0)))


def _or(x: ExpressionTuple):
    return Number(str(float(_value_left(x) != 0 or _value_right(x) != 0)))


def _not(x: Number):
    return Number(str(float(not(x.value != 0))))


def _equal(x: ExpressionTuple):
    return Number(str(float(_value_left(x) == _value_right(x))))


def _value_left(x: ExpressionTuple):
    assert len(x.value) == 2
    return x.value[0].value


def _value_right(x: ExpressionTuple):
    assert len(x.value) == 2
    return x.value[1].value


def _size(x: Union[String, ExpressionTuple]):
    return Number(str(len(x.value)))


def _is_empty(x: Union[String, ExpressionTuple]):
    return Number(str(0 if x.value else 1))


def _concat_tuple(x: ExpressionTuple) -> ExpressionTuple:
    expressions = []
    for e in x.value:
        assert isinstance(e, ExpressionTuple)
        expressions += e.value
    return ExpressionTuple(expressions)


def _concat_string(x: ExpressionTuple) -> String:
    expressions = '"'
    for e in x.value:
        assert isinstance(e, String)
        expressions += e.value
    expressions += '"'
    return String(expressions)


def _concat(x: ExpressionTuple):
    if isinstance(x.value[0], ExpressionTuple):
        return _concat_tuple(x)
    if isinstance(x.value[0], String):
        return _concat_string(x)
    raise TypeError


def _sum(x: ExpressionTuple):
    return Number(str(sum(element.value for element in x.value)))


def _min(x: ExpressionTuple):
    return Number(str(min(element.value for element in x.value)))


def _max(x: ExpressionTuple):
    return Number(str(max(element.value for element in x.value)))


def _read_text_file(file_path: str) -> str:
    with open(file_path, 'r') as file:
        return file.read()


def _import(x: String):
    code = _read_text_file(x.value)
    tokens = lexer(code)
    expression, _ = parse_expression(TokenSlice(tokens))
    environment = {}
    return expression.evaluate(environment)


ENVIRONMENT = {'add': _add,
               'mul': _mul,
               'sub': _sub,
               'div': _div,
               'equal': _equal,
               'and': _and,
               'or': _or,
               'not': _not,
               'size': _size,
               'is_empty': _is_empty,
               'concat': _concat,
               'sum': _sum,
               'min': _min,
               'max': _max,
               'import': _import}
