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


def if_then_else(x: Tuple):
    assert len(x) == 3
    condition = x[0]["value"]
    true_expression = x[1]["value"]
    false_expression = x[2]["value"]
    return Number(true_expression if condition != 0 else false_expression).to_json()


def logical_and(x: Tuple) -> float:
    return Number(float(x[0]["value"] != 0 and x[1]["value"] != 0)).to_json()


def logical_or(x: Tuple) -> float:
    return Number(float(x[0]["value"] != 0 or x[1]["value"] != 0)).to_json()


def logical_not(x: float) -> float:
    return Number(float(not(x["value"] != 0))).to_json()


ENVIRONMENT = {'add': add, 'mul': mul, 'sub': sub, 'div': div,
               'if_then_else': if_then_else, 'and': logical_and,
               'or': logical_or, 'not': logical_not}
