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
