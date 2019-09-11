from typing import Union

from lexing import lexer
from parsing import Array, Number, parse_expression, String
from token_slice import TokenSlice


def _difference(x: Array):
    return Number(_value_left(x) - _value_right(x))


def _division(x: Array):
    return Number(_value_left(x) / _value_right(x))


def _check_equality(x: Array):
    return Number(str(float(_value_left(x) == _value_right(x))))


def _check_inequality(x: Array):
    return Number(str(float(_value_left(x) != _value_right(x))))


def _value_left(x: Array):
    assert len(x.value) == 2
    return x.value[0].value


def _value_right(x: Array):
    assert len(x.value) == 2
    return x.value[1].value


def _size(x: Union[String, Array]):
    return Number(str(len(x.value)))


def _is_empty(x: Union[String, Array]):
    return Number(str(0 if x.value else 1))


def _concat_tuple(x: Array) -> Array:
    expressions = []
    for e in x.value:
        assert isinstance(e, Array)
        expressions += e.value
    return Array(expressions)


def _concat_string(x: Array) -> String:
    expressions = '"'
    for e in x.value:
        assert isinstance(e, String)
        expressions += e.value
    expressions += '"'
    return String(expressions)


def _concat(x: Array):
    if isinstance(x.value[0], Array):
        return _concat_tuple(x)
    if isinstance(x.value[0], String):
        return _concat_string(x)
    raise TypeError


def _sum(x: Array):
    return Number(str(sum(element.value for element in x.value)))


def _product(x: Array):
    result = 1
    for element in x.value:
        result *= element.value
    return Number(str(result))


def _min(x: Array):
    return Number(str(min(element.value for element in x.value)))


def _max(x: Array):
    return Number(str(max(element.value for element in x.value)))


def _all(x: Array):
    return Number(str(float(all(element.value for element in x.value))))


def _none(x: Array):
    return Number(str(float(not any(element.value for element in x.value))))


def _any(x: Array):
    return Number(str(float(any(element.value for element in x.value))))


def _read_text_file(file_path: str) -> str:
    with open(file_path, 'r') as file:
        return file.read()


def _import(x: String):
    code = _read_text_file(x.value)
    tokens = lexer(code)
    expression, _ = parse_expression(TokenSlice(tokens))
    environment = {}
    return expression.evaluate(environment)


ENVIRONMENT = {'difference': _difference,
               'division': _division,
               'check_equality': _check_equality,
               'check_inequality': _check_inequality,
               'none': _none,
               'size': _size,
               'is_empty': _is_empty,
               'concat': _concat,
               'sum': _sum,
               'product': _product,
               'min': _min,
               'max': _max,
               'all': _all,
               'any': _any,
               'import': _import}

