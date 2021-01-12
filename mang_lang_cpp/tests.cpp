#include "mang_lang.h"
#include <functional>
#include <initializer_list>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

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
            const auto output_actual = function(input);
            if (output_expected == output_actual) {
                ++num_good;
            } else {
                ++num_bad;
                cout << endl << "" << function_name << "(" << input << ")"
                    << " expected " << output_expected << " got " << output_actual << endl;
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
    });
    test.evaluate("conditional", {
        {"if 1 then 2 else 3", "2"},
        {"if 0 then 2 else 3", "3"},
    });
    test.reformat("symbol", {
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
    return test.exitCode();
}
