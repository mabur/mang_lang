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
            if isinstance(expression, FunctionDefinition):
                new_environment[expression.function_name] = expression
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
        if len(input) == 1:
            input = input[0]
        function = environment[self.name]
        if isinstance(function, Expression):
            # Todo: add argument definition to environment
            new_environment = deepcopy(environment)
            new_environment[function.argument_name] = input
            return function.expression.evaluate(new_environment)
        return function(input)


class VariableDefinition(Expression):
    def __init__(self, name: str, expression: Expression) -> None:
        self.name = name
        self.expression = expression

    def num_tokens(self) -> int:
        return 2 + self.expression.num_tokens()

    def evaluate(self, environment: Environment):
        return (self.name, self.expression.evaluate(environment))


class FunctionDefinition(Expression):
    def __init__(self, function_name: str, argument_name: str, expression: Expression) -> None:
        self.function_name = function_name
        self.argument_name = argument_name
        self.expression = expression

    def num_tokens(self) -> int:
        return 5 + self.expression.num_tokens()

    def evaluate(self, environment: Environment):
        return (self.function_name, self.argument_name, self.expression)


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


class TokenSlice:
    def __init__(self, tokens: Sequence[Token], begin_index: int = 0):
        assert begin_index <= len(tokens)
        self.tokens = tokens
        self.begin_index = begin_index

    def front(self) -> str:
        return self.tokens[self.begin_index].value

    def does_match(self, token_pattern: Sequence[TokenType]) -> bool:
        if len(self.tokens) < self.begin_index + len(token_pattern):
            return False
        return all(
            self.tokens[self.begin_index + i].type == token for i, token in
            enumerate(token_pattern))


def step(tokens: TokenSlice, num_steps: int) -> TokenSlice:
    return TokenSlice(tokens=tokens.tokens, begin_index=tokens.begin_index + num_steps)


def parse_expression(token_slice: TokenSlice) -> Expression:
    if token_slice.does_match([TokenType.NUMBER]):
        return _parse_number(token_slice)

    if token_slice.does_match([TokenType.SYMBOL, TokenType.PARENTHESIS_BEGIN, TokenType.SYMBOL, TokenType.PARENTHESIS_END, TokenType.EQUAL]):
        return _parse_function_definition(token_slice)

    if token_slice.does_match([TokenType.SYMBOL, TokenType.PARENTHESIS_BEGIN]):
        return _parse_function_call(token_slice)

    if token_slice.does_match([TokenType.SYMBOL, TokenType.BRACKET_BEGIN]):
        return _parse_tuple_indexing(token_slice)

    if token_slice.does_match([TokenType.SYMBOL, TokenType.DOT, TokenType.SYMBOL]):
        return _parse_definition_lookup(token_slice)

    if token_slice.does_match([TokenType.SYMBOL, TokenType.EQUAL]):
       return _parse_variable_definition(token_slice)

    if token_slice.does_match([TokenType.SYMBOL]):
        return _parse_constant(token_slice)

    if token_slice.does_match([TokenType.PARENTHESIS_BEGIN]):
        return _parse_tuple(token_slice)

    raise ValueError('Bad token pattern: {}'.format(token_slice.front()))


def _do_tokens_match(tokens: TokenSlice, token_pattern: Sequence[Token])\
        -> bool:
    if len(tokens.tokens) < tokens.begin_index + len(token_pattern):
        return False
    return all(tokens.tokens[tokens.begin_index + i].type == token for i, token in enumerate(token_pattern))


def _parse_number(tokens: TokenSlice) -> Number:
    return Number(value=tokens.front())


def _parse_constant(tokens: TokenSlice) -> Constant:
    return Constant(name=tokens.front())


def _parse_tuple(tokens: TokenSlice) -> Tuple:
    expressions = ()
    _assert_token(tokens, expected=TokenType.PARENTHESIS_BEGIN)
    tokens = step(tokens, 1)
    while tokens.tokens[tokens.begin_index].type != TokenType.PARENTHESIS_END:
        expression = parse_expression(tokens)
        expressions += (expression,)
        tokens = step(tokens, expression.num_tokens())
        if tokens.tokens[tokens.begin_index].type == TokenType.COMMA:
            _assert_token(tokens, expected=TokenType.COMMA)
            tokens = step(tokens, 1)
    _assert_token(tokens, expected=TokenType.PARENTHESIS_END)
    tokens = step(tokens, 1)
    return Tuple(expressions=expressions)


def _parse_function_call(tokens: TokenSlice) -> FunctionCall:
    name = tokens.front()
    tokens = step(tokens, 1)
    tuple =  _parse_tuple(tokens)
    return FunctionCall(name=name, tuple=tuple)


def _parse_variable_definition(tokens: TokenSlice) -> VariableDefinition:
    name = tokens.front()
    tokens = step(tokens, 1)
    _assert_token(tokens, expected=TokenType.EQUAL)
    tokens = step(tokens, 1)
    expression = parse_expression(tokens)
    return VariableDefinition(name=name, expression=expression)


def _parse_function_definition(tokens: TokenSlice) -> FunctionDefinition:
    function_name = tokens.front()
    tokens = step(tokens, 1)
    _assert_token(tokens, expected=TokenType.PARENTHESIS_BEGIN)
    tokens = step(tokens, 1)
    argument_name = tokens.front()
    tokens = step(tokens, 1)
    _assert_token(tokens, expected=TokenType.PARENTHESIS_END)
    tokens = step(tokens, 1)
    _assert_token(tokens, expected=TokenType.EQUAL)
    tokens = step(tokens, 1)
    expression = parse_expression(tokens)
    return FunctionDefinition(function_name=function_name, argument_name=argument_name, expression=expression)


def _parse_tuple_indexing(tokens: TokenSlice) -> TupleIndexing:
    constant = _parse_constant(tokens)
    tokens = step(tokens, constant.num_tokens())
    _assert_token(tokens, expected=TokenType.BRACKET_BEGIN)
    tokens = step(tokens, 1)
    expression = parse_expression(tokens)
    tokens = step(tokens, expression.num_tokens())
    _assert_token(tokens, expected=TokenType.BRACKET_END)
    tokens = step(tokens, 1)
    return TupleIndexing(constant=constant, index=expression)


def _parse_definition_lookup(tokens: TokenSlice) -> DefinitionLookup:
    tuple = _parse_constant(tokens)
    tokens = step(tokens, tuple.num_tokens())
    _assert_token(tokens, expected=TokenType.DOT)
    tokens = step(tokens, 1)
    symbol = _parse_constant(tokens)
    tokens = step(tokens, symbol.num_tokens())
    return DefinitionLookup(tuple=tuple, symbol=symbol)


def _assert_token(tokens: TokenSlice, expected: TokenType) -> None:
    assert tokens.front() == expected.value[-1]
