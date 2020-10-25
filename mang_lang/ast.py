from copy import deepcopy
from typing import Callable, Sequence, Optional, MutableMapping, Any, Union, Mapping

from error_handling import CodeFragment, run_time_error_printer

Environment = MutableMapping[str, Any]
Json = Union[float, str, Mapping[str, Any], Sequence]


class Expression:
    def __init__(self, code: CodeFragment):
        self.code = code

    def to_json(self) -> Json:
        raise NotImplemented

    def evaluate(self, environment: Environment):
        raise NotImplemented()


class Number(Expression):
    def __init__(self, value: str, code: CodeFragment) -> None:
        super().__init__(code)
        self.value = float(value)

    def to_json(self) -> Json:
        return {"type": "number", "value": self.value}

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        return self


class String(Expression):
    def __init__(self, value: str, code: CodeFragment) -> None:
        super().__init__(code)
        self.value = value[1:-1]

    def to_json(self) -> Json:
        return {"type": "string", "value": self.value}

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        return self


class Array(Expression):
    def __init__(self, expressions: Sequence[Expression], code: CodeFragment) -> None:
        super().__init__(code)
        self.value = expressions

    def to_json(self) -> Json:
        return [e.to_json() for e in self.value]

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        expressions = [e.evaluate(new_environment) for e in self.value]
        return Array(expressions, code=self.code)


class VariableDefinition(Expression):
    def __init__(self, name: str, expression: Expression, code: CodeFragment) -> None:
        super().__init__(code)
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
        return VariableDefinition(name=self.name, expression=value, code=self.code)


class Dictionary(Expression):
    def __init__(
            self,
            expressions: Sequence[VariableDefinition],
            code: CodeFragment,
    ) -> None:
        super().__init__(code)
        self.value = expressions

    def to_json(self) -> Json:
        return [e.to_json() for e in self.value]

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        expressions = [e.evaluate(new_environment) for e in self.value]
        return Array(expressions, code=self.code)


class Conditional(Expression):
    def __init__(
            self,
            condition: Expression,
            then_expression: Expression,
            else_expression: Expression,
            code: CodeFragment,
    ) -> None:
        super().__init__(code)
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


class Function(Expression):
    def __init__(
            self,
            argument_name: str,
            expression: Expression,
            code: CodeFragment,
    ) -> None:
        super().__init__(code)
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
    def __init__(
            self,
            left: str,
            right: Optional[Expression],
            code: CodeFragment,
    ) -> None:
        super().__init__(code)
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

        if isinstance(function, Callable):
            return function(input, self.code)

        raise TypeError
