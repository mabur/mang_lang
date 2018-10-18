# expression = number | constant | array | function_call
# number = 123...
# constant = symbol
# symbol = abc...
# array = "(" [expression {"," expression}] ")"
# function_call = symbol, array

from copy import deepcopy
from typing import Any, Callable, Mapping, Sequence, Tuple
from lexing import Token, TokenType


Environment = Mapping[str, Any]


class Expression:

    def evaluate(self, environment: Environment):
        raise NotImplemented()


class ExpressionTuple(Expression):
    def __init__(self, expressions: Sequence[Expression]) -> None:
        self.expressions = expressions


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


    def evaluate(self, environment: Environment):
        return float(self.value)


class Constant(Expression):
    def __init__(self, name: str) -> None:
        self.name = name


    def evaluate(self, environment: Environment):
        return environment[self.name]


class FunctionCall(Expression):
    def __init__(self, name: Constant, tuple: ExpressionTuple) -> None:
        self.name = name
        self.tuple = tuple


    def evaluate(self, environment: Environment):
        input = self.tuple.evaluate(environment)
        if len(input) == 1:
            input = input[0]
        function = self.name.evaluate(environment)
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


    def evaluate(self, environment: Environment):
        return (self.name, self.expression.evaluate(environment))


class FunctionDefinition(Expression):
    def __init__(self, function_name: str, argument_name: str, expression: Expression) -> None:
        self.function_name = function_name
        self.argument_name = argument_name
        self.expression = expression


    def evaluate(self, environment: Environment):
        return (self.function_name, self.argument_name, self.expression)


class TupleIndexing(Expression):
    def __init__(self, constant: Constant, index: Expression) -> None:
        self.constant = constant
        self.index = index


    def evaluate(self, environment: Environment):
        constant = self.constant.evaluate(environment)
        index = int(self.index.evaluate(environment))
        return constant[index]


class DefinitionLookup(Expression):
    def __init__(self, tuple: Constant, symbol: Constant) -> None:
        self.tuple = tuple
        self.symbol = symbol


    def evaluate(self, environment: Environment):
        tuple = environment[self.tuple.name]
        return next(e[1] for e in tuple if e[0] == self.symbol.name)


class TokenSlice:
    def __init__(self, tokens: Sequence[Token], begin_index: int = 0):
        assert begin_index <= len(tokens)
        self.tokens = tokens
        self.begin_index = begin_index

    def front(self) -> Token:
        return self.tokens[self.begin_index]

    def do_match(self, token_pattern: Sequence[TokenType]) -> bool:
        if len(self.tokens) < self.begin_index + len(token_pattern):
            return False
        return all(
            self.tokens[self.begin_index + i].type == token for i, token in
            enumerate(token_pattern))


class ParsePattern:
    def __init__(self,
                 parse_function: Callable[[TokenSlice], Tuple[Expression, TokenSlice]],
                 pattern: Sequence[TokenType]) -> None:
        self.parse_function = parse_function
        self.pattern = pattern


def step(tokens: TokenSlice, num_steps: int) -> TokenSlice:
    return TokenSlice(tokens=tokens.tokens, begin_index=tokens.begin_index + num_steps)


def _parse_known_token(tokens: TokenSlice, expected: TokenType) -> TokenSlice:
    assert tokens.front().value == expected.value[-1]
    return step(tokens, 1)


def _parse_token(tokens: TokenSlice) -> Tuple[str, TokenSlice]:
    return (tokens.front().value, step(tokens, 1))


