from copy import deepcopy
from typing import Any, Mapping, MutableMapping, Sequence, Optional,\
    Tuple, Union
from lexing import TokenType
from slice import Slice
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

def _parse_optional_white_space(slice: Slice) -> Slice:
    while slice and (slice.front() == ' ' or slice.front() == '\n'):
        slice.pop()
    return slice

def _parse_number(slice: Slice) -> Tuple[Number, Slice]:
    value, slice = lexing.parse_number(slice)
    return (Number(value), slice)


def _parse_string(slice: Slice) -> Tuple[String, Slice]:
    value, slice = lexing.parse_string(slice)
    return (String(value), slice)


def _parse_array(slice: Slice) -> Tuple[Array, Slice]:
    expressions = ()
    _, slice = lexing.FixedParser(TokenType.ARRAY_BEGIN)(slice)
    slice = _parse_optional_white_space(slice)
    while not slice.startswith(TokenType.ARRAY_END.value):
        expression, slice = parse_expression(slice)
        expressions += (expression,)
        if slice.startswith(TokenType.COMMA.value):
            _, slice = lexing.FixedParser(TokenType.COMMA)(slice)
            slice = _parse_optional_white_space(slice)
    _, slice = lexing.FixedParser(TokenType.ARRAY_END)(slice)
    slice = _parse_optional_white_space(slice)
    return (Array(expressions=expressions), slice)


def _parse_dictionary(slice: Slice) -> Tuple[Dictionary, Slice]:
    variable_definitions = []
    _, slice = lexing.FixedParser(TokenType.DICTIONARY_BEGIN)(slice)
    slice = _parse_optional_white_space(slice)
    while not slice.startswith(TokenType.DICTIONARY_END.value):
        variable_definition, slice = _parse_variable_definition(slice)
        variable_definitions.append(variable_definition)
        if slice.startswith(TokenType.COMMA.value):
            _, slice = lexing.FixedParser(TokenType.COMMA)(slice)
            slice = _parse_optional_white_space(slice)
    _, slice = lexing.FixedParser(TokenType.DICTIONARY_END)(slice)
    slice = _parse_optional_white_space(slice)
    return (Dictionary(expressions=variable_definitions), slice)


def _parse_lookup(slice: Slice) -> Tuple[Lookup, Slice]:
    slice = _parse_optional_white_space(slice)
    value, slice = lexing.parse_symbol(slice)
    slice = _parse_optional_white_space(slice)
    left = value
    if not slice.startswith(TokenType.OF.value):
        return (Lookup(left=left, right=None), slice)
    token, slice = lexing.FixedParser(TokenType.OF)(slice)
    slice = _parse_optional_white_space(slice)
    right, slice = parse_expression(slice)
    return (Lookup(left=left, right=right), slice)


def _parse_variable_definition(slice: Slice) -> Tuple[VariableDefinition, Slice]:
    value, slice = lexing.parse_symbol(slice)
    name = value
    slice = _parse_optional_white_space(slice)
    _, slice = lexing.FixedParser(TokenType.EQUAL)(slice)
    slice = _parse_optional_white_space(slice)
    expression, slice = parse_expression(slice)
    return (VariableDefinition(name=name, expression=expression), slice)


def _parse_function(slice: Slice) -> Tuple[Function, Slice]:
    _, slice = lexing.FixedParser(TokenType.FROM)(slice)
    slice = _parse_optional_white_space(slice)
    value, slice = lexing.parse_symbol(slice)
    argument_name = value
    slice = _parse_optional_white_space(slice)
    _, slice = lexing.FixedParser(TokenType.TO)(slice)
    slice = _parse_optional_white_space(slice)
    expression, slice = parse_expression(slice)
    return (Function(argument_name=argument_name,
                     expression=expression), slice)


def _parse_conditional(slice: Slice) -> Tuple[Conditional, Slice]:
    _, slice = lexing.FixedParser(TokenType.IF)(slice)
    slice = _parse_optional_white_space(slice)
    condition, slice = parse_expression(slice)
    _, slice = lexing.FixedParser(TokenType.THEN)(slice)
    slice = _parse_optional_white_space(slice)
    then_expression, slice = parse_expression(slice)
    _, slice = lexing.FixedParser(TokenType.ELSE)(slice)
    slice = _parse_optional_white_space(slice)
    else_expression, slice = parse_expression(slice)
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression), slice)


def _parse_array_comprehension(slice: Slice)\
        -> Tuple[ArrayComprehension, Slice]:
    _, slice = lexing.FixedParser(TokenType.EACH)(slice)
    slice = _parse_optional_white_space(slice)
    all_expression, slice = parse_expression(slice)
    _, slice = lexing.FixedParser(TokenType.FOR)(slice)
    slice = _parse_optional_white_space(slice)
    for_expression, slice = parse_expression(slice)
    _, slice = lexing.FixedParser(TokenType.IN)(slice)
    slice = _parse_optional_white_space(slice)
    in_expression, slice = parse_expression(slice)
    if_expression = None
    if slice.startswith(TokenType.IF.value):
        _, slice = lexing.FixedParser(TokenType.IF)(slice)
        slice = _parse_optional_white_space(slice)
        if_expression, slice = parse_expression(slice)
    return (ArrayComprehension(all_expression=all_expression,
                               for_expression=for_expression,
                               in_expression=in_expression,
                               if_expression=if_expression), slice)


parser_from_token = {
    "[": _parse_array,
    "{": _parse_dictionary,
    "if ": _parse_conditional,
    "each ": _parse_array_comprehension,
    "from ": _parse_function,
    "\"": _parse_string,
}

for char in '+-.1234567890':
    parser_from_token[char] = _parse_number

for char in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_':
    parser_from_token[char] = _parse_lookup

def parse_expression(slice: Slice) -> Tuple[Expression, Slice]:
    try:
        for sequence, parser in parser_from_token.items():
            if slice.startswith(sequence):
                return parser(slice)
    except KeyError:
        raise ValueError('Bad token pattern: {}'.format(slice[0]))


def lex_and_parse(code: str) -> Expression:
    expression, _ = parse_expression(Slice(code))
    return expression
