# expression = number | constant | array | function_call
# number = 123...
# constant = symbol
# symbol = abc...
# array = "(" [expression {"," expression}] ")"
# function_call = symbol, array

from copy import deepcopy
from typing import Any, Mapping, Sequence
from lexing import Token, TokenType


Environment = Mapping[str, Any]


class Expression:
    def num_tokens(self) -> int:
        raise NotImplemented()

    def evaluate(self, environment: Environment):
        raise NotImplemented()


class Tuple(Expression):
    def __init__(self, expressions: Sequence[Expression]) -> None:
        self.expressions = expressions

    def num_tokens(self) -> int:
        return 1 + sum(e.num_tokens() + 1 for e in self.expressions)

    def evaluate(self, environment: Environment):
        value = ()
        new_environment = deepcopy(environment)
        for expression in self.expressions:
            expression_value = expression.evaluate(new_environment)
            value += (expression_value,)
            if isinstance(expression, VariableDefinition):
                new_environment[expression.name] = expression_value[1]
        return value


class Number(Expression):
    def __init__(self, value: str) -> None:
        self.value = value

    def num_tokens(self) -> int:
        return 1

    def evaluate(self, environment: Environment):
        return float(self.value)


class Constant(Expression):
    def __init__(self, name: str) -> None:
        self.name = name

    def num_tokens(self) -> int:
        return 1

    def evaluate(self, environment: Environment):
        return environment[self.name]


class FunctionCall(Expression):
    def __init__(self, name: str, tuple: Tuple) -> None:
        self.name = name
        self.tuple = tuple

    def num_tokens(self) -> int:
        return 1 + self.tuple.num_tokens()

    def evaluate(self, environment: Environment):
        input = self.tuple.evaluate(environment)
        function = environment[self.name]
        return function(input)


class VariableDefinition(Expression):
    def __init__(self, name: str, expression: Expression) -> None:
        self.name = name
        self.expression = expression

    def num_tokens(self) -> int:
        return 2 + self.expression.num_tokens()

    def evaluate(self, environment: Environment):
        return (self.name, self.expression.evaluate(environment))


class TupleIndexing(Expression):
    def __init__(self, constant: Constant, index: Expression) -> None:
        self.constant = constant
        self.index = index

    def num_tokens(self) -> int:
        return self.constant.num_tokens() + 1 + self.index.num_tokens() + 1

    def evaluate(self, environment: Environment):
        constant = self.constant.evaluate(environment)
        index = int(self.index.evaluate(environment))
        return constant[index]


class DefinitionLookup(Expression):
    def __init__(self, tuple: Constant, symbol: Constant) -> None:
        self.tuple = tuple
        self.symbol = symbol

    def num_tokens(self) -> int:
        return self.tuple.num_tokens() + 1 + self.symbol.num_tokens()

    def evaluate(self, environment: Environment):
        tuple = environment[self.tuple.name]
        return next(e[1] for e in tuple if e[0] == self.symbol.name)


def parse_expression(tokens: Sequence[Token], begin_index: int = 0) -> Expression:
    end_index = len(tokens)
    assert begin_index < end_index

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.NUMBER]):
        return _parse_number(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL, TokenType.PARENTHESIS_BEGIN]):
        return _parse_function_call(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL, TokenType.BRACKET_BEGIN]):
        return _parse_tuple_indexing(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL, TokenType.DOT, TokenType.SYMBOL]):
        return _parse_definition_lookup(tokens=tokens, begin_index=begin_index)

    if _do_tokens_match(tokens=tokens, begin_index=begin_index, token_pattern=[TokenType.SYMBOL, TokenType.EQUAL]):
       return _parse_variable_definition(tokens=tokens, begin_index=begin_index)

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
    expressions = ()
    begin_index += 1 # eat (
    while tokens[begin_index].type != TokenType.PARENTHESIS_END:
        expression = parse_expression(tokens=tokens, begin_index=begin_index)
        expressions += (expression,)
        begin_index += expression.num_tokens()
        if tokens[begin_index].type == TokenType.COMMA:
            begin_index += 1 # eat ,
    begin_index += 1 # eat )
    return Tuple(expressions=expressions)


def _parse_function_call(tokens: Sequence[Token], begin_index: int) -> FunctionCall:
    name = tokens[begin_index].value
    tuple =  _parse_tuple(tokens=tokens, begin_index=begin_index + 1)
    return FunctionCall(name=name, tuple=tuple)


def _parse_variable_definition(tokens: Sequence[Token], begin_index: int) -> VariableDefinition:
    name = tokens[begin_index].value
    begin_index += 1
    equal = tokens[begin_index].value
    assert equal == TokenType.EQUAL.value
    begin_index += 1
    expression = parse_expression(tokens, begin_index)
    return VariableDefinition(name=name, expression=expression)


def _parse_tuple_indexing(tokens: Sequence[Token], begin_index: int) -> FunctionCall:
    name = tokens[begin_index].value
    begin_index += 1
    bracket_begin = tokens[begin_index].value
    assert bracket_begin == TokenType.BRACKET_BEGIN.value[-1]
    begin_index += 1
    expression =  parse_expression(tokens=tokens, begin_index=begin_index)
    begin_index += expression.num_tokens()
    bracket_end = tokens[begin_index].value
    assert bracket_end == TokenType.BRACKET_END.value[-1]
    return TupleIndexing(constant=Constant(name=name), index=expression)


def _parse_definition_lookup(tokens: Sequence[Token], begin_index: int) -> FunctionCall:
    tuple_name = tokens[begin_index].value
    begin_index += 1
    dot = tokens[begin_index].value
    assert dot == TokenType.DOT.value[-1]
    begin_index += 1
    member_name = tokens[begin_index].value
    return DefinitionLookup(tuple=Constant(tuple_name),symbol=Constant(member_name))
