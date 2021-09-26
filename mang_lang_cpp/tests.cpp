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
        {R"(')')", R"(')')"},
        {R"(')')", R"(')')"},
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
        {R"#("()")#", R"#("()")#"},
        {R"("{}")", R"("{}")"},
    });
    test.evaluate("list", {
        {"()", "()"},
        {"( )", "()"},
        {"(  )", "()"},
        {"(1)", "(1)"},
        {"( 1)", "(1)"},
        {"(1 )", "(1)"},
        {"( 1 )", "(1)"},
        {"(1 2)", "(1 2)"},
        {"(1  2)", "(1 2)"},
        {"( 1  2 )", "(1 2)"},
        {"(())", "(())"},
        {"(() ())", "(() ())"},
        {"((()))", "((()))"},
    });
    test.reformat("dictionary iteration", {
        {"{while 1 end}", "{while 1 end}"},
        {"{i=2 while i i=dec!i end}", "{i=2 while i i=dec!i end}"},
        {"{i=10 while i i=dec!i end j=1}", "{i=10 while i i=dec!i end j=1}"},
    });
    test.evaluate("dictionary iterations", {
        {"{i=2 while i i=dec!i end}", "{i=0}"},
        {"{i=2 while i i=dec!i end j=1}", "{i=0 j=1}"},
        {"{i=2 sum=0 while i sum=add!(sum i) i=dec!i end}", "{i=0 sum=3}"},
        {"{i=1000 sum=0 while i sum=add!(sum i) i=dec!i end}", "{i=0 sum=500500}"},
    });
    test.evaluate("dictionary", {
        {"{}", "{}"},
        {"{ }", "{}"},
        {"{a=1}", "{a=1}"},
        {"{a0=1}", "{a0=1}"},
        {"{a_0=1}", "{a_0=1}"},
        {"{ a = 1 }", "{a=1}"},
        {"{a=1 b=2}", "{a=1 b=2}"},
        {"{ a = 1  b = 2 }", "{a=1 b=2}"},
    });
    test.reformat("conditional", {
        {"if 1 then 2 else 3", "if 1 then 2 else 3"},
        {"if  1  then  2  else  3", "if 1 then 2 else 3"},
        {"if a then b else c", "if a then b else c"},
    });
    test.evaluate("conditional", {
        {"if 1 then 2 else 3", "2"},
        {"if 0 then 2 else 3", "3"},
        {"if (0) then 2 else 3", "2"},
        {"if () then 2 else 3", "3"},
    });
    test.reformat("symbol", {
        {"a", "a"},
        {"{a=1 b=a}", "{a=1 b=a}"},
    });
    test.evaluate("symbol", {
        {"{a=1 b=a}", "{a=1 b=1}"},
        {"{a=1 b_0=a}", "{a=1 b_0=1}"},
        {"{a=1 b={c=a}}", "{a=1 b={c=1}}"},
        {"{a=1 b={c={d=a}}}", "{a=1 b={c={d=1}}}"},
        {"{a=1 b=(a)}", "{a=1 b=(1)}"},
        {"{a=1 b=((a))}", "{a=1 b=((1))}"},
        {"{a=1 b=c@{c=a}}", "{a=1 b=1}"},
        {"{a=1 b=add!(a a)}", "{a=1 b=2}"},
        {"{a=1 b=if a then a else 2}", "{a=1 b=1}"},
    });
    test.reformat("child_symbol", {
        {"a@{a=1}", "a@{a=1}"},
        {"A_0@{A_0=1}", "A_0@{A_0=1}"},
    });
    test.evaluate("child_symbol", {
        {"a@{a=1}", "1"},
        {"A_0@{A_0=1}", "1"},
        {"_0@{_0=1}", "1"},
        {"y@{x=5 y=x}", "5"},
        {"c@{a=1 b=a c=b}", "1"},
        {"y@x@{x = {y = 1}}", "1"},
        {"b@x@{x={a=1 b=2}}", "2"},
        {"z@y@x@{x = {y = {z = 1}}}", "1"},
        {"w@z@y@x@{x = {y = {z = {w=1}}}}", "1"},
        {"b@{a={f=in x out 1} g=f@a b=g!3}", "1"},
        {"b@{a={f = in x out inc!x} g=f@a b = g!3}", "4"},
        {"c@{a={b={f=in x out inc!x}} g=f@b@a c = g!3}", "4"},
        {"ABBA@{ABBA = 1}", "1"},
        {"ABBA@{ABBA = 1 PADDA = 2}", "1"},
    });
    test.evaluate("child_symbol_keyword_confusion", {
        {"input@{input=5}", "5"},
        {"output@{output=5}", "5"},
        {"iffy@{iffy=5}", "5"},
        {"thenner@{thenner=5}", "5"},
        {"elsewhere@{elsewhere=5}", "5"},
        {"whiler@{whiler=5}", "5"},
        {"endar@{endar=5}", "5"},
    });
    test.reformat("lookup_function", {
        {"add!()", "add!()"},
    });
    test.evaluate("min", {
        {"min!(0)", "0"},
        {"min!(0 1)", "0"},
        {"min!(1 0)", "0"},
        {"min!(3 6 1)", "1"},
        {"min!(7 -3 8 -9)", "-9"},
    });
    test.evaluate("min", {
        {"max!(0)", "0"},
        {"max!(0 1)", "1"},
        {"max!(1 0)", "1"},
        {"max!(3 6 1)", "6"},
        {"max!(7 -3 8 -9)", "8"},
    });
    test.evaluate("add", {
        {"add!()", "0"},
        {"add!(0)", "0"},
        {"add!(1)", "1"},
        {"add!(0 1)", "1"},
        {"add!(0 1 2)", "3"},
    });
    test.evaluate("mul", {
        {"mul!()", "1"},
        {"mul!(0)", "0"},
        {"mul!(1)", "1"},
        {"mul!(1 2)", "2"},
        {"mul!(1 2 3)", "6"},
        {"mul!(1 2 3 4)", "24"},
    });
    test.evaluate("sub", {
        {"sub!(0 0)", "0"},
        {"sub!(6 3)", "3"},
        {"sub!(4 8)", "-4"},
    });
    test.evaluate("div", {
        {"div!(0 1)", "0"},
        {"div!(2 1)", "2"},
        {"div!(9 3)", "3"},
    });
    test.evaluate("less", {
        {"less?(0 0)", "0"},
        {"less?(0 1)", "1"},
        {"less?(-1 0)", "1"},
        {"less?(1 0)", "0"},
        {"less?(0 -1)", "0"},
        {"less?(1 1)", "0"},
        {"less?(-1 -1)", "0"},
        {"less?()", "1"},
        {"less?(0)", "1"},
        {"less?(0 1 2)", "1"},
        {"less?(0 1 2 3)", "1"},
        {"less?(0 1 2 3 3)", "0"},
    });
    test.evaluate("less_or_equal", {
        {"less_or_equal?(0 0)", "1"},
        {"less_or_equal?(0 1)", "1"},
        {"less_or_equal?(-1 0)", "1"},
        {"less_or_equal?(1 0)", "0"},
        {"less_or_equal?(0 -1)", "0"},
        {"less_or_equal?(1 1)", "1"},
        {"less_or_equal?(-1 -1)", "1"},
        {"less_or_equal?()", "1"},
        {"less_or_equal?(0)", "1"},
        {"less_or_equal?(0 1 2)", "1"},
        {"less_or_equal?(0 1 2 3)", "1"},
        {"less_or_equal?(0 1 2 3 3)", "1"},
    });
    test.evaluate("round", {
        {"round!-0.9", "-1"},
        {"round!-0.6", "-1"},
        {"round!-0.4", "-0"},
        {"round!-0.1", "-0"},
        {"round!0", "0"},
        {"round!0.1", "0"},
        {"round!0.4", "0"},
        {"round!0.6", "1"},
        {"round!0.9", "1"},
    });
    test.evaluate("round_up", {
        {"round_up!-0.9", "-0"},
        {"round_up!-0.6", "-0"},
        {"round_up!-0.4", "-0"},
        {"round_up!-0.1", "-0"},
        {"round_up!0", "0"},
        {"round_up!0.1", "1"},
        {"round_up!0.4", "1"},
        {"round_up!0.6", "1"},
        {"round_up!0.9", "1"},
    });
    test.evaluate("round_down", {
        {"round_down!-0.9", "-1"},
        {"round_down!-0.6", "-1"},
        {"round_down!-0.4", "-1"},
        {"round_down!-0.1", "-1"},
        {"round_down!0", "0"},
        {"round_down!0.1", "0"},
        {"round_down!0.4", "0"},
        {"round_down!0.6", "0"},
        {"round_down!0.9", "0"},
    });
    test.evaluate("abs", {
        {"abs!-1", "1"},
        {"abs!0", "0"},
        {"abs!1", "1"},
    });
    test.evaluate("sqrt", {
        {"sqrt!0", "0"},
        {"sqrt!1", "1"},
        {"sqrt!4", "2"},
    });
    test.evaluate("number", {
        {"number!'0'", "48"},
        {"number!'9'", "57"},
        {"number!'A'", "65"},
        {"number!'Z'", "90"},
        {"number!'a'", "97"},
        {"number!'z'", "122"},
    });
    test.evaluate("character", {
        {"character!48", "'0'"},
        {"character!57", "'9'"},
        {"character!65", "'A'"},
        {"character!90", "'Z'"},
        {"character!97", "'a'"},
        {"character!122", "'z'"},
    });
    test.evaluate("boolean", {
        {"boolean!-2", "1"},
        {"boolean!-1", "1"},
        {"boolean!-0", "0"},
        {"boolean!0", "0"},
        {"boolean!+0", "0"},
        {"boolean!1", "1"},
        {"boolean!2", "1"},
        {"boolean!()", "0"},
        {"boolean!(0)", "1"},
        {"boolean!(0 1)", "1"},
        {"boolean!{}", "0"},
        {"boolean!{x=0}", "1"},
        {"boolean!{x=0 y=1}", "1"},
        {R"(boolean!"")", "0"},
        {R"(boolean!"0")", "1"},
        {R"(boolean!"1")", "1"},
        {R"(boolean!"a")", "1"},
        {R"(boolean!"ab")", "1"},
    });
    test.evaluate("not", {
        {"not?-2", "0"},
        {"not?-1", "0"},
        {"not?-0", "1"},
        {"not?0", "1"},
        {"not?+0", "1"},
        {"not?1", "0"},
        {"not?2", "0"},
        {"not?()", "1"},
        {"not?(0)", "0"},
        {"not?(0 1)", "0"},
        {"not?{}", "1"},
        {"not?{x=0}", "0"},
        {"not?{x=0 y=1}", "0"},
        {R"(not?"")", "1"},
        {R"(not?"0")", "0"},
        {R"(not?"1")", "0"},
        {R"(not?"a")", "0"},
        {R"(not?"ab")", "0"},
    });
    test.evaluate("all", {
        {"all?()", "1"},
        {"all?(0)", "0"},
        {"all?(1)", "1"},
        {"all?(0 0)", "0"},
        {"all?(0 1)", "0"},
        {"all?(1 1)", "1"},
    });
    test.evaluate("any", {
        {"any?()", "0"},
        {"any?(0)", "0"},
        {"any?(1)", "1"},
        {"any?(0 0)", "0"},
        {"any?(0 1)", "1"},
        {"any?(1 1)", "1"},
    });
    test.evaluate("none", {
        {"none?()", "1"},
        {"none?(0)", "1"},
        {"none?(1)", "0"},
        {"none?(0 0)", "1"},
        {"none?(0 1)", "0"},
        {"none?(1 1)", "0"},
    });
    test.evaluate("equal number", {
        {"equal?(0 0)", "1"},
        {"equal?(0 1)", "0"},
        {"equal?(1 0)", "0"},
        {"equal?(1 1)", "1"},
    });
    test.evaluate("unequal number", {
        {"unequal?(0 0)", "0"},
        {"unequal?(0 1)", "1"},
        {"unequal?(1 0)", "1"},
        {"unequal?(1 1)", "0"},
    });
    test.evaluate("equal character", {
        {"equal?('a' 'a')", "1"},
        {"equal?('a' 'b')", "0"},
        {"equal?('b' 'a')", "0"},
        {"equal?('b' 'b')", "1"},
    });
    test.evaluate("unequal character", {
        {"unequal?('a' 'a')", "0"},
        {"unequal?('a' 'b')", "1"},
        {"unequal?('b' 'a')", "1"},
        {"unequal?('b' 'b')", "0"},
    });
    test.evaluate("equal list", {
        {"equal?(() ())", "1"},
        {"equal?((1) (1))", "1"},
        {"equal?((0) (1))", "0"},
        {"equal?((0 1) (0 1))", "1"},
        {"equal?((0 1) (1 1))", "0"},
        {"equal?((0 1) (0))", "0"},
    });
    test.evaluate("unequal list", {
        {"unequal?(() ())", "0"},
        {"unequal?((1) (1))", "0"},
        {"unequal?((0) (1))", "1"},
        {"unequal?((0 1) (0 1))", "0"},
        {"unequal?((0 1) (1 1))", "1"},
        {"unequal?((0 1) (0))", "1"},
    });
    test.evaluate("equal string", {
        {R"(equal?("" ""))", "1"},
        {R"(equal?("a" ""))", "0"},
        {R"(equal?("" "a"))", "0"},
        {R"(equal?("a" "a"))", "1"},
        {R"(equal?("a" "b"))", "0"},
        {R"(equal?("ab" "ab"))", "1"},
        {R"(equal?("abc" "ab"))", "0"},
        {R"(equal?("ab" "abc"))", "0"},
    });
    test.evaluate("unequal string", {
        {R"(unequal?("" ""))", "0"},
        {R"(unequal?("a" ""))", "1"},
        {R"(unequal?("" "a"))", "1"},
        {R"(unequal?("a" "a"))", "0"},
        {R"(unequal?("a" "b"))", "1"},
        {R"(unequal?("ab" "ab"))", "0"},
        {R"(unequal?("abc" "ab"))", "1"},
        {R"(unequal?("ab" "abc"))", "1"},
    });
    test.evaluate("function", {
        {"in x out x",       "in x out x"},
        {"f@{f=in x out x}", "in x out x"},
    });
    test.evaluate("function dictionary", {
        {"in {x} out x", "in {x} out x"},
        {"in {x y} out x", "in {x y} out x"},
        {"in  {  x    y  }  out  x", "in {x y} out x"},
    });
    test.evaluate("function list", {
        {"in (x) out x", "in (x) out x"},
        {"in (x y) out x", "in (x y) out x"},
        {"in  (  x    y  )  out  x", "in (x y) out x"},
    });
    test.evaluate("lookup function", {
        {"a@{f=in x out x a=f!0}", "0"},
        {"a@{f=in x out x a=f!()}", "()"},
        {"a@{f=in x out 1 a=f!0}", "1"},
        {"z@{f=in x out y@{y = 3} z=f!2}", "3"},
        {"z@{f=in x out result@{y=3 result=add!(x y)}  z=f!2}", "5"},
        {"z@{y=2 f=in x out result@{y=3 result=add!(x y)} z=f!2}", "5"},
        {"x@{a={b=1 f=in x out b} b=2 f=f@a x=f!()}", "1"},
        {"x@{f=in a out 1 g = in b out f!b x = g!2}", "1"},
        {"y@{apply=in (f x) out f!x y = apply!(inc 2)}", "3"},
        {"y@{apply=in (f x) out f!x id=in x out apply!(in x out x x) y = id!1}", "1"},
        {"y@{f=in list out map!(in y out 2 list) y=f!(0 0)}", "(2 2)"},
        {"a@{call=in f out f!() g=in x out 0 a=call!g}", "0"},
        {"a@{call=in f out f!() b={a=0 g=in x out a} g=g@b a=call!g}", "0"},
        {"a@{call=in(f)out f!(()) b={a=0 g=in(x)out a} g=g@b a=call!(g)}", "0"},
        {"y@{a=1 g=in y out a f=in list out map!(g list) y=f!(0 0)}", "(1 1)"},
        {"y@{a=1 f=in list out map!(in y out a list) y=f!(0 0)}", "(1 1)"},
        {"b@{a={a=0 f=in x out a} g=f@a b=g!1}", "0"},
        {"y@{f=in (x list) out map!(in y out x list) y=f!(2 (0 0))}", "(2 2)"},
    });
    test.evaluate("recursive function", {
        {"y@{f=in x out if x then add!(x f!dec!x) else 0 y=f!3}", "6"},
    });
    test.evaluate("lookup function dictionary", {
        {"a@{f=in {x} out x a=f!{x=0}}", "0"},
        {"a@{f=in {x y} out add!(x y) a=f!{x=2 y=3}}", "5"},
        {"a@{f=in {x y z} out add!(x y z) a=f!{x=2 y=3 z=4}}", "9"},
    });
    test.evaluate("lookup function list", {
        {"a@{f=in (x) out x a=f!(0)}", "0"},
        {"a@{f=in (x y) out add!(x y) a=f!(2 3)}", "5"},
    });
    test.evaluate("new list", {
        {"new!()", "()"},
        {"new!(1)", "()"},
        {"new!(1 2)", "()"},
    });
    test.evaluate("new string", {
        {R"(new!"")", R"("")"},
        {R"(new!"a")", R"("")"},
        {R"(new!"ab")", R"("")"},
    });
    test.evaluate("first list", {
        {"first@(4)", "4"},
        {"first@(3 4)", "3"},
    });
    test.evaluate("first string", {
        {R"(first@"b")", R"('b')"},
        {R"(first@"ab")", R"('a')"},
    });
    test.evaluate("rest list", {
        {"rest@(4)", "()"},
        {"rest@(4 3)", "(3)"},
        {"rest@(4 3 7)", "(3 7)"},
    });
    test.evaluate("rest string", {
        {R"(rest@"a")", R"("")"},
        {R"(rest@"ab")", R"("b")"},
        {R"(rest@"abc")", R"("bc")"},
    });
    test.evaluate("reverse list", {
        {"reverse!()", "()"},
        {"reverse!(0)", "(0)"},
        {"reverse!(0 1)", "(1 0)"},
        {"reverse!(0 1 2)", "(2 1 0)"},
    });
    test.evaluate("reverse string", {
        {R"(reverse!"")", R"("")"},
        {R"(reverse!"a")", R"("a")"},
        {R"(reverse!"ab")", R"("ba")"},
        {R"(reverse!"abc")", R"("cba")"},
    });
    test.evaluate("prepend list", {
        {"prepend!(3 ())", "(3)"},
        {"prepend!(4 (5))", "(4 5)"},
        {"prepend!(4 (6 8))", "(4 6 8)"},
    });
    test.evaluate("prepend string", {
        {R"(prepend!('a' ""))", R"("a")"},
        {R"(prepend!('a' "b"))", R"("ab")"},
        {R"(prepend!('a' "bc"))", R"("abc")"},
    });
    test.evaluate("inc", {
        {"inc!0", "1"},
    });
    test.evaluate("dec", {
        {"dec!0", "-1"},
    });
    test.evaluate("is_digit", {
        {"is_digit?'A'", "0"},
        {"is_digit?'Z'", "0"},
        {"is_digit?'a'", "0"},
        {"is_digit?'z'", "0"},
        {"is_digit?'0'", "1"},
        {"is_digit?'9'", "1"},
    });
    test.evaluate("is_upper", {
        {"is_upper?'A'", "1"},
        {"is_upper?'Z'", "1"},
        {"is_upper?'a'", "0"},
        {"is_upper?'z'", "0"},
        {"is_upper?'0'", "0"},
        {"is_upper?'9'", "0"},
    });
    test.evaluate("is_lower", {
        {"is_lower?'A'", "0"},
        {"is_lower?'Z'", "0"},
        {"is_lower?'a'", "1"},
        {"is_lower?'z'", "1"},
        {"is_lower?'0'", "0"},
        {"is_lower?'9'", "0"},
    });
    test.evaluate("is_letter", {
        {"is_letter?'A'", "1"},
        {"is_letter?'Z'", "1"},
        {"is_letter?'a'", "1"},
        {"is_letter?'z'", "1"},
        {"is_letter?'0'", "0"},
        {"is_letter?'9'", "0"},
    });
    test.evaluate("to_upper", {
        {"to_upper!'A'", "'A'"},
        {"to_upper!'Z'", "'Z'"},
        {"to_upper!'a'", "'A'"},
        {"to_upper!'z'", "'Z'"},
        {"to_upper!'0'", "'0'"},
        {"to_upper!'9'", "'9'"},
    });
    test.evaluate("to_lower", {
        {"to_lower!'A'", "'a'"},
        {"to_lower!'Z'", "'z'"},
        {"to_lower!'a'", "'a'"},
        {"to_lower!'z'", "'z'"},
        {"to_lower!'0'", "'0'"},
        {"to_lower!'9'", "'9'"},
    });
    test.evaluate("range", {
        {"range!0", "()"},
        {"range!1", "(0)"},
        {"range!2", "(0 1)"},
        {"range!3", "(0 1 2)"},
    });
    test.evaluate("iteration", {
        {"count!range!100", "100"},
        {"add!range!100", "4950"},
    });
    test.evaluate("count list", {
        {"count!()", "0"},
        {"count!(())", "1"},
        {"count!(() ())", "2"},
    });
    test.evaluate("count string", {
        {R"(count!"")", "0"},
        {R"(count!"a")", "1"},
        {R"(count!"ab")", "2"},
    });
    test.evaluate("count_item list", {
        {"count_item!(1 ())", "0"},
        {"count_item!(1 (1))", "1"},
        {"count_item!(1 (1 1))", "2"},
        {"count_item!(1 (1 0 1))", "2"},
    });
    test.evaluate("count_item string", {
        {R"(count_item!('a' ""))", "0"},
        {R"(count_item!('a' "a"))", "1"},
        {R"(count_item!('a' "aa"))", "2"},
        {R"(count_item!('a' "aba"))", "2"},
    });
    test.evaluate("count_if list", {
        {"count_if!(in x out 0 ())", "0"},
        {"count_if!(in x out 0 (1))", "0"},
        {"count_if!(in x out 1 ())", "0"},
        {"count_if!(in x out 1 (1))", "1"},
        {"count_if!(in x out equal?(x 3) (1 2 3))", "1"},
        {"count_if!(in x out equal?(x 3) (3 2 3))", "2"},
    });
    test.evaluate("count_if string", {
        {R"(count_if!(in x out 0 ""))", "0"},
        {R"(count_if!(in x out 0 "a"))", "0"},
        {R"(count_if!(in x out 1 ""))", "0"},
        {R"(count_if!(in x out 1 "a"))", "1"},
        {R"(count_if!(in x out equal?(x 'c') "abc"))", "1"},
        {R"(count_if!(in x out equal?(x 'c') "cbc"))", "2"},
    });
    test.evaluate("map list", {
        {"map!(inc ())", "()"},
        {"map!(inc (0))", "(1)"},
        {"map!(inc (0 1))", "(1 2)"},
        {"map!(in x out 2 (0 0))", "(2 2)"},
        {"a@{b=2 f=in x out b a=map!(f (0 0))}", "(2 2)"},
    });
    test.evaluate("map_string", {
        {R"(map_string!(to_upper ""))", R"("")"},
        {R"(map_string!(to_upper "abc"))", R"("ABC")"},
    });
    test.evaluate("filter list", {
        {"filter!(in x out 1 ())", "()"},
        {"filter!(in x out 1 (()))", "(())"},
        {"filter!(in x out 0 (()))", "()"},
    });
    test.evaluate("filter string", {
        {R"(filter!(in x out 0 ""))", R"("")"},
        {R"(filter!(in x out 0 "a"))", R"("")"},
        {R"(filter!(in x out 1 ""))", R"("")"},
        {R"(filter!(in x out 1 "a"))", R"("a")"},
        {R"(filter!(in x out equal?(x 'a') "a"))", R"("a")"},
        {R"(filter!(in x out equal?(x 'a') "ab"))", R"("a")"},
        {R"(filter!(in x out equal?(x 'a') "ba"))", R"("a")"},
        {R"(filter!(in x out equal?(x 'a') "aba"))", R"("aa")"},
    });
    test.evaluate("replace list", {
        {"replace!(1 2 ())", "()"},
        {"replace!(1 2 (1))", "(2)"},
        {"replace!(1 2 (2))", "(2)"},
        {"replace!(1 2 (0 1 0 1 1))", "(0 2 0 2 2)"},
    });
    test.evaluate("replace string", {
        {R"(replace!('a' 'b' ""))", R"("")"},
        {R"(replace!('a' 'b' "a"))", R"("b")"},
        {R"(replace!('a' 'b' "c"))", R"("c")"},
        {R"(replace!('a' 'b' "ab_ba"))", R"("bb_bb")"},
    });
    test.evaluate("replace_if list", {
        {"replace_if!(in x out equal?(x 1) 2 ())", "()"},
        {"replace_if!(in x out equal?(x 1) 2 (1))", "(2)"},
        {"replace_if!(in x out equal?(x 1) 2 (2))", "(2)"},
        {"replace_if!(in x out equal?(x 1) 2 (0 1 0 1 1))", "(0 2 0 2 2)"},
    });
    test.evaluate("replace_if string", {
        {R"(replace_if!(in x out equal?(x 'a') 'b' ""))", R"("")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "a"))", R"("b")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "c"))", R"("c")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "ab_ba"))", R"("bb_bb")"},
    });
    test.evaluate("enumerate list", {
        {"enumerate!()", "()"},
        {"enumerate!(4)", "({index=0 item=4})"},
        {"enumerate!(4 3)", "({index=0 item=4} {index=1 item=3})"},
    });
    test.evaluate("enumerate string", {
        {R"(enumerate!"")", R"(())"},
        {R"(enumerate!"a")", R"(({index=0 item='a'}))"},
        {R"(enumerate!"ab")", R"(({index=0 item='a'} {index=1 item='b'}))"},
    });
    test.evaluate("concat list", {
        {"concat!(() ())", "()"},
        {"concat!(() (2))", "(2)"},
        {"concat!((1) ())", "(1)"},
        {"concat!((1) (2))", "(1 2)"},
        {"concat!((1 2 3) (4 5 6))", "(1 2 3 4 5 6)"},
    });
    test.evaluate("concat string", {
        {R"(concat!("" ""))", R"("")"},
        {R"(concat!("" "b"))", R"("b")"},
        {R"(concat!("a" ""))", R"("a")"},
        {R"(concat!("a" "b"))", R"("ab")"},
        {R"(concat!("abc" "def"))", R"("abcdef")"},
    });
    test.evaluate("drop list", {
        {"drop!(0 (3 7 6))", "(3 7 6)"},
        {"drop!(1 (3 7 6))", "(7 6)"},
        {"drop!(2 (3 7 6))", "(6)"},
        {"drop!(3 (3 7 6))", "()"},
    });
    test.evaluate("drop string", {
        {R"(drop!(0 "abc"))", R"("abc")"},
        {R"(drop!(1 "abc"))", R"("bc")"},
        {R"(drop!(2 "abc"))", R"("c")"},
        {R"(drop!(3 "abc"))", R"("")"},
    });
    test.evaluate("take list", {
        {"take!(0 (3 7 6))", "()"},
        {"take!(1 (3 7 6))", "(3)"},
        {"take!(2 (3 7 6))", "(3 7)"},
        {"take!(3 (3 7 6))", "(3 7 6)"},
    });
    test.evaluate("take string", {
        {R"(take!(0 "abc"))", R"("")"},
        {R"(take!(1 "abc"))", R"("a")"},
        {R"(take!(2 "abc"))", R"("ab")"},
        {R"(take!(3 "abc"))", R"("abc")"},
    });
    test.evaluate("get_index list", {
        {"get_index!(0 (3 7 6))", "3"},
        {"get_index!(1 (3 7 6))", "7"},
        {"get_index!(2 (3 7 6))", "6"},
    });
    test.evaluate("get_index string", {
        {R"(get_index!(0 "abc"))", R"('a')"},
        {R"(get_index!(1 "abc"))", R"('b')"},
        {R"(get_index!(2 "abc"))", R"('c')"},
    });
    return test.exitCode();
}
