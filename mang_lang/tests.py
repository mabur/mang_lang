from typing import Any, Mapping
import unittest
from interpreter import interpret


def V(value) -> Mapping[str, Any]:
    return {"type": "number", "value": value}


class TestArray(unittest.TestCase):
    def test_array0(self):
        self.assertEqual((), interpret('()'))

    def test_array1a(self):
        self.assertEqual((V(1),), interpret('(1)'))

    def test_array1b(self):
        self.assertEqual(V(1), interpret('1'))

    def test_array2(self):
        self.assertEqual((V(1), V(2)), interpret('(1,2)'))

    def test_array3(self):
        self.assertEqual((V(1), V(2), V(3)), interpret('(1,2,3)'))


class TestBuiltinFunctions(unittest.TestCase):

    def test_add(self):
        self.assertEqual(V(8), interpret('add(5, 3)'))

    def test_sub(self):
        self.assertEqual(V(2), interpret('sub(5, 3)'))

    def test_mul(self):
        self.assertEqual(V(15), interpret('mul(5, 3)'))

    def test_div(self):
        self.assertEqual(V(2), interpret('div(6, 3)'))

    def test_if_then_else_true(self):
        self.assertEqual(V(1), interpret('if 1 then 1 else 2'))

    def test_if_then_else_false(self):
        self.assertEqual(V(2), interpret('if 0 then 1 else 2'))

    def test_if_then_else_true2(self):
        self.assertEqual(V(1), interpret('if -1 then 1 else 2'))


class TestAnd(unittest.TestCase):

    def test0(self):
        self.assertEqual(V(1), interpret('and(1, 1)'))

    def test1(self):
        self.assertEqual(V(0), interpret('and(0, 1)'))

    def test2(self):
        self.assertEqual(V(0), interpret('and(1, 0)'))

    def test3(self):
        self.assertEqual(V(0), interpret('and(0, 0)'))


class TestOr(unittest.TestCase):

    def test0(self):
        self.assertEqual(V(1), interpret('or(1, 1)'))

    def test1(self):
        self.assertEqual(V(1), interpret('or(0, 1)'))

    def test2(self):
        self.assertEqual(V(1), interpret('or(1, 0)'))

    def test3(self):
        self.assertEqual(V(0), interpret('or(0, 0)'))


class TestNot(unittest.TestCase):

    def test0(self):
        self.assertEqual(V(0), interpret('not(1)'))

    def test1(self):
        self.assertEqual(V(1), interpret('not(0)'))


class TestEqual(unittest.TestCase):

    def test0(self):
        self.assertEqual(V(1), interpret('equal(1, 1)'))

    def test1(self):
        self.assertEqual(V(0), interpret('equal(0, 1)'))

    def test2(self):
        self.assertEqual(V(0), interpret('equal(1, 0)'))

    def test3(self):
        self.assertEqual(V(1), interpret('equal(0, 0)'))


class TestExpression(unittest.TestCase):
    def test_composition(self):
        self.assertEqual(V(12), interpret('add(sub(5, 3), mul(2, 5))'))

    def test_whites_pace(self):
        self.assertEqual(V(12), interpret('  add( sub(5 , 3), mul   (2,5))'))


class TestTuple(unittest.TestCase):
    def test_tuple(self):
        self.assertEqual((V(3), V(7)), interpret('(add(1, 2), add(3,4))'))

    def test_mixed_tuple(self):
        self.assertEqual(
            (V(3), V(3), {'type': 'variable_definition', 'name': 'x', 'value': V(3)}),
            interpret('(3, add(1, 2), x = 3)'))


class TestDefinitions(unittest.TestCase):
    def test_definition_result_constant(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'result', 'value': V(5)},
                         interpret('result = 5'))

    def test_definition_result_constant_function_call(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'result', 'value': V(3)},
                         interpret('result = add(1, 2)'))

    def test_definition_constant_function_call(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'x', 'value': V(3)},
                         interpret('x = add(1, 2)'))

    def test_definitions(self):
        self.assertEqual(({'type': 'variable_definition', 'name': 'x', 'value': V(1)},
                          {'type': 'variable_definition', 'name': 'y', 'value': V(2)}),
                         interpret('(x = 1, y = 2)'))

    def test_tuple_definition_and_function_call(self):
        self.assertEqual(({'type': 'variable_definition', 'name': 'x', 'value': (V(1),V(2))}, V(3)),
                         interpret('(x=(1,2), add(x))'))


class TestIndirection(unittest.TestCase):
    def test_indirection1(self):
        self.assertEqual(({'type': 'variable_definition', 'name': 'x', 'value': V(5)},
                          {'type': 'variable_definition', 'name': 'y', 'value': V(5)}),
                         interpret('(x = 5, y = x)'))

    def test_indirection2(self):
        self.assertEqual((
            {'type': 'variable_definition', 'name': 'a', 'value': V(1)},
            {'type': 'variable_definition', 'name': 'b', 'value': V(1)},
            {'type': 'variable_definition', 'name': 'c', 'value': V(1)}),
            interpret('(a = 1, b = a, c = b)'))

    def test_indirection3(self):
        self.assertEqual((
            {'type': 'variable_definition', 'name': 'x', 'value': V(5)},
            {'type': 'variable_definition', 'name': 'y', 'value': V(3)},
            V(8)),
            interpret('(x = 5, y = 3, add(x, y))'))


class TestIndexing(unittest.TestCase):
    def test_indexing_number(self):
        self.assertEqual(({'type': 'variable_definition', 'name': 'x', 'value': (V(2), V(3))}, V(2)),
                         interpret('(x = (2, 3), x[0])'))

    def test_indexing_constant(self):
        self.assertEqual(
            (
                {'type': 'variable_definition', 'name': 'x', 'value': (V(2), V(3))},
                {'type': 'variable_definition', 'name': 'y', 'value': V(1)},
                V(3)
            ),
            interpret('(x = (2, 3), y = 1, x[y])'))


class TestDefinitionLookup(unittest.TestCase):
    def test_indexing_number0(self):
        self.assertEqual(
            (
                {'type': 'variable_definition', 'name': 'x', 'value': ({'type': 'variable_definition', 'name': 'y', 'value': V(1)},)},
                V(1)
            ),
            interpret('(x = (y = 1), x.y)'))

    def test_indexing_number1(self):
        self.assertEqual(
            (
                {'type': 'variable_definition', 'name': 'x', 'value':
                    (
                        {'type': 'variable_definition', 'name': 'a', 'value': V(1)},
                        {'type': 'variable_definition', 'name': 'b', 'value': V(2)}
                    )
                 },
                V(2)
            ),
            interpret('(x=(a=1,b=2), x.b)'))


class TestFunctionDefinition(unittest.TestCase):
    def test_function_definition(self):
        actual = interpret('f(x)=add(x)')
        expected = {'type': 'function_definition',
                    'function_name': 'f',
                    'argument_name': 'x'}
        self.assertEqual(expected['type'], actual['type'])
        self.assertEqual(expected['function_name'], actual['function_name'])
        self.assertEqual(expected['argument_name'], actual['argument_name'])

    def test_constant_function_definition_and_call(self):
        actual = interpret('(f(x) = 3, f(1))')
        expected = ((None), V(3))
        self.assertEqual(expected[1], actual[1])

    def test_function_definition_and_call(self):
        actual = interpret('(f(x) = add(x), f(2,3))')
        expected = ((None), V(5))
        self.assertEqual(expected[1], actual[1])

if __name__ == '__main__':
    unittest.main()
