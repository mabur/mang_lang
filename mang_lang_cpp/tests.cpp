#include "mang_lang.h"
#include <functional>
#include <initializer_list>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "SinglyLinkedList.h"

struct Test {
    Test() = default;
    ~Test() {
        using namespace std;
        cout << endl << num_good_total << "/" << num_good_total + num_bad_total
            << " tests successful in total" << std::endl;
        if (num_bad_total != 0) {
            cout << num_bad_total << " TESTS FAILING!" << endl;
        }
    }
    size_t exitCode() {
        return num_bad_total;
    }
    size_t num_good_total= 0;
    size_t num_bad_total = 0;

    template<typename T>
    void assertEqual(
        const T left,
        const T right
    ) {
        using namespace std;
        if (left == right)
        {
            cout << "Test successful: " << left << " == " << right << endl;
            ++num_good_total;
        }
        else
        {
            cout << "Test failed: " << left << " != " << right << endl;
            ++num_bad_total;
        }
    }

    template<typename Input, typename Output>
    void parameterizedTest(
        std::function<Output(Input)> function,
        std::string function_name,
        std::string case_name,
        std::initializer_list<std::pair<Input, Output>> data
    ) {
        using namespace std;
        auto num_good = 0;
        auto num_bad = 0;
        for (const auto& test : data) {
            const auto& input = test.first;
            const auto& output_expected = test.second;
            try {
                const auto output_actual = function(input);
                if (output_expected == output_actual) {
                    ++num_good;
                } else {
                    ++num_bad;
                    cout << endl << "" << function_name << "(" << input << ")"
                        << " expected " << output_expected << " got "
                        << output_actual << endl;
                }
            } catch (const std::exception& e) {
                ++num_bad;
                cout << endl << "" << function_name << "(" << input << ")"
                    << " expected " << output_expected << " got exception "
                    << e.what() << endl;
            }
        }
        cout << num_good << "/" << num_good + num_bad
            << " tests successful for case "
            << function_name << ":" << case_name << endl;
        num_good_total += num_good;
        num_bad_total += num_bad;
    }
};

struct MangLangTest : public Test {
    using InputOutputList = std::initializer_list<std::pair<std::string, std::string>>;

    void reformat(std::string case_name, InputOutputList data) {
        parameterizedTest<std::string, std::string>(
            ::reformat, "reformat", case_name, data
        );
    }
    void evaluate(std::string case_name, InputOutputList data) {
        parameterizedTest<std::string, std::string>(
            ::evaluate, "evaluate", case_name, data
        );
    }
};

