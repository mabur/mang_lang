from typing import Any, Mapping
import unittest
from unittest import mock
from interpreter import interpret


def V(value) -> Mapping[str, Any]:
    return {"type": "number", "value": value}


def S(value) -> Mapping[str, Any]:
    return {"type": "string", "value": value}


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

    def test_if_then_else_references(self):
        self.assertEqual(V(1), interpret('(x=1,if x then x else x)')[1])

    def test_size1(self):
        self.assertEqual(V(3), interpret('size(8,4,6)'))

    def test_size2(self):
        self.assertEqual(V(1), interpret('size((9))'))

    def test_size3(self):
        self.assertEqual(V(0), interpret('size(())'))

    def test_size4(self):
        self.assertEqual(V(0), interpret('size()'))

    def test_concat1(self):
        self.assertEqual((V(1), V(2), V(3), V(4)),
                         interpret('concat((1,2),(3,4))'))

    def test_concat2(self):
        self.assertEqual((V(1), V(2), V(3), V(4), V(5), V(6)),
                         interpret('concat((1,2),(3,4),(5,6))'))

    def test_concat3(self):
        self.assertEqual((V(1), V(2), V(3)),
                         interpret('concat((1),(2),(3))'))


class TestRecursion(unittest.TestCase):
    def test_recursion(self):
        code = '(f(x) = if equal(x, 0) then 1 else mul(x, f(sub(x, 1))), f(10))'
        self.assertEqual(V(3628800), interpret(code)[1])


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


class TestVariableScope(unittest.TestCase):
    def test_scope1(self):
        actual = interpret('(x = {y = 3} = y, x)')
        expected = ((None), V(3))
        self.assertEqual(expected[1], actual[1])

    def test_scope2(self):
        actual = interpret('(x = {y = 3, z = 4} = add(y, z), x)')
        expected = ((None), V(7))
        self.assertEqual(expected[1], actual[1])

    def test_scope3(self):
        actual = interpret('(z = 4, x = {y = 3} = add(y, z), x)')
        expected = ((None), (None), V(7))
        self.assertEqual(expected[2], actual[2])

    def test_scope4(self):
        actual = interpret('(z = 4, x = {} = z, x)')
        expected = ((None), (None), V(4))
        self.assertEqual(expected[2], actual[2])

    def test_scope5(self):
        actual = interpret('(z = 4, x = {z = 3} = z, x)')
        expected = ((None), (None), V(3))
        self.assertEqual(expected[2], actual[2])


class TestFunctionScope(unittest.TestCase):
    def test_scope1(self):
        actual = interpret('(f(x) = {y = 3} = y, f(2))')
        expected = ((None), V(3))
        self.assertEqual(expected[1], actual[1])

    def test_scope2(self):
        actual = interpret('(f(x) = {y = 3} = add(x, y), f(2))')
        expected = ((None), V(5))
        self.assertEqual(expected[1], actual[1])

    def test_scope3(self):
        actual = interpret('(y = 2, f(x) = {y = 3} = add(x, y), f(2))')
        expected = ((None), (None), V(5))
        self.assertEqual(expected[2], actual[2])


class TestString(unittest.TestCase):
    def test_string(self):
        self.assertEqual(S("hej"), interpret('"hej"'))

    def test_string_size0(self):
        self.assertEqual(V(0), interpret('size("")'))

    def test_string_size1(self):
        self.assertEqual(V(1), interpret('size("f")'))

    def test_string_size2(self):
        self.assertEqual(V(2), interpret('size("du")'))

    def test_string_concat1(self):
        self.assertEqual(S(""), interpret('concat("","")'))

    def test_string_concat2(self):
        self.assertEqual(S("abcd"), interpret('concat("ab","cd")'))

    def test_string_concat3(self):
        self.assertEqual(S("ab"), interpret('concat("ab","")'))

    def test_string_concat4(self):
        self.assertEqual(S("cd"), interpret('concat("","cd")'))

    def test_string_concat5(self):
        self.assertEqual(S("abc"), interpret('concat("a","b","c")'))

    def test_string_index0(self):
        self.assertEqual(S("a"), interpret('(x="abc",x[0])')[1])

    def test_string_index1(self):
        self.assertEqual(S("b"), interpret('(x="abc",x[1])')[1])


class TestImport(unittest.TestCase):
    def test1(self):
        with mock.patch('parsing.read_text_file', return_value='3'):
            self.assertEqual(V(3), interpret('import("file_name")'))

    def test2(self):
        with mock.patch('parsing.read_text_file', return_value='(\nx=3,\ny=4\n)'):
            self.assertEqual(V(4), interpret('(z = import("file_name"), z.y)')[1])


class TestTupleComprehension(unittest.TestCase):
    def test1(self):
        actual = interpret('(t = (2, 3), all mul(e, e) for e in t)')
        expected = (V(4), V(9))
        self.assertEqual(expected, actual[1])


if __name__ == '__main__':
    unittest.main()
