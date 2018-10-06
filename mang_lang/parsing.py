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

    def evaluate(self, environment):
        raise NotImplemented()


class Tuple(Expression):
    def __init__(self, expressions: Sequence[Expression]) -> None:
        self.expressions = expressions

    def num_tokens(self) -> int:
        return 1 + sum(e.num_tokens() + 1 for e in self.expressions)

    def evaluate(self, environment):
        value = []
        for expression in self.expressions:
            value.extend(expression.evaluate(environment))
        return value


class Number(Expression):
    def __init__(self, value: str) -> None:
        self.value = value

    def num_tokens(self) -> int:
        return 1

    def evaluate(self, environment):
        return [float(self.value)]


class Constant(Expression):
    def __init__(self, name: str) -> None:
        self.name = name

    def num_tokens(self) -> int:
        return 1

    def evaluate(self, environment):
        return environment[self.name]


class FunctionCall(Expression):
    def __init__(self, name: str, tuple: Tuple) -> None:
        self.name = name
        self.tuple = tuple

    def num_tokens(self) -> int:
        return 1 + self.tuple.num_tokens()

    def evaluate(self, environment):
        input = self.tuple.evaluate(environment)
        function = getattr(environment, self.name)
        return function(input)


class Definition(Expression):
    def __init__(self, name: str, expression: Expression) -> None:
        self.name = name
        self.expression = expression

    def num_tokens(self) -> int:
        return 2 + self.expression.num_tokens()

    def evaluate(self, environment):
        return self.expression.evaluate(environment)


class Environment(Expression):
    def __init__(self, definitions: Sequence[Definition]) -> None:
        self.definitions = definitions

    def num_tokens(self) -> int:
        return [d.num_tokens() + 1 for d in self.definitions] - 1

    def evaluate(self, environment):
        values = {d.name: d.evaluate(environment) for d in self.definitions}
        return values.get('result', [])


def parse_expression(tokens: Sequence[Token], begin_index: int = 0) -> Expression:
    end_index = len(tokens)
    assert begin_index < end_index

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.NUMBER]):
        return _parse_number(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL, TokenType.PARENTHESIS_BEGIN]):
        return _parse_function_call(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL, TokenType.EQUAL]):
        return _parse_environment(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL]):
        return _parse_constant(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.PARENTHESIS_BEGIN]):
        return _parse_tuple(tokens=tokens, begin_index=begin_index)

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


def _parse_tuple(tokens: Sequence[Token], begin_index: int) -> Tuple:
    expressions = []
    begin_index += 1 # eat (
    while tokens[begin_index].type != TokenType.PARENTHESIS_END:
        expression = parse_expression(tokens=tokens, begin_index=begin_index)
        expressions.append(expression)
        begin_index += expression.num_tokens()
        if tokens[begin_index].type == TokenType.COMMA:
            begin_index += 1 # eat ,
    begin_index += 1 # eat )
    return Tuple(expressions=expressions)


def _parse_function_call(tokens: Sequence[Token], begin_index: int) -> FunctionCall:
    name = tokens[begin_index].value
    tuple =  _parse_tuple(tokens=tokens, begin_index=begin_index + 1)
    return FunctionCall(name=name, tuple=tuple)


def _parse_definition(tokens: Sequence[Token], begin_index: int) -> Definition:
    name = tokens[begin_index].value
    begin_index += 1
    equal = tokens[begin_index].value
    assert equal == TokenType.EQUAL.value
    begin_index += 1
    expression = parse_expression(tokens, begin_index)
    return Definition(name=name, expression=expression)


def _parse_environment(tokens: Sequence[Token], begin_index: int) -> FunctionCall:
    first_definition = _parse_definition(tokens, begin_index)
    definitions = [first_definition]
    begin_index += first_definition.expression.num_tokens()
    while tokens[begin_index].type == TokenType.NEW_LINE:
        begin_index += 1
        definition = _parse_definition(tokens, begin_index)
        definitions.append(definition)
    return Environment(definitions)
