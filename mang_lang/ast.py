from copy import deepcopy
from typing import Sequence, Optional, MutableMapping, Any, Union, Mapping

from error_handling import run_time_error_printer

Environment = MutableMapping[str, Any]
Json = Union[float, str, Mapping[str, Any], Sequence]


class Expression:
    def __init__(self, section=None):
        self.section = section

    def to_json(self) -> Json:
        raise NotImplemented

    def evaluate(self, environment: Environment):
        raise NotImplemented()


class Array(Expression):
    def __init__(self, expressions: Sequence[Expression], section) -> None:
        super().__init__(section)
        self.value = expressions

    def to_json(self) -> Json:
        return tuple(e.to_json() for e in self.value)

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        expressions = [e.evaluate(new_environment) for e in self.value]
        return Array(expressions, section=self.section)


class Number(Expression):
    def __init__(self, value: str, section) -> None:
        super().__init__(section)
        self.value = float(value)

    def to_json(self) -> Json:
        return {"type": "number", "value": self.value}

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        return self


class String(Expression):
    def __init__(self, value: str, section) -> None:
        super().__init__(section)
        self.value = value[1:-1]

    def to_json(self) -> Json:
        return {"type": "string", "value": self.value}

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        return self


class VariableDefinition(Expression):
    def __init__(self, name: str, expression: Expression, section=None) -> None:
        super().__init__(section)
        self.name = name
        self.expression = expression

    def to_json(self) -> Json:
        return {"type": "variable_definition",
                "name": self.name,
                "value": self.expression.to_json()}

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        value = self.expression.evaluate(new_environment)
        environment[self.name] = value
        return VariableDefinition(name=self.name, expression=value, section=self.section)


class Dictionary(Expression):
    def __init__(self, expressions: Sequence[VariableDefinition], section) -> None:
        super().__init__(section)
        self.value = expressions

    def to_json(self) -> Json:
        return tuple(e.to_json() for e in self.value)

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        expressions = [e.evaluate(new_environment) for e in self.value]
        return Array(expressions, section=self.section)


class Function(Expression):
    def __init__(self, argument_name: str, expression: Expression, section) -> None:
        super().__init__(section)
        self.argument_name = argument_name
        self.expression = expression

    def to_json(self) -> Json:
        return {"type": "function_definition",
                "argument_name": self.argument_name,
                "expression": self.expression.to_json()}

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        return self


class Lookup(Expression):
    def __init__(self, left: str, right: Optional[Expression], section) -> None:
        super().__init__(section)
        self.left = left
        self.right = right

    def to_json(self) -> Json:
        return {"type": "lookup",
                "left": self.left,
                "right": self.right.to_json() if self.right is not None else ''}

    @run_time_error_printer
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
        return function(input, self.section)


class Conditional(Expression):
    def __init__(self, condition: Expression, then_expression: Expression,
                 else_expression: Expression, section) -> None:
        super().__init__(section)
        self.condition = condition
        self.then_expression = then_expression
        self.else_expression = else_expression

    def to_json(self) -> Json:
        return {"type": "conditional",
                "condition": self.condition.to_json(),
                "then_expression": self.then_expression.to_json(),
                "else_expression": self.else_expression.to_json()}

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        if self.condition.evaluate(environment).value:
            return self.then_expression.evaluate(environment)
        else:
            return self.else_expression.evaluate(environment)


class ArrayComprehension(Expression):
    def __init__(self, all_expression: Expression, for_expression: Expression,
                 in_expression: Expression, if_expression: Optional[Expression], section) -> None:
        super().__init__(section)
        self.all_expression = all_expression
        self.for_expression = for_expression
        self.in_expression = in_expression
        self.if_expression = if_expression

    def to_json(self) -> Json:
        return {"type": "tuple_comprehension",
                "all_expression": self.all_expression.to_json(),
                "for_expression": self.for_expression.to_json(),
                "in_expression": self.in_expression.to_json()}

    @run_time_error_printer
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
        return Array(result, section=self.section)