int main() {
    MangLangTest test;
    test.evaluate("number", {
        {"-1", "-1"},
        {"-1.0", "-1"},
        {"-0", "-0"},
        {"-0.0", "-0"},
        {"0", "0"},
        {"0.0", "0"},
        {"+0", "0"},
        {"+0.0", "0"},
        {"1", "1"},
        {"1.0", "1"},
        {"+1", "1"},
        {"+1.0", "1"},
    });
    test.evaluate("character", {
        {R"('a')", R"('a')"},
        {R"('1')", R"('1')"},
        {R"('+')", R"('+')"},
        {R"('-')", R"('-')"},
        {R"('"')", R"('"')"},
        {R"('[')", R"('[')"},
        {R"(']')", R"(']')"},
        {R"('{')", R"('{')"},
        {R"('}')", R"('}')"},
    });
    test.evaluate("string", {
        {R"("")", R"("")"},
        {R"("a")", R"("a")"},
        {R"("ab")", R"("ab")"},
        {R"("abc")", R"("abc")"},
        {R"("1")", R"("1")"},
        {R"("-1")", R"("-1")"},
        {R"("+1")", R"("+1")"},
        {R"("[]")", R"("[]")"},
        {R"("{}")", R"("{}")"},
    });
    test.evaluate("list", {
        {"[]", "[]"},
        {"[ ]", "[]"},
        {"[  ]", "[]"},
        {"[1]", "[1]"},
        {"[ 1]", "[1]"},
        {"[1 ]", "[1]"},
        {"[ 1 ]", "[1]"},
        {"[1,2]", "[1,2]"},
        {"[1, 2]", "[1,2]"},
        {"[ 1, 2 ]", "[1,2]"},
        {"[[]]", "[[]]"},
        {"[[],[]]", "[[],[]]"},
        {"[[[]]]", "[[[]]]"},
    });
    test.evaluate("dictionary", {
        {"{}", "{}"},
        {"{ }", "{}"},
        {"{a=1}", "{a=1}"},
        {"{ a = 1 }", "{a=1}"},
        {"{a=1,b=2}", "{a=1,b=2}"},
        {"{ a = 1 , b = 2 }", "{a=1,b=2}"},
    });
    test.reformat("conditional", {
        {"if 1 then 2 else 3", "if 1 then 2 else 3"},
        {"if  1  then  2  else  3", "if 1 then 2 else 3"},
        {"if a then b else c", "if a then b else c"},
    });
    test.evaluate("conditional", {
        {"if 1 then 2 else 3", "2"},
        {"if 0 then 2 else 3", "3"},
        {"if [0] then 2 else 3", "2"},
        {"if [] then 2 else 3", "3"},
    });
    test.reformat("symbol", {
        {"a", "a"},
        {"{a=1, b=a}", "{a=1,b=a}"},
    });
    test.evaluate("symbol", {
        {"{a=1, b=a}", "{a=1,b=1}"},
    });
    test.reformat("child_symbol", {
        {"a<{a=1}", "a<{a=1}"},
    });
    test.evaluate("child_symbol", {
        {"a<{a=1}", "1"},
    });
    test.reformat("lookup_function", {
        {"add []", "add []"},
    });
    test.evaluate("min", {
        {"min[0]", "0"},
        {"min[0,1]", "0"},
        {"min[1,0]", "0"},
        {"min[3,6,1]", "1"},
        {"min[7,-3,8,-9]", "-9"},
    });
    test.evaluate("add", {
        {"add[]", "0"},
        {"add[0]", "0"},
        {"add[1]", "1"},
        {"add[0,1]", "1"},
        {"add[0,1,2]", "3"},
    });
    test.evaluate("mul", {
        {"mul[]", "1"},
        {"mul[0]", "0"},
        {"mul[1]", "1"},
        {"mul[1,2]", "2"},
        {"mul[1,2,3]", "6"},
        {"mul[1,2,3,4]", "24"},
    });
    test.evaluate("sub", {
        {"sub[0,0]", "0"},
        {"sub[6,3]", "3"},
        {"sub[4,8]", "-4"},
    });
    test.evaluate("div", {
        {"div[0,1]", "0"},
        {"div[2,1]", "2"},
        {"div[9,3]", "3"},
    });
    test.evaluate("abs", {
        {"abs -1", "1"},
        {"abs 0", "0"},
        {"abs 1", "1"},
    });
    test.evaluate("sqrt", {
        {"sqrt 0", "0"},
        {"sqrt 1", "1"},
        {"sqrt 4", "2"},
    });
    test.evaluate("boolean", {
        {"boolean -2", "1"},
        {"boolean -1", "1"},
        {"boolean -0", "0"},
        {"boolean 0", "0"},
        {"boolean +0", "0"},
        {"boolean 1", "1"},
        {"boolean 2", "1"},
        {"boolean[]", "0"},
        {"boolean[0]", "1"},
        {"boolean[0,1]", "1"},
        {"boolean{}", "0"},
        {"boolean{x=0}", "1"},
        {"boolean{x=0,y=1}", "1"},
        {R"(boolean"")", "0"},
        {R"(boolean"0")", "1"},
        {R"(boolean"1")", "1"},
        {R"(boolean"a")", "1"},
        {R"(boolean"ab")", "1"},
    });
    test.evaluate("not", {
        {"not -2", "0"},
        {"not -1", "0"},
        {"not -0", "1"},
        {"not 0", "1"},
        {"not +0", "1"},
        {"not 1", "0"},
        {"not 2", "0"},
        {"not[]", "1"},
        {"not[0]", "0"},
        {"not[0,1]", "0"},
        {"not{}", "1"},
        {"not{x=0}", "0"},
        {"not{x=0,y=1}", "0"},
        {R"(not"")", "1"},
        {R"(not"0")", "0"},
        {R"(not"1")", "0"},
        {R"(not"a")", "0"},
        {R"(not"ab")", "0"},
    });
    test.evaluate("all", {
        {"all[]", "1"},
        {"all[0]", "0"},
        {"all[1]", "1"},
        {"all[0,0]", "0"},
        {"all[0,1]", "0"},
        {"all[1,1]", "1"},
    });
    test.evaluate("any", {
        {"any[]", "0"},
        {"any[0]", "0"},
        {"any[1]", "1"},
        {"any[0,0]", "0"},
        {"any[0,1]", "1"},
        {"any[1,1]", "1"},
    });
    test.evaluate("none", {
        {"none[]", "1"},
        {"none[0]", "1"},
        {"none[1]", "0"},
        {"none[0,0]", "1"},
        {"none[0,1]", "0"},
        {"none[1,1]", "0"},
    });
    test.evaluate("equal number", {
        {"equal[0,0]", "1"},
        {"equal[0,1]", "0"},
        {"equal[1,0]", "0"},
        {"equal[1,1]", "1"},
    });
    test.evaluate("unequal number", {
        {"unequal[0,0]", "0"},
        {"unequal[0,1]", "1"},
        {"unequal[1,0]", "1"},
        {"unequal[1,1]", "0"},
    });
    test.evaluate("equal character", {
        {"equal['a','a']", "1"},
        {"equal['a','b']", "0"},
        {"equal['b','a']", "0"},
        {"equal['b','b']", "1"},
    });
    test.evaluate("unequal character", {
        {"unequal['a','a']", "0"},
        {"unequal['a','b']", "1"},
        {"unequal['b','a']", "1"},
        {"unequal['b','b']", "0"},
    });
    test.evaluate("equal list", {
        {"equal[[],[]]", "1"},
        {"equal[[1],[1]]", "1"},
        {"equal[[0],[1]]", "0"},
        {"equal[[0,1],[0,1]]", "1"},
        {"equal[[0,1],[1,1]]", "0"},
        {"equal[[0,1],[0]]", "0"},
    });
    test.evaluate("unequal list", {
        {"unequal[[],[]]", "0"},
        {"unequal[[1],[1]]", "0"},
        {"unequal[[0],[1]]", "1"},
        {"unequal[[0,1],[0,1]]", "0"},
        {"unequal[[0,1],[1,1]]", "1"},
        {"unequal[[0,1],[0]]", "1"},
    });
    test.evaluate("equal string", {
        {R"(equal["",""])", "1"},
        {R"(equal["a",""])", "0"},
        {R"(equal["","a"])", "0"},
        {R"(equal["a","a"])", "1"},
        {R"(equal["a","b"])", "0"},
        {R"(equal["ab","ab"])", "1"},
        {R"(equal["abc","ab"])", "0"},
        {R"(equal["ab","abc"])", "0"},
    });
    test.evaluate("unequal string", {
        {R"(unequal["",""])", "0"},
        {R"(unequal["a",""])", "1"},
        {R"(unequal["","a"])", "1"},
        {R"(unequal["a","a"])", "0"},
        {R"(unequal["a","b"])", "1"},
        {R"(unequal["ab","ab"])", "0"},
        {R"(unequal["abc","ab"])", "1"},
        {R"(unequal["ab","abc"])", "1"},
    });
    test.evaluate("function", {
        {"from x to x", "from x to x"},
        {"f<{f=from x to x}", "from x to x"},
        {"a<{f=from x to x,a=f 0}", "0"},
        {"a<{f=from x to x,a=f []}", "[]"},
        {"a<{f=from x to 1,a=f 0}", "1"},
        {"a<{inc=from x to add[x,1],a=inc 0}", "1"},
    });
    test.evaluate("first list", {
        {"first[4]", "4"},
        {"first[3,4]", "3"},
    });
    test.evaluate("first string", {
        {R"(first "b")", R"('b')"},
        {R"(first "ab")", R"('a')"},
    });
    test.evaluate("rest list", {
        {"rest[4]", "[]"},
        {"rest[4,3]", "[3]"},
        {"rest[4,3,7]", "[3,7]"},
    });
    test.evaluate("rest string", {
        {R"(rest "a")", R"("")"},
        {R"(rest "ab")", R"("b")"},
        {R"(rest "abc")", R"("bc")"},
    });
    test.evaluate("reverse list", {
        {"reverse[]", "[]"},
        {"reverse[0]", "[0]"},
        {"reverse[0,1]", "[1,0]"},
        {"reverse[0,1,2]", "[2,1,0]"},
    });
    test.evaluate("reverse string", {
        {R"(reverse "")", R"("")"},
        {R"(reverse "a")", R"("a")"},
        {R"(reverse "ab")", R"("ba")"},
        {R"(reverse "abc")", R"("cba")"},
    });
    test.evaluate("prepend list", {
        {"prepend{first=3,rest=[]}", "[3]"},
        {"prepend{first=4,rest=[5]}", "[4,5]"},
        {"prepend{first=4,rest=[6,8]}", "[4,6,8]"},
    });
    test.evaluate("prepend string", {
        {R"(prepend{first='a',rest=""})", R"("a")"},
        {R"(prepend{first='a',rest="b"})", R"("ab")"},
        {R"(prepend{first='a',rest="bc"})", R"("abc")"},
    });
    test.evaluate("inc", {
        {"inc 0", "1"},
    });
    test.evaluate("dec", {
        {"dec 0", "-1"},
    });
    test.evaluate("count list", {
        {"count []", "0"},
        {"count [[]]", "1"},
        {"count [[], []]", "2"},
    });
    test.evaluate("count string", {
        {R"(count "")", "0"},
        {R"(count "a")", "1"},
        {R"(count "ab")", "2"},
    });
    test.evaluate("count_item list", {
        {"count_item{list=[],item=1}", "0"},
        {"count_item{list=[1],item=1}", "1"},
        {"count_item{list=[1,1],item=1}", "2"},
        {"count_item{list=[1,0,1],item=1}", "2"},
    });
    test.evaluate("count_item string", {
        {R"(count_item{list="",item='a'})", "0"},
        {R"(count_item{list="a",item='a'})", "1"},
        {R"(count_item{list="aa",item='a'})", "2"},
        {R"(count_item{list="aba",item='a'})", "2"},
    });
    test.evaluate("count_if list", {
        {"count_if{list=[],predicate=from x to 0}", "0"},
        {"count_if{list=[1],predicate=from x to 0}", "0"},
        {"count_if{list=[],predicate=from x to 1}", "0"},
        {"count_if{list=[1],predicate=from x to 1}", "1"},
        {"count_if{list=[1,2,3],predicate=from x to equal[x,3]}", "1"},
        {"count_if{list=[3,2,3],predicate=from x to equal[x,3]}", "2"},
    });
    test.evaluate("count_if string", {
        {R"(count_if{list="",predicate=from x to 0})", "0"},
        {R"(count_if{list="a",predicate=from x to 0})", "0"},
        {R"(count_if{list="",predicate=from x to 1})", "0"},
        {R"(count_if{list="a",predicate=from x to 1})", "1"},
        {R"(count_if{list="abc",predicate=from x to equal[x,'c']})", "1"},
        {R"(count_if{list="cbc",predicate=from x to equal[x,'c']})", "2"},
    });
    test.evaluate("map list", {
        {"map{list=[],f=inc}", "[]"},
        {"map{list=[0],f=inc}", "[1]"},
        {"map{list=[0,1],f=inc}", "[1,2]"},
    });
    test.evaluate("filter list", {
        {"filter{list=[],predicate=from x to 1}", "[]"},
        {"filter{list=[[]],predicate=from x to 1}", "[[]]"},
        {"filter{list=[[]],predicate=from x to 0}", "[]"},
    });
    test.evaluate("get_index list", {
        {"get_index{index=0,list=[3,7,6]}", "3"},
        {"get_index{index=1,list=[3,7,6]}", "7"},
        {"get_index{index=2,list=[3,7,6]}", "6"},
    });
    test.evaluate("get_index string", {
        {R"(get_index{index=0,list="abc"})", R"('a')"},
        {R"(get_index{index=1,list="abc"})", R"('b')"},
        {R"(get_index{index=2,list="abc"})", R"('c')"},
    });

    {
        auto list = SinglyLinkedList<std::string>{};
        list = prepend(list, std::string{"a"});
        test.assertEqual<std::string>("a", list->first);
    }
    {
        auto list = SinglyLinkedList<std::string>{};
        list = prepend(list, std::string{"a"});
        list = prepend(list, std::string{"b"});
        test.assertEqual<std::string>("b", list->first);
    }
    {
        auto list = SinglyLinkedList<std::string>{};
        auto actual = leftFold(list, std::string{}, std::plus<std::string>{});
        test.assertEqual<std::string>("", actual);
    }
    {
        auto list = SinglyLinkedList<std::string>{};
        list = prepend(list, std::string{"a"});
        auto actual = leftFold(list, std::string{}, std::plus<std::string>{});
        test.assertEqual<std::string>("a", actual);
    }
    {
        auto list = SinglyLinkedList<std::string>{};
        list = prepend(list, std::string{"a"});
        list = prepend(list, std::string{"b"});
        auto actual = leftFold(list, std::string{}, std::plus<std::string>{});
        test.assertEqual<std::string>("ba", actual);
    }
    {
        auto list = SinglyLinkedList<std::string>{};
        list = prepend(list, std::string{"a"});
        list = reverse(list);
        test.assertEqual<std::string>("a", list->first);
    }
    {
        auto list = SinglyLinkedList<std::string>{};
        list = prepend(list, std::string{"a"});
        list = prepend(list, std::string{"b"});
        list = reverse(list);
        test.assertEqual<std::string>("a", list->first);
    }
    {
        auto list = SinglyLinkedList<int>{};
        list = prepend(list, 0);
        list = prepend(list, 1);
        auto actual = map(list, [](auto x){return x + 10;});
        test.assertEqual(11, actual->first);
    }
    {
        auto list = SinglyLinkedList<int>{};
        auto actual = findIf(list, [](auto){return true;});
        test.assertEqual(list.get(), actual.get());
    }
    {
        auto list = SinglyLinkedList<int>{};
        list = prepend(list, 0);
        auto actual = findIf(list, [](auto){return true;});
        test.assertEqual(list->first, actual->first);
    }
    {
        auto list = SinglyLinkedList<int>{};
        list = prepend(list, 1);
        list = prepend(list, 0);
        auto actual = findIf(list, [](auto x){return x != 0;});
        test.assertEqual(1, actual->first);
    }

    return test.exitCode();
}
