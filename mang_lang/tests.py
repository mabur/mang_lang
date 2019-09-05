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
    def test_sum(self):
        self.assertEqual(V(8), interpret('sum of (5, 3)'))

    def test_sub(self):
        self.assertEqual(V(2), interpret('difference of (5, 3)'))

    def test_product(self):
        self.assertEqual(V(15), interpret('product of (5, 3)'))

    def test_div(self):
        self.assertEqual(V(2), interpret('division of (6, 3)'))

    def test_if_then_else_true(self):
        self.assertEqual(V(1), interpret('if 1 then 1 else 2'))

    def test_if_then_else_false(self):
        self.assertEqual(V(2), interpret('if 0 then 1 else 2'))

    def test_if_then_else_true2(self):
        self.assertEqual(V(1), interpret('if -1 then 1 else 2'))

    def test_if_then_else_references(self):
        self.assertEqual(V(1), interpret('(x=1,if x then x else x)')[-1])

    def test_size1(self):
        self.assertEqual(V(3), interpret('size of (8,4,6)'))

    def test_size2(self):
        self.assertEqual(V(1), interpret('size of (9)'))

    def test_size3(self):
        self.assertEqual(V(0), interpret('size of ()'))

    def test_size4(self):
        self.assertEqual(V(0), interpret('size of ()'))

    def test_is_empty1(self):
        self.assertEqual(V(1), interpret('(x=(), is_empty of x)')[-1])

    def test_is_empty2(self):
        self.assertEqual(V(0), interpret('(x=(1), is_empty of x)')[-1])

    def test_is_empty3(self):
        self.assertEqual(V(0), interpret('(x=(1,2), is_empty of x)')[-1])

    def test_concat1(self):
        self.assertEqual((V(1), V(2), V(3), V(4)),
                         interpret('concat of ((1,2),(3,4))'))

    def test_concat2(self):
        self.assertEqual((V(1), V(2), V(3), V(4), V(5), V(6)),
                         interpret('concat of ((1,2),(3,4),(5,6))'))

    def test_concat3(self):
        self.assertEqual((V(1), V(2), V(3)),
                         interpret('concat of ((1),(2),(3))'))


class TestRecursion(unittest.TestCase):
    def test_recursion(self):
        code = '(f = from x to if equal of (x, 0) then 1 else product of (x, f of difference of (x, 1)), f of 10)'
        self.assertEqual(V(3628800), interpret(code)[-1])


class TestAll(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('all of (1, 1)'))

    def test1(self):
        self.assertEqual(V(0), interpret('all of (0, 1)'))

    def test2(self):
        self.assertEqual(V(0), interpret('all of (1, 0)'))

    def test3(self):
        self.assertEqual(V(0), interpret('all of (0, 0)'))


class TestAny(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('any of (1, 1)'))

    def test1(self):
        self.assertEqual(V(1), interpret('any of (0, 1)'))

    def test2(self):
        self.assertEqual(V(1), interpret('any of (1, 0)'))

    def test3(self):
        self.assertEqual(V(0), interpret('any of (0, 0)'))


class TestNone(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(0), interpret('none of (1)'))

    def test1(self):
        self.assertEqual(V(1), interpret('none of (0)'))

    def test2(self):
        self.assertEqual(V(1), interpret('none of (0, 0)'))

    def test3(self):
        self.assertEqual(V(0), interpret('none of (0, 1)'))

    def test4(self):
        self.assertEqual(V(0), interpret('none of (1, 1)'))


class TestEqual(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('equal of (1, 1)'))

    def test1(self):
        self.assertEqual(V(0), interpret('equal of (0, 1)'))

    def test2(self):
        self.assertEqual(V(0), interpret('equal of (1, 0)'))

    def test3(self):
        self.assertEqual(V(1), interpret('equal of (0, 0)'))


class TestSumMinMax(unittest.TestCase):
    def test_sum(self):
        self.assertEqual(V(6), interpret('sum of (1,2,3)'))

    def test_min(self):
        self.assertEqual(V(1), interpret('min of (1,2,3)'))

    def test_max(self):
        self.assertEqual(V(3), interpret('max of (1,2,3)'))


