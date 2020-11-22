import traceback
from typing import (
    Any, Callable, Iterator, Mapping, Optional, Sequence, Tuple,
    Union
)

from error_handling import CodeFragment, print_error_description

Json = Union[float, str, Mapping[str, Any], Sequence]


class AlreadyRegisteredException(Exception):
    pass


def print_evaluation_tree(expression):
    node = expression
    while node:
        print(node if isinstance(node, dict) else node.to_json())
        node = None if isinstance(node, dict) else node.parent


class Expression:
    def __init__(self, code: CodeFragment, parent: "Expression"):
        self.code = code
        self.parent = parent

    def to_json(self) -> Json:
        raise NotImplemented

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> "Expression":
        raise NotImplemented

    def get(self, name: str) -> Optional["Expression"]:
        return self.parent.get(name)

    def evaluate(self, parent: Mapping[str, "Expression"]) -> "Expression":
        self.parent = parent
        try:
            return self.inner_evaluate(parent)
        except AlreadyRegisteredException:
            pass
        except:
            traceback.print_exc()
            print('Run time error when evaluating {}:'.format(self.__class__.__name__))
            print_error_description(error_label='RUN TIME ERROR', code=self.code)
            print_evaluation_tree(self)
            raise AlreadyRegisteredException


class Number(Expression):
    def __init__(self, value: str, code: CodeFragment, parent: Expression=None) -> None:
        super().__init__(code, parent)
        self.value = float(value)

    def to_json(self) -> Json:
        return {"type": "number", "value": self.value}

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> "Expression":
        return self


class String(Expression):
    def __init__(self, value: str, code: CodeFragment, parent: Expression=None) -> None:
        super().__init__(code, parent)
        self.value = value[1:-1]

    def to_json(self) -> Json:
        return {"type": "string", "value": self.value}

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> "Expression":
        return self


class Array(Expression):
    def __init__(self, expressions: Sequence[Expression], code: CodeFragment, parent: Expression=None) -> None:
        super().__init__(code, parent)
        self.value = expressions

    def to_json(self) -> Json:
        return [e.to_json() for e in self.value]

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> Expression:
        expressions = [e.evaluate(parent) for e in self.value]
        return Array(expressions, code=self.code, parent=parent)


class Dictionary(Expression):
    def __init__(self, code: CodeFragment, parent: Expression=None) -> None:
        super().__init__(code, parent)
        self.names = []
        self.expressions = []

    def to_json(self) -> Json:
        return [
            {"type": "variable_definition", "name": k, "value": v.to_json()}
            for k, v in self.items()
        ]

    def items(self) -> Iterator[Tuple[str, Expression]]:
        return zip(self.names, self.expressions)

    def get(self, name: str) -> Expression:
        result = next((v for k, v in self.items() if k == name), None)
        if result:
            return result
        result = self.parent.get(name)
        if result:
            return result
        print('Could not find symbol: {}'.format(name))
        raise KeyError

    def append(self, name, expression):
        self.names.append(name)
        self.expressions.append(expression)

    def make_environment(self) -> dict:
        return dict(self.items())

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> Expression:
        evaluated = Dictionary(code=self.code, parent=parent)
        for name, expression in self.items():
            expression = expression.evaluate(evaluated)
            evaluated.append(name=name, expression=expression)
        return evaluated


class Conditional(Expression):
    def __init__(
            self,
            condition: Expression,
            then_expression: Expression,
            else_expression: Expression,
            code: CodeFragment,
            parent: Expression = None,
    ) -> None:
        super().__init__(code, parent)
        self.condition = condition
        self.then_expression = then_expression
        self.else_expression = else_expression

    def to_json(self) -> Json:
        return {"type": "conditional",
                "condition": self.condition.to_json(),
                "then_expression": self.then_expression.to_json(),
                "else_expression": self.else_expression.to_json()}

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> Expression:
        if self.condition.evaluate(parent).value:
            return self.then_expression.evaluate(parent)
        else:
            return self.else_expression.evaluate(parent)


class Function(Expression):
    def __init__(
            self,
            argument_name: str,
            expression: Expression,
            code: CodeFragment,
            parent: Expression = None,
    ) -> None:
        super().__init__(code, parent)
        self.argument_name = argument_name
        self.expression = expression

    def to_json(self) -> Json:
        return {"type": "function_definition",
                "argument_name": self.argument_name,
                "expression": self.expression.to_json()}

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> "Expression":
        return self

    def evaluate_call(self, input: Expression) -> Expression:
        middle_man = Dictionary(code=self.code, parent=self.parent)
        middle_man.append(name=self.argument_name, expression=input)
        return self.expression.evaluate(middle_man)


class LookupFunction(Expression):
    def __init__(
            self,
            name: str,
            input: Expression,
            code: CodeFragment,
            parent: Expression = None,
    ) -> None:
        super().__init__(code, parent)
        self.name = name
        self.input = input

    def to_json(self) -> Json:
        return {"type": "lookup",
                "left": self.name,
                "right": self.input.to_json() if self.input is not None else ''}

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> Expression:
        function = parent.get(self.name)
        input = self.input.evaluate(parent)
        if isinstance(function, Function):
            return function.evaluate_call(input=input)
        if isinstance(function, Callable):
            return function(input, self.code)
        raise TypeError


class LookupSymbol(Expression):
    def __init__(self, name: str, code: CodeFragment, parent: Expression=None) -> None:
        super().__init__(code, parent)
        self.name = name

    def to_json(self) -> Json:
        return {"type": "lookup", "left": self.name}

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> Expression:
        result = parent.get(self.name)
        assert result,\
            "Could not find symbol {} in current scope or above".format(self.name)
        return result


class LookupChild(Expression):
    def __init__(self, name: str, child: Expression, code: CodeFragment, parent: Expression=None) -> None:
        super().__init__(code, parent)
        self.name = name
        self.child = child

    def to_json(self) -> Json:
        return {"type": "lookup",
                "name": self.name,
                "child": self.child.to_json() if self.child is not None else ''}

    def inner_evaluate(self, parent: Mapping[str, "Expression"]) -> Expression:
        child = self.child.evaluate(parent)
        assert isinstance(child, Dictionary)
        result = child.get(self.name)
        assert result, "Could not find symbol {} in child scope".format(self.name)
        return result
