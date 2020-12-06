from typing import Any, Mapping
import unittest
from interpreter import interpret


def V(value) -> Mapping[str, Any]:
    return value


def S(value) -> Mapping[str, Any]:
    return value


class TestBuiltinFunctions(unittest.TestCase):
    def test_if_then_else_references(self):
        self.assertEqual(V(1), interpret('y<{x=1,y=if x then x else x}'))

    def test_count1(self):
        self.assertEqual(V(3), interpret('count [8,4,6]'))

    def test_count2(self):
        self.assertEqual(V(1), interpret('count [9]'))

    def test_count3(self):
        self.assertEqual(V(0), interpret('count []'))

    def test_count4(self):
        self.assertEqual(V(0), interpret('count []'))

    def test_is_empty1(self):
        self.assertEqual(V(1), interpret('is_empty []'))

    def test_is_empty2(self):
        self.assertEqual(V(0), interpret('is_empty [1]'))

    def test_is_empty3(self):
        self.assertEqual(V(0), interpret('is_empty [1,2]'))

    def test_concat1(self):
        self.assertEqual([V(1), V(2), V(3), V(4)],
                         interpret('concat [[1,2],[3,4]]'))

    def test_concat2(self):
        self.assertEqual([V(1), V(2), V(3), V(4), V(5), V(6)],
                         interpret('concat [[1,2],[3,4],[5,6]]'))

    def test_concat3(self):
        self.assertEqual([V(1), V(2), V(3)],
                         interpret('concat [[1],[2],[3]]'))

    def test_first(self):
        self.assertEqual(V(6), interpret('first [6, 4, 8]'))

    def test_first_string(self):
        self.assertEqual(S("6"), interpret('first "648"'))

    def test_last(self):
        self.assertEqual(V(8), interpret('last [6, 4, 8]'))

    def test_last_string(self):
        self.assertEqual(S("8"), interpret('last "648"'))

    def test_first_part0(self):
        self.assertEqual(V(2), interpret('count first_part [6, 4, 8]'))

    def test_first_part1(self):
        self.assertEqual(V(6), interpret('first first_part [6, 4, 8]'))

    def test_first_part2(self):
        self.assertEqual(V(4), interpret('last first_part [6, 4, 8]'))

    def test_first_part3(self):
        self.assertEqual(V(0), interpret('count first_part [6]'))

    def test_first_part4(self):
        self.assertEqual(V(0), interpret('count first_part []'))

    def test_last_part0(self):
        self.assertEqual(V(2), interpret('count last_part [6, 4, 8]'))

    def test_last_part1(self):
        self.assertEqual(V(4), interpret('first last_part [6, 4, 8]'))

    def test_last_part2(self):
        self.assertEqual(V(8), interpret('last last_part [6, 4, 8]'))

    def test_last_part3(self):
        self.assertEqual(V(0), interpret('count last_part [6]'))

    def test_last_part4(self):
        self.assertEqual(V(0), interpret('count last_part []'))


class TestFirstPartString(unittest.TestCase):
    def test0(self):
        self.assertEqual(S(""), interpret('first_part ""'))

    def test1(self):
        self.assertEqual(S(""), interpret('first_part "a"'))

    def test2(self):
        self.assertEqual(S("a"), interpret('first_part "ab"'))

    def test3(self):
        self.assertEqual(S("ab"), interpret('first_part "abc"'))


class TestLastPartString(unittest.TestCase):
    def test0(self):
        self.assertEqual(S(""), interpret('last_part ""'))

    def test1(self):
        self.assertEqual(S(""), interpret('last_part "a"'))

    def test2(self):
        self.assertEqual(S("b"), interpret('last_part "ab"'))

    def test3(self):
        self.assertEqual(S("bc"), interpret('last_part "abc"'))


class TestString(unittest.TestCase):
    def test_string1(self):
        self.assertEqual(S("hej"), interpret('"hej"'))

    def test_string2(self):
        self.assertEqual(S("home\dir/image.png"), interpret('"home\dir/image.png"'))

    def test_string_count0(self):
        self.assertEqual(V(0), interpret('count ""'))

    def test_string_count1(self):
        self.assertEqual(V(1), interpret('count "f"'))

    def test_string_count2(self):
        self.assertEqual(V(2), interpret('count "du"'))

    def test_string_concat1(self):
        self.assertEqual(S(""), interpret('concat ["",""]'))

    def test_string_concat2(self):
        self.assertEqual(S("abcd"), interpret('concat ["ab","cd"]'))

    def test_string_concat3(self):
        self.assertEqual(S("ab"), interpret('concat ["ab",""]'))

    def test_string_concat4(self):
        self.assertEqual(S("cd"), interpret('concat ["","cd"]'))

    def test_string_concat5(self):
        self.assertEqual(S("abc"), interpret('concat ["a","b","c"]'))

    def test_string_index0(self):
        self.assertEqual(S("a"), interpret('y<{x="abc", y=first x}'))

    def test_string_index1(self):
        self.assertEqual(S("c"), interpret('y<{x="abc", y=last x}'))


if __name__ == '__main__':
    unittest.main()
