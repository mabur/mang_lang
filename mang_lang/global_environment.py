from typing import Tuple
from parsing import Number, String

def add(x: Tuple):
    assert len(x) == 2
    return Number(x[0]["value"] + x[1]["value"]).to_json()


def mul(x: Tuple):
    assert len(x) == 2
    return Number(x[0]["value"] * x[1]["value"]).to_json()


def sub(x: Tuple):
    assert len(x) == 2
    return Number(x[0]["value"] - x[1]["value"]).to_json()


def div(x: Tuple):
    assert len(x) == 2
    return Number(x[0]["value"] / x[1]["value"]).to_json()


def logical_and(x: Tuple):
    return Number(str(float(x[0]["value"] != 0 and x[1]["value"] != 0))).to_json()


def logical_or(x: Tuple):
    return Number(str(float(x[0]["value"] != 0 or x[1]["value"] != 0))).to_json()


def logical_not(x: float):
    return Number(str(float(not(x["value"] != 0)))).to_json()


def equal(x: Tuple):
    return Number(str(float(x[0]["value"] == x[1]["value"]))).to_json()


def size(x):
    if isinstance(x, Tuple):
        return Number(str(len(x))).to_json()
    assert x['type'] == "string"
    return Number(str(len(x["value"]))).to_json()


def _concat_tuple(x: Tuple) -> Tuple:
    expressions = ()
    for t in x:
        expressions += t
    return expressions

def _concat_string(x) -> str:
    expressions = '"'
    for s in x:
        expressions += s["value"]
    expressions += '"'
    return String(expressions).to_json()


def concat(x):
    if isinstance(x[0], Tuple):
        return _concat_tuple(x)
    if isinstance(x[0], dict) and x[0]["type"] == "string":
        return _concat_string(x)
    raise TypeError


ENVIRONMENT = {'add': add, 'mul': mul, 'sub': sub, 'div': div, 'equal': equal,
               'and': logical_and, 'or': logical_or, 'not': logical_not,
               'size': size, 'concat': concat}
