from value_array import ValueArray


def add(x: ValueArray) -> ValueArray:
    assert len(x) == 2
    return [x[0] + x[1]]


def mul(x: ValueArray) -> ValueArray:
    assert len(x) == 2
    return [x[0] * x[1]]


def sub(x: ValueArray) -> ValueArray:
    assert len(x) == 2
    return [x[0] - x[1]]


def div(x: ValueArray) -> ValueArray:
    assert len(x) == 2
    return [x[0] / x[1]]