class TestExpression(unittest.TestCase):
    def test_composition(self):
        self.assertEqual(V(12), interpret('sum of (difference of (5, 3), product of (2, 5))'))

    def test_white_space(self):
        self.assertEqual(V(12), interpret('  sum of ( difference of (5 , 3), product   of (2,5))'))

    def test_new_line(self):
        self.assertEqual(V(5), interpret('''sum of
        (2,3)'''))


class TestTuple(unittest.TestCase):
    def test_tuple(self):
        self.assertEqual((V(3), V(7)), interpret('(sum of (1, 2), sum of (3,4))'))

    def test_mixed_tuple(self):
        self.assertEqual(
            (V(3), V(3), {'type': 'variable_definition', 'name': 'x', 'value': V(3)}),
            interpret('(3, sum of (1, 2), x = 3)'))


class TestDefinitions(unittest.TestCase):
    def test_definition_result_constant(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'result', 'value': V(5)},
                         interpret('result = 5'))

    def test_definition_result_constant_function_call(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'result', 'value': V(3)},
                         interpret('result = sum of (1, 2)'))

    def test_definition_constant_function_call(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'x', 'value': V(3)},
                         interpret('x = sum of (1, 2)'))

    def test_definitions(self):
        self.assertEqual(({'type': 'variable_definition', 'name': 'x', 'value': V(1)},
                          {'type': 'variable_definition', 'name': 'y', 'value': V(2)}),
                         interpret('(x = 1, y = 2)'))

    def test_tuple_definition_and_function_call(self):
        self.assertEqual(({'type': 'variable_definition', 'name': 'x', 'value': (V(1),V(2))}, V(3)),
                         interpret('(x=(1,2), sum of x)'))


class TestKeywordVariableClashes(unittest.TestCase):
    def test_in(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'input', 'value': V(5)},
                         interpret('input = 5'))

    def test_if(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'iffy', 'value': V(5)},
                         interpret('iffy = 5'))

    def test_all(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'allround', 'value': V(5)},
                         interpret('allround = 5'))

    def test_then(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'thenner', 'value': V(5)},
                         interpret('thenner = 5'))

    def test_else(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'elsewhere', 'value': V(5)},
                         interpret('elsewhere = 5'))

    def test_import(self):
        self.assertEqual({'type': 'variable_definition', 'name': 'important', 'value': V(5)},
                         interpret('important = 5'))


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
            interpret('(x = 5, y = 3, sum of (x, y))'))


class TestIndexing(unittest.TestCase):
    def test_indexing_number(self):
        self.assertEqual(({'type': 'variable_definition', 'name': 'x', 'value': (V(2), V(3))}, V(2)),
                         interpret('(x = (2, 3), 0 of x)'))


class TestDefinitionLookup(unittest.TestCase):
    def test0(self):
        self.assertEqual(
            (
                {'type': 'variable_definition', 'name': 'x', 'value': ({'type': 'variable_definition', 'name': 'y', 'value': V(1)},)},
                V(1)
            ),
            interpret('(x = (y = 1), y of x)'))

    def test1(self):
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
            interpret('(x=(a=1,b=2), b of x)'))

    def test2(self):
        self.assertEqual(V(1), interpret('(x = (y = (z = 1)), z of y of x)')[-1])

    def test3(self):
        self.assertEqual(V(1), interpret('(x = (y = (z = (w=1))), w of z of y of x)')[-1])

    def test4(self):
        self.assertEqual(V(1), interpret('(a=(f=from x to 1), g=f of a, g of 3)')[-1])

    def test5(self):
        self.assertEqual(V(4), interpret('(a=(f = from x to sum of (x,1)), g=f of a, g of 3)')[-1])

    def test6(self):
        self.assertEqual(V(4), interpret('(a=(b=(f=from x to sum of (x,1))), g=f of b of a, g of 3)')[-1])


class TestFunctionDefinition(unittest.TestCase):
    def test_function_definition(self):
        actual = interpret('from x to add x')
        expected = {'type': 'function_definition',
                    'argument_name': 'x'}
        self.assertEqual(expected['type'], actual['type'])
        self.assertEqual(expected['argument_name'], actual['argument_name'])

    def test_constant_function_definition_and_call(self):
        self.assertEqual(V(3), interpret('(f = from x to 3, f of 1)')[-1])

    def test_function_definition_and_call(self):
        self.assertEqual(V(5), interpret('(f = from x to sum of x, f of (2,3))')[-1])


