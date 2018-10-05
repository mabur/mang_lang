import unittest
from interpreter import interpret


class TestArray(unittest.TestCase):
    def test_array0(self):
        self.assertEqual(interpret('()'), [])

    def test_array1a(self):
        self.assertEqual(interpret('(1)'), [1])

    def test_array1b(self):
        self.assertEqual(interpret('1'), [1])

    def test_array2(self):
        self.assertEqual(interpret('(1,2)'), [1, 2])

    def test_array3(self):
        self.assertEqual(interpret('(1,2,3)'), [1, 2, 3])


class TestBuiltinFunctions(unittest.TestCase):

    def test_add(self):
        self.assertEqual(interpret('add(5, 3)'), [8])

    def test_sub(self):
        self.assertEqual(interpret('sub(5, 3)'), [2])

    def test_mul(self):
        self.assertEqual(interpret('mul(5, 3)'), [15])

    def test_div(self):
        self.assertEqual(interpret('div(6, 3)'), [2])


class TestExpression(unittest.TestCase):
    def test_composition(self):
        self.assertEqual(interpret('add(sub(5, 3), mul(2, 5))'), [12])

    def test_whites_pace(self):
        self.assertEqual(interpret('  add( sub(5 , 3), mul   (2,5))'), [12])


class TestDefinitions(unittest.TestCase):
    def test_definition_result_constant(self):
        self.assertEqual(interpret('result = 5'), [5])

    def test_definition_result_constant_function_call(self):
        self.assertEqual(interpret('result = add(1, 2)'), [3])

    def test_definition_constant_function_call(self):
        self.assertEqual(interpret('x = add(1, 2)'), [])

if __name__ == '__main__':
    unittest.main()
