#include "mang_lang.h"
#include <chrono>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct Test {
    Test() = default;
    ~Test() {
        using namespace std;
        cout << endl << num_good_total << "/" << num_good_total + num_bad_total
            << " tests successful in total. ";
        if (num_bad_total != 0) {
            cout << num_bad_total << " TESTS FAILING! ";
        }
        cout << "Duration " << std::fixed << std::setprecision(1)
            << duration_total.count() << " seconds.";
    }
    size_t exitCode() {
        return num_bad_total;
    }
    size_t num_good_total= 0;
    size_t num_bad_total = 0;
    std::chrono::duration<double> duration_total = {};

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
        auto start = std::chrono::steady_clock::now();
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
        auto end = std::chrono::steady_clock::now();
        duration_total += end - start;
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
    void evaluate_types(std::string case_name, InputOutputList data) {
        parameterizedTest<std::string, std::string>(
            ::evaluate_types, "evaluate types", case_name, data
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
    test.evaluate_types("number", {
        {"-1", "NUMBER"},
        {"-1.0", "NUMBER"},
        {"-0", "NUMBER"},
        {"-0.0", "NUMBER"},
        {"0", "NUMBER"},
        {"0.0", "NUMBER"},
        {"+0", "NUMBER"},
        {"+0.0", "NUMBER"},
        {"1", "NUMBER"},
        {"1.0", "NUMBER"},
        {"+1", "NUMBER"},
        {"+1.0", "NUMBER"},
    });
    test.evaluate("character", {
        {R"(\a)", R"(\a)"},
        {R"(\1)", R"(\1)"},
        {R"(\+)", R"(\+)"},
        {R"(\-)", R"(\-)"},
        {R"(\")", R"(\")"},
        {R"(\))", R"(\))"},
        {R"(\))", R"(\))"},
        {R"(\{)", R"(\{)"},
        {R"(\})", R"(\})"},
    });
    test.evaluate_types("character", {
        {R"(\a)", "CHARACTER"},
        {R"(\1)", "CHARACTER"},
        {R"(\+)", "CHARACTER"},
        {R"(\-)", "CHARACTER"},
        {R"(\")", "CHARACTER"},
        {R"(\))", "CHARACTER"},
        {R"(\))", "CHARACTER"},
        {R"(\{)", "CHARACTER"},
        {R"(\})", "CHARACTER"},
    });
    test.evaluate("boolean", {
        {"yes", "yes"},
        {"no", "no"},
    });
    test.evaluate_types("boolean", {
        {"yes", "BOOLEAN"},
        {"no", "BOOLEAN"},
    });
    test.evaluate("missing", {
        {"missing", "missing"},
    });
    test.evaluate_types("missing", {
        {"missing", "EMPTY"},
    });
    test.evaluate("label", {
        {"'a'", "'a'"},
        {"'ab'", "'ab'"},
        {"'a1'", "'a1'"},
        {"'_'", "'_'"},
    });
    test.evaluate_types("label", {
        {"'a'", "LABEL"},
        {"'ab'", "LABEL"},
        {"'a1'", "LABEL"},
        {"'_'", "LABEL"},
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
    test.evaluate_types("string", {
        {R"("")", "EMPTY_STRING"},
        {R"("a")", "STRING"},
        {R"("ab")", "STRING"},
        {R"("abc")", "STRING"},
        {R"("1")", "STRING"},
        {R"("-1")", "STRING"},
        {R"("+1")", "STRING"},
        {R"#("()")#", "STRING"},
        {R"("{}")", "STRING"},
    });
    test.evaluate("stack", {
        {"[]", "[]"},
        {"[ ]", "[]"},
        {"[  ]", "[]"},
        {"[1]", "[1]"},
        {"[ 1]", "[1]"},
        {"[1 ]", "[1]"},
        {"[ 1 ]", "[1]"},
        {"[1 2]", "[1 2]"},
        {"[1  2]", "[1 2]"},
        {"[ 1  2 ]", "[1 2]"},
        {"[[]]", "[[]]"},
        {"[[] []]", "[[] []]"},
        {"[[[]]]", "[[[]]]"},
    });
    test.evaluate_types("stack", {
        {"[]", "EMPTY_STACK"},
        {"[ ]", "EMPTY_STACK"},
        {"[  ]", "EMPTY_STACK"},
        {"[1]", "[NUMBER]"},
        {"[ 1]", "[NUMBER]"},
        {"[1 ]", "[NUMBER]"},
        {"[ 1 ]", "[NUMBER]"},
        {"[1 2]", "[NUMBER]"},
        {"[1  2]", "[NUMBER]"},
        {"[ 1  2 ]", "[NUMBER]"},
        {"[[]]", "[EMPTY_STACK]"},
        {"[[] []]", "[EMPTY_STACK]"},
        {"[[[]]]", "[[EMPTY_STACK]]"},
    });
    test.reformat("tuple", {
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
    test.evaluate_types("tuple", {
        {"()", "()"},
        {"( )", "()"},
        {"(  )", "()"},
        {"(1)", "(NUMBER)"},
        {"( 1)", "(NUMBER)"},
        {"(1 )", "(NUMBER)"},
        {"( 1 )", "(NUMBER)"},
        {"(1 2)", "(NUMBER NUMBER)"},
        {"(1  2)", "(NUMBER NUMBER)"},
        {"( 1  2 )", "(NUMBER NUMBER)"},
        {"(())", "(())"},
        {"(() ())", "(() ())"},
        {"((()))", "((()))"},
    });
    test.evaluate("tuple", {
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
    test.evaluate_types("dictionary iteration", {
        {"{while 1 end}", "{}"},
        {"{i=2 while i i=dec!i end}", "{i=NUMBER}"},
        {"{i=10 while i i=dec!i end j=1}", "{i=NUMBER j=NUMBER}"},
    });
    test.evaluate("dictionary iterations", {
        {"{i=2 while i i=dec!i end}", "{i=0}"},
        {"{i=2 while i i=dec!i end j=1}", "{i=0 j=1}"},
        {"{i=2 tot=0 while i tot=add!(tot i) i=dec!i end}", "{i=0 tot=3}"},
        {"{i=1000 tot=0 while i tot=add!(tot i) i=dec!i end}", "{i=0 tot=500500}"},
    });
    test.evaluate_types("dictionary", {
        {"{}", "{}"},
        {"{ }", "{}"},
        {"{a=1}", "{a=NUMBER}"},
        {"{a0=1}", "{a0=NUMBER}"},
        {"{a_0=1}", "{a_0=NUMBER}"},
        {"{ a = 1 }", "{a=NUMBER}"},
        {"{a=1 a=2}", "{a=NUMBER}"},
        {"{a=1 b=2}", "{a=NUMBER b=NUMBER}"},
        {"{ a = 1  b = 2 }", "{a=NUMBER b=NUMBER}"},
        {"{a=1 b=a}", "{a=NUMBER b=NUMBER}"},
    });
    test.evaluate("dictionary", {
        {"{}", "{}"},
        {"{ }", "{}"},
        {"{a=1}", "{a=1}"},
        {"{a0=1}", "{a0=1}"},
        {"{a_0=1}", "{a_0=1}"},
        {"{ a = 1 }", "{a=1}"},
        {"{a=1 a=2}", "{a=2}"},
        {"{a=1 b=2}", "{a=1 b=2}"},
        {"{ a = 1  b = 2 }", "{a=1 b=2}"},
        {"{a=1 b=a}", "{a=1 b=1}"},
        {R"({a=1})", R"({a=1})"},
    });
    test.reformat("conditional", {
        {"if 1 then 2 else 3", "if 1 then 2 else 3"},
        {"if  1  then  2  else  3", "if 1 then 2 else 3"},
        {"if a then b else c", "if a then b else c"},
    });
    test.evaluate_types("conditional", {
        {"if 1 then 2 else 3", "NUMBER"},
        {"if 0 then 2 else 3", "NUMBER"},
        {"if [0] then 2 else 3", "NUMBER"},
        {"if [] then 2 else 3", "NUMBER"},
    });
    test.evaluate("conditional", {
        {"if 1 then 2 else 3", "2"},
        {"if 0 then 2 else 3", "3"},
        {"if [0] then 2 else 3", "2"},
        {"if [] then 2 else 3", "3"},
    });
    test.reformat("is", {
        {"is 0 0 then 0 else 0", "is 0 0 then 0 else 0"},
        {"is 0 0 then 0 1 then 1 else 0", "is 0 0 then 0 1 then 1 else 0"},
    });
    test.evaluate_types("is", {
        {"is 0 0 then 1 else 2", "NUMBER"},
        {"is 1 0 then 1 else 2", "NUMBER"},
        {"is 0 0 then 0 1 then 1 2 then 4 else 5", "NUMBER"},
        {"is 1 0 then 0 1 then 1 2 then 4 else 5", "NUMBER"},
        {"is 2 0 then 0 1 then 1 2 then 4 else 5", "NUMBER"},
        {"is 3 0 then 0 1 then 1 2 then 4 else 5", "NUMBER"},
        {"is missing missing then 1 else 2", "NUMBER"},
        {"is missing 0 then 1 else 2", "NUMBER"},
        {"is 0 missing then 1 else 2", "NUMBER"},
    });
    test.evaluate("is", {
        {"is 0 0 then 1 else 2", "1"},
        {"is 1 0 then 1 else 2", "2"},
        {"is 0 0 then 0 1 then 1 2 then 4 else 5", "0"},
        {"is 1 0 then 0 1 then 1 2 then 4 else 5", "1"},
        {"is 2 0 then 0 1 then 1 2 then 4 else 5", "4"},
        {"is 3 0 then 0 1 then 1 2 then 4 else 5", "5"},
        {"is missing missing then 1 else 2", "1"},
        {"is missing 0 then 1 else 2", "2"},
        {"is 0 missing then 1 else 2", "2"},
    });
    test.reformat("symbol", {
        {"a", "a"},
        {"{a=1 b=a}", "{a=1 b=a}"},
    });
    test.evaluate_types("symbol", {
        {"{a=1 b=a}", "{a=NUMBER b=NUMBER}"},
        {"{a=1 b_0=a}", "{a=NUMBER b_0=NUMBER}"},
        {"{a=1 b={c=a}}", "{a=NUMBER b={c=NUMBER}}"},
        {"{a=1 b={c={d=a}}}", "{a=NUMBER b={c={d=NUMBER}}}"},
        {"{a=1 b=[a]}", "{a=NUMBER b=[NUMBER]}"},
        {"{a=1 b=[[a]]}", "{a=NUMBER b=[[NUMBER]]}"},
        {"{a=1 b=c@{c=a}}", "{a=NUMBER b=NUMBER}"},
        {"{a=1 b=add!(a a)}", "{a=NUMBER b=NUMBER}"},
        {"{a=1 b=if a then a else 2}", "{a=NUMBER b=NUMBER}"},
    });
    test.evaluate("symbol", {
        {"{a=1 b=a}", "{a=1 b=1}"},
        {"{a=1 b_0=a}", "{a=1 b_0=1}"},
        {"{a=1 b={c=a}}", "{a=1 b={c=1}}"},
        {"{a=1 b={c={d=a}}}", "{a=1 b={c={d=1}}}"},
        {"{a=1 b=[a]}", "{a=1 b=[1]}"},
        {"{a=1 b=[[a]]}", "{a=1 b=[[1]]}"},
        {"{a=1 b=c@{c=a}}", "{a=1 b=1}"},
        {"{a=1 b=add!(a a)}", "{a=1 b=2}"},
        {"{a=1 b=if a then a else 2}", "{a=1 b=1}"},
    });
    test.reformat("child_symbol", {
        {"a@{a=1}", "a@{a=1}"},
        {"A_0@{A_0=1}", "A_0@{A_0=1}"},
    });
    test.evaluate_types("child_symbol", {
        {"a@{a=1}", "NUMBER"},
        {"A_0@{A_0=1}", "NUMBER"},
        {"_0@{_0=1}", "NUMBER"},
        {"y@{x=5 y=x}", "NUMBER"},
        {"c@{a=1 b=a c=b}", "NUMBER"},
        {"y@x@{x = {y = 1}}", "NUMBER"},
        {"b@x@{x={a=no b=2}}", "NUMBER"},
        {"z@y@x@{x = {y = {z = 1}}}", "NUMBER"},
        {"w@z@y@x@{x = {y = {z = {w=1}}}}", "NUMBER"},
        {"b@{a={f=in x out 1} g=f@a b=g!3}", "NUMBER"},
        {"b@{a={f = in x out inc!x} g=f@a b = g!3}", "NUMBER"},
        {"c@{a={b={f=in x out inc!x}} g=f@b@a c = g!3}", "NUMBER"},
        {"ABBA@{ABBA = 1}", "NUMBER"},
        {"ABBA@{ABBA = 1 PADDA = no}", "NUMBER"},
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
        {"add!(1 2)", "add!(1 2)"},
    });
    test.evaluate_types("function", {
        {"in x out x",       "FUNCTION"},
    });
    test.evaluate("function", {
        {"in x out x",       "in x out x"},
        {"f@{f=in x out x}", "in x out x"},
    });
    test.evaluate_types("function dictionary", {
        {"in {x} out x", "FUNCTION_DICTIONARY"},
        {"in {x y} out x", "FUNCTION_DICTIONARY"},
        {"in  {  x    y  }  out  x", "FUNCTION_DICTIONARY"},
    });
    test.evaluate("function dictionary", {
        {"in {x} out x", "in {x} out x"},
        {"in {x y} out x", "in {x y} out x"},
        {"in  {  x    y  }  out  x", "in {x y} out x"},
    });
    test.evaluate_types("function tuple", {
        {"in (x) out x", "FUNCTION_TUPLE"},
        {"in (x y) out x", "FUNCTION_TUPLE"},
        {"in  (  x    y  )  out  x", "FUNCTION_TUPLE"},
    });
    test.evaluate("function tuple", {
        {"in (x) out x", "in (x) out x"},
        {"in (x y) out x", "in (x y) out x"},
        {"in  (  x    y  )  out  x", "in (x y) out x"},
    });
    test.evaluate_types("lookup function", {
        {"a@{f=in x out x a=f!0}", "NUMBER"},
        {"a@{f=in x out x a=f![]}", "EMPTY_STACK"},
        {"a@{f=in x out 1 a=f!0}", "NUMBER"},
        {"z@{f=in x out y@{y = 3} z=f!2}", "NUMBER"},
        {"z@{f=in x out result@{y=3 result=add!(x y)}  z=f!2}", "NUMBER"},
        {"z@{y=2 f=in x out result@{y=3 result=add!(x y)} z=f!2}", "NUMBER"},
        {"x@{a={b=1 f=in x out b} b=2 f=f@a x=f![]}", "NUMBER"},
        {"x@{f=in a out 1 g = in b out f!b x = g!2}", "NUMBER"},
        {"y@{apply=in (f x) out f!x y = apply!(inc 2)}", "NUMBER"},
        {"y@{apply=in (f x) out f!x id=in x out apply!(in x out x x) y = id!1}", "NUMBER"},
        {"y@{f=in stack out map!(in y out 2 stack) y=f![0 0]}", "[NUMBER]"},
        {"a@{call=in f out f![] g=in x out 0 a=call!g}", "NUMBER"},
        {"a@{call=in f out f![] b={a=0 g=in x out a} g=g@b a=call!g}", "NUMBER"},
        {"a@{call=in(f)out f!([]) b={a=0 g=in(x)out a} g=g@b a=call!(g)}", "NUMBER"},
        {"y@{a=1 g=in y out a f=in stack out map!(g stack) y=f![0 0]}", "[NUMBER]"},
        {"y@{a=1 f=in stack out map!(in y out a stack) y=f![0 0]}", "[NUMBER]"},
        {"b@{a={a=0 f=in x out a} g=f@a b=g!1}", "NUMBER"},
        {"y@{f=in (x stack) out map!(in y out x stack) y=f!(2 [0 0])}", "[NUMBER]"},
    });
    test.evaluate("lookup function", {
        {"a@{f=in x out x a=f!0}", "0"},
        {"a@{f=in x out x a=f![]}", "[]"},
        {"a@{f=in x out 1 a=f!0}", "1"},
        {"z@{f=in x out y@{y = 3} z=f!2}", "3"},
        {"z@{f=in x out result@{y=3 result=add!(x y)}  z=f!2}", "5"},
        {"z@{y=2 f=in x out result@{y=3 result=add!(x y)} z=f!2}", "5"},
        {"x@{a={b=1 f=in x out b} b=2 f=f@a x=f![]}", "1"},
        {"x@{f=in a out 1 g = in b out f!b x = g!2}", "1"},
        {"y@{apply=in (f x) out f!x y = apply!(inc 2)}", "3"},
        {"y@{apply=in (f x) out f!x id=in x out apply!(in x out x x) y = id!1}", "1"},
        {"y@{f=in stack out map!(in y out 2 stack) y=f![0 0]}", "[2 2]"},
        {"a@{call=in f out f![] g=in x out 0 a=call!g}", "0"},
        {"a@{call=in f out f![] b={a=0 g=in x out a} g=g@b a=call!g}", "0"},
        {"a@{call=in(f)out f!([]) b={a=0 g=in(x)out a} g=g@b a=call!(g)}", "0"},
        {"y@{a=1 g=in y out a f=in stack out map!(g stack) y=f![0 0]}", "[1 1]"},
        {"y@{a=1 f=in stack out map!(in y out a stack) y=f![0 0]}", "[1 1]"},
        {"b@{a={a=0 f=in x out a} g=f@a b=g!1}", "0"},
        {"y@{f=in (x stack) out map!(in y out x stack) y=f!(2 [0 0])}", "[2 2]"},
    });
    test.evaluate("recursive function", {
        {"y@{f=in x out if x then add!(x f!dec!x) else 0 y=f!3}", "6"},
    });
    test.evaluate_types("lookup function dictionary", {
        {"a@{f=in {x} out x a=f!{x=0}}", "NUMBER"},
        {"a@{f=in {x y} out add!(x y) a=f!{x=2 y=3}}", "NUMBER"},
        {"a@{b=2 f=in {x} out add!(b x) a=f!{x=0}}", "NUMBER"},
    });
    test.evaluate("lookup function dictionary", {
        {"a@{f=in {x} out x a=f!{x=0}}", "0"},
        {"a@{f=in {x y} out add!(x y) a=f!{x=2 y=3}}", "5"},
        {"a@{b=2 f=in {x} out add!(b x) a=f!{x=0}}", "2"},
    });
    test.evaluate_types("lookup function tuple", {
        {"a@{f=in (x) out x a=f!(0)}", "NUMBER"},
        {"a@{f=in (x y) out add!(x y) a=f!(2 3)}", "NUMBER"},
    });
    test.evaluate("lookup function tuple", {
        {"a@{f=in (x) out x a=f!(0)}", "0"},
        {"a@{f=in (x y) out add!(x y) a=f!(2 3)}", "5"},
    });
    test.evaluate("add", {
        {"add!(1 0)", "1"},
        {"add!(0 1)", "1"},
        {"add!(-1 +1)", "0"},
    });
    test.evaluate("mul", {
        {"mul!(0 1)", "0"},
        {"mul!(1 2)", "2"},
        {"mul!(2 3)", "6"},
        {"mul!(-1 +1)", "-1"},
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
        {"less?(0 0)", "no"},
        {"less?(0 1)", "yes"},
        {"less?(-1 0)", "yes"},
        {"less?(1 0)", "no"},
        {"less?(0 -1)", "no"},
        {"less?(1 1)", "no"},
        {"less?(-1 -1)", "no"},
    });
    test.evaluate("is_increasing", {
        {"is_increasing?[0 0]", "yes"},
        {"is_increasing?[0 1]", "yes"},
        {"is_increasing?[-1 0]", "yes"},
        {"is_increasing?[1 0]", "no"},
        {"is_increasing?[0 -1]", "no"},
        {"is_increasing?[1 1]", "yes"},
        {"is_increasing?[-1 -1]", "yes"},
        {"is_increasing?[]", "yes"},
        {"is_increasing?[0]", "yes"},
        {"is_increasing?[0 1 2]", "yes"},
        {"is_increasing?[0 1 2 3]", "yes"},
        {"is_increasing?[0 1 2 3 3]", "yes"},
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
        {R"(number!\0)", "48"},
        {R"(number!\9)", "57"},
        {R"(number!\A)", "65"},
        {R"(number!\Z)", "90"},
        {R"(number!\a)", "97"},
        {R"(number!\z)", "122"},
    });
    test.evaluate("character", {
        {"character!48", R"(\0)"},
        {"character!57", R"(\9)"},
        {"character!65", R"(\A)"},
        {"character!90", R"(\Z)"},
        {"character!97", R"(\a)"},
        {"character!122", R"(\z)"},
    });
    test.evaluate_types("number constants", {
        {"inf", "NUMBER"},
        {"-inf", "NUMBER"},
        {"nan", "NUMBER"},
        {"pi", "NUMBER"},
        {"tau", "NUMBER"},
    });
    test.evaluate("number constants", {
        {"inf", "inf"},
        {"-inf", "-inf"},
        {"nan", "nan"},
        {"pi", "3.14159265359"},
        {"tau", "6.28318530718"},
    });
    test.evaluate("boolean", {
        {"boolean!-2", "yes"},
        {"boolean!-1", "yes"},
        {"boolean!-0", "no"},
        {"boolean!0", "no"},
        {"boolean!+0", "no"},
        {"boolean!1", "yes"},
        {"boolean!2", "yes"},
        {"boolean![]", "no"},
        {"boolean![0]", "yes"},
        {"boolean![0 1]", "yes"},
        {"boolean!{}", "no"},
        {"boolean!{x=0}", "yes"},
        {"boolean!{x=0 y=1}", "yes"},
        {R"(boolean!"")", "no"},
        {R"(boolean!"0")", "yes"},
        {R"(boolean!"1")", "yes"},
        {R"(boolean!"a")", "yes"},
        {R"(boolean!"ab")", "yes"},
    });
    test.evaluate("not", {
        {"not?-2", "no"},
        {"not?-1", "no"},
        {"not?-0", "yes"},
        {"not?0", "yes"},
        {"not?+0", "yes"},
        {"not?1", "no"},
        {"not?2", "no"},
        {"not?[]", "yes"},
        {"not?[0]", "no"},
        {"not?[0 1]", "no"},
        {"not?{}", "yes"},
        {"not?{x=0}", "no"},
        {"not?{x=0 y=1}", "no"},
        {R"(not?"")", "yes"},
        {R"(not?"0")", "no"},
        {R"(not?"1")", "no"},
        {R"(not?"a")", "no"},
        {R"(not?"ab")", "no"},
    });
    test.evaluate("and", {
        {"and?[no no]", "no"},
        {"and?[no yes]", "no"},
        {"and?[yes no]", "no"},
        {"and?[yes yes]", "yes"},
    });
    test.evaluate("or", {
        {"or?[no no]", "no"},
        {"or?[no yes]", "yes"},
        {"or?[yes no]", "yes"},
        {"or?[yes yes]", "yes"},
    });
    test.evaluate("all", {
        {"all?[]", "yes"},
        {"all?[0]", "no"},
        {"all?[1]", "yes"},
        {"all?[0 0]", "no"},
        {"all?[0 1]", "no"},
        {"all?[1 1]", "yes"},
    });
    test.evaluate_types("all", {
        {"all?[]", "BOOLEAN"},
        {"all?[0]", "BOOLEAN"},
        {"all?[1]", "BOOLEAN"},
        {"all?[0 0]", "BOOLEAN"},
        {"all?[0 1]", "BOOLEAN"},
        {"all?[1 1]", "BOOLEAN"},
    });
    test.evaluate("any", {
        {"any?[]", "no"},
        {"any?[0]", "no"},
        {"any?[1]", "yes"},
        {"any?[0 0]", "no"},
        {"any?[0 1]", "yes"},
        {"any?[1 1]", "yes"},
    });
    test.evaluate_types("any", {
        {"any?[]", "BOOLEAN"},
        {"any?[0]", "BOOLEAN"},
        {"any?[1]", "BOOLEAN"},
        {"any?[0 0]", "BOOLEAN"},
        {"any?[0 1]", "BOOLEAN"},
        {"any?[1 1]", "BOOLEAN"},
    });
    test.evaluate("none", {
        {"none?[]", "yes"},
        {"none?[0]", "yes"},
        {"none?[1]", "no"},
        {"none?[0 0]", "yes"},
        {"none?[0 1]", "no"},
        {"none?[1 1]", "no"},
    });
    test.evaluate_types("none", {
        {"none?[]", "BOOLEAN"},
        {"none?[0]", "BOOLEAN"},
        {"none?[1]", "BOOLEAN"},
        {"none?[0 0]", "BOOLEAN"},
        {"none?[0 1]", "BOOLEAN"},
        {"none?[1 1]", "BOOLEAN"},
    });
    test.evaluate("equal number", {
        {"equal?(0 0)", "yes"},
        {"equal?(0 1)", "no"},
        {"equal?(1 0)", "no"},
        {"equal?(1 1)", "yes"},
    });
    test.evaluate_types("equal number", {
        {"equal?(0 0)", "BOOLEAN"},
        {"equal?(0 1)", "BOOLEAN"},
        {"equal?(1 0)", "BOOLEAN"},
        {"equal?(1 1)", "BOOLEAN"},
    });
    test.evaluate("unequal number", {
        {"unequal?(0 0)", "no"},
        {"unequal?(0 1)", "yes"},
        {"unequal?(1 0)", "yes"},
        {"unequal?(1 1)", "no"},
    });
    test.evaluate("equal character", {
        {R"(equal?(\a \a))", "yes"},
        {R"(equal?(\a \b))", "no"},
        {R"(equal?(\b \a))", "no"},
        {R"(equal?(\b \b))", "yes"},
    });
    test.evaluate("unequal character", {
        {R"(unequal?(\a \a))", "no"},
        {R"(unequal?(\a \b))", "yes"},
        {R"(unequal?(\b \a))", "yes"},
        {R"(unequal?(\b \b))", "no"},
    });
    test.evaluate("equal boolean", {
        {"equal?(yes yes)", "yes"},
        {"equal?(yes no)", "no"},
        {"equal?(no yes)", "no"},
        {"equal?(no no)", "yes"},
    });
    test.evaluate("unequal boolean", {
        {"unequal?(yes yes)", "no"},
        {"unequal?(yes no)", "yes"},
        {"unequal?(no yes)", "yes"},
        {"unequal?(no no)", "no"},
    });
    test.evaluate("equal stack", {
        {"equal?([] [])", "yes"},
        {"equal?([1] [1])", "yes"},
        {"equal?([0] [1])", "no"},
        {"equal?([0 1] [0 1])", "yes"},
        {"equal?([0 1] [1 1])", "no"},
        {"equal?([0 1] [0])", "no"},
    });
    test.evaluate_types("equal stack", {
        {"equal?([] [])", "BOOLEAN"},
        {"equal?([1] [1])", "BOOLEAN"},
        {"equal?([0] [1])", "BOOLEAN"},
        {"equal?([0 1] [0 1])", "BOOLEAN"},
        {"equal?([0 1] [1 1])", "BOOLEAN"},
        {"equal?([0 1] [0])", "BOOLEAN"},
    });
    test.evaluate("unequal stack", {
        {"unequal?([] [])", "no"},
        {"unequal?([1] [1])", "no"},
        {"unequal?([0] [1])", "yes"},
        {"unequal?([0 1] [0 1])", "no"},
        {"unequal?([0 1] [1 1])", "yes"},
        {"unequal?([0 1] [0])", "yes"},
    });
    test.evaluate("equal string", {
        {R"(equal?("" ""))", "yes"},
        {R"(equal?("a" ""))", "no"},
        {R"(equal?("" "a"))", "no"},
        {R"(equal?("a" "a"))", "yes"},
        {R"(equal?("a" "b"))", "no"},
        {R"(equal?("ab" "ab"))", "yes"},
        {R"(equal?("abc" "ab"))", "no"},
        {R"(equal?("ab" "abc"))", "no"},
    });
    test.evaluate("unequal string", {
        {R"(unequal?("" ""))", "no"},
        {R"(unequal?("a" ""))", "yes"},
        {R"(unequal?("" "a"))", "yes"},
        {R"(unequal?("a" "a"))", "no"},
        {R"(unequal?("a" "b"))", "yes"},
        {R"(unequal?("ab" "ab"))", "no"},
        {R"(unequal?("abc" "ab"))", "yes"},
        {R"(unequal?("ab" "abc"))", "yes"},
    });
    test.evaluate_types("clear stack", {
        {"clear![]", "EMPTY_STACK"},
        {"clear![1]", "EMPTY_STACK"},
        {"clear![1 2]", "EMPTY_STACK"},
    });
    test.evaluate("clear stack", {
        {"clear![]", "[]"},
        {"clear![1]", "[]"},
        {"clear![1 2]", "[]"},
    });
    test.evaluate_types("clear string", {
        {R"(clear!"")", "EMPTY_STRING"},
        {R"(clear!"a")", "EMPTY_STRING"},
        {R"(clear!"ab")", "EMPTY_STRING"},
    });
    test.evaluate("clear string", {
        {R"(clear!"")", R"("")"},
        {R"(clear!"a")", R"("")"},
        {R"(clear!"ab")", R"("")"},
    });
    test.evaluate_types("top stack", {
        {"top@[4]", "NUMBER"},
        {"top@[3 4]", "NUMBER"},
    });
    test.evaluate("top stack", {
        {"top@[4]", "4"},
        {"top@[3 4]", "3"},
    });
    test.evaluate_types("top string", {
        {R"(top@"b")", "CHARACTER"},
        {R"(top@"ab")", "CHARACTER"},
    });
    test.evaluate("top string", {
        {R"(top@"b")", R"(\b)"},
        {R"(top@"ab")", R"(\a)"},
    });
    test.evaluate_types("rest stack", {
        {"rest@[4]", "[NUMBER]"},
        {"rest@[4 3]", "[NUMBER]"},
        {"rest@[4 3 7]", "[NUMBER]"},
    });
    test.evaluate("rest stack", {
        {"rest@[4]", "[]"},
        {"rest@[4 3]", "[3]"},
        {"rest@[4 3 7]", "[3 7]"},
    });
    test.evaluate_types("rest string", {
        {R"(rest@"a")", "STRING"},
        {R"(rest@"ab")", "STRING"},
        {R"(rest@"abc")", "STRING"},
    });
    test.evaluate("rest string", {
        {R"(rest@"a")", R"("")"},
        {R"(rest@"ab")", R"("b")"},
        {R"(rest@"abc")", R"("bc")"},
    });
    test.evaluate_types("put stack", {
        {"put!(3 [])", "[NUMBER]"},
        {"put!(4 [5])", "[NUMBER]"},
        {"put!(4 [6 8])", "[NUMBER]"},
    });
    test.evaluate("put stack", {
        {"put!(3 [])", "[3]"},
        {"put!(4 [5])", "[4 5]"},
        {"put!(4 [6 8])", "[4 6 8]"},
    });
    test.evaluate_types("put string", {
        {R"(put!(\a ""))", "STRING"},
        {R"(put!(\a "b"))", "STRING"},
        {R"(put!(\a "bc"))", "STRING"},
    });
    test.evaluate("put string", {
        {R"(put!(\a ""))", R"("a")"},
        {R"(put!(\a "b"))", R"("ab")"},
        {R"(put!(\a "bc"))", R"("abc")"},
    });/*
    test.evaluate("get dictionary", {
        {R"(get!(0 {<0>=1}))", "1"},
        {R"(get!("a" {<"a">=1}))", "1"},
        {R"(get!('a' {a=1}))", "1"},
        {R"(get!('a' {<'a'>=1}))", "1"},
        {R"(get!('a' {}))", "missing"},
    });
    test.evaluate("get_names", {
        {"get_names!{}", "[]"},
        {"get_names!{a=1}", "['a']"},
        {"get_names!{<0>=1}", "[0]"},
        {R"(get_names!{<"a">=1})", R"(["a"])"},
        {"get!(top@get_names!{a=0} {a=1})", "1"},
    });
    test.evaluate("get_values", {
        {"get_values!{a=0}", "[0]"},
        {"get_values!{a=0 b=1 c=2}", "[0 1 2]"},
        {"get_values!{a='a' b='b' c='c'}", "['a' 'b' 'c']"},
    });
    test.evaluate("get_items", {
        {"get_items!{a=0}", "[('a' 0)]"},
        {"get_items!{a=0 b=1 c=2}", "[('a' 0) ('b' 1) ('c' 2)]"},
        {"get_items!{a='a' b='b' c='c'}", "[('a' 'a') ('b' 'b') ('c' 'c')]"},
    });*/
    test.evaluate("inc", {
        {"inc!0", "1"},
    });
    test.evaluate("dec", {
        {"dec!0", "-1"},
    });
    test.evaluate_types("is_digit", {
        {R"(is_digit?\A)", "BOOLEAN"},
    });
    test.evaluate("is_digit", {
        {R"(is_digit?\A)", "no"},
        {R"(is_digit?\Z)", "no"},
        {R"(is_digit?\a)", "no"},
        {R"(is_digit?\z)", "no"},
        {R"(is_digit?\0)", "yes"},
        {R"(is_digit?\9)", "yes"},
    });
    test.evaluate_types("is_upper", {
        {R"(is_upper?\A)", "BOOLEAN"},
    });
    test.evaluate("is_upper", {
        {R"(is_upper?\A)", "yes"},
        {R"(is_upper?\Z)", "yes"},
        {R"(is_upper?\a)", "no"},
        {R"(is_upper?\z)", "no"},
        {R"(is_upper?\0)", "no"},
        {R"(is_upper?\9)", "no"},
    });
    test.evaluate_types("is_lower", {
        {R"(is_lower?\A)", "BOOLEAN"},
    });
    test.evaluate("is_lower", {
        {R"(is_lower?\A)", "no"},
        {R"(is_lower?\Z)", "no"},
        {R"(is_lower?\a)", "yes"},
        {R"(is_lower?\z)", "yes"},
        {R"(is_lower?\0)", "no"},
        {R"(is_lower?\9)", "no"},
    });
    test.evaluate_types("is_letter", {
        {R"(is_letter?\A)", "BOOLEAN"},
    });
    test.evaluate("is_letter", {
        {R"(is_letter?\A)", "yes"},
        {R"(is_letter?\Z)", "yes"},
        {R"(is_letter?\a)", "yes"},
        {R"(is_letter?\z)", "yes"},
        {R"(is_letter?\0)", "no"},
        {R"(is_letter?\9)", "no"},
    });
    test.evaluate("to_upper", {
        {R"(to_upper!\A)", R"(\A)"},
        {R"(to_upper!\Z)", R"(\Z)"},
        {R"(to_upper!\a)", R"(\A)"},
        {R"(to_upper!\z)", R"(\Z)"},
        {R"(to_upper!\0)", R"(\0)"},
        {R"(to_upper!\9)", R"(\9)"},
    });
    test.evaluate("to_lower", {
        {R"(to_lower!\A)", R"(\a)"},
        {R"(to_lower!\Z)", R"(\z)"},
        {R"(to_lower!\a)", R"(\a)"},
        {R"(to_lower!\z)", R"(\z)"},
        {R"(to_lower!\0)", R"(\0)"},
        {R"(to_lower!\9)", R"(\9)"},
    });
    test.evaluate("parse_digit", {
        {R"(parse_digit!\0)", "0"},
        {R"(parse_digit!\1)", "1"},
        {R"(parse_digit!\2)", "2"},
        {R"(parse_digit!\3)", "3"},
        {R"(parse_digit!\4)", "4"},
        {R"(parse_digit!\5)", "5"},
        {R"(parse_digit!\6)", "6"},
        {R"(parse_digit!\7)", "7"},
        {R"(parse_digit!\8)", "8"},
        {R"(parse_digit!\9)", "9"},
    });
    test.evaluate_types("min", {
        {"min![]", "NUMBER"},
        {"min![0]", "NUMBER"},
        {"min![0 1]", "NUMBER"},
        {"min![1 0]", "NUMBER"},
        {"min![3 6 1]", "NUMBER"},
        {"min![7 -3 8 -9]", "NUMBER"},
    });
    test.evaluate("min", {
        {"min![]", "inf"},
        {"min![0]", "0"},
        {"min![0 1]", "0"},
        {"min![1 0]", "0"},
        {"min![3 6 1]", "1"},
        {"min![7 -3 8 -9]", "-9"},
    });
    test.evaluate_types("max", {
        {"max![]", "NUMBER"},
        {"max![0]", "NUMBER"},
        {"max![0 1]", "NUMBER"},
        {"max![1 0]", "NUMBER"},
        {"max![3 6 1]", "NUMBER"},
        {"max![7 -3 8 -9]", "NUMBER"},
    });
    test.evaluate("max", {
        {"max![]", "-inf"},
        {"max![0]", "0"},
        {"max![0 1]", "1"},
        {"max![1 0]", "1"},
        {"max![3 6 1]", "6"},
        {"max![7 -3 8 -9]", "8"},
    });
    test.evaluate_types("range", {
        {"range!0", "[NUMBER]"},
        {"range!1", "[NUMBER]"},
        {"range!2", "[NUMBER]"},
        {"range!3", "[NUMBER]"},
    });
    test.evaluate("range", {
        {"range!0", "[]"},
        {"range!1", "[0]"},
        {"range!2", "[0 1]"},
        {"range!3", "[0 1 2]"},
    });
    test.evaluate("iteration", {
        {"count!range!100", "100"},
        {"sum!range!100", "4950"},
    });
    test.evaluate_types("count stack", {
        {"count![]", "NUMBER"},
        {"count![[]]", "NUMBER"},
        {"count![[] []]", "NUMBER"},
    });
    test.evaluate("count stack", {
        {"count![]", "0"},
        {"count![[]]", "1"},
        {"count![[] []]", "2"},
    });
    test.evaluate_types("count string", {
        {R"(count!"")", "NUMBER"},
        {R"(count!"a")", "NUMBER"},
        {R"(count!"ab")", "NUMBER"},
    });
    test.evaluate("count string", {
        {R"(count!"")", "0"},
        {R"(count!"a")", "1"},
        {R"(count!"ab")", "2"},
    });
    test.evaluate_types("count_item stack", {
        {"count_item!(1 [])", "NUMBER"},
        {"count_item!(1 [1])", "NUMBER"},
        {"count_item!(1 [1 1])", "NUMBER"},
        {"count_item!(1 [1 0 1])", "NUMBER"},
    });
    test.evaluate("count_item stack", {
        {"count_item!(1 [])", "0"},
        {"count_item!(1 [1])", "1"},
        {"count_item!(1 [1 1])", "2"},
        {"count_item!(1 [1 0 1])", "2"},
    });
    test.evaluate("count_item string", {
        {R"(count_item!(\a ""))", "0"},
        {R"(count_item!(\a "a"))", "1"},
        {R"(count_item!(\a "aa"))", "2"},
        {R"(count_item!(\a "aba"))", "2"},
    });
    test.evaluate("count_if stack", {
        {"count_if!(in x out 0 [])", "0"},
        {"count_if!(in x out 0 [1])", "0"},
        {"count_if!(in x out 1 [])", "0"},
        {"count_if!(in x out 1 [1])", "1"},
        {"count_if!(in x out equal?(x 3) [1 2 3])", "1"},
        {"count_if!(in x out equal?(x 3) [3 2 3])", "2"},
    });
    test.evaluate("count_if string", {
        {R"(count_if!(in x out 0 ""))", "0"},
        {R"(count_if!(in x out 0 "a"))", "0"},
        {R"(count_if!(in x out 1 ""))", "0"},
        {R"(count_if!(in x out 1 "a"))", "1"},
        {R"(count_if!(in x out equal?(x \c) "abc"))", "1"},
        {R"(count_if!(in x out equal?(x \c) "cbc"))", "2"},
    });
    test.evaluate_types("reverse stack", {
        {"reverse![]", "[ANY]"},
        {"reverse![0]", "[NUMBER]"},
        {"reverse![0 1]", "[NUMBER]"},
        {"reverse![0 1 2]", "[NUMBER]"},
    });
    test.evaluate("reverse stack", {
        {"reverse![]", "[]"},
        {"reverse![0]", "[0]"},
        {"reverse![0 1]", "[1 0]"},
        {"reverse![0 1 2]", "[2 1 0]"},
    });
    test.evaluate_types("reverse string", {
        {R"(reverse!"")", "STRING"},
        {R"(reverse!"a")", "STRING"},
        {R"(reverse!"ab")", "STRING"},
        {R"(reverse!"abc")", "STRING"},
    });
    test.evaluate("reverse string", {
        {R"(reverse!"")", R"("")"},
        {R"(reverse!"a")", R"("a")"},
        {R"(reverse!"ab")", R"("ba")"},
        {R"(reverse!"abc")", R"("cba")"},
    });
    test.evaluate("map stack", {
        {"map!(inc [])", "[]"},
        {"map!(inc [0])", "[1]"},
        {"map!(inc [0 1])", "[1 2]"},
        {"map!(in x out 2 [0 0])", "[2 2]"},
        {"a@{b=2 f=in x out b a=map!(f [0 0])}", "[2 2]"},
    });
    test.evaluate_types("map stack", {
        {"map!(inc [])", "[NUMBER]"},
        {"map!(inc [0])", "[NUMBER]"},
        {"map!(inc [0 1])", "[NUMBER]"},
        {"map!(in x out 2 [0 0])", "[NUMBER]"},
        {"a@{b=2 f=in x out b a=map!(f [0 0])}", "[NUMBER]"},
    });
    test.evaluate("map_string", {
        {R"(map_string!(to_upper ""))", R"("")"},
        {R"(map_string!(to_upper "abc"))", R"("ABC")"},
    });
    test.evaluate_types("map_string", {
        {R"(map_string!(to_upper ""))", "STRING"},
        {R"(map_string!(to_upper "abc"))", "STRING"},
    });
    test.evaluate("clear_if stack", {
        {"clear_if!(in x out 1 [])", "[]"},
        {"clear_if!(in x out 1 [[]])", "[]"},
        {"clear_if!(in x out 0 [[]])", "[[]]"},
        {"clear_if!(in x out less?(x 5) [7 4 6 1 9 3 2])", "[7 6 9]"},
    });
    test.evaluate("clear_if string", {
        {R"(clear_if!(in x out 0 ""))", R"("")"},
        {R"(clear_if!(in x out 0 "a"))", R"("a")"},
        {R"(clear_if!(in x out 1 ""))", R"("")"},
        {R"(clear_if!(in x out 1 "a"))", R"("")"},
        {R"(clear_if!(in x out equal?(x \a) "a"))", R"("")"},
        {R"(clear_if!(in x out equal?(x \a) "ab"))", R"("b")"},
        {R"(clear_if!(in x out equal?(x \a) "ba"))", R"("b")"},
        {R"(clear_if!(in x out equal?(x \a) "bab"))", R"("bb")"},
    });
    test.evaluate("clear_item stack", {
        {"clear_item!(1 [])", "[]"},
        {"clear_item!(1 [0])", "[0]"},
        {"clear_item!(1 [1])", "[]"},
        {"clear_item!(1 [1 7 1 2 7 1 1])", "[7 2 7]"},
    });
    test.evaluate("replace stack", {
        {"replace!(1 [])", "[]"},
        {"replace!(1 [1])", "[1]"},
        {"replace!(1 [2])", "[1]"},
        {"replace!(1 [0 1 0 1 1])", "[1 1 1 1 1]"},
    });
    test.evaluate("replace string", {
        {R"(replace!(\a ""))", R"("")"},
        {R"(replace!(\a "a"))", R"("a")"},
        {R"(replace!(\a "c"))", R"("a")"},
        {R"(replace!(\a "ab_ba"))", R"("aaaaa")"},
    });
    test.evaluate("replace_item stack", {
        {"replace_item!(1 2 [])", "[]"},
        {"replace_item!(1 2 [1])", "[2]"},
        {"replace_item!(1 2 [2])", "[2]"},
        {"replace_item!(1 2 [0 1 0 1 1])", "[0 2 0 2 2]"},
    });
    test.evaluate("replace_item string", {
        {R"(replace_item!(\a \b ""))", R"("")"},
        {R"(replace_item!(\a \b "a"))", R"("b")"},
        {R"(replace_item!(\a \b "c"))", R"("c")"},
        {R"(replace_item!(\a \b "ab_ba"))", R"("bb_bb")"},
    });
    test.evaluate("replace_if stack", {
        {"replace_if!(in x out equal?(x 1) 2 [])", "[]"},
        {"replace_if!(in x out equal?(x 1) 2 [1])", "[2]"},
        {"replace_if!(in x out equal?(x 1) 2 [2])", "[2]"},
        {"replace_if!(in x out equal?(x 1) 2 [0 1 0 1 1])", "[0 2 0 2 2]"},
    });
    test.evaluate("replace_if string", {
        {R"(replace_if!(in x out equal?(x \a) \b ""))", R"("")"},
        {R"(replace_if!(in x out equal?(x \a) \b "a"))", R"("b")"},
        {R"(replace_if!(in x out equal?(x \a) \b "c"))", R"("c")"},
        {R"(replace_if!(in x out equal?(x \a) \b "ab_ba"))", R"("bb_bb")"},
    });
    test.evaluate_types("enumerate stack", {
        {"enumerate![]", "[(NUMBER ANY)]"},
        {"enumerate![4]", "[(NUMBER NUMBER)]"},
        {"enumerate![4 3]", "[(NUMBER NUMBER)]"},
    });
    test.evaluate("enumerate stack", {
        {"enumerate![]", "[]"},
        {"enumerate![4]", "[(0 4)]"},
        {"enumerate![4 3]", "[(0 4) (1 3)]"},
    });
    test.evaluate("enumerate string", {
        {R"(enumerate!"")", R"([])"},
        {R"(enumerate!"a")", R"([(0 \a)])"},
        {R"(enumerate!"ab")", R"([(0 \a) (1 \b)])"},
    });
    test.evaluate("sum", {
        {"sum![]", "0"},
        {"sum![1]", "1"},
        {"sum![1 2]", "3"},
        {"sum![1 2 3]", "6"},
    });
    test.evaluate_types("sum", {
        {"sum![]", "NUMBER"},
        {"sum![1]", "NUMBER"},
        {"sum![1 2]", "NUMBER"},
        {"sum![1 2 3]", "NUMBER"},
    });
    test.evaluate("product", {
        {"product![]", "1"},
        {"product![1]", "1"},
        {"product![1 2]", "2"},
        {"product![1 2 3]", "6"},
    });
    test.evaluate_types("product", {
        {"product![]", "NUMBER"},
        {"product![1]", "NUMBER"},
        {"product![1 2]", "NUMBER"},
        {"product![1 2 3]", "NUMBER"},
    });
    test.evaluate("put_each", {
        {"put_each!([] [])", "[]"},
        {"put_each!([] [2])", "[2]"},
        {"put_each!([1] [])", "[1]"},
        {"put_each!([1] [2])", "[1 2]"},
        {"put_each!([1 2] [3 4])", "[2 1 3 4]"},
        {"put_each!([1 2 3] [4 5 6])", "[3 2 1 4 5 6]"},
    });
    test.evaluate_types("put_each", {
        {"put_each!([] [])", "[ANY]"},
        {"put_each!([] [2])", "[ANY]"}, // TODO
        {"put_each!([1] [])", "[NUMBER]"},
        {"put_each!([1] [2])", "[NUMBER]"},
        {"put_each!([1 2] [3 4])", "[NUMBER]"},
        {"put_each!([1 2 3] [4 5 6])", "[NUMBER]"},
    });
    test.evaluate("put_each string", {
        {R"(put_each!("" ""))", R"("")"},
        {R"(put_each!("" "a"))", R"("a")"},
        {R"(put_each!("a" ""))", R"("a")"},
        {R"(put_each!("a" "b"))", R"("ab")"},
        {R"(put_each!("ab" "cd"))", R"("bacd")"},
        {R"(put_each!("abc" "def"))", R"("cbadef")"},
    });
    test.evaluate_types("put_each string", {
        {R"(put_each!("" ""))", "STRING"},
        {R"(put_each!("" "a"))", "STRING"},
        {R"(put_each!("a" ""))", "STRING"},
        {R"(put_each!("a" "b"))", "STRING"},
        {R"(put_each!("ab" "cd"))", "STRING"},
        {R"(put_each!("abc" "def"))", "STRING"},
    });
    test.evaluate("concat", {
        {"concat![[] []]", "[]"},
        {"concat![[] [2]]", "[2]"},
        {"concat![[1] []]", "[1]"},
        {"concat![[1] [2]]", "[1 2]"},
        {"concat![[1] [2 3] [4 5 6]]", "[1 2 3 4 5 6]"},
    });
    test.evaluate_types("concat", {
        {"concat![[] []]", "[ANY]"},
        {"concat![[] [2]]", "[ANY]"}, // TODO
        {"concat![[1] []]", "[NUMBER]"},
        {"concat![[1] [2]]", "[NUMBER]"},
        {"concat![[1] [2 3] [4 5 6]]", "[NUMBER]"},
    });
    test.evaluate("concat_strings", {
        {R"(concat_strings!["" ""])", R"("")"},
        {R"(concat_strings!["" "b"])", R"("b")"},
        {R"(concat_strings!["a" ""])", R"("a")"},
        {R"(concat_strings!["a" "b"])", R"("ab")"},
        {R"(concat_strings!["a" "bc" "def"])", R"("abcdef")"},
    });
    test.evaluate_types("concat_strings", {
        {R"(concat_strings!["" ""])", "STRING"},
        {R"(concat_strings!["" "b"])", "STRING"},
        {R"(concat_strings!["a" ""])", "STRING"},
        {R"(concat_strings!["a" "b"])", "STRING"},
        {R"(concat_strings!["a" "bc" "def"])", "STRING"},
    });
    test.evaluate("split", {
        {"split!(0 [])", "[[] []]"},
        {"split!(1 [])", "[[] []]"},
        {"split!(0 [1])", "[[] [1]]"},
        {"split!(1 [1 2])", "[[1] [2]]"},
        {"split!(2 [1 2 3 4])", "[[2 1] [3 4]]"},
    });
    test.evaluate("drop stack", {
        {"drop!(0 [3 7 6])", "[3 7 6]"},
        {"drop!(1 [3 7 6])", "[7 6]"},
        {"drop!(2 [3 7 6])", "[6]"},
        {"drop!(3 [3 7 6])", "[]"},
    });
    test.evaluate("drop string", {
        {R"(drop!(0 "abc"))", R"("abc")"},
        {R"(drop!(1 "abc"))", R"("bc")"},
        {R"(drop!(2 "abc"))", R"("c")"},
        {R"(drop!(3 "abc"))", R"("")"},
    });
    test.evaluate("find_if string", {
        {R"(find_if!(is_upper ""))", R"("")"},
        {R"(find_if!(is_upper "a"))", R"("")"},
        {R"(find_if!(is_upper "A"))", R"("A")"},
        {R"(find_if!(is_upper "abCD"))", R"("CD")"},
    });
    test.evaluate("find_item string", {
        {R"(find_item!(\a ""))", R"("")"},
        {R"(find_item!(\a "a"))", R"("a")"},
        {R"(find_item!(\a "b"))", R"("")"},
        {R"(find_item!(\a "ab"))", R"("ab")"},
        {R"(find_item!(\a "ba"))", R"("a")"},
    });
    test.evaluate("take stack", {
        {"take!(0 [3 7 6])", "[]"},
        {"take!(1 [3 7 6])", "[3]"},
        {"take!(2 [3 7 6])", "[3 7]"},
        {"take!(3 [3 7 6])", "[3 7 6]"},
    });
    test.evaluate("take string", {
        {R"(take!(0 "abc"))", R"("")"},
        {R"(take!(1 "abc"))", R"("a")"},
        {R"(take!(2 "abc"))", R"("ab")"},
        {R"(take!(3 "abc"))", R"("abc")"},
    });
    test.evaluate("get_index stack", {
        {"get_index!(0 [3 7 6])", "3"},
        {"get_index!(1 [3 7 6])", "7"},
        {"get_index!(2 [3 7 6])", "6"},
    });
    test.evaluate("get_index string", {
        {R"(get_index!(0 "abc"))", R"(\a)"},
        {R"(get_index!(1 "abc"))", R"(\b)"},
        {R"(get_index!(2 "abc"))", R"(\c)"},
    });
    test.evaluate("merge_sorted", {
        {"merge_sorted!([] [])", "[]"},
        {"merge_sorted!([0] [])", "[0]"},
        {"merge_sorted!([] [0])", "[0]"},
        {"merge_sorted!([0] [0])", "[0 0]"},
        {"merge_sorted!([0 2] [1 3])", "[0 1 2 3]"},
        {"merge_sorted!([0 1 2 5 8 9] [0 2 4 6 7])", "[0 0 1 2 2 4 5 6 7 8 9]"},
    });
    test.evaluate("indexing tuple", {
        {"first@(11 12 13 14 15 16 17 18 19 20)", "11"},
        {"second@(11 12 13 14 15 16 17 18 19 20)", "12"},
        {"third@(11 12 13 14 15 16 17 18 19 20)", "13"},
        {"fourth@(11 12 13 14 15 16 17 18 19 20)", "14"},
        {"fifth@(11 12 13 14 15 16 17 18 19 20)", "15"},
        {"sixth@(11 12 13 14 15 16 17 18 19 20)", "16"},
        {"seventh@(11 12 13 14 15 16 17 18 19 20)", "17"},
        {"eighth@(11 12 13 14 15 16 17 18 19 20)", "18"},
        {"ninth@(11 12 13 14 15 16 17 18 19 20)", "19"},
        {"tenth@(11 12 13 14 15 16 17 18 19 20)", "20"},
    });
    test.evaluate("indexing stack", {
        {"first![11 12 13 14 15 16 17 18 19]", "11"},
        {"second![11 12 13 14 15 16 17 18 19]", "12"},
        {"third![11 12 13 14 15 16 17 18 19]", "13"},
        {"fourth![11 12 13 14 15 16 17 18 19]", "14"},
        {"fifth![11 12 13 14 15 16 17 18 19]", "15"},
        {"sixth![11 12 13 14 15 16 17 18 19]", "16"},
        {"seventh![11 12 13 14 15 16 17 18 19]", "17"},
        {"eighth![11 12 13 14 15 16 17 18 19]", "18"},
        {"ninth![11 12 13 14 15 16 17 18 19]", "19"},
        {"tenth![11 12 13 14 15 16 17 18 19 20]", "20"},
    });
    test.evaluate("zip", {
        {"zip!([] [])", "[]"},
        {"zip!([0] [1])", "[(0 1)]"},
        {"zip!([0 1] [2 3])", "[(0 2) (1 3)]"},
        {"zip!([0 1 2] [3 4 5])", "[(0 3) (1 4) (2 5)]"},
    });
    test.evaluate_types("zip", {
        {"zip!([] [])", "[(ANY ANY)]"},
        {"zip!([0] [1])", "[(NUMBER NUMBER)]"},
        {"zip!([0 1] [2 3])", "[(NUMBER NUMBER)]"},
        {"zip!([0 1 2] [3 4 5])", "[(NUMBER NUMBER)]"},
    });
    test.evaluate("consecutive_pairs", {
        {"consecutive_pairs![6 4 8]", "[(6 4) (4 8)]"},
    });/*
    test.evaluate("unique", {
        {"unique![]", "[]"},
        {"unique![1]", "[1]"},
        {"unique![1 2]", "[1 2]"},
        {"unique![1 1]", "[1]"},
        {"unique![1 1 2 3 1 4 2 4 0]", "[1 2 3 4 0]"},
    });*/
    return test.exitCode();
}
