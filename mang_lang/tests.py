from typing import Any, Mapping
import unittest
from interpreter import interpret


def V(value) -> Mapping[str, Any]:
    return {"type": "number", "value": value}


def S(value) -> Mapping[str, Any]:
    return {"type": "string", "value": value}


class TestArray(unittest.TestCase):
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


class TestBuiltinFunctions(unittest.TestCase):
    def test_sum(self):
        self.assertEqual(V(8), interpret('sum of [5, 3]'))

    def test_sub(self):
        self.assertEqual(V(2), interpret('difference of [5, 3]'))

    def test_product(self):
        self.assertEqual(V(15), interpret('product of [5, 3]'))

    def test_div(self):
        self.assertEqual(V(2), interpret('division of [6, 3]'))

    def test_if_then_else_true(self):
        self.assertEqual(V(1), interpret('if 1 then 1 else 2'))

    def test_if_then_else_false(self):
        self.assertEqual(V(2), interpret('if 0 then 1 else 2'))

    def test_if_then_else_true2(self):
        self.assertEqual(V(1), interpret('if -1 then 1 else 2'))

    def test_if_then_else_references(self):
        self.assertEqual(V(1), interpret('y of {x=1,y=if x then x else x}'))

    def test_size1(self):
        self.assertEqual(V(3), interpret('size of [8,4,6]'))

    def test_size2(self):
        self.assertEqual(V(1), interpret('size of [9]'))

    def test_size3(self):
        self.assertEqual(V(0), interpret('size of []'))

    def test_size4(self):
        self.assertEqual(V(0), interpret('size of []'))

    def test_is_empty1(self):
        self.assertEqual(V(1), interpret('is_empty of []'))

    def test_is_empty2(self):
        self.assertEqual(V(0), interpret('is_empty of [1]'))

    def test_is_empty3(self):
        self.assertEqual(V(0), interpret('is_empty of [1,2]'))

    def test_concat1(self):
        self.assertEqual([V(1), V(2), V(3), V(4)],
                         interpret('concat of [[1,2],[3,4]]'))

    def test_concat2(self):
        self.assertEqual([V(1), V(2), V(3), V(4), V(5), V(6)],
                         interpret('concat of [[1,2],[3,4],[5,6]]'))

    def test_concat3(self):
        self.assertEqual([V(1), V(2), V(3)],
                         interpret('concat of [[1],[2],[3]]'))

    def test_first(self):
        self.assertEqual(V(6), interpret('first of [6, 4, 8]'))

    def test_last(self):
        self.assertEqual(V(8), interpret('last of [6, 4, 8]'))

    def test_first_part0(self):
        self.assertEqual(V(2), interpret('size of first_part of [6, 4, 8]'))

    def test_first_part1(self):
        self.assertEqual(V(6), interpret('first of first_part of [6, 4, 8]'))

    def test_first_part2(self):
        self.assertEqual(V(4), interpret('last of first_part of [6, 4, 8]'))

    def test_first_part3(self):
        self.assertEqual(V(0), interpret('size of first_part of [6]'))

    def test_first_part4(self):
        self.assertEqual(V(0), interpret('size of first_part of []'))

    def test_last_part0(self):
        self.assertEqual(V(2), interpret('size of last_part of [6, 4, 8]'))

    def test_last_part1(self):
        self.assertEqual(V(4), interpret('first of last_part of [6, 4, 8]'))

    def test_last_part2(self):
        self.assertEqual(V(8), interpret('last of last_part of [6, 4, 8]'))

    def test_last_part3(self):
        self.assertEqual(V(0), interpret('size of last_part of [6]'))

    def test_last_part4(self):
        self.assertEqual(V(0), interpret('size of last_part of []'))


class TestRecursion(unittest.TestCase):
    def test_recursion(self):
        code = 'y of {f = from x to if check_equality of [x, 0] then 1 else product of [x, f of difference of [x, 1]], y=f of 10}'
        self.assertEqual(V(3628800), interpret(code))


