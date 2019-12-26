from copy import deepcopy
from typing import Any, Callable, Mapping, MutableMapping, Sequence, Optional,\
    Tuple, Union
from lexing import TokenType, lexer
from slice import Slice
from token_slice import TokenSlice
import lexing

Environment = MutableMapping[str, Any]
Json = Union[float, str, Mapping[str, Any], Sequence]

class Expression:
    def to_json(self) -> Json:
        raise NotImplemented

    def evaluate(self, environment: Environment):
        raise NotImplemented()


class Array(Expression):
    def __init__(self, expressions: Sequence[Expression]) -> None:
        self.value = expressions

    def to_json(self) -> Json:
        return tuple(e.to_json() for e in self.value)

    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        expressions = [e.evaluate(new_environment) for e in self.value]
        return Array(expressions)


class Number(Expression):
    def __init__(self, value: str) -> None:
        self.value = float(value)

    def to_json(self) -> Json:
        return {"type": "number", "value": self.value}

    def evaluate(self, environment: Environment) -> Expression:
        return self


class String(Expression):
    def __init__(self, value: str) -> None:
        self.value = value[1:-1]

    def to_json(self) -> Json:
        return {"type": "string", "value": self.value}

    def evaluate(self, environment: Environment) -> Expression:
        return self


class VariableDefinition(Expression):
    def __init__(self, name: str, expression: Expression) -> None:
        self.name = name
        self.expression = expression

    def to_json(self) -> Json:
        return {"type": "variable_definition",
                "name": self.name,
                "value": self.expression.to_json()}

    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        value = self.expression.evaluate(new_environment)
        environment[self.name] = value
        return VariableDefinition(name=self.name, expression=value)


class Dictionary(Expression):
    def __init__(self, expressions: Sequence[VariableDefinition]) -> None:
        self.value = expressions

    def to_json(self) -> Json:
        return tuple(e.to_json() for e in self.value)

    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        expressions = [e.evaluate(new_environment) for e in self.value]
        return Array(expressions)


class Function(Expression):
    def __init__(self, argument_name: str, expression: Expression) -> None:
        self.argument_name = argument_name
        self.expression = expression

    def to_json(self) -> Json:
        return {"type": "function_definition",
                "argument_name": self.argument_name,
                "expression": self.expression.to_json()}

    def evaluate(self, environment: Environment) -> Expression:
        return self


class Lookup(Expression):
    def __init__(self, left: str, right: Optional[Expression]) -> None:
        self.left = left
        self.right = right

    def to_json(self) -> Json:
        return {"type": "lookup",
                "left": self.left,
                "right": self.right.to_json() if self.right is not None else ''}

    def evaluate(self, environment: Environment) -> Expression:
        # Lookup in current scope
        if self.right is None:
            return environment[self.left]

        # Lookup in child scope
        if self.left not in environment:
            tuple = self.right.evaluate(environment)
            assert isinstance(tuple, Array)
            child_environment = deepcopy(environment)
            try:
                child_environment[self.left] = next(
                    e.expression for e in tuple.value if e.name == self.left)
            except StopIteration:
                print('Could not find symbol: {}'.format(self.left))
                raise
            return child_environment[self.left]

        # Function call
        function = environment[self.left]
        input = self.right.evaluate(environment)
        if isinstance(function, Function):
            new_environment = deepcopy(environment)
            new_environment[function.argument_name] = input
            return function.expression.evaluate(new_environment)
        return function(input)


class Conditional(Expression):
    def __init__(self, condition: Expression, then_expression: Expression,
                 else_expression: Expression) -> None:
        self.condition = condition
        self.then_expression = then_expression
        self.else_expression = else_expression

    def to_json(self) -> Json:
        return {"type": "conditional",
                "condition": self.condition.to_json(),
                "then_expression": self.then_expression.to_json(),
                "else_expression": self.else_expression.to_json()}

    def evaluate(self, environment: Environment) -> Expression:
        if self.condition.evaluate(environment).value:
            return self.then_expression.evaluate(environment)
        else:
            return self.else_expression.evaluate(environment)


class ArrayComprehension(Expression):
    def __init__(self, all_expression: Expression, for_expression: Expression,
                 in_expression: Expression, if_expression: Optional[Expression]) -> None:
        self.all_expression = all_expression
        self.for_expression = for_expression
        self.in_expression = in_expression
        self.if_expression = if_expression

    def to_json(self) -> Json:
        return {"type": "tuple_comprehension",
                "all_expression": self.all_expression.to_json(),
                "for_expression": self.for_expression.to_json(),
                "in_expression": self.in_expression.to_json()}

    def evaluate(self, environment: Environment) -> Expression:
        in_expression = self.in_expression.evaluate(environment)
        assert isinstance(in_expression, Array)
        assert isinstance(self.for_expression, Lookup)
        variable_name = self.for_expression.left
        result = []
        for x in in_expression.value:
            local_environment = deepcopy(environment)
            local_environment[variable_name] = x
            y = self.all_expression.evaluate(local_environment)
            if self.if_expression:
                z = self.if_expression.evaluate(local_environment)
                if not bool(z.value):
                    continue
            result.append(y)
        return Array(result)

def _make_token_slice(slice) -> TokenSlice:
    return TokenSlice(lexer(slice))

