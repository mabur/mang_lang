import unittest
from interpreter import interpret


class TestCountList(unittest.TestCase):
    def test1(self):
        self.assertEqual(3, interpret('count [8,4,6]'))

    def test2(self):
        self.assertEqual(1, interpret('count [9]'))

    def test3(self):
        self.assertEqual(0, interpret('count []'))

    def test4(self):
        self.assertEqual(0, interpret('count []'))


class TestCountString(unittest.TestCase):
    def test0(self):
        self.assertEqual(0, interpret('count ""'))

    def test1(self):
        self.assertEqual(1, interpret('count "f"'))

    def test2(self):
        self.assertEqual(2, interpret('count "du"'))


class TestIsEmptyList(unittest.TestCase):
    def test1(self):
        self.assertEqual(1, interpret('is_empty []'))

    def test2(self):
        self.assertEqual(0, interpret('is_empty [1]'))

    def test3(self):
        self.assertEqual(0, interpret('is_empty [1,2]'))


class TestConcatList(unittest.TestCase):
    def test1(self):
        self.assertEqual([1, 2, 3, 4],
                         interpret('concat [[1,2],[3,4]]'))

    def test2(self):
        self.assertEqual([1, 2, 3, 4, 5, 6],
                         interpret('concat [[1,2],[3,4],[5,6]]'))

    def test3(self):
        self.assertEqual([1, 2, 3],
                         interpret('concat [[1],[2],[3]]'))


class TestConcatString(unittest.TestCase):
    def test(self):
        self.assertEqual("", interpret('concat ["",""]'))

    def test2(self):
        self.assertEqual("abcd", interpret('concat ["ab","cd"]'))

    def test3(self):
        self.assertEqual("ab", interpret('concat ["ab",""]'))

    def test4(self):
        self.assertEqual("cd", interpret('concat ["","cd"]'))

    def test5(self):
        self.assertEqual("abc", interpret('concat ["a","b","c"]'))



class TestFirst(unittest.TestCase):
    def test_first_list(self):
        self.assertEqual(6, interpret('first [6, 4, 8]'))

    def test_first_string(self):
        self.assertEqual("6", interpret('first "648"'))


class TestLast(unittest.TestCase):
    def test_last_list(self):
        self.assertEqual(8, interpret('last [6, 4, 8]'))

    def test_last_string(self):
        self.assertEqual("8", interpret('last "648"'))


class TestFirstPartList(unittest.TestCase):
    def test0(self):
        self.assertEqual(2, interpret('count first_part [6, 4, 8]'))

    def test1(self):
        self.assertEqual(6, interpret('first first_part [6, 4, 8]'))

    def test2(self):
        self.assertEqual(4, interpret('last first_part [6, 4, 8]'))

    def test3(self):
        self.assertEqual(0, interpret('count first_part [6]'))

    def test4(self):
        self.assertEqual(0, interpret('count first_part []'))


class TestFirstPartString(unittest.TestCase):
    def test0(self):
        self.assertEqual("", interpret('first_part ""'))

    def test1(self):
        self.assertEqual("", interpret('first_part "a"'))

    def test2(self):
        self.assertEqual("a", interpret('first_part "ab"'))

    def test3(self):
        self.assertEqual("ab", interpret('first_part "abc"'))


class TestLastPartList(unittest.TestCase):
    def test0(self):
        self.assertEqual(2, interpret('count last_part [6, 4, 8]'))

    def test1(self):
        self.assertEqual(4, interpret('first last_part [6, 4, 8]'))

    def test2(self):
        self.assertEqual(8, interpret('last last_part [6, 4, 8]'))

    def test3(self):
        self.assertEqual(0, interpret('count last_part [6]'))

    def test4(self):
        self.assertEqual(0, interpret('count last_part []'))


class TestLastPartString(unittest.TestCase):
    def test0(self):
        self.assertEqual("", interpret('last_part ""'))

    def test1(self):
        self.assertEqual("", interpret('last_part "a"'))

    def test2(self):
        self.assertEqual("b", interpret('last_part "ab"'))

    def test3(self):
        self.assertEqual("bc", interpret('last_part "abc"'))


if __name__ == '__main__':
    unittest.main()
