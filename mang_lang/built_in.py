from typing import Union

from parsing import parse
from ast import Array, Number, String
from error_handling import CodeFragment


def _sub(x: Array, code: CodeFragment):
    return Number(_value_left(x) - _value_right(x), code)


def _div(x: Array, code: CodeFragment):
    return Number(_value_left(x) / _value_right(x), code)


def _check_equality(x: Array, code: CodeFragment):
    return Number(str(float(_value_left(x) == _value_right(x))), code)


def _check_inequality(x: Array, code: CodeFragment):
    return Number(str(float(_value_left(x) != _value_right(x))), code)


def _value_left(x: Array):
    assert len(x.value) == 2
    return x.value[0].value


def _value_right(x: Array):
    assert len(x.value) == 2
    return x.value[1].value


def _size(x: Union[String, Array], code: CodeFragment):
    return Number(str(len(x.value)), code)


def _is_empty(x: Union[String, Array], code: CodeFragment):
    return Number(str(0 if x.value else 1), code)


def _concat_tuple(x: Array, code: CodeFragment) -> Array:
    expressions = []
    for e in x.value:
        assert isinstance(e, Array)
        expressions += e.value
    return Array(expressions, code)


def _concat_string(x: Array, code: CodeFragment) -> String:
    expressions = '"'
    for e in x.value:
        assert isinstance(e, String)
        expressions += e.value
    expressions += '"'
    return String(expressions, code)


def _concat(x: Array, code: CodeFragment):
    if isinstance(x.value[0], Array):
        return _concat_tuple(x, code)
    if isinstance(x.value[0], String):
        return _concat_string(x, code)
    raise TypeError


def _first(x: Union[Array, String], code: CodeFragment):
    element = x.value[0]
    if isinstance(x, Array):
        return element
    if isinstance(x, String):
        return String('"{}"'.format(element), code)


def _last(x: Union[Array, String], code: CodeFragment):
    element = x.value[-1]
    if isinstance(x, Array):
        return element
    if isinstance(x, String):
        return String('"{}"'.format(element), code)


def _first_part(x: Array, code: CodeFragment) -> Array:
    return Array(x.value[:-1], code)


def _last_part(x: Array, code: CodeFragment) -> Array:
    return Array(x.value[1:], code)


def _add(x: Array, code: CodeFragment):
    return Number(str(sum(element.value for element in x.value)), code)


def _mul(x: Array, code: CodeFragment):
    result = 1
    for element in x.value:
        result *= element.value
    return Number(str(result), code)


def _min(x: Array, code: CodeFragment):
    return Number(str(min(element.value for element in x.value)), code)


def _max(x: Array, code: CodeFragment):
    return Number(str(max(element.value for element in x.value)), code)


def _all(x: Array, code: CodeFragment):
    return Number(str(float(all(element.value for element in x.value))), code)


def _none(x: Array, code: CodeFragment):
    return Number(str(float(not any(element.value for element in x.value))), code)


def _any(x: Array, code: CodeFragment):
    return Number(str(float(any(element.value for element in x.value))), code)


def _read_text_file(file_path: str) -> str:
    with open(file_path, 'r') as file:
        return file.read()


def _import(x: String, code: CodeFragment):
    code = _read_text_file(x.value)
    expression = parse(code)
    environment = {}
    return expression.evaluate(environment)


ENVIRONMENT = {
    'sub': _sub,
    'div': _div,
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
    'add': _add,
    'mul': _mul,
    'min': _min,
    'max': _max,
    'all': _all,
    'any': _any,
    'import': _import,
}
