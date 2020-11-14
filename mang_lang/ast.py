from copy import deepcopy
from typing import (
    Any, Callable, Iterator, Mapping, MutableMapping, Optional, Sequence, Tuple,
    Union
)

from error_handling import CodeFragment, run_time_error_printer

Environment = MutableMapping[str, Any]
Json = Union[float, str, Mapping[str, Any], Sequence]


class Expression:
    def __init__(self, code: CodeFragment):
        self.code = code
        self.parent = None

    def to_json(self) -> Json:
        raise NotImplemented

    def evaluate(self, parent) -> "Expression":
        raise NotImplemented()

    def get(self, name: str) -> Optional["Expression"]:
        return self.parent.get(name)


class Number(Expression):
    def __init__(self, value: str, code: CodeFragment) -> None:
        super().__init__(code)
        self.value = float(value)

    def to_json(self) -> Json:
        return {"type": "number", "value": self.value}

    @run_time_error_printer
    def evaluate(self, parent) -> Expression:
        return self


class String(Expression):
    def __init__(self, value: str, code: CodeFragment) -> None:
        super().__init__(code)
        self.value = value[1:-1]

    def to_json(self) -> Json:
        return {"type": "string", "value": self.value}

    @run_time_error_printer
    def evaluate(self, parent) -> Expression:
        return self


class Array(Expression):
    def __init__(self, expressions: Sequence[Expression], code: CodeFragment) -> None:
        super().__init__(code)
        self.value = expressions

    def to_json(self) -> Json:
        return [e.to_json() for e in self.value]

    @run_time_error_printer
    def evaluate(self, parent) -> Expression:
        expressions = [e.evaluate(self) for e in self.value]
        return Array(expressions, code=self.code)


class Dictionary(Expression):
    def __init__(
            self,
            names: Sequence[str],
            expressions: Sequence[Expression],
            code: CodeFragment,
    ) -> None:
        super().__init__(code)
        self.names = names
        self.expressions = expressions

    def to_json(self) -> Json:
        return [
            {"type": "variable_definition", "name": k, "value": v.to_json()}
            for k, v in self.items()
        ]

    def items(self) -> Iterator[Tuple[str, Expression]]:
        return zip(self.names, self.expressions)

    def get(self, name: str) -> Expression:
        try:
            return next(v for k, v in self.items() if k == name)
        except StopIteration:
            if self.parent:
                result = self.parent.get(name)
                if result:
                    return result
            print('Could not find symbol: {}'.format(name))
            raise

    def make_environment(self) -> dict:
        return dict(self.items())

    @run_time_error_printer
    def evaluate(self, parent) -> Expression:
        evaluated = Dictionary(names=[], expressions=[], code=self.code)
        evaluated.parent = parent
        for name, expression in self.items():
            value = expression.evaluate(evaluated)
            evaluated.expressions.append(value)
            evaluated.names.append(name)
        return evaluated


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
    def evaluate(self, parent) -> Expression:
        if self.condition.evaluate(self).value:
            return self.then_expression.evaluate(self)
        else:
            return self.else_expression.evaluate(self)


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
    def evaluate(self, parent) -> Expression:
        return self

    def evaluate_call(self, input: Expression) -> Expression:
        middle_man = Dictionary(
            names=[self.argument_name],
            expressions=[input],
            code=None,
        ).evaluate(self.parent)
        return self.expression.evaluate(middle_man)


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
    def evaluate(self, parent) -> Expression:
        function = parent.get(self.left)
        input = self.right.evaluate(self)
        if isinstance(function, Function):
            return function.evaluate_call(input=input)
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
    def evaluate(self, parent) -> Expression:
        # Lookup in current scope or above
        return parent.get(self.left)


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
    def evaluate(self, parent) -> Expression:
        dictionary = self.right.evaluate(self)
        assert isinstance(dictionary, Dictionary)
        name = self.left
        return dictionary.get(name)
