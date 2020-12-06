from typing import Any, Mapping
import unittest
from interpreter import interpret


def V(value) -> Mapping[str, Any]:
    return value


def S(value) -> Mapping[str, Any]:
    return value


class TestLists(unittest.TestCase):
    def test_array0(self):
        self.assertEqual([], interpret('[]'))

    def test_array1a(self):
        self.assertEqual([V(1)], interpret('[1]'))

    def test_array1b(self):
        self.assertEqual(V(1), interpret('1'))

    def test_array2(self):
        self.assertEqual([V(1), V(2)], interpret('[1,2]'))

    def test_array3(self):
        self.assertEqual([V(1), V(2), V(3)], interpret('[1,2,3]'))


class TestConditionals(unittest.TestCase):
    def test_if_then_else_true(self):
        self.assertEqual(V(1), interpret('if 1 then 1 else 2'))

    def test_if_then_else_false(self):
        self.assertEqual(V(2), interpret('if 0 then 1 else 2'))

    def test_if_then_else_true2(self):
        self.assertEqual(V(1), interpret('if -1 then 1 else 2'))


class TestRecursion(unittest.TestCase):
    def test_recursion(self):
        code = 'y<{f = from x to if equal [x, 0] then 1 else mul [x, f sub [x, 1]], y=f 10}'
        self.assertEqual(V(3628800), interpret(code))


class TestExpression(unittest.TestCase):
    def test_composition(self):
        self.assertEqual(V(12), interpret('add [sub [5, 3], mul [2, 5]]'))

    def test_white_space(self):
        self.assertEqual(V(12), interpret('  add [ sub [5 , 3], mul   [2,5]]'))

    def test_new_line(self):
        self.assertEqual(V(5), interpret('''add
        [2,3]'''))


class TestTuple(unittest.TestCase):
    def test_tuple(self):
        self.assertEqual([V(3), V(7)], interpret('[add [1, 2], add [3,4]]'))


class TestDefinitions(unittest.TestCase):
    def test_definition_result_constant(self):
        self.assertEqual(V(5), interpret('result<{result = 5}'))

    def test_definition_result_constant_function_call(self):
        self.assertEqual(V(3), interpret('result<{result = add [1, 2]}'))

    def test_definition_constant_function_call(self):
        self.assertEqual(V(3), interpret('x<{x = add [1, 2]}'))

    def test_definitions(self):
        self.assertEqual(V(2), interpret('y<{x = 1, y = 2}'))

    def test_tuple_definition_and_function_call(self):
        self.assertEqual(V(3), interpret('y<{x=[1,2], y=add x}'))


class TestKeywordVariableClashes(unittest.TestCase):
    def test_in(self):
        self.assertEqual(V(5), interpret('input<{input = 5}'))

    def test_if(self):
        self.assertEqual(V(5), interpret('iffy<{iffy = 5}'))

    def test_each(self):
        self.assertEqual(V(5), interpret('eachy<{eachy = 5}'))

    def test_from(self):
        self.assertEqual(V(5), interpret('fromage<{fromage = 5}'))

    def test_all(self):
        self.assertEqual(V(5), interpret('allround<{allround = 5}'))

    def test_then(self):
        self.assertEqual(V(5), interpret('thenner<{thenner = 5}'))

    def test_else(self):
        self.assertEqual(V(5), interpret('elsewhere<{elsewhere = 5}'))

    def test_import(self):
        self.assertEqual(V(5), interpret('important<{important = 5}'))


class TestIndirection(unittest.TestCase):
    def test_indirection1(self):
        self.assertEqual(V(5), interpret('y<{x = 5, y = x}'))

    def test_indirection2(self):
        self.assertEqual(V(1), interpret('c<{a = 1, b = a, c = b}'))

    def test_indirection3(self):
        self.assertEqual(V(8), interpret('z<{x = 5, y = 3, z = add [x, y]}'))


class TestDefinitionLookup(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('y<x<{x = {y = 1}}'))

    def test1(self):
        self.assertEqual(V(2), interpret('b<x<{x={a=1,b=2}}'))

    def test2(self):
        self.assertEqual(V(1), interpret('z<y<x<{x = {y = {z = 1}}}'))

    def test3(self):
        self.assertEqual(V(1), interpret('w<z<y<x<{x = {y = {z = {w=1}}}}'))

    def test4(self):
        self.assertEqual(V(1), interpret('b<{a={f=from x to 1}, g=f<a, b=g 3}'))

    def test5(self):
        self.assertEqual(V(4), interpret('b<{a={f = from x to add [x,1]}, g=f<a, b = g 3}'))

    def test6(self):
        self.assertEqual(V(4), interpret('c<{a={b={f=from x to add [x,1]}}, g=f<b<a, c = g 3}'))

    def test7(self):
        self.assertEqual(V(1), interpret('ABBA<{ABBA = 1}'))

    def test8(self):
        self.assertEqual(V(1), interpret('ABBA<{ABBA = 1, PADDA = 2}'))


class TestFunctionDefinition(unittest.TestCase):
    def test_function_definition(self):
        actual = interpret('from x to add x')
        expected = {'type': 'function_definition',
                    'argument_name': 'x'}
        self.assertEqual(expected['type'], actual['type'])
        self.assertEqual(expected['argument_name'], actual['argument_name'])

    def test_constant_function_definition_and_call(self):
        self.assertEqual(V(3), interpret('y<{f = from x to 3, y=f 1}'))

    def test_function_definition_and_call(self):
        self.assertEqual(V(5), interpret('y<{f = from x to add x, y = f [2,3]}'))


class TestFunctionScope(unittest.TestCase):
    def test_scope1(self):
        self.assertEqual(V(3), interpret('z<{f = from x to y<{y = 3}, z=f 2}'))

    def test_scope2(self):
        self.assertEqual(V(5), interpret('z<{f = from x to result<{y=3,result=add [x, y]}, z = f 2}'))

    def test_scope3(self):
        self.assertEqual(V(5), interpret('z<{y = 2, f = from x to result<{y=3, result=add [x, y]}, z = f 2}'))


class TestClosures(unittest.TestCase):
    def test_closure(self):
        self.assertEqual(V(1), interpret("x<{a={b=1,f=from x to b},b=2,f=f<a,x=f[]}"))


class TestImport(unittest.TestCase):
    def test1(self):
        self.assertEqual(V(3), interpret('import "tests/test_data/test1.ml"'))

    def test2(self):
        self.assertEqual(V(4), interpret('y<import "tests/test_data/test2.ml"'))

    def test3(self):
        self.assertEqual(V(9), interpret('b<{a = import "tests/test_data/test3.ml", square = square<a, b=square 3}'))


class TestErrorMessages(unittest.TestCase):
    def test_syntax_error(self):
        with self.assertRaises(TypeError):
            interpret('{\na=0,\nb=,\nc=2\n}')

    def test_run_time_error(self):
        with self.assertRaises(AttributeError):
            interpret('{\nw=0,\nx=a\n{b=1,\nc=1},\ny=1\n}')


if __name__ == '__main__':
    unittest.main()
