import unittest
from interpreter import interpret


class TestAll(unittest.TestCase):
    def test0(self):
        self.assertEqual(1, interpret('all [1, 1]'))

    def test1(self):
        self.assertEqual(0, interpret('all [0, 1]'))

    def test2(self):
        self.assertEqual(0, interpret('all [1, 0]'))

    def test3(self):
        self.assertEqual(0, interpret('all [0, 0]'))


class TestAny(unittest.TestCase):
    def test0(self):
        self.assertEqual(1, interpret('any [1, 1]'))

    def test1(self):
        self.assertEqual(1, interpret('any [0, 1]'))

    def test2(self):
        self.assertEqual(1, interpret('any [1, 0]'))

    def test3(self):
        self.assertEqual(0, interpret('any [0, 0]'))


class TestNone(unittest.TestCase):
    def test0(self):
        self.assertEqual(0, interpret('none [1]'))

    def test1(self):
        self.assertEqual(1, interpret('none [0]'))

    def test2(self):
        self.assertEqual(1, interpret('none [0, 0]'))

    def test3(self):
        self.assertEqual(0, interpret('none [0, 1]'))

    def test4(self):
        self.assertEqual(0, interpret('none [1, 1]'))


if __name__ == '__main__':
    unittest.main()
