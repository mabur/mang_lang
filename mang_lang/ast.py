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
        pass

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        pass


class Dictionary(Expression):
    def __init__(
            self,
            expressions: Sequence[VariableDefinition],
            code: CodeFragment,
    ) -> None:
        super().__init__(code)
        self.value = expressions

    def to_json(self) -> Json:
        return [
            {"type": "variable_definition",
             "name": e.name,
             "value": e.expression.to_json()
             } for e in self.value]

    @staticmethod
    def child_evaluate(name, expression, code, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        value = expression.evaluate(new_environment)
        environment[name] = value
        return VariableDefinition(name=name, expression=value, code=code)

    def lookup(self, name):
        try:
            return next(e.expression for e in self.value if e.name == name)
        except StopIteration:
            print('Could not find symbol: {}'.format(name))
            raise

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        new_environment = deepcopy(environment)
        expressions = [self.child_evaluate(e.name, e.expression, e.expression.code, new_environment) for e in self.value]
        return Dictionary(expressions, code=self.code)


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


class LookupFunction(Expression):
    def __init__(
            self,
            left: str,
            right: Expression,
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


class LookupSymbol(Expression):
    def __init__(
            self,
            left: str,
            code: CodeFragment,
    ) -> None:
        super().__init__(code)
        self.left = left

    def to_json(self) -> Json:
        return {"type": "lookup",
                "left": self.left}

    @run_time_error_printer
    def evaluate(self, environment: Environment) -> Expression:
        # Lookup in current scope
        return environment[self.left]


class LookupChild(Expression):
    def __init__(
            self,
            left: str,
            right: Expression,
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
        dictionary = self.right.evaluate(environment)
        assert isinstance(dictionary, Dictionary)
        child_environment = deepcopy(environment)
        name = self.left
        value = dictionary.lookup(name)
        child_environment[name] = value
        return value
