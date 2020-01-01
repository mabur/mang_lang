from typing import Union

from parsing import parse
from ast import Array, Number, String


def _difference(x: Array):
    return Number(_value_left(x) - _value_right(x), section=None)


def _division(x: Array):
    return Number(_value_left(x) / _value_right(x), section=None)


def _check_equality(x: Array):
    return Number(str(float(_value_left(x) == _value_right(x))), section=None)


def _check_inequality(x: Array):
    return Number(str(float(_value_left(x) != _value_right(x))), section=None)


def _value_left(x: Array):
    assert len(x.value) == 2
    return x.value[0].value


def _value_right(x: Array):
    assert len(x.value) == 2
    return x.value[1].value


def _size(x: Union[String, Array]):
    return Number(str(len(x.value)), section=None)


def _is_empty(x: Union[String, Array]):
    return Number(str(0 if x.value else 1), section=None)


def _concat_tuple(x: Array) -> Array:
    expressions = []
    for e in x.value:
        assert isinstance(e, Array)
        expressions += e.value
    return Array(expressions, section=None)


def _concat_string(x: Array) -> String:
    expressions = '"'
    for e in x.value:
        assert isinstance(e, String)
        expressions += e.value
    expressions += '"'
    return String(expressions, section=None)


def _concat(x: Array):
    if isinstance(x.value[0], Array):
        return _concat_tuple(x)
    if isinstance(x.value[0], String):
        return _concat_string(x)
    raise TypeError


def _first(x: Union[Array, String]):
    element = x.value[0]
    if isinstance(x, Array):
        return element
    if isinstance(x, String):
        return String('"{}"'.format(element), section=None)


def _last(x: Union[Array, String]):
    element = x.value[-1]
    if isinstance(x, Array):
        return element
    if isinstance(x, String):
        return String('"{}"'.format(element), section=None)


def _first_part(x: Array) -> Array:
    return Array(x.value[:-1], section=None)


def _last_part(x: Array) -> Array:
    return Array(x.value[1:], section=None)


def _sum(x: Array):
    return Number(str(sum(element.value for element in x.value)), section=None)


def _product(x: Array):
    result = 1
    for element in x.value:
        result *= element.value
    return Number(str(result), section=None)


def _min(x: Array):
    return Number(str(min(element.value for element in x.value)), section=None)


def _max(x: Array):
    return Number(str(max(element.value for element in x.value)), section=None)


def _all(x: Array):
    return Number(str(float(all(element.value for element in x.value))), section=None)


def _none(x: Array):
    return Number(str(float(not any(element.value for element in x.value))), section=None)


def _any(x: Array):
    return Number(str(float(any(element.value for element in x.value))), section=None)


def _read_text_file(file_path: str) -> str:
    with open(file_path, 'r') as file:
        return file.read()


def _import(x: String):
    code = _read_text_file(x.value)
    expression = parse(code)
    environment = {}
    return expression.evaluate(environment)


ENVIRONMENT = {
    'difference': _difference,
    'division': _division,
    'check_equality': _check_equality,
    'check_inequality': _check_inequality,
    'none': _none,
    'size': _size,
    'is_empty': _is_empty,
    'concat': _concat,
    'first': _first,
    'last': _last,
    'first_part': _first_part,
    'last_part': _last_part,
    'sum': _sum,
    'product': _product,
    'min': _min,
    'max': _max,
    'all': _all,
    'any': _any,
    'import': _import,
}

