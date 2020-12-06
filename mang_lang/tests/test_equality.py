from typing import Any, Mapping
import unittest
from interpreter import interpret


def V(value) -> Mapping[str, Any]:
    return value


def S(value) -> Mapping[str, Any]:
    return value


class TestEqualNumbers(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('equal [1, 1]'))

    def test1(self):
        self.assertEqual(V(0), interpret('equal [0, 1]'))

    def test2(self):
        self.assertEqual(V(0), interpret('equal [1, 0]'))

    def test3(self):
        self.assertEqual(V(1), interpret('equal [0, 0]'))


class TestEqualStrings(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(1), interpret('equal ["b", "b"]'))

    def test1(self):
        self.assertEqual(V(0), interpret('equal ["a", "b"]'))

    def test2(self):
        self.assertEqual(V(0), interpret('equal ["b", "a"]'))

    def test3(self):
        self.assertEqual(V(1), interpret('equal ["a", "a"]'))


class TestUnequalNumbers(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(0), interpret('unequal [1, 1]'))

    def test1(self):
        self.assertEqual(V(1), interpret('unequal [0, 1]'))

    def test2(self):
        self.assertEqual(V(1), interpret('unequal [1, 0]'))

    def test3(self):
        self.assertEqual(V(0), interpret('unequal [0, 0]'))


class TestUnequalStrings(unittest.TestCase):
    def test0(self):
        self.assertEqual(V(0), interpret('unequal ["b", "b"]'))

    def test1(self):
        self.assertEqual(V(1), interpret('unequal ["a", "b"]'))

    def test2(self):
        self.assertEqual(V(1), interpret('unequal ["b", "a"]'))

    def test3(self):
        self.assertEqual(V(0), interpret('unequal ["a", "a"]'))


if __name__ == '__main__':
    unittest.main()