def parse_expression(tokens: TokenSlice) -> Tuple[Expression, TokenSlice]:
    PARSE_PATTERNS = [
        ParsePattern(_parse_number, [TokenType.NUMBER]),
        ParsePattern(_parse_function_definition, [TokenType.SYMBOL, TokenType.PARENTHESIS_BEGIN, TokenType.SYMBOL, TokenType.PARENTHESIS_END, TokenType.EQUAL]),
        ParsePattern(_parse_function_call, [TokenType.SYMBOL, TokenType.PARENTHESIS_BEGIN]),
        ParsePattern(_parse_tuple_indexing, [TokenType.SYMBOL, TokenType.BRACKET_BEGIN]),
        ParsePattern(_parse_definition_lookup, [TokenType.SYMBOL, TokenType.DOT, TokenType.SYMBOL]),
        ParsePattern(_parse_variable_definition, [TokenType.SYMBOL, TokenType.EQUAL]),
        ParsePattern(_parse_constant, [TokenType.SYMBOL]),
        ParsePattern(_parse_tuple, [TokenType.PARENTHESIS_BEGIN])]

    for parse_pattern in PARSE_PATTERNS:
        if tokens.do_match(parse_pattern.pattern):
            return parse_pattern.parse_function(tokens)

    raise ValueError('Bad token pattern: {}'.format(tokens.front().value))


def _parse_number(tokens: TokenSlice) -> Tuple[Number, TokenSlice]:
    value, tokens = _parse_token(tokens)
    return (Number(value), tokens)


def _parse_constant(tokens: TokenSlice) -> Tuple[Constant, TokenSlice]:
    name, tokens = _parse_token(tokens)
    return (Constant(name), tokens)


def _parse_tuple(tokens: TokenSlice) -> Tuple[ExpressionTuple, TokenSlice]:
    expressions = ()
    tokens = _parse_known_token(tokens, TokenType.PARENTHESIS_BEGIN)
    while tokens.front().type != TokenType.PARENTHESIS_END:
        expression, tokens = parse_expression(tokens)
        expressions += (expression,)
        if tokens.front().type == TokenType.COMMA:
            tokens = _parse_known_token(tokens, TokenType.COMMA)
    tokens = _parse_known_token(tokens, TokenType.PARENTHESIS_END)
    return (ExpressionTuple(expressions=expressions), tokens)


def _parse_function_call(tokens: TokenSlice) -> Tuple[FunctionCall, TokenSlice]:
    name, tokens = _parse_constant(tokens)
    tuple, tokens = _parse_tuple(tokens)
    return (FunctionCall(name=name, tuple=tuple), tokens)


def _parse_variable_definition(tokens: TokenSlice) -> Tuple[VariableDefinition, TokenSlice]:
    name, tokens = _parse_token(tokens)
    tokens = _parse_known_token(tokens, TokenType.EQUAL)
    expression, tokens = parse_expression(tokens)
    return (VariableDefinition(name=name, expression=expression), tokens)


def _parse_function_definition(tokens: TokenSlice) -> Tuple[FunctionDefinition, TokenSlice]:
    function_name, tokens = _parse_token(tokens)
    tokens = _parse_known_token(tokens, TokenType.PARENTHESIS_BEGIN)
    argument_name, tokens = _parse_token(tokens)
    tokens = _parse_known_token(tokens, TokenType.PARENTHESIS_END)
    tokens = _parse_known_token(tokens, TokenType.EQUAL)
    expression, tokens = parse_expression(tokens)
    return (FunctionDefinition(function_name=function_name, argument_name=argument_name, expression=expression),
            tokens)


def _parse_tuple_indexing(tokens: TokenSlice) -> Tuple[TupleIndexing, TokenSlice]:
    constant, tokens = _parse_constant(tokens)
    tokens = _parse_known_token(tokens, TokenType.BRACKET_BEGIN)
    expression, tokens = parse_expression(tokens)
    tokens = _parse_known_token(tokens, TokenType.BRACKET_END)
    return (TupleIndexing(constant=constant, index=expression), tokens)


def _parse_definition_lookup(tokens: TokenSlice) -> Tuple[DefinitionLookup, TokenSlice]:
    tuple, tokens = _parse_constant(tokens)
    tokens = _parse_known_token(tokens, TokenType.DOT)
    symbol, tokens = _parse_constant(tokens)
    return (DefinitionLookup(tuple=tuple, symbol=symbol), tokens)