class TestAll(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('all of [1, 1]'))

    def test1(self):
        self.assertEqual(V(0), interpret('all of [0, 1]'))

    def test2(self):
        self.assertEqual(V(0), interpret('all of [1, 0]'))

    def test3(self):
        self.assertEqual(V(0), interpret('all of [0, 0]'))


class TestAny(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('any of [1, 1]'))

    def test1(self):
        self.assertEqual(V(1), interpret('any of [0, 1]'))

    def test2(self):
        self.assertEqual(V(1), interpret('any of [1, 0]'))

    def test3(self):
        self.assertEqual(V(0), interpret('any of [0, 0]'))


class TestNone(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(0), interpret('none of [1]'))

    def test1(self):
        self.assertEqual(V(1), interpret('none of [0]'))

    def test2(self):
        self.assertEqual(V(1), interpret('none of [0, 0]'))

    def test3(self):
        self.assertEqual(V(0), interpret('none of [0, 1]'))

    def test4(self):
        self.assertEqual(V(0), interpret('none of [1, 1]'))


class TestEqual(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('check_equality of [1, 1]'))

    def test1(self):
        self.assertEqual(V(0), interpret('check_equality of [0, 1]'))

    def test2(self):
        self.assertEqual(V(0), interpret('check_equality of [1, 0]'))

    def test3(self):
        self.assertEqual(V(1), interpret('check_equality of [0, 0]'))


class TestInequality(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(0), interpret('check_inequality of [1, 1]'))

    def test1(self):
        self.assertEqual(V(1), interpret('check_inequality of [0, 1]'))

    def test2(self):
        self.assertEqual(V(1), interpret('check_inequality of [1, 0]'))

    def test3(self):
        self.assertEqual(V(0), interpret('check_inequality of [0, 0]'))


class TestSumMinMax(unittest.TestCase):
    def test_sum(self):
        self.assertEqual(V(6), interpret('sum of [1,2,3]'))

    def test_min(self):
        self.assertEqual(V(1), interpret('min of [1,2,3]'))

    def test_max(self):
        self.assertEqual(V(3), interpret('max of [1,2,3]'))


class TestExpression(unittest.TestCase):
    def test_composition(self):
        self.assertEqual(V(12), interpret('sum of [difference of [5, 3], product of [2, 5]]'))

    def test_white_space(self):
        self.assertEqual(V(12), interpret('  sum of [ difference of [5 , 3], product   of [2,5]]'))

    def test_new_line(self):
        self.assertEqual(V(5), interpret('''sum of
        [2,3]'''))


class TestTuple(unittest.TestCase):
    def test_tuple(self):
        self.assertEqual([V(3), V(7)], interpret('[sum of [1, 2], sum of [3,4]]'))


class TestDefinitions(unittest.TestCase):
    def test_definition_result_constant(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'result', 'value': V(5)}],
                         interpret('{result = 5}'))

    def test_definition_result_constant_function_call(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'result', 'value': V(3)}],
                         interpret('{result = sum of [1, 2]}'))

    def test_definition_constant_function_call(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'x', 'value': V(3)}],
                         interpret('{x = sum of [1, 2]}'))

    def test_definitions(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'x', 'value': V(1)},
                          {'type': 'variable_definition', 'name': 'y', 'value': V(2)}],
                         interpret('{x = 1, y = 2}'))

    def test_tuple_definition_and_function_call(self):
        self.assertEqual(V(3), interpret('y of {x=[1,2], y=sum of x}'))


class TestKeywordVariableClashes(unittest.TestCase):
    def test_in(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'input', 'value': V(5)}],
                         interpret('{input = 5}'))

    def test_in2(self):
        self.assertEqual(V(5), interpret('input of {input = 5}'))

    def test_if(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'iffy', 'value': V(5)}],
                         interpret('{iffy = 5}'))

    def test_if2(self):
        self.assertEqual(V(5), interpret('iffy of {iffy = 5}'))

    def test_each(self):
        self.assertEqual(V(5), interpret('eachy of {eachy = 5}'))

    def test_from(self):
        self.assertEqual(V(5), interpret('fromage of {fromage = 5}'))

    def test_all(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'allround', 'value': V(5)}],
                         interpret('{allround = 5}'))

    def test_then(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'thenner', 'value': V(5)}],
                         interpret('{thenner = 5}'))

    def test_then2(self):
        self.assertEqual(V(5), interpret('thenner of {thenner = 5}'))

    def test_else(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'elsewhere', 'value': V(5)}],
                         interpret('{elsewhere = 5}'))

    def test_else2(self):
        self.assertEqual(V(5), interpret('elsewhere of {elsewhere = 5}'))

    def test_import(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'important', 'value': V(5)}],
                         interpret('{important = 5}'))

    def test_import2(self):
        self.assertEqual(V(5), interpret('important of {important = 5}'))


