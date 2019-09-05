# expression = number | constant | array | function_call
# number = 123...
# constant = symbol
# symbol = abc...
# array = "(" [expression {"," expression}] ")"
# function_call = symbol, array

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


class ExpressionTuple(Expression):
    def __init__(self, expressions: Sequence[Expression]) -> None:
        self.value = expressions

    def to_json(self) -> Json:
        return tuple(e.to_json() for e in self.value)

    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        expressions = [e.evaluate(new_environment) for e in self.value]
        return ExpressionTuple(expressions)


class ScopeTuple(Expression):
    def __init__(self, expressions: Sequence[Expression]) -> None:
        self.expressions = expressions

    def to_json(self) -> Json:
        return tuple(e.to_json() for e in self.expressions)

    def evaluate(self, environment: Environment) -> Expression:
        expressions = [e.evaluate(environment) for e in self.expressions]
        return ScopeTuple(expressions)


class Number(Expression):
    def __init__(self, value: str) -> None:
        self.value = float(value)

    def to_json(self) -> Json:
        return {"type": "number",
                "value": self.value}

    def evaluate(self, environment: Environment) -> Expression:
        return self


class String(Expression):
    def __init__(self, value: str) -> None:
        self.value = value[1:-1]

    def to_json(self) -> Json:
        return {"type": "string",
                "value": self.value}

    def evaluate(self, environment: Environment) -> Expression:
        return self


class Reference(Expression):
    def __init__(self, name: str) -> None:
        self.name = name

    def to_json(self) -> Json:
        return {"type": "reference", "name": self.name}

    def evaluate(self, environment: Environment) -> Expression:
        return environment[self.name]


class FunctionCall(Expression):
    def __init__(self, left: Reference, right: Expression) -> None:
        self.left = left
        self.right = right

    def to_json(self) -> Json:
        return {"type": "function_call",
                "function_name": self.left.to_json()['name'],
                "input": self.right.to_json()}

    def evaluate(self, environment: Environment) -> Expression:
        function = self.left.evaluate(environment)
        input = self.right.evaluate(environment)
        if isinstance(function, FunctionDefinition):
            new_environment = deepcopy(environment)
            new_environment[function.argument_name] = input
            if function.scope:
                _ = function.scope.evaluate(new_environment)
            return function.expression.evaluate(new_environment)
        return function(input)


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


class FunctionDefinition(Expression):
    def __init__(self, argument_name: str,
                 expression: Expression, scope: ScopeTuple) -> None:
        self.argument_name = argument_name
        self.expression = expression
        self.scope = scope

    def to_json(self) -> Json:
        return {"type": "function_definition",
                "argument_name": self.argument_name,
                "expression": self.expression.to_json()}

    def evaluate(self, environment: Environment) -> Expression:
        return self


class Indexing(Expression):
    def __init__(self, reference: Reference, index: Number) -> None:
        self.reference = reference
        self.index = index

    def to_json(self) -> Json:
        return {"type": "indexing",
                "reference_name": self.reference.to_json()['name'],
                "index": self.index.to_json()}

    def evaluate(self, environment: Environment) -> Expression:
        reference = self.reference.evaluate(environment)
        index = int(self.index.evaluate(environment).value)
        element = reference.value[index]
        if isinstance(reference, ExpressionTuple):
            return element
        if isinstance(reference, String):
            return String('"{}"'.format(element))
        raise TypeError


class DefinitionLookup(Expression):
    def __init__(self, left: Reference, right: Expression) -> None:
        self.left = left
        self.right = right

    def to_json(self) -> Json:
        return {"type": "definition_lookup",
                "parent": self.right.to_json(),
                "child": self.left.to_json()}

    def evaluate(self, environment: Environment) -> Expression:
        tuple = self.right.evaluate(environment)
        assert isinstance(tuple, ExpressionTuple)
        child_environment = deepcopy(environment)
        child_environment[self.left.name] = next(
            e.expression for e in tuple.value if e.name == self.left.name)
        return self.left.evaluate(child_environment)


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


class TupleComprehension(Expression):
    def __init__(self, all_expression: Expression, for_expression: Expression,
                 in_expression: Reference, if_expression: Optional[Expression]) -> None:
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
        assert isinstance(in_expression, ExpressionTuple)
        assert isinstance(self.for_expression, Reference)
        variable_name = self.for_expression.name
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
        return ExpressionTuple(result)


def _parse_number(tokens: TokenSlice) -> Tuple[Number, TokenSlice]:
    value = tokens.parse_token()
    return (Number(value), tokens)


def _parse_string(tokens: TokenSlice) -> Tuple[Number, TokenSlice]:
    value = tokens.parse_token()
    return (String(value), tokens)


def _parse_reference(tokens: TokenSlice) -> Tuple[Reference, TokenSlice]:
    name = tokens.parse_token()
    return (Reference(name), tokens)


def _parse_tuple(tokens: TokenSlice) -> Tuple[ExpressionTuple, TokenSlice]:
    expressions = ()
    tokens.parse_known_token(TokenType.PARENTHESIS_BEGIN)
    while not tokens.do_match([TokenType.PARENTHESIS_END]):
        expression, tokens = parse_expression(tokens)
        expressions += (expression,)
        if tokens.do_match([TokenType.COMMA]):
            tokens.parse_known_token(TokenType.COMMA)
    tokens.parse_known_token(TokenType.PARENTHESIS_END)
    return (ExpressionTuple(expressions=expressions), tokens)