def _parse_optional_white_space(slice: Slice) -> Slice:
    while slice.front() == ' ' or slice.front() == '\n':
        slice.pop()
    return slice

def _parse_number(slice: Slice) -> Tuple[Number, TokenSlice]:
    token, slice = lexing.parse_number(slice)
    return (Number(token.value), _make_token_slice(slice))


def _parse_string(slice: Slice) -> Tuple[String, TokenSlice]:
    token, slice = lexing.parse_string(slice)
    return (String(token.value), _make_token_slice(slice))


def _parse_array(code: Slice) -> Tuple[Array, TokenSlice]:
    tokens = TokenSlice(lexer(code))
    expressions = ()
    tokens.parse(TokenType.ARRAY_BEGIN)
    while not tokens.do_match(TokenType.ARRAY_END):
        expression, tokens = parse_expression(tokens.as_string())
        expressions += (expression,)
        if tokens.do_match(TokenType.COMMA):
            tokens.parse(TokenType.COMMA)
    tokens.parse(TokenType.ARRAY_END)
    return (Array(expressions=expressions), tokens)


def _parse_dictionary(code: Slice) -> Tuple[Dictionary, TokenSlice]:
    tokens = TokenSlice(lexer(code))
    variable_definitions = []
    tokens.parse(TokenType.DICTIONARY_BEGIN)
    while not tokens.do_match(TokenType.DICTIONARY_END):
        variable_definition, tokens = _parse_variable_definition(tokens.as_string())
        variable_definitions.append(variable_definition)
        if tokens.do_match(TokenType.COMMA):
            tokens.parse(TokenType.COMMA)
    tokens.parse(TokenType.DICTIONARY_END)
    return (Dictionary(expressions=variable_definitions), tokens)


def _parse_lookup(code: Slice) -> Tuple[Lookup, TokenSlice]:
    tokens = TokenSlice(lexer(code))
    left = tokens.parse(TokenType.SYMBOL)
    if not tokens.do_match(TokenType.OF):
        return (Lookup(left=left, right=None), tokens)
    tokens.parse(TokenType.OF)
    right, tokens = parse_expression(tokens.as_string())
    return (Lookup(left=left, right=right), tokens)


def _parse_variable_definition(slice: Slice) -> Tuple[VariableDefinition, TokenSlice]:
    token, slice = lexing.parse_symbol(slice)
    name = token.value
    slice = _parse_optional_white_space(slice)
    _, slice = lexing.FixedParser(TokenType.EQUAL)(slice)
    slice = _parse_optional_white_space(slice)
    expression, tokens = parse_expression(slice)
    return (VariableDefinition(name=name, expression=expression), tokens)


def _parse_function(code: Slice) -> Tuple[Function, TokenSlice]:
    tokens = TokenSlice(lexer(code))
    tokens.parse(TokenType.FROM)
    argument_name = tokens.parse(TokenType.SYMBOL)
    tokens.parse(TokenType.TO)
    expression, tokens = parse_expression(tokens.as_string())
    return (Function(argument_name=argument_name,
                     expression=expression), tokens)


def _parse_conditional(code: Slice) -> Tuple[Conditional, TokenSlice]:
    tokens = TokenSlice(lexer(code))
    tokens.parse(TokenType.IF)
    condition, tokens = parse_expression(tokens.as_string())
    tokens.parse(TokenType.THEN)
    then_expression, tokens = parse_expression(tokens.as_string())
    tokens.parse(TokenType.ELSE)
    else_expression, tokens = parse_expression(tokens.as_string())
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression), tokens)


def _parse_array_comprehension(code: Slice)\
        -> Tuple[ArrayComprehension, TokenSlice]:
    tokens = TokenSlice(lexer(code))
    tokens.parse(TokenType.EACH)
    all_expression, tokens = parse_expression(tokens.as_string())
    tokens.parse(TokenType.FOR)
    for_expression, tokens = parse_expression(tokens.as_string())
    tokens.parse(TokenType.IN)
    in_expression, tokens = parse_expression(tokens.as_string())
    if_expression = None
    if tokens.do_match(TokenType.IF):
        tokens.parse(TokenType.IF)
        if_expression, tokens = parse_expression(tokens.as_string())
    return (ArrayComprehension(all_expression=all_expression,
                               for_expression=for_expression,
                               in_expression=in_expression,
                               if_expression=if_expression), tokens)


def parse_expression(code: Slice) -> Tuple[Expression, TokenSlice]:
    tokens = TokenSlice(lexer(code))
    parser_from_token = {
        TokenType.NUMBER: _parse_number,
        TokenType.STRING: _parse_string,
        TokenType.IF: _parse_conditional,
        TokenType.EACH: _parse_array_comprehension,
        TokenType.FROM: _parse_function,
        TokenType.SYMBOL: _parse_lookup,
        TokenType.ARRAY_BEGIN: _parse_array,
        TokenType.DICTIONARY_BEGIN: _parse_dictionary,
    }
    next_token = tokens[0]
    try:
        parser = parser_from_token[next_token.type]
        return parser(code)
    except KeyError:
        raise ValueError('Bad token pattern: {}'.format(next_token.value))


def lex_and_parse(code: str) -> Expression:
    expression, _ = parse_expression(Slice(code))
    return expression
