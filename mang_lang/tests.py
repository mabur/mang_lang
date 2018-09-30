import unittest
from interpreter import interpret

class TestBuiltinFunctions(unittest.TestCase):

    def test_add(self):
        self.assertEqual(interpret('add(5, 3)'), [8])

    def test_sub(self):
        self.assertEqual(interpret('sub(5, 3)'), [2])

    def test_mul(self):
        self.assertEqual(interpret('mul(5, 3)'), [15])

    def test_div(self):
        self.assertEqual(interpret('div(6, 3)'), [2])

if __name__ == '__main__':
    unittest.main()
