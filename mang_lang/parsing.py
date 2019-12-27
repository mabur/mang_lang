from copy import deepcopy
from enum import Enum
from typing import Any, Mapping, MutableMapping, Sequence, Optional,\
    Tuple, Union
from slice import Slice

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


def _parse_while(text: Slice, predicate) -> Tuple[str, Slice]:
    value = ''
    while text and predicate(text.front()):
        value += text.pop()
    return value, text


def _parse_symbol(text: Slice) -> Tuple[str, Slice]:
    value, text = _parse_while(text, lambda c: c.isalnum() or c == '_')
    assert value
    return value, text


def _parse_optional_white_space(text: Slice) -> Slice:
    value, text = _parse_while(text, lambda c: c == ' ' or c == '\n')
    return text


def _parse_number(text: Slice) -> Tuple[Number, Slice]:
    value, text = _parse_while(text, lambda c: c in '-+.1234567890')
    assert value
    return Number(value), text


def _parse_string(text: Slice) -> Tuple[String, Slice]:
    assert text.front() == '\"'
    value = ''
    value += text.pop()
    body, text = _parse_while(text, lambda c: c != '\"')
    value += body
    value += text.pop()
    return String(value), text


def _parse_array(text: Slice) -> Tuple[Array, Slice]:
    expressions = ()
    _, text = FixedParser(TokenType.ARRAY_BEGIN)(text)
    while not text.startswith(TokenType.ARRAY_END.value):
        expression, text = parse_expression(text)
        expressions += (expression,)
        if text.startswith(TokenType.COMMA.value):
            _, text = FixedParser(TokenType.COMMA)(text)
    _, text = FixedParser(TokenType.ARRAY_END)(text)
    return Array(expressions=expressions), text


def _parse_dictionary(text: Slice) -> Tuple[Dictionary, Slice]:
    variable_definitions = []
    _, text = FixedParser(TokenType.DICTIONARY_BEGIN)(text)
    while not text.startswith(TokenType.DICTIONARY_END.value):
        variable_definition, text = _parse_variable_definition(text)
        variable_definitions.append(variable_definition)
        if text.startswith(TokenType.COMMA.value):
            _, text = FixedParser(TokenType.COMMA)(text)
    _, text = FixedParser(TokenType.DICTIONARY_END)(text)
    return Dictionary(expressions=variable_definitions), text


def _parse_lookup(text: Slice) -> Tuple[Lookup, Slice]:
    text = _parse_optional_white_space(text)
    value, text = _parse_symbol(text)
    text = _parse_optional_white_space(text)
    left = value
    if not text.startswith(TokenType.OF.value):
        return Lookup(left=left, right=None), text
    _, text = FixedParser(TokenType.OF)(text)
    right, text = parse_expression(text)
    return Lookup(left=left, right=right), text


def _parse_variable_definition(text: Slice) -> Tuple[VariableDefinition, Slice]:
    value, text = _parse_symbol(text)
    name = value
    text = _parse_optional_white_space(text)
    _, text = FixedParser(TokenType.EQUAL)(text)
    expression, text = parse_expression(text)
    return VariableDefinition(name=name, expression=expression), text


def _parse_function(text: Slice) -> Tuple[Function, Slice]:
    _, text = FixedParser(TokenType.FROM)(text)
    value, text = _parse_symbol(text)
    argument_name = value
    text = _parse_optional_white_space(text)
    _, text = FixedParser(TokenType.TO)(text)
    expression, text = parse_expression(text)
    return Function(argument_name=argument_name, expression=expression), text


def _parse_conditional(text: Slice) -> Tuple[Conditional, Slice]:
    _, text = FixedParser(TokenType.IF)(text)
    condition, text = parse_expression(text)
    _, text = FixedParser(TokenType.THEN)(text)
    then_expression, text = parse_expression(text)
    _, text = FixedParser(TokenType.ELSE)(text)
    else_expression, text = parse_expression(text)
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression), text)


def _parse_array_comprehension(text: Slice)\
        -> Tuple[ArrayComprehension, Slice]:
    _, text = FixedParser(TokenType.EACH)(text)
    all_expression, text = parse_expression(text)
    _, text = FixedParser(TokenType.FOR)(text)
    for_expression, text = parse_expression(text)
    _, text = FixedParser(TokenType.IN)(text)
    in_expression, text = parse_expression(text)
    if_expression = None
    if text.startswith(TokenType.IF.value):
        _, text = FixedParser(TokenType.IF)(text)
        if_expression, text = parse_expression(text)
    return (ArrayComprehension(all_expression=all_expression,
                               for_expression=for_expression,
                               in_expression=in_expression,
                               if_expression=if_expression), text)


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


class TokenType(Enum):
    ARRAY_BEGIN = "["
    ARRAY_END = "]"
    DICTIONARY_BEGIN = "{"
    DICTIONARY_END = "}"
    EQUAL = "="
    COMMA = ","
    IF = "if "
    THEN = "then "
    ELSE = "else "
    EACH = "each "
    FOR = "for "
    IN = "in "
    FROM = "from "
    TO = "to "
    OF = "of "


class FixedParser:
    def __init__(self, token_type: TokenType):
        self.token_type = token_type

    def __call__(self, text: Slice) -> Tuple[str, Slice]:
        value = ''
        for _ in self.token_type.value:
            value += text.pop()
        text = _parse_optional_white_space(text)
        return (value, text)
