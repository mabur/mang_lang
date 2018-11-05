from typing import Tuple
from parsing import Number

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


def logical_and(x: Tuple) -> float:
    return Number(float(x[0]["value"] != 0 and x[1]["value"] != 0)).to_json()


def logical_or(x: Tuple) -> float:
    return Number(float(x[0]["value"] != 0 or x[1]["value"] != 0)).to_json()


def logical_not(x: float) -> float:
    return Number(float(not(x["value"] != 0))).to_json()


def equal(x: Tuple) -> float:
    return Number(float(x[0]["value"] == x[1]["value"])).to_json()


def size(x: Tuple):
    return Number(float(len(x))).to_json()


ENVIRONMENT = {'add': add, 'mul': mul, 'sub': sub, 'div': div, 'equal': equal,
               'and': logical_and, 'or': logical_or, 'not': logical_not,
               'size': size}
