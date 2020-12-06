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


class TestFind(unittest.TestCase):
    def test0(self):
        self.assertEqual([[], []], interpret('find {list=[], item=1}'))

    def test1(self):
        self.assertEqual([[0], []], interpret('find {list=[0], item=1}'))

    def test2(self):
        self.assertEqual([[], [1]], interpret('find {list=[1], item=1}'))

    def test3(self):
        self.assertEqual([[0, 0],[]], interpret('find {list=[0, 0], item=1}'))

    def test4(self):
        self.assertEqual([[], [1, 0]], interpret('find {list=[1, 0], item=1}'))

    def test5(self):
        self.assertEqual([[0], [1]], interpret('find {list=[0, 1], item=1}'))

    def test6(self):
        self.assertEqual([[], [1, 1]], interpret('find {list=[1, 1], item=1}'))

    def test7(self):
        self.assertEqual([[], [1, 2, 3]], interpret('find {list=[1,2,3], item=1}'))

    def test8(self):
        self.assertEqual([[2], [1, 3]], interpret('find {list=[2,1,3], item=1}'))

    def test9(self):
        self.assertEqual([[2, 3], [1]], interpret('find {list=[2,3,1], item=1}'))


class TestFindIf(unittest.TestCase):
    def test0(self):
        self.assertEqual([[], []], interpret('find_if {list=[], predicate=from x to equal[x,1]}'))

    def test1(self):
        self.assertEqual([[], [1]], interpret('find_if {list=[1], predicate=from x to equal[x,1]}'))

    def test2(self):
        self.assertEqual([[], [1, 1]], interpret('find_if {list=[1, 1], predicate=from x to equal[x,1]}'))

    def test3(self):
        self.assertEqual([[], []], interpret('find_if {list=[], predicate=from x to 0}'))

    def test4(self):
        self.assertEqual([[1], []], interpret('find_if {list=[1], predicate=from x to 0}'))

    def test5(self):
        self.assertEqual([[1, 1], []], interpret('find_if {list=[1,1], predicate=from x to 0}'))

    def test6(self):
        self.assertEqual([[1, 1, 1], []], interpret('find_if {list=[1,1,1], predicate=from x to 0}'))


class TestMap(unittest.TestCase):
    def test0(self):
        self.assertEqual([], interpret('map{list=[],f=from x to mul[x,x]}'))

    def test1(self):
        self.assertEqual([1], interpret('map{list=[1],f=from x to mul[x,x]}'))

    def test2(self):
        self.assertEqual([1, 4], interpret('map{list=[1,2],f=from x to mul[x,x]}'))

    def test3(self):
        self.assertEqual([1, 4, 9], interpret('map{list=[1,2,3],f=from x to mul[x,x]}'))


class TestCount(unittest.TestCase):
    def test0(self):
        self.assertEqual(0, interpret('count []'))

    def test1(self):
        self.assertEqual(1, interpret('count [0]'))

    def test2(self):
        self.assertEqual(2, interpret('count [0, 0]'))

    def test3(self):
        self.assertEqual(3, interpret('count [0, 0, 0]'))


class TestCountString(unittest.TestCase):
    def test0(self):
        self.assertEqual(0, interpret('count ""'))

    def test1(self):
        self.assertEqual(1, interpret('count "0"'))

    def test2(self):
        self.assertEqual(2, interpret('count "00"'))

    def test3(self):
        self.assertEqual(3, interpret('count "000"'))


class TestCountItem(unittest.TestCase):
    def test0(self):
        self.assertEqual(0, interpret('count_item{list=[], item=3}'))

    def test1(self):
        self.assertEqual(0, interpret('count_item{list=[1], item=3}'))

    def test2(self):
        self.assertEqual(1, interpret('count_item{list=[3], item=3}'))

    def test3(self):
        self.assertEqual(1, interpret('count_item{list=[1,3], item=3}'))

    def test4(self):
        self.assertEqual(2, interpret('count_item{list=[3,1,3], item=3}'))


class TestCountIf(unittest.TestCase):
    def test0(self):
        self.assertEqual(0, interpret('count_if{list=[], predicate=from x to equal[x, 3]}'))

    def test1(self):
        self.assertEqual(0, interpret('count_if{list=[1], predicate=from x to equal[x, 3]}'))

    def test2(self):
        self.assertEqual(1, interpret('count_if{list=[3], predicate=from x to equal[x, 3]}'))

    def test3(self):
        self.assertEqual(1, interpret('count_if{list=[1,3], predicate=from x to equal[x, 3]}'))

    def test4(self):
        self.assertEqual(2, interpret('count_if{list=[3,1,3], predicate=from x to equal[x, 3]}'))