class TestFunctionScope(unittest.TestCase):
    def test_scope1(self):
        self.assertEqual(V(3), interpret('(f = from x where (y = 3) to y, f of 2)')[-1])

    def test_scope2(self):
        self.assertEqual(V(5), interpret('(f = from x where (y = 3) to sum of (x, y), f of 2)')[-1])

    def test_scope3(self):
        self.assertEqual(V(5), interpret('(y = 2, f = from x where (y = 3) to sum of (x, y), f of 2)')[-1])


class TestString(unittest.TestCase):
    def test_string(self):
        self.assertEqual(S("hej"), interpret('"hej"'))

    def test_string_size0(self):
        self.assertEqual(V(0), interpret('size of ""'))

    def test_string_size1(self):
        self.assertEqual(V(1), interpret('size of "f"'))

    def test_string_size2(self):
        self.assertEqual(V(2), interpret('size of "du"'))

    def test_string_concat1(self):
        self.assertEqual(S(""), interpret('concat of ("","")'))

    def test_string_concat2(self):
        self.assertEqual(S("abcd"), interpret('concat of ("ab","cd")'))

    def test_string_concat3(self):
        self.assertEqual(S("ab"), interpret('concat of ("ab","")'))

    def test_string_concat4(self):
        self.assertEqual(S("cd"), interpret('concat of ("","cd")'))

    def test_string_concat5(self):
        self.assertEqual(S("abc"), interpret('concat of ("a","b","c")'))

    def test_string_index0(self):
        self.assertEqual(S("a"), interpret('(x="abc", 0 of x)')[1])

    def test_string_index1(self):
        self.assertEqual(S("b"), interpret('(x="abc", 1 of x)')[1])


class TestImport(unittest.TestCase):
    def test1(self):
        with mock.patch('global_environment._read_text_file', return_value='3'):
            self.assertEqual(V(3), interpret('import of "file_name"'))

    def test2(self):
        with mock.patch('global_environment._read_text_file', return_value='(\nx=3,\ny=4\n)'):
            self.assertEqual(V(4), interpret('(z = import of "file_name", y of z)')[-1])

    def test3(self):
        with mock.patch('global_environment._read_text_file', return_value='(square = from x to product of (x,x))'):
            self.assertEqual(V(9), interpret('(a = import of "file_name", square = square of a, square of 3)')[-1])


class TestTupleComprehension(unittest.TestCase):
    def test1(self):
        actual = interpret('each e for e in ()')
        expected = tuple()
        self.assertEqual(expected, actual)

    def test2(self):
        actual = interpret('each e for e in (2)')
        expected = (V(2),)
        self.assertEqual(expected, actual)

    def test3(self):
        actual = interpret('each e for e in (2, 3)')
        expected = (V(2), V(3))
        self.assertEqual(expected, actual)

    def test4(self):
        actual = interpret('(t = (), each e for e in t)')
        expected = tuple()
        self.assertEqual(expected, actual[-1])

    def test5(self):
        actual = interpret('(t = (2), each e for e in t)')
        expected = (V(2),)
        self.assertEqual(expected, actual[-1])

    def test6(self):
        actual = interpret('(t = (2, 3), each e for e in t)')
        expected = (V(2), V(3))
        self.assertEqual(expected, actual[-1])

    def test7(self):
        actual = interpret('(t = (2, 3), each product of (e, e) for e in t)')
        expected = (V(4), V(9))
        self.assertEqual(expected, actual[-1])

    def test8(self):
        actual = interpret('(f = from x to 1, t = (2, 3), each f of e for e in t)')
        expected = (V(1), V(1))
        self.assertEqual(expected, actual[-1])

    def test9(self):
        actual = interpret('each e for e in (1, 2, 3, 4) if equal of (e, 2)')
        expected = (V(2),)
        self.assertEqual(expected, actual)

    def test10(self):
        actual = interpret('each e for e in (1, 2, 3, 4) if none of (equal of (e, 2))')
        expected = (V(1), V(3), V(4))
        self.assertEqual(expected, actual)


if __name__ == '__main__':
    unittest.main()