class TestIndirection(unittest.TestCase):
    def test_indirection1(self):
        self.assertEqual([{'type': 'variable_definition', 'name': 'x', 'value': V(5)},
                          {'type': 'variable_definition', 'name': 'y', 'value': V(5)}],
                         interpret('{x = 5, y = x}'))

    def test_indirection2(self):
        self.assertEqual([
            {'type': 'variable_definition', 'name': 'a', 'value': V(1)},
            {'type': 'variable_definition', 'name': 'b', 'value': V(1)},
            {'type': 'variable_definition', 'name': 'c', 'value': V(1)}],
            interpret('{a = 1, b = a, c = b}'))

    def test_indirection3(self):
        self.assertEqual([
            {'type': 'variable_definition', 'name': 'x', 'value': V(5)},
            {'type': 'variable_definition', 'name': 'y', 'value': V(3)},
            {'type': 'variable_definition', 'name': 'z', 'value': V(8)}],
            interpret('{x = 5, y = 3, z = sum of [x, y]}'))


class TestDefinitionLookup(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('y of x of {x = {y = 1}}'))

    def test1(self):
        self.assertEqual(V(2), interpret('b of x of {x={a=1,b=2}}'))

    def test2(self):
        self.assertEqual(V(1), interpret('z of y of x of {x = {y = {z = 1}}}'))

    def test3(self):
        self.assertEqual(V(1), interpret('w of z of y of x of {x = {y = {z = {w=1}}}}'))

    def test4(self):
        self.assertEqual(V(1), interpret('b of {a={f=from x to 1}, g=f of a, b=g of 3}'))

    def test5(self):
        self.assertEqual(V(4), interpret('b of {a={f = from x to sum of [x,1]}, g=f of a, b = g of 3}'))

    def test6(self):
        self.assertEqual(V(4), interpret('c of {a={b={f=from x to sum of [x,1]}}, g=f of b of a, c = g of 3}'))

    def test7(self):
        self.assertEqual(V(1), interpret('ABBA of {ABBA = 1}'))


class TestFunctionDefinition(unittest.TestCase):
    def test_function_definition(self):
        actual = interpret('from x to add x')
        expected = {'type': 'function_definition',
                    'argument_name': 'x'}
        self.assertEqual(expected['type'], actual['type'])
        self.assertEqual(expected['argument_name'], actual['argument_name'])

    def test_constant_function_definition_and_call(self):
        self.assertEqual(V(3), interpret('y of {f = from x to 3, y=f of 1}'))

    def test_function_definition_and_call(self):
        self.assertEqual(V(5), interpret('y of {f = from x to sum of x, y = f of [2,3]}'))


class TestFunctionScope(unittest.TestCase):
    def test_scope1(self):
        self.assertEqual(V(3), interpret('z of {f = from x to y of {y = 3}, z=f of 2}'))

    def test_scope2(self):
        self.assertEqual(V(5), interpret('z of {f = from x to result of {y=3,result=sum of [x, y]}, z = f of 2}'))

    def test_scope3(self):
        self.assertEqual(V(5), interpret('z of {y = 2, f = from x to result of {y=3, result=sum of [x, y]}, z = f of 2}'))