class TestFilter(unittest.TestCase):
    def test0(self):
        self.assertEqual([], interpret('filter{list=[], predicate=from x to equal[x, 3]}'))

    def test1(self):
        self.assertEqual([], interpret('filter{list=[1], predicate=from x to equal[x, 3]}'))

    def test2(self):
        self.assertEqual([], interpret('filter{list=[1,2], predicate=from x to equal[x, 3]}'))

    def test3(self):
        self.assertEqual([3], interpret('filter{list=[1,2,3], predicate=from x to equal[x, 3]}'))

    def test4(self):
        self.assertEqual([3], interpret('filter{list=[3,2,1], predicate=from x to equal[x, 3]}'))

    def test5(self):
        self.assertEqual([3, 3], interpret('filter{list=[3,2,3], predicate=from x to equal[x, 3]}'))


class TestEnumerate(unittest.TestCase):
    def test0(self):
        self.assertEqual([], interpret('enumerate []'))

    def test1(self):
        self.assertEqual(3, interpret('count enumerate [6,5,7]'))

    def test2(self):
        self.assertEqual(0, interpret('index < first enumerate [6,5,7]'))

    def test3(self):
        self.assertEqual(2, interpret('index < last enumerate [6,5,7]'))

    def test4(self):
        self.assertEqual(6, interpret('item < first enumerate [6,5,7]'))

    def test5(self):
        self.assertEqual(7, interpret('item < last enumerate [6,5,7]'))


class TestSplit(unittest.TestCase):
    def test0(self):
        self.assertEqual([], interpret('split{list=[],separator=0}'))

    def test1(self):
        self.assertEqual([[1]], interpret('split{list=[1],separator=0}'))

    def test2(self):
        self.assertEqual([[1, 2]], interpret('split{list=[1,2],separator=0}'))

    def test3(self):
        self.assertEqual([[1], [2]], interpret('split{list=[1,0,2],separator=0}'))

    def test4(self):
        self.assertEqual([[]], interpret('split{list=[0],separator=0}'))

    def test5(self):
        self.assertEqual([[],[]], interpret('split{list=[0, 0],separator=0}'))

    def test6(self):
        self.assertEqual([[1], [2]], interpret('split{list=[1,0,2,0],separator=0}'))

    def test7(self):
        self.assertEqual([[1], [2], []], interpret('split{list=[1,0,2,0,0],separator=0}'))

    def test8(self):
        self.assertEqual([[], [1], [2]], interpret('split{list=[0,1,0,2],separator=0}'))

    def test9(self):
        self.assertEqual([[1, 2], [1], [2, 3, 4]], interpret('split{list=[1,2,0,1,0,2,3,4],separator=0}'))


class TestReverse(unittest.TestCase):
    def test0(self):
        self.assertEqual([], interpret('reverse[]'))

    def test1(self):
        self.assertEqual([0], interpret('reverse[0]'))

    def test2(self):
        self.assertEqual([1, 0], interpret('reverse[0, 1]'))

    def test3(self):
        self.assertEqual([2, 1, 0], interpret('reverse[0, 1, 2]'))


class TestGetIndex(unittest.TestCase):
    def test0(self):
        self.assertEqual(2, interpret('get_index{list=[2,5,9],index=0}'))

    def test1(self):
        self.assertEqual(5, interpret('get_index{list=[2,5,9],index=1}'))

    def test2(self):
        self.assertEqual(9, interpret('get_index{list=[2,5,9],index=2}'))


class TestGetWrappedIndex(unittest.TestCase):
    def test0(self):
        self.assertEqual(2, interpret('get_wrapped_index{list=[2,5,9],index=0}'))

    def test1(self):
        self.assertEqual(5, interpret('get_wrapped_index{list=[2,5,9],index=1}'))

    def test2(self):
        self.assertEqual(9, interpret('get_wrapped_index{list=[2,5,9],index=2}'))

    def test3(self):
        self.assertEqual(2, interpret('get_wrapped_index{list=[2,5,9],index=3}'))

    def test4(self):
        self.assertEqual(5, interpret('get_wrapped_index{list=[2,5,9],index=4}'))

    def test5(self):
        self.assertEqual(9, interpret('get_wrapped_index{list=[2,5,9],index=5}'))

    def test6(self):
        self.assertEqual(2, interpret('get_wrapped_index{list=[2,5,9],index=6}'))

    def test7(self):
        self.assertEqual(9, interpret('get_wrapped_index{list=[2,5,9],index=-4}'))

    def test8(self):
        self.assertEqual(2, interpret('get_wrapped_index{list=[2,5,9],index=-3}'))

    def test9(self):
        self.assertEqual(5, interpret('get_wrapped_index{list=[2,5,9],index=-2}'))

    def test10(self):
        self.assertEqual(9, interpret('get_wrapped_index{list=[2,5,9],index=-1}'))


if __name__ == '__main__':
    unittest.main()
