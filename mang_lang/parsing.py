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
STRING_BEGIN = "\""
STRING_END = "\""


def _parse_sequence(text: Slice, sequence) -> Tuple[str, Slice]:
    value = ''
    for _ in sequence:
        value += text.pop()
    text = _parse_optional_white_space(text)
    return value, text


def _parse_while(text: Slice, predicate) -> Tuple[str, Slice]:
    value = ''
    while text and predicate(text.front()):
        value += text.pop()
    return value, text


def _parse_optional_white_space(text: Slice) -> Slice:
    value, text = _parse_while(text, lambda c: c == ' ' or c == '\n')
    return text


def _parse_symbol(text: Slice) -> Tuple[str, Slice]:
    value, text = _parse_while(text, lambda c: c.isalnum() or c == '_')
    assert value
    text = _parse_optional_white_space(text)
    return value, text


def _parse_number(text: Slice) -> Tuple[Number, Slice]:
    value, text = _parse_while(text, lambda c: c in '-+.1234567890')
    assert value
    text = _parse_optional_white_space(text)
    return Number(value), text


def _parse_string(text: Slice) -> Tuple[String, Slice]:
    assert text.front() == STRING_BEGIN
    value = text.pop()
    body, text = _parse_while(text, lambda c: c != STRING_END)
    value += body
    value += text.pop()
    text = _parse_optional_white_space(text)
    return String(value), text


def _parse_array(text: Slice) -> Tuple[Array, Slice]:
    expressions = ()
    _, text = _parse_sequence(text, ARRAY_BEGIN)
    while not text.startswith(ARRAY_END):
        expression, text = parse_expression(text)
        expressions += (expression,)
        if text.startswith(COMMA):
            _, text = _parse_sequence(text, COMMA)
    _, text = _parse_sequence(text, ARRAY_END)
    return Array(expressions=expressions), text


def _parse_dictionary(text: Slice) -> Tuple[Dictionary, Slice]:
    variable_definitions = []
    _, text = _parse_sequence(text, DICTIONARY_BEGIN)
    while not text.startswith(DICTIONARY_END):
        variable_definition, text = _parse_variable_definition(text)
        variable_definitions.append(variable_definition)
        if text.startswith(COMMA):
            _, text = _parse_sequence(text, COMMA)
    _, text = _parse_sequence(text, DICTIONARY_END)
    return Dictionary(expressions=variable_definitions), text


def _parse_lookup(text: Slice) -> Tuple[Lookup, Slice]:
    value, text = _parse_symbol(text)
    left = value
    if not text.startswith(OF):
        return Lookup(left=left, right=None), text
    _, text = _parse_sequence(text, OF)
    right, text = parse_expression(text)
    return Lookup(left=left, right=right), text


def _parse_variable_definition(text: Slice) -> Tuple[VariableDefinition, Slice]:
    value, text = _parse_symbol(text)
    name = value
    _, text = _parse_sequence(text, EQUAL)
    expression, text = parse_expression(text)
    return VariableDefinition(name=name, expression=expression), text


def _parse_function(text: Slice) -> Tuple[Function, Slice]:
    _, text = _parse_sequence(text, FROM)
    value, text = _parse_symbol(text)
    argument_name = value
    _, text = _parse_sequence(text, TO)
    expression, text = parse_expression(text)
    return Function(argument_name=argument_name, expression=expression), text


def _parse_conditional(text: Slice) -> Tuple[Conditional, Slice]:
    _, text = _parse_sequence(text, IF)
    condition, text = parse_expression(text)
    _, text = _parse_sequence(text, THEN)
    then_expression, text = parse_expression(text)
    _, text = _parse_sequence(text, ELSE)
    else_expression, text = parse_expression(text)
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression), text)


def _parse_array_comprehension(text: Slice)\
        -> Tuple[ArrayComprehension, Slice]:
    _, text = _parse_sequence(text, EACH)
    all_expression, text = parse_expression(text)
    _, text = _parse_sequence(text, FOR)
    for_expression, text = parse_expression(text)
    _, text = _parse_sequence(text, IN)
    in_expression, text = parse_expression(text)
    if_expression = None
    if text.startswith(IF):
        _, text = _parse_sequence(text, IF)
        if_expression, text = parse_expression(text)
    return (ArrayComprehension(all_expression=all_expression,
                               for_expression=for_expression,
                               in_expression=in_expression,
                               if_expression=if_expression), text)


parser_from_token = {
    ARRAY_BEGIN: _parse_array,
    DICTIONARY_BEGIN: _parse_dictionary,
    IF: _parse_conditional,
    EACH: _parse_array_comprehension,
    FROM: _parse_function,
    STRING_BEGIN: _parse_string,
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
    expression, _ = parse_expression(_parse_optional_white_space(Slice(code)))
    return expression