class TestString(unittest.TestCase):
    def test_string1(self):
        self.assertEqual(S("hej"), interpret('"hej"'))

    def test_string2(self):
        self.assertEqual(S("home\dir/image.png"), interpret('"home\dir/image.png"'))

    def test_string_size0(self):
        self.assertEqual(V(0), interpret('size of ""'))

    def test_string_size1(self):
        self.assertEqual(V(1), interpret('size of "f"'))

    def test_string_size2(self):
        self.assertEqual(V(2), interpret('size of "du"'))

    def test_string_concat1(self):
        self.assertEqual(S(""), interpret('concat of ["",""]'))

    def test_string_concat2(self):
        self.assertEqual(S("abcd"), interpret('concat of ["ab","cd"]'))

    def test_string_concat3(self):
        self.assertEqual(S("ab"), interpret('concat of ["ab",""]'))

    def test_string_concat4(self):
        self.assertEqual(S("cd"), interpret('concat of ["","cd"]'))

    def test_string_concat5(self):
        self.assertEqual(S("abc"), interpret('concat of ["a","b","c"]'))

    def test_string_index0(self):
        self.assertEqual(S("a"), interpret('y of {x="abc", y=first of x}'))

    def test_string_index1(self):
        self.assertEqual(S("c"), interpret('y of {x="abc", y=last of x}'))


class TestImport(unittest.TestCase):
    def test1(self):
        self.assertEqual(V(3), interpret('import of "test_data/test1.ml"'))

    def test2(self):
        self.assertEqual(V(4), interpret('y of import of "test_data/test2.ml"'))

    def test3(self):
        self.assertEqual(V(9), interpret('b of {a = import of "test_data/test3.ml", square = square of a, b=square of 3}'))


class TestTupleComprehension(unittest.TestCase):
    def test1(self):
        actual = interpret('each e for e in []')
        expected = []
        self.assertEqual(expected, actual)

    def test2(self):
        actual = interpret('each e for e in [2]')
        expected = [V(2)]
        self.assertEqual(expected, actual)

    def test3(self):
        actual = interpret('each e for e in [2, 3]')
        expected = [V(2), V(3)]
        self.assertEqual(expected, actual)

    def test4(self):
        actual = interpret('x of {t = [], x = each e for e in t}')
        expected = []
        self.assertEqual(expected, actual)

    def test5(self):
        self.assertEqual([V(2)], interpret('x of {t = [2], x = each e for e in t}'))

    def test6(self):
        self.assertEqual([V(2), V(3)], interpret('x of {t = [2, 3], x = each e for e in t}'))

    def test7(self):
        self.assertEqual([V(4), V(9)], interpret('x of {t = [2, 3], x = each product of [e, e] for e in t}'))

    def test8(self):
        self.assertEqual([V(1), V(1)], interpret('x of {f = from x to 1, t = [2, 3], x = each f of e for e in t}'))

    def test9(self):
        actual = interpret('each e for e in [1, 2, 3, 4] if check_equality of [e, 2]')
        expected = [V(2)]
        self.assertEqual(expected, actual)

    def test10(self):
        actual = interpret('each e for e in [1, 2, 3, 4] if none of [check_equality of [e, 2]]')
        expected = [V(1), V(3), V(4)]
        self.assertEqual(expected, actual)

    def test11(self):
        actual = interpret('each 9 for e in [1, 2, 3]')
        expected = [V(9), V(9), V(9)]
        self.assertEqual(expected, actual)


class TestStandardLibrary(unittest.TestCase):
    def test_find0(self):
        self.assertEqual([[], []], interpret('find of {list=[], query=1}'))

    def test_find1(self):
        self.assertEqual([[], [V(1), V(2), V(3)]], interpret('find of {list=[1,2,3], query=1}'))

    @unittest.skip
    def test_find2(self):
        self.assertEqual([[V(1)], [V(2), V(3)]], interpret('find of {list=[1], query=2}'))


class TestErrorMessages(unittest.TestCase):
    def test_syntax_error(self):
        with self.assertRaises(TypeError):
            interpret('{\na=0,\nb=,\nc=2\n}')

    def test_run_time_error(self):
        with self.assertRaises(AttributeError):
            interpret('{\nw=0,\nx=a of\n{b=1,\nc=1},\ny=1\n}')

if __name__ == '__main__':
    unittest.main()
