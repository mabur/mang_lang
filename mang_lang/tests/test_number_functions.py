import unittest
from interpreter import interpret


class TestArithmetic(unittest.TestCase):
    def test_add(self):
        self.assertEqual(8, interpret('add [5, 3]'))

    def test_sub(self):
        self.assertEqual(2, interpret('sub [5, 3]'))

    def test_mul(self):
        self.assertEqual(15, interpret('mul [5, 3]'))

    def test_div(self):
        self.assertEqual(2, interpret('div [6, 3]'))


class TestIncreasing(unittest.TestCase):
    def test0(self):
        self.assertEqual(0, interpret('increasing [0, 0]'))

    def test1(self):
        self.assertEqual(1, interpret('increasing [0, 1]'))

    def test2(self):
        self.assertEqual(0, interpret('increasing [1, 1]'))

    def test3(self):
        self.assertEqual(0, interpret('increasing [1, 0]'))


class TestWeaklyIncreasing(unittest.TestCase):
    def test0(self):
        self.assertEqual(1, interpret('weakly_increasing [0, 0]'))

    def test1(self):
        self.assertEqual(1, interpret('weakly_increasing [0, 1]'))

    def test2(self):
        self.assertEqual(1, interpret('weakly_increasing [1, 1]'))

    def test3(self):
        self.assertEqual(0, interpret('weakly_increasing [1, 0]'))


class TestDecreasing(unittest.TestCase):
    def test0(self):
        self.assertEqual(0, interpret('decreasing [0, 0]'))

    def test1(self):
        self.assertEqual(0, interpret('decreasing [0, 1]'))

    def test2(self):
        self.assertEqual(0, interpret('decreasing [1, 1]'))

    def test3(self):
        self.assertEqual(1, interpret('decreasing [1, 0]'))


class TestWeaklyDecreasing(unittest.TestCase):
    def test0(self):
        self.assertEqual(1, interpret('weakly_decreasing [0, 0]'))

    def test1(self):
        self.assertEqual(0, interpret('weakly_decreasing [0, 1]'))

    def test2(self):
        self.assertEqual(1, interpret('weakly_decreasing [1, 1]'))

    def test3(self):
        self.assertEqual(1, interpret('weakly_decreasing [1, 0]'))


class TestaddMinMax(unittest.TestCase):
    def test_add(self):
        self.assertEqual(6, interpret('add [1,2,3]'))

    def test_min(self):
        self.assertEqual(1, interpret('min [1,2,3]'))

    def test_max(self):
        self.assertEqual(3, interpret('max [1,2,3]'))


class TestSqrt(unittest.TestCase):
    def test4(self):
        self.assertEqual(2, interpret('sqrt 4'))

    def test10(self):
        self.assertEqual(10, interpret('sqrt 100'))


class TestAbs(unittest.TestCase):
    def test0(self):
        self.assertEqual(1, interpret('abs -1'))

    def test1(self):
        self.assertEqual(0, interpret('abs 0'))

    def test2(self):
        self.assertEqual(1, interpret('abs 1'))



if __name__ == '__main__':
    unittest.main()
