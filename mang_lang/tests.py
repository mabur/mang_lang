import unittest
from interpreter import interpret


class TestArray(unittest.TestCase):
    def test_array0(self):
        self.assertEqual((), interpret('()'))

    def test_array1a(self):
        self.assertEqual((1,), interpret('(1)'))

    def test_array1b(self):
        self.assertEqual(1, interpret('1'))

    def test_array2(self):
        self.assertEqual((1, 2), interpret('(1,2)'))

    def test_array3(self):
        self.assertEqual((1, 2, 3), interpret('(1,2,3)'))


class TestBuiltinFunctions(unittest.TestCase):

    def test_add(self):
        self.assertEqual(8, interpret('add(5, 3)'))

    def test_sub(self):
        self.assertEqual(2, interpret('sub(5, 3)'))

    def test_mul(self):
        self.assertEqual(15, interpret('mul(5, 3)'))

    def test_div(self):
        self.assertEqual(2, interpret('div(6, 3)'))


class TestExpression(unittest.TestCase):
    def test_composition(self):
        self.assertEqual(12, interpret('add(sub(5, 3), mul(2, 5))'))

    def test_whites_pace(self):
        self.assertEqual(12, interpret('  add( sub(5 , 3), mul   (2,5))'))


class TestTuple(unittest.TestCase):
    def test_tuple(self):
        self.assertEqual((3, 7), interpret('(add(1, 2), add(3,4))'))


class TestDefinitions(unittest.TestCase):
    def test_definition_result_constant(self):
        self.assertEqual(('result', 5), interpret('result = 5'))

    def test_definition_result_constant_function_call(self):
        self.assertEqual(('result', 3), interpret('result = add(1, 2)'))

    def test_definition_constant_function_call(self):
        self.assertEqual(('x', 3), interpret('x = add(1, 2)'))

    def test_definitions(self):
        self.assertEqual((('x', 1), ('y', 2)), interpret('(x = 1, y = 2)'))

if __name__ == '__main__':
    unittest.main()
