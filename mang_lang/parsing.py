# expression = number | constant | array | function_call
# number = 123...
# constant = symbol
# symbol = abc...
# array = "(" [expression {"," expression}] ")"
# function_call = symbol, array

from typing import Sequence

from lexing import Token, TokenType


class Expression:
    def num_tokens(self) -> int:
        raise NotImplemented()


class Array(Expression):
    def __init__(self, expressions: Sequence[Expression]) -> None:
        self.expressions = expressions

    def num_tokens(self) -> int:
        return 1 + sum(e.num_tokens() + 1 for e in self.expressions)


class Number(Expression):
    def __init__(self, value: str) -> None:
        self.value = value

    def num_tokens(self) -> int:
        return 1


class Constant(Expression):
    def __init__(self, name: str) -> None:
        self.name = name

    def num_tokens(self) -> int:
        return 1


class FunctionCall(Expression):
    def __init__(self, name: str, array: Array) -> None:
        self.name = name
        self.array = array

    def num_tokens(self) -> int:
        return 1 + self.array.num_tokens()


def parse_expression(tokens: Sequence[Token], begin_index: int = 0) -> Expression:
    end_index = len(tokens)
    assert begin_index < end_index

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.NUMBER]):
        return _parse_number(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL, TokenType.PARENTHESIS_BEGIN]):
        return _parse_function_call(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL]):
        return _parse_constant(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.PARENTHESIS_BEGIN]):
        return _parse_array(tokens=tokens, begin_index=begin_index)

    raise ValueError('Bad token pattern: {}'.format(tokens[begin_index].value))


def _do_tokens_match(tokens: Sequence[Token],
                     begin_index: int,
                     token_pattern: Sequence[Token]) -> bool:
    if len(tokens) < begin_index + len(token_pattern):
        return False
    return all(tokens[begin_index + i].type == token for i, token in enumerate(token_pattern))


def _parse_number(tokens: Sequence[Token], begin_index: int) -> Number:
    return Number(value=tokens[begin_index].value)


def _parse_constant(tokens: Sequence[Token], begin_index: int) -> Constant:
    return Constant(name=tokens[begin_index].value)


def _parse_array(tokens: Sequence[Token], begin_index: int) -> Array:
    expressions = []
    begin_index += 1 # eat (
    while tokens[begin_index].type != TokenType.PARENTHESIS_END:
        expression = parse_expression(tokens=tokens, begin_index=begin_index)
        expressions.append(expression)
        begin_index += expression.num_tokens()
        if tokens[begin_index].type == TokenType.COMMA:
            begin_index += 1 # eat ,
    begin_index += 1 # eat )
    return Array(expressions=expressions)


def _parse_function_call(tokens: Sequence[Token], begin_index: int) -> FunctionCall:
    name = tokens[begin_index].value
    array =  _parse_array(tokens=tokens, begin_index=begin_index + 1)
    return FunctionCall(name=name, array=array)
