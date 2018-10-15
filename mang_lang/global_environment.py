from typing import Tuple


def add(x: Tuple):
    assert len(x) == 2
    return x[0] + x[1]


def mul(x: Tuple):
    assert len(x) == 2
    return x[0] * x[1]


def sub(x: Tuple):
    assert len(x) == 2
    return x[0] - x[1]


def div(x: Tuple):
    assert len(x) == 2
    return x[0] / x[1]


def if_then_else(x: Tuple):
    assert len(x) == 3
    condition = x[0]
    true_expression = x[1]
    false_expression = x[2]
    return true_expression if condition != 0 else false_expression


def logical_and(x: Tuple) -> float:
    return float(x[0] != 0 and x[1] != 0)


def logical_or(x: Tuple) -> float:
    return float(x[0] != 0 or x[1] != 0)


def logical_not(x: float) -> float:
    return float(not(x != 0))


ENVIRONMENT = {'add': add, 'mul': mul, 'sub': sub, 'div': div,
               'if_then_else': if_then_else, 'and': logical_and,
               'or': logical_or, 'not': logical_not}
