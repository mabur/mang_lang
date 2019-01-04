from typing import Union

from parsing import ExpressionTuple, Number, String


def add(x: ExpressionTuple):
    return Number(_value_left(x) + _value_right(x))


def mul(x: ExpressionTuple):
    return Number(_value_left(x) * _value_right(x))


def sub(x: ExpressionTuple):
    return Number(_value_left(x) - _value_right(x))


def div(x: ExpressionTuple):
    return Number(_value_left(x) / _value_right(x))


def logical_and(x: ExpressionTuple):
    return Number(str(float(_value_left(x) != 0 and _value_right(x) != 0)))


def logical_or(x: ExpressionTuple):
    return Number(str(float(_value_left(x) != 0 or _value_right(x) != 0)))


def logical_not(x: Number):
    return Number(str(float(not(x.value != 0))))


def equal(x: ExpressionTuple):
    return Number(str(float(_value_left(x) == _value_right(x))))


def _value_left(x: ExpressionTuple):
    assert len(x.value) == 2
    return x.value[0].value


def _value_right(x: ExpressionTuple):
    assert len(x.value) == 2
    return x.value[1].value


def size(x: Union[String, ExpressionTuple]):
        return Number(str(len(x.value)))


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


def concat(x: ExpressionTuple):
    if isinstance(x.value[0], ExpressionTuple):
        return _concat_tuple(x)
    if isinstance(x.value[0], String):
        return _concat_string(x)
    raise TypeError


ENVIRONMENT = {'add': add, 'mul': mul, 'sub': sub, 'div': div, 'equal': equal,
               'and': logical_and, 'or': logical_or, 'not': logical_not,
               'size': size, 'concat': concat}