def _parse_scope(tokens: TokenSlice) -> Tuple[ScopeTuple, TokenSlice]:
    expressions = ()
    tokens.parse_known_token(TokenType.PARENTHESIS_BEGIN)
    while not tokens.do_match([TokenType.PARENTHESIS_END]):
        expression, tokens = parse_expression(tokens)
        expressions += (expression,)
        if tokens.do_match([TokenType.COMMA]):
            tokens.parse_known_token(TokenType.COMMA)
    tokens.parse_known_token(TokenType.PARENTHESIS_END)
    return (ScopeTuple(expressions=expressions), tokens)


def _parse_function_call(tokens: TokenSlice) -> Tuple[FunctionCall, TokenSlice]:
    left, tokens = _parse_reference(tokens)
    tokens.parse_known_token(TokenType.OF)
    right, tokens = parse_expression(tokens)
    return (FunctionCall(left=left, right=right), tokens)


def _parse_optional_function_scope(tokens: TokenSlice) -> Tuple[ScopeTuple, TokenSlice]:
    if not tokens.do_match([TokenType.WHERE]):
        return None, tokens
    tokens.parse_known_token(TokenType.WHERE)
    return _parse_scope(tokens)


def _parse_variable_definition(tokens: TokenSlice) -> Tuple[VariableDefinition, TokenSlice]:
    name = tokens.parse_token()
    tokens.parse_known_token(TokenType.EQUAL)
    expression, tokens = parse_expression(tokens)
    return (VariableDefinition(name=name, expression=expression), tokens)


def _parse_function_definition(tokens: TokenSlice) -> Tuple[FunctionDefinition, TokenSlice]:
    tokens.parse_known_token(TokenType.FROM)
    argument_name = tokens.parse_token()
    scope, tokens = _parse_optional_function_scope(tokens)
    tokens.parse_known_token(TokenType.TO)
    expression, tokens = parse_expression(tokens)
    return (FunctionDefinition(argument_name=argument_name,
                               scope=scope,
                               expression=expression), tokens)


def _parse_indexing(tokens: TokenSlice) -> Tuple[Indexing, TokenSlice]:
    number, tokens = _parse_number(tokens)
    tokens.parse_known_token(TokenType.OF)
    reference, tokens = _parse_reference(tokens)
    return (Indexing(reference=reference, index=number), tokens)


def _parse_definition_lookup(tokens: TokenSlice) -> Tuple[DefinitionLookup, TokenSlice]:
    left, tokens = _parse_reference(tokens)
    tokens.parse_known_token(TokenType.DOT)
    right, tokens = parse_expression(tokens)
    return (DefinitionLookup(left=left, right=right), tokens)


def _parse_conditional(tokens: TokenSlice) -> Tuple[Conditional, TokenSlice]:
    tokens.parse_known_token(TokenType.IF)
    condition, tokens = parse_expression(tokens)
    tokens.parse_known_token(TokenType.THEN)
    then_expression, tokens = parse_expression(tokens)
    tokens.parse_known_token(TokenType.ELSE)
    else_expression, tokens = parse_expression(tokens)
    return (Conditional(condition=condition, then_expression=then_expression,
                        else_expression=else_expression), tokens)


def _parse_tuple_comprehension(tokens: TokenSlice)\
        -> Tuple[TupleComprehension, TokenSlice]:
    tokens.parse_known_token(TokenType.ALL)
    all_expression, tokens = parse_expression(tokens)
    tokens.parse_known_token(TokenType.FOR)
    for_expression, tokens = parse_expression(tokens)
    tokens.parse_known_token(TokenType.IN)
    in_expression, tokens = parse_expression(tokens)
    if_expression = None
    if tokens.do_match([TokenType.IF]):
        tokens.parse_known_token(TokenType.IF)
        if_expression, tokens = parse_expression(tokens)
    return (TupleComprehension(all_expression=all_expression,
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
        ParsePattern(_parse_indexing, [TokenType.NUMBER, TokenType.OF]),
        ParsePattern(_parse_number, [TokenType.NUMBER]),
        ParsePattern(_parse_string, [TokenType.STRING]),
        ParsePattern(_parse_conditional, [TokenType.IF]),
        ParsePattern(_parse_tuple_comprehension, [TokenType.ALL]),
        ParsePattern(_parse_function_definition, [TokenType.FROM]),
        ParsePattern(_parse_function_call, [TokenType.SYMBOL, TokenType.OF]),
        ParsePattern(_parse_definition_lookup, [TokenType.SYMBOL, TokenType.DOT]),
        ParsePattern(_parse_variable_definition, [TokenType.SYMBOL, TokenType.EQUAL]),
        ParsePattern(_parse_reference, [TokenType.SYMBOL]),
        ParsePattern(_parse_tuple, [TokenType.PARENTHESIS_BEGIN]),
    ]

    for parse_pattern in PARSE_PATTERNS:
        if tokens.do_match(parse_pattern.pattern):
            return parse_pattern.parse_function(tokens)

    raise ValueError('Bad token pattern: {}'.format(tokens.front()))
