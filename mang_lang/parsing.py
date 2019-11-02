from copy import deepcopy
from typing import Any, Callable, Mapping, MutableMapping, Sequence, Optional,\
    Tuple, Union
from lexing import TokenType
from token_slice import TokenSlice

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
                 in_expression: Lookup, if_expression: Optional[Expression]) -> None:
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


def _parse_number(tokens: TokenSlice) -> Tuple[Number, TokenSlice]:
    value = tokens.parse_token()
    return (Number(value), tokens)


def _parse_string(tokens: TokenSlice) -> Tuple[Number, TokenSlice]:
    value = tokens.parse_token()
    return (String(value), tokens)


def _parse_array(tokens: TokenSlice) -> Tuple[Array, TokenSlice]:
    expressions = ()
    tokens.parse_known_token(TokenType.ARRAY_BEGIN)
    while not tokens.do_match([TokenType.ARRAY_END]):
        expression, tokens = parse_expression(tokens)
        expressions += (expression,)
        if tokens.do_match([TokenType.COMMA]):
            tokens.parse_known_token(TokenType.COMMA)
    tokens.parse_known_token(TokenType.ARRAY_END)
    return (Array(expressions=expressions), tokens)


def _parse_dictionary(tokens: TokenSlice) -> Tuple[Dictionary, TokenSlice]:
    variable_definitions = []
    tokens.parse_known_token(TokenType.DICTIONARY_BEGIN)
    while not tokens.do_match([TokenType.DICTIONARY_END]):
        variable_definition, tokens = _parse_variable_definition(tokens)
        variable_definitions.append(variable_definition)
        if tokens.do_match([TokenType.COMMA]):
            tokens.parse_known_token(TokenType.COMMA)
    tokens.parse_known_token(TokenType.DICTIONARY_END)
    return (Dictionary(expressions=variable_definitions), tokens)


def _parse_lookup(tokens: TokenSlice) -> Tuple[Lookup, TokenSlice]:
    left = tokens.parse_token()
    if not tokens.do_match([TokenType.OF]):
        return (Lookup(left=left, right=None), tokens)
    tokens.parse_known_token(TokenType.OF)
    right, tokens = parse_expression(tokens)
    return (Lookup(left=left, right=right), tokens)


def _parse_variable_definition(tokens: TokenSlice) -> Tuple[VariableDefinition, TokenSlice]:
    name = tokens.parse_token()
    tokens.parse_known_token(TokenType.EQUAL)
    expression, tokens = parse_expression(tokens)
    return (VariableDefinition(name=name, expression=expression), tokens)


def _parse_function(tokens: TokenSlice) -> Tuple[Function, TokenSlice]:
    tokens.parse_known_token(TokenType.FROM)
    argument_name = tokens.parse_token()
    tokens.parse_known_token(TokenType.TO)
    expression, tokens = parse_expression(tokens)
    return (Function(argument_name=argument_name,
                     expression=expression), tokens)


def _parse_conditional(tokens: TokenSlice) -> Tuple[Conditional, TokenSlice]:
    tokens.parse_known_token(TokenType.IF)
    condition, tokens = parse_expression(tokens)
    tokens.parse_known_token(TokenType.THEN)
    then_expression, tokens = parse_expression(tokens)
    tokens.parse_known_token(TokenType.ELSE)
    else_expression, tokens = parse_expression(tokens)
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression), tokens)


def _parse_array_comprehension(tokens: TokenSlice)\
        -> Tuple[ArrayComprehension, TokenSlice]:
    tokens.parse_known_token(TokenType.EACH)
    all_expression, tokens = parse_expression(tokens)
    tokens.parse_known_token(TokenType.FOR)
    for_expression, tokens = parse_expression(tokens)
    tokens.parse_known_token(TokenType.IN)
    in_expression, tokens = parse_expression(tokens)
    if_expression = None
    if tokens.do_match([TokenType.IF]):
        tokens.parse_known_token(TokenType.IF)
        if_expression, tokens = parse_expression(tokens)
    return (ArrayComprehension(all_expression=all_expression,
                               for_expression=for_expression,
                               in_expression=in_expression,
                               if_expression=if_expression), tokens)

class ParsePattern:
    def __init__(self,
                 parse_function: Callable[[TokenSlice], Tuple[Expression, TokenSlice]],
                 pattern: Sequence[TokenType]) -> None:
        self.parse_function = parse_function
        self.pattern = pattern


def parse_expression(tokens: TokenSlice) -> Tuple[Expression, TokenSlice]:
    PARSE_PATTERNS = [
        ParsePattern(_parse_number, [TokenType.NUMBER]),
        ParsePattern(_parse_string, [TokenType.STRING]),
        ParsePattern(_parse_conditional, [TokenType.IF]),
        ParsePattern(_parse_array_comprehension, [TokenType.EACH]),
        ParsePattern(_parse_function, [TokenType.FROM]),
        ParsePattern(_parse_lookup, [TokenType.SYMBOL]),
        ParsePattern(_parse_array, [TokenType.ARRAY_BEGIN]),
        ParsePattern(_parse_dictionary, [TokenType.DICTIONARY_BEGIN]),
    ]

    for parse_pattern in PARSE_PATTERNS:
        if tokens.do_match(parse_pattern.pattern):
            return parse_pattern.parse_function(tokens)

    raise ValueError('Bad token pattern: {}'.format(tokens[0].value))
