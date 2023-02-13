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
            << duration_total.count() << " seconds." << endl;
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
    void evaluate_all(std::string case_name, InputOutputList data) {
        parameterizedTest<std::string, std::string>(
            ::evaluate_all, "evaluate_all", case_name, data
        );
    }
};

int main() {
    MangLangTest test;
    test.evaluate_all("number", {
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
    test.evaluate_all("character", {
        {"'a'", "'a'"},
        {"'1'", "'1'"},
        {"'+'", "'+'"},
        {"'-'", "'-'"},
        {R"('"')", R"('"')"},
        {"')'", "')'"},
        {"')'", "')'"},
        {"'{'", "'{'"},
        {"'}'", "'}'"},
    });
    test.evaluate_types("character", {
        {"'a'", "CHARACTER"},
        {"'1'", "CHARACTER"},
        {"'+'", "CHARACTER"},
        {"'-'", "CHARACTER"},
        {R"('"')", "CHARACTER"},
        {"')'", "CHARACTER"},
        {"')'", "CHARACTER"},
        {"'{'", "CHARACTER"},
        {"'}'", "CHARACTER"},
    });
    test.evaluate_all("boolean", {
        {"yes", "yes"},
        {"no", "no"},
    });
    test.evaluate_types("boolean", {
        {"yes", "BOOLEAN"},
        {"no", "BOOLEAN"},
    });
    test.evaluate_all("missing", {
        {"missing", "missing"},
    });
    test.evaluate_types("missing", {
        {"missing", "EMPTY"},
    });
    test.evaluate_all("string", {
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
    test.evaluate_all("stack", {
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
    test.reformat("table", {
        {"<>", "<>"},
        {"<1:2>", "<1:2>"},
        {"< 1 : 2 >", "<1:2>"},
        {"<<>:<>>", "<<>:<>>"},
        {"<(0 0):(1 1)>", "<(0 0):(1 1)>"},
        {"<inc!0:inc!1>", "<inc!0:inc!1>"},
    });
    test.evaluate_types("table", {
        {"<>", "<>"},
        {"<1:2>", "<NUMBER:NUMBER>"},
        {"< 1 : 2 >", "<NUMBER:NUMBER>"},
        {"<<>:<>>", "<<>:<>>"},
        {"<(0 0):(1 1)>", "<(NUMBER NUMBER):(NUMBER NUMBER)>"},
        {"<inc!0:inc!1>", "<NUMBER:NUMBER>"},
        {"<3:6 4:8 1:2 2:4>","<NUMBER:NUMBER>"}
    });
    test.evaluate_all("table", {
        {"<>", "<>"},
        {"<1:2>", "<1:2>"},
        {"< 1 : 2 >", "<1:2>"},
        {"<<>:<>>", "<<>:<>>"},
        {"<(0 0):(1 1)>", "<(0 0):(1 1)>"},
        {"<inc!0:inc!1>", "<1:2>"},
        {"<3:6 4:8 1:2 2:4>","<1:2 2:4 3:6 4:8>"}
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
    test.evaluate_all("tuple", {
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
    test.reformat("dictionary for", {
        {"{for i in c end}", "{for i in c end}"},
    });
    test.evaluate_types("dictionary iteration", {
        {"{while 1 end}", "{}"},
        {"{i=2 while i i=dec!i end}", "{i=NUMBER}"},
        {"{i=10 while i i=dec!i end j=1}", "{i=NUMBER j=NUMBER}"},
    });
    test.evaluate_all("dictionary iterations", {
        {"{i=2 while i i=dec!i end}", "{i=0}"},
        {"{i=2 while i i=dec!i end j=1}", "{i=0 j=1}"},
        {"{i=2 tot=0 while i tot=add!(tot i) i=dec!i end}", "{i=0 tot=3}"},
        {"{i=1000 tot=0 while i tot=add!(tot i) i=dec!i end}", "{i=0 tot=500500}"},
        {"{i=2 c=[] while i c+=i i=dec!i end}", "{i=0 c=[1 2]}"},
    });
    test.evaluate_all("dictionary for", {
        {"{c=[] for i in c end}", "{c=[]}"},
        {"{c=[1] for i in c end}", "{c=[] i=1}"},
        {"{c=[1 2] for i in c end}", "{c=[] i=2}"},
        {"{a=[] c=[1 2 3] for i in c a+=i end}", "{a=[3 2 1] c=[] i=3}"},
        {"{c=[1 2] for i in c d=[2 3] for j in d end end}", "{c=[] i=2 d=[] j=3}"},
        {"r@{f=in c out {for i in c end} r=f![]}", "{}"},
        {"r@{f=in c out {for i in c end} r=f![1]}", "{i=1 c=[]}"}
    });
    test.evaluate_types("dictionary for", {
        {"{c=[] for i in c end}", "{c=EMPTY_STACK i=ANY}"},
        {"{c=<> for i in c end}", "{c=<> i=(ANY ANY)}"},
        {"{c=<> d=<> for i in c d+=i end}", "{c=<> d=<ANY:ANY> i=(ANY ANY)}"}, 
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
        {"{a=[] a+=1}", "{a=[NUMBER]}"},
        {"{a=[1] a+=2 a+=3}", "{a=[NUMBER]}"},
    });
    test.evaluate_all("dictionary", {
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
        {"{a=[] a+=1}", "{a=[1]}"},
        {"{a=[1] a+=2 a+=3}", "{a=[3 2 1]}"},
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
    test.evaluate_all("conditional", {
        {"if 1 then 2 else 3", "2"},
        {"if 0 then 2 else 3", "3"},
        {"if [0] then 2 else 3", "2"},
        {"if [] then 2 else 3", "3"},
        {"if yes then [] else [1]", "[]"},
        {"if yes then [1] else []", "[1]"},
        {R"(if yes then "" else "a")", R"("")"},
        {R"(if yes then "a" else "")", R"("a")"},
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
    test.evaluate_all("is", {
        {"is 0 0 then 1 else 2", "1"},
        {"is 1 0 then 1 else 2", "2"},
        {"is 0 0 then 0 1 then 1 2 then 4 else 5", "0"},
        {"is 1 0 then 0 1 then 1 2 then 4 else 5", "1"},
        {"is 2 0 then 0 1 then 1 2 then 4 else 5", "4"},
        {"is 3 0 then 0 1 then 1 2 then 4 else 5", "5"},
        {"is missing missing then 1 else 2", "1"},
        {"is missing 0 then 1 else 2", "2"},
        {"is 0 missing then 1 else 2", "2"},
        {"is (1 2) (1 2) then 1 else 2", "1"},
        {"is (1 2) (1 3) then 1 else 2", "2"},
        {"is (1 (2)) (1 (2)) then 1 else 2", "1"},
        {"is (1 (2)) (1 (3)) then 1 else 2", "2"},
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
    test.evaluate_all("symbol", {
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
    test.evaluate_all("child_symbol", {
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
    test.evaluate_all("child_symbol_keyword_confusion", {
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
    test.evaluate_all("function", {
        {"in x out x",       "in x out x"},
        {"f@{f=in x out x}", "in x out x"},
    });
    test.evaluate_types("function dictionary", {
        {"in {x} out x", "FUNCTION_DICTIONARY"},
        {"in {x y} out x", "FUNCTION_DICTIONARY"},
        {"in  {  x    y  }  out  x", "FUNCTION_DICTIONARY"},
    });
    test.evaluate_all("function dictionary", {
        {"in {x} out x", "in {x} out x"},
        {"in {x y} out x", "in {x y} out x"},
        {"in  {  x    y  }  out  x", "in {x y} out x"},
    });
    test.evaluate_types("function tuple", {
        {"in (x) out x", "FUNCTION_TUPLE"},
        {"in (x y) out x", "FUNCTION_TUPLE"},
        {"in  (  x    y  )  out  x", "FUNCTION_TUPLE"},
    });
    test.evaluate_all("function tuple", {
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
        {"y@{f=in stack out map_stack!(in y out 2 stack) y=f![0 0]}", "[NUMBER]"},
        {"a@{call=in f out f![] g=in x out 0 a=call!g}", "NUMBER"},
        {"a@{call=in f out f![] b={a=0 g=in x out a} g=g@b a=call!g}", "NUMBER"},
        {"a@{call=in(f)out f!([]) b={a=0 g=in(x)out a} g=g@b a=call!(g)}", "NUMBER"},
        {"y@{a=1 g=in y out a f=in stack out map_stack!(g stack) y=f![0 0]}", "[NUMBER]"},
        {"y@{a=1 f=in stack out map_stack!(in y out a stack) y=f![0 0]}", "[NUMBER]"},
        {"b@{a={a=0 f=in x out a} g=f@a b=g!1}", "NUMBER"},
        {"y@{f=in (x stack) out map_stack!(in y out x stack) y=f!(2 [0 0])}", "[NUMBER]"},
    });
    test.evaluate_all("lookup function", {
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
        {"y@{f=in stack out map_stack!(in y out 2 stack) y=f![0 0]}", "[2 2]"},
        {"a@{call=in f out f![] g=in x out 0 a=call!g}", "0"},
        {"a@{call=in f out f![] b={a=0 g=in x out a} g=g@b a=call!g}", "0"},
        {"a@{call=in(f)out f!([]) b={a=0 g=in(x)out a} g=g@b a=call!(g)}", "0"},
        {"y@{a=1 g=in y out a f=in stack out map_stack!(g stack) y=f![0 0]}", "[1 1]"},
        {"y@{a=1 f=in stack out map_stack!(in y out a stack) y=f![0 0]}", "[1 1]"},
        {"b@{a={a=0 f=in x out a} g=f@a b=g!1}", "0"},
        {"y@{f=in (x stack) out map_stack!(in y out x stack) y=f!(2 [0 0])}", "[2 2]"},
    });
    test.evaluate("recursive function", { // TODO: support type checking
        {"y@{f=in x out if x then add!(x f!dec!x) else 0 y=f!3}", "6"},
    });
    test.evaluate_types("lookup function dictionary", {
        {"a@{f=in {x} out x a=f!{x=0}}", "NUMBER"},
        {"a@{f=in {x y} out add!(x y) a=f!{x=2 y=3}}", "NUMBER"},
        {"a@{b=2 f=in {x} out add!(b x) a=f!{x=0}}", "NUMBER"},
    });
    test.evaluate_all("lookup function dictionary", {
        {"a@{f=in {x} out x a=f!{x=0}}", "0"},
        {"a@{f=in {x y} out add!(x y) a=f!{x=2 y=3}}", "5"},
        {"a@{b=2 f=in {x} out add!(b x) a=f!{x=0}}", "2"},
    });
    test.evaluate_types("lookup function tuple", {
        {"a@{f=in (x) out x a=f!(0)}", "NUMBER"},
        {"a@{f=in (x y) out add!(x y) a=f!(2 3)}", "NUMBER"},
    });
    test.evaluate_all("lookup function tuple", {
        {"a@{f=in (x) out x a=f!(0)}", "0"},
        {"a@{f=in (x y) out add!(x y) a=f!(2 3)}", "5"},
    });
    test.evaluate_all("lookup tuple indexing", {
        {"a@{c=(3 2 1) a=c!0}", "3"},
        {"a@{c=(3 2 1) a=c!1}", "2"},
        {"a@{c=(3 2 1) a=c!2}", "1"},
        {"a@{i=1 c=(3 2 1) a=c!i}", "2"},
    });
    test.evaluate_all("lookup stack indexing", {
        {"a@{c=[3 2 1] a=c!0}", "3"},
        {"a@{c=[3 2 1] a=c!1}", "2"},
        {"a@{c=[3 2 1] a=c!2}", "1"},
        {"a@{i=1 c=[3 2 1] a=c!i}", "2"},
        {"a@{c=range!100 a=c!99}", "99"},
    });
    test.evaluate_all("lookup string indexing", {
        {R"(a@{c="abc" a=c!0})", "'a'"},
        {R"(a@{c="abc" a=c!1})", "'b'"},
        {R"(a@{c="abc" a=c!2})", "'c'"},
        {R"(a@{i=1 c="abc" a=c!i})", "'b'"},
    });
    test.evaluate_all("lookup table indexing", {
        {"a@{c=<2:3 4:5> a=c!2}", "3"},
        {"a@{c=<2:3 4:5> a=c!4}", "5"},
    });
    test.evaluate_all("add", {
        {"add!(1 0)", "1"},
        {"add!(0 1)", "1"},
        {"add!(-1 +1)", "0"},
    });
    test.evaluate_all("mul", {
        {"mul!(0 1)", "0"},
        {"mul!(1 2)", "2"},
        {"mul!(2 3)", "6"},
        {"mul!(-1 +1)", "-1"},
    });
    test.evaluate_all("sub", {
        {"sub!(0 0)", "0"},
        {"sub!(6 3)", "3"},
        {"sub!(4 8)", "-4"},
    });
    test.evaluate_all("div", {
        {"div!(0 1)", "0"},
        {"div!(2 1)", "2"},
        {"div!(9 3)", "3"},
    });
    test.evaluate_all("less", {
        {"less?(0 0)", "no"},
        {"less?(0 1)", "yes"},
        {"less?(-1 0)", "yes"},
        {"less?(1 0)", "no"},
        {"less?(0 -1)", "no"},
        {"less?(1 1)", "no"},
        {"less?(-1 -1)", "no"},
    });
    test.evaluate_all("is_increasing", {
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
    test.evaluate_all("round", {
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
    test.evaluate_all("round_up", {
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
    test.evaluate_all("round_down", {
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
    test.evaluate_all("abs", {
        {"abs!-1", "1"},
        {"abs!0", "0"},
        {"abs!1", "1"},
    });
    test.evaluate_all("sqrt", {
        {"sqrt!0", "0"},
        {"sqrt!1", "1"},
        {"sqrt!4", "2"},
    });
    test.evaluate_all("number", {
        {"number!'0'", "48"},
        {"number!'9'", "57"},
        {"number!'A'", "65"},
        {"number!'Z'", "90"},
        {"number!'a'", "97"},
        {"number!'z'", "122"},
    });
    test.evaluate_all("character", {
        {"character!48", "'0'"},
        {"character!57", "'9'"},
        {"character!65", "'A'"},
        {"character!90", "'Z'"},
        {"character!97", "'a'"},
        {"character!122", "'z'"},
    });
    test.evaluate_types("number constants", {
        {"inf", "NUMBER"},
        {"-inf", "NUMBER"},
        {"nan", "NUMBER"},
        {"pi", "NUMBER"},
        {"tau", "NUMBER"},
    });
    test.evaluate_all("number constants", {
        {"inf", "inf"},
        {"-inf", "-inf"},
        {"nan", "nan"},
        {"pi", "3.14159265359"},
        {"tau", "6.28318530718"},
    });
    test.evaluate_all("boolean", {
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
        {R"(boolean!"")", "no"},
        {R"(boolean!"0")", "yes"},
        {R"(boolean!"1")", "yes"},
        {R"(boolean!"a")", "yes"},
        {R"(boolean!"ab")", "yes"},
    });
    test.evaluate_all("not", {
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
        {R"(not?"")", "yes"},
        {R"(not?"0")", "no"},
        {R"(not?"1")", "no"},
        {R"(not?"a")", "no"},
        {R"(not?"ab")", "no"},
    });
    test.evaluate_all("and", {
        {"and?[no no]", "no"},
        {"and?[no yes]", "no"},
        {"and?[yes no]", "no"},
        {"and?[yes yes]", "yes"},
    });
    test.evaluate_all("or", {
        {"or?[no no]", "no"},
        {"or?[no yes]", "yes"},
        {"or?[yes no]", "yes"},
        {"or?[yes yes]", "yes"},
    });
    test.evaluate_all("all", {
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
    test.evaluate_all("any", {
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
    test.evaluate_all("none", {
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
    test.evaluate_all("equal number", {
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
    test.evaluate_all("unequal number", {
        {"unequal?(0 0)", "no"},
        {"unequal?(0 1)", "yes"},
        {"unequal?(1 0)", "yes"},
        {"unequal?(1 1)", "no"},
    });
    test.evaluate_all("equal character", {
        {"equal?('a' 'a')", "yes"},
        {"equal?('a' 'b')", "no"},
        {"equal?('b' 'a')", "no"},
        {"equal?('b' 'b')", "yes"},
    });
    test.evaluate_all("unequal character", {
        {"unequal?('a' 'a')", "no"},
        {"unequal?('a' 'b')", "yes"},
        {"unequal?('b' 'a')", "yes"},
        {"unequal?('b' 'b')", "no"},
    });
    test.evaluate_all("equal boolean", {
        {"equal?(yes yes)", "yes"},
        {"equal?(yes no)", "no"},
        {"equal?(no yes)", "no"},
        {"equal?(no no)", "yes"},
    });
    test.evaluate_all("unequal boolean", {
        {"unequal?(yes yes)", "no"},
        {"unequal?(yes no)", "yes"},
        {"unequal?(no yes)", "yes"},
        {"unequal?(no no)", "no"},
    });
    test.evaluate_all("equal stack", {
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
    test.evaluate_all("unequal stack", {
        {"unequal?([] [])", "no"},
        {"unequal?([1] [1])", "no"},
        {"unequal?([0] [1])", "yes"},
        {"unequal?([0 1] [0 1])", "no"},
        {"unequal?([0 1] [1 1])", "yes"},
        {"unequal?([0 1] [0])", "yes"},
    });
    test.evaluate_all("equal string", {
        {R"(equal?("" ""))", "yes"},
        {R"(equal?("a" ""))", "no"},
        {R"(equal?("" "a"))", "no"},
        {R"(equal?("a" "a"))", "yes"},
        {R"(equal?("a" "b"))", "no"},
        {R"(equal?("ab" "ab"))", "yes"},
        {R"(equal?("abc" "ab"))", "no"},
        {R"(equal?("ab" "abc"))", "no"},
    });
    test.evaluate_all("unequal string", {
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
    test.evaluate_all("clear stack", {
        {"clear![]", "[]"},
        {"clear![1]", "[]"},
        {"clear![1 2]", "[]"},
    });
    test.evaluate_all("clear table", {
        {"clear!<>", "<>"},
        {"clear!<1:11>", "<>"},
        {"clear!<1:11 2:22>", "<>"},
    });
    test.evaluate_types("clear string", {
        {R"(clear!"")", "EMPTY_STRING"},
        {R"(clear!"a")", "EMPTY_STRING"},
        {R"(clear!"ab")", "EMPTY_STRING"},
    });
    test.evaluate_all("clear string", {
        {R"(clear!"")", R"("")"},
        {R"(clear!"a")", R"("")"},
        {R"(clear!"ab")", R"("")"},
    });
    test.evaluate_types("take stack", {
        {"take![4]", "NUMBER"},
        {"take![3 4]", "NUMBER"},
    });
    test.evaluate_all("take stack", {
        {"take![4]", "4"},
        {"take![3 4]", "3"},
    });
    test.evaluate_types("take string", {
        {R"(take!"b")", "CHARACTER"},
        {R"(take!"ab")", "CHARACTER"},
    });
    test.evaluate_all("take string", {
        {R"(take!"b")", "'b'"},
        {R"(take!"ab")", "'a'"},
    });
    test.evaluate_types("drop stack", {
        {"drop![4]", "EMPTY_STACK"}, // TODO: is this good?
        {"drop![4 3]", "[NUMBER]"},
        {"drop![4 3 7]", "[NUMBER]"},
        {"drop![[]]", "EMPTY_STACK"},
        {"drop![]", "EMPTY_STACK"},
    });
    test.evaluate_all("drop stack", {
        {"drop![4]", "[]"},
        {"drop![4 3]", "[3]"},
        {"drop![4 3 7]", "[3 7]"},
        {"drop![[]]", "[]"},
    });
    test.evaluate_types("drop string", {
        {R"(drop!"a")", "EMPTY_STRING"}, // TODO: is this good?
        {R"(drop!"ab")", "STRING"},
        {R"(drop!"abc")", "STRING"},
    });
    test.evaluate_all("drop string", {
        {R"(drop!"a")", R"("")"},
        {R"(drop!"ab")", R"("b")"},
        {R"(drop!"abc")", R"("bc")"},
    });
    test.evaluate_types("put stack", {
        {"put!(3 [])", "[NUMBER]"},
        {"put!(4 [5])", "[NUMBER]"},
        {"put!(4 [6 8])", "[NUMBER]"},
    });
    test.evaluate_all("put stack", {
        {"put!(3 [])", "[3]"},
        {"put!(4 [5])", "[4 5]"},
        {"put!(4 [6 8])", "[4 6 8]"},
    });
    test.evaluate_types("put string", {
        {R"(put!('a' ""))", "STRING"},
        {R"(put!('a' "b"))", "STRING"},
        {R"(put!('a' "bc"))", "STRING"},
    });
    test.evaluate_all("put string", {
        {R"(put!('a' ""))", R"("a")"},
        {R"(put!('a' "b"))", R"("ab")"},
        {R"(put!('a' "bc"))", R"("abc")"},
    });
    test.evaluate_all("put table", {
        {"put!((1 11) <>)", "<1:11>"},
        {"put!((4 44) <5:55>)", "<4:44 5:55>"},
        {"put!((4 44) <6:66 8:88>)", "<4:44 6:66 8:88>"},
        {"put!((0 1) <>)", "<0:1>"},
        {"put!((0 1) <0:2>)", "<0:1>"},
        {"put!((0 5) <2:3 1:2 0:1>)", "<0:5 1:2 2:3>"},
    });
    test.evaluate_all("get table", {
        {"get!(0 <> 2)", "2"},
        {"get!(0 <0:1> 2)", "1"},
        {"get!(1 <0:1> 2)", "2"},
        {"get!(0 <0:1 1:2 3:3> 2)", "1"},
        {"get!((3) <(1):[1] (2):[2] (3):[3]> [])", "[3]"},
    });
    test.evaluate_all("get_keys", {
        {"get_keys!<>", "[]"},
        {"get_keys!<0:1>", "[0]"},
        {"get_keys!<0:1 2:3>", "[0 2]"},
        {"get_keys!<2:3 0:1>", "[0 2]"},
    });
    test.evaluate_all("get_values", {
        {"get_values!<1:0>", "[0]"},
        {"get_values!<10:0 11:1 12:2>", "[0 1 2]"},
        {"get_values!<3:0 2:1 1:2>", "[2 1 0]"},
    });
    test.evaluate_all("get_items", {
        {"get_items!<0:1>", "[(0 1)]"},
        {"get_items!<0:1 2:3 4:5>", "[(0 1) (2 3) (4 5)]"},
        {"get_items!<4:1 2:3 0:5>", "[(0 5) (2 3) (4 1)]"},
    });
    test.evaluate_all("inc", {
        {"inc!0", "1"},
    });
    test.evaluate_all("dec", {
        {"dec!0", "-1"},
    });
    test.evaluate_types("is_digit", {
        {"is_digit?'A'", "BOOLEAN"},
    });
    test.evaluate_all("is_digit", {
        {"is_digit?'A'", "no"},
        {"is_digit?'Z'", "no"},
        {"is_digit?'a'", "no"},
        {"is_digit?'z'", "no"},
        {"is_digit?'0'", "yes"},
        {"is_digit?'9'", "yes"},
    });
    test.evaluate_types("is_upper", {
        {"is_upper?'A')", "BOOLEAN"},
    });
    test.evaluate_all("is_upper", {
        {"is_upper?'A'", "yes"},
        {"is_upper?'Z'", "yes"},
        {"is_upper?'a'", "no"},
        {"is_upper?'z'", "no"},
        {"is_upper?'0'", "no"},
        {"is_upper?'9'", "no"},
    });
    test.evaluate_types("is_lower", {
        {"is_lower?'A'", "BOOLEAN"},
    });
    test.evaluate_all("is_lower", {
        {"is_lower?'A')", "no"},
        {"is_lower?'Z')", "no"},
        {"is_lower?'a')", "yes"},
        {"is_lower?'z')", "yes"},
        {"is_lower?'0')", "no"},
        {"is_lower?'9')", "no"},
    });
    test.evaluate_types("is_letter", {
        {"is_letter?'A'", "BOOLEAN"},
    });
    test.evaluate_all("is_letter", {
        {"is_letter?'A'", "yes"},
        {"is_letter?'Z'", "yes"},
        {"is_letter?'a'", "yes"},
        {"is_letter?'z'", "yes"},
        {"is_letter?'0'", "no"},
        {"is_letter?'9'", "no"},
    });
    test.evaluate_all("to_upper", {
        {"to_upper!'A'", "'A'"},
        {"to_upper!'Z'", "'Z'"},
        {"to_upper!'a'", "'A'"},
        {"to_upper!'z'", "'Z'"},
        {"to_upper!'0'", "'0'"},
        {"to_upper!'9'", "'9'"},
    });
    test.evaluate_all("to_lower", {
        {"to_lower!'A'", "'a'"},
        {"to_lower!'Z'", "'z'"},
        {"to_lower!'a'", "'a'"},
        {"to_lower!'z'", "'z'"},
        {"to_lower!'0'", "'0'"},
        {"to_lower!'9'", "'9'"},
    });
    test.evaluate_all("parse_digit", {
        {"parse_digit!'0'", "0"},
        {"parse_digit!'1'", "1"},
        {"parse_digit!'2'", "2"},
        {"parse_digit!'3'", "3"},
        {"parse_digit!'4'", "4"},
        {"parse_digit!'5'", "5"},
        {"parse_digit!'6'", "6"},
        {"parse_digit!'7'", "7"},
        {"parse_digit!'8'", "8"},
        {"parse_digit!'9'", "9"},
    });
    test.evaluate_all("parse_natural_number", {
        {R"(parse_natural_number!"0")", "0"},
        {R"(parse_natural_number!"1")", "1"},
        {R"(parse_natural_number!"2")", "2"},
        {R"(parse_natural_number!"10")", "10"},
        {R"(parse_natural_number!"11")", "11"},
        {R"(parse_natural_number!"12")", "12"},
        {R"(parse_natural_number!"20")", "20"},
        {R"(parse_natural_number!"123")", "123"},
    });
    test.evaluate_types("min_item stack", {
        {"min_item![]", "NUMBER"},
        {"min_item![0]", "NUMBER"},
        {"min_item![0 1]", "NUMBER"},
        {"min_item![1 0]", "NUMBER"},
        {"min_item![3 6 1]", "NUMBER"},
        {"min_item![7 -3 8 -9]", "NUMBER"},
    });
    test.evaluate_all("min_item stack", {
        {"min_item![]", "inf"},
        {"min_item![0]", "0"},
        {"min_item![0 1]", "0"},
        {"min_item![1 0]", "0"},
        {"min_item![3 6 1]", "1"},
        {"min_item![7 -3 8 -9]", "-9"},
    });
    test.evaluate_types("max_item stack", {
        {"max_item![]", "NUMBER"},
        {"max_item![0]", "NUMBER"},
        {"max_item![0 1]", "NUMBER"},
        {"max_item![1 0]", "NUMBER"},
        {"max_item![3 6 1]", "NUMBER"},
        {"max_item![7 -3 8 -9]", "NUMBER"},
    });
    test.evaluate_all("max_item stack", {
        {"max_item![]", "-inf"},
        {"max_item![0]", "0"},
        {"max_item![0 1]", "1"},
        {"max_item![1 0]", "1"},
        {"max_item![3 6 1]", "6"},
        {"max_item![7 -3 8 -9]", "8"},
    });
    test.evaluate_all("min_key stack", {
        {"min_key!(in (w h) out mul!(w h) [(1 2) (2 2) (3 1)])", "(1 2)"},
    });
    test.evaluate_all("max_key stack", {
        {"max_key!(in (w h) out mul!(w h) [(1 2) (2 2) (3 1)])", "(2 2)"},
    });
    test.evaluate_all("min_predicate stack", {
        {"min_predicate!(in (a b) out less?(mul!a mul!b) [(1 2) (2 2) (3 1)])", "(1 2)"},
    });
    test.evaluate_all("max_predicate stack", {
        {"max_predicate!(in (a b) out less?(mul!a mul!b) [(1 2) (2 2) (3 1)])", "(2 2)"},
    });
    test.evaluate_types("range", {
        {"range!0", "[NUMBER]"},
        {"range!1", "[NUMBER]"},
        {"range!2", "[NUMBER]"},
        {"range!3", "[NUMBER]"},
    });
    test.evaluate_all("range", {
        {"range!0", "[]"},
        {"range!1", "[0]"},
        {"range!2", "[0 1]"},
        {"range!3", "[0 1 2]"},
    });
    test.evaluate_all("iteration", {
        {"count!range!100", "100"},
        {"sum!range!100", "4950"},
    });
    test.evaluate_types("count stack", {
        {"count![]", "NUMBER"},
        {"count![[]]", "NUMBER"},
        {"count![[] []]", "NUMBER"},
    });
    test.evaluate_all("count stack", {
        {"count![]", "0"},
        {"count![[]]", "1"},
        {"count![[] []]", "2"},
    });
    test.evaluate_all("count table", {
        {"count!<>", "0"},
        {"count!<1:1>", "1"},
        {"count!<1:1 1:1>", "1"},
        {"count!<1:1 2:2>", "2"},
    });
    test.evaluate_types("count string", {
        {R"(count!"")", "NUMBER"},
        {R"(count!"a")", "NUMBER"},
        {R"(count!"ab")", "NUMBER"},
    });
    test.evaluate_all("count string", {
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
    test.evaluate_all("count_item stack", {
        {"count_item!(1 [])", "0"},
        {"count_item!(1 [1])", "1"},
        {"count_item!(1 [1 1])", "2"},
        {"count_item!(1 [1 0 1])", "2"},
    });
    test.evaluate_all("count_item string", {
        {R"(count_item!('a' ""))", "0"},
        {R"(count_item!('a' "a"))", "1"},
        {R"(count_item!('a' "aa"))", "2"},
        {R"(count_item!('a' "aba"))", "2"},
    });
    test.evaluate_all("count_if stack", {
        {"count_if!(in x out 0 [])", "0"},
        {"count_if!(in x out 0 [1])", "0"},
        {"count_if!(in x out 1 [])", "0"},
        {"count_if!(in x out 1 [1])", "1"},
        {"count_if!(in x out equal?(x 3) [1 2 3])", "1"},
        {"count_if!(in x out equal?(x 3) [3 2 3])", "2"},
    });
    test.evaluate_all("count_if table", {
        {"count_if!(less <0:0>)", "0"},
        {"count_if!(less <0:1>)", "1"},
        {"count_if!(less <0:0 1:2 2:3 4:5 7:6>)", "3"},
    });
    test.evaluate_all("count_if string", {
        {R"(count_if!(in x out 0 ""))", "0"},
        {R"(count_if!(in x out 0 "a"))", "0"},
        {R"(count_if!(in x out 1 ""))", "0"},
        {R"(count_if!(in x out 1 "a"))", "1"},
        {R"(count_if!(in x out equal?(x 'c') "abc"))", "1"},
        {R"(count_if!(in x out equal?(x 'c') "cbc"))", "2"},
    });
    test.evaluate_types("reverse stack", {
        {"reverse![]", "[ANY]"},
        {"reverse![0]", "[NUMBER]"},
        {"reverse![0 1]", "[NUMBER]"},
        {"reverse![0 1 2]", "[NUMBER]"},
    });
    test.evaluate_all("reverse stack", {
        {"reverse![]", "[]"},
        {"reverse![0]", "[0]"},
        {"reverse![0 1]", "[1 0]"},
        {"reverse![0 1 2]", "[2 1 0]"},
    });
    test.evaluate_types("reverse table", {
        {"reverse!<>", "<ANY:ANY>"},
        {"reverse!<0:0>", "<0:NUMBER>"},
        {"reverse!<0:0 1:1>", "<1:NUMBER>"},
        {"reverse!<0:0 1:1 2:2>", "<2:NUMBER>"},
    });
    test.evaluate_all("reverse table", {
        {"reverse!<>", "<>"},
        {"reverse!<0:0>", "<0:0>"},
        {"reverse!<0:0 1:1>", "<0:0 1:1>"},
        {"reverse!<0:0 1:1 2:2>", "<0:0 1:1 2:2>"},
    });
    test.evaluate_types("reverse string", {
        {R"(reverse!"")", "STRING"},
        {R"(reverse!"a")", "STRING"},
        {R"(reverse!"ab")", "STRING"},
        {R"(reverse!"abc")", "STRING"},
    });
    test.evaluate_all("reverse string", {
        {R"(reverse!"")", R"("")"},
        {R"(reverse!"a")", R"("a")"},
        {R"(reverse!"ab")", R"("ba")"},
        {R"(reverse!"abc")", R"("cba")"},
    });
    test.evaluate_all("make_stack stack", {
        {"make_stack![]", "[]"},
        {"make_stack![3 1 2]", "[3 1 2]"},
    });
    test.evaluate_all("make_stack string", {
        {R"(make_stack!"")", "[]"},
        {R"(make_stack!"cab")", "['c' 'a' 'b']"},
    });
    test.evaluate_all("make_stack table", {
        {"make_stack!<>", "[]"},
        {"make_stack!<3:33 1:11 2:22>", "[(1 11) (2 22) (3 33)]"},
    });
    test.evaluate_all("make_string", {
        {R"(make_string![])", R"("")"},
        {R"(make_string!['c' 'a' 'b'])", R"("cab")"},
        {R"(make_string!"")", R"("")"},
        {R"(make_string!"cab")", R"("cab")"},
    });
    test.evaluate_types("make_table", {
        {"make_table![]", "<>"},
        {"make_table![(3 33) (1 11) (2 22)]", "<3:NUMBER>"},
        {"make_table!<>", "<ANY:ANY>"},
        {"make_table!<3:33 1:11 2:22>", "<2:NUMBER>"},
    });
    test.evaluate_all("make_table", {
        {"make_table![]", "<>"},
        {"make_table![(3 33) (1 11) (2 22)]", "<1:11 2:22 3:33>"},
        {"make_table!<>", "<>"},
        {"make_table!<3:33 1:11 2:22>", "<1:11 2:22 3:33>"},
    });
    test.evaluate_all("map_stack", {
        {"map_stack!(inc [])", "[]"},
        {"map_stack!(inc [0])", "[1]"},
        {"map_stack!(inc [0 1])", "[1 2]"},
        {"map_stack!(in x out 2 [0 0])", "[2 2]"},
        {"a@{b=2 f=in x out b a=map_stack!(f [0 0])}", "[2 2]"},
    });
    test.evaluate_types("map_stack", {
        {"map_stack!(inc [])", "[NUMBER]"},
        {"map_stack!(inc [0])", "[NUMBER]"},
        {"map_stack!(inc [0 1])", "[NUMBER]"},
        {"map_stack!(in x out 2 [0 0])", "[NUMBER]"},
        {"a@{b=2 f=in x out b a=map_stack!(f [0 0])}", "[NUMBER]"},
    });
    test.evaluate_all("map_string", {
        {R"(map_string!(to_upper ""))", R"("")"},
        {R"(map_string!(to_upper "abc"))", R"("ABC")"},
    });
    test.evaluate_types("map_string", {
        {R"(map_string!(to_upper ""))", "STRING"},
        {R"(map_string!(to_upper "abc"))", "STRING"},
    });
    test.evaluate_all("map table", {
        {"map_table!(in x out (x x) [1 2])", "<1:1 2:2>"},
        {"map_table!(in (x y) out (x inc!y) <1:11 2:22>)", "<1:12 2:23>"},
    });
    test.evaluate_all("clear_if stack", {
        {"clear_if!(in x out 1 [])", "[]"},
        {"clear_if!(in x out 1 [[]])", "[]"},
        {"clear_if!(in x out 0 [[]])", "[[]]"},
        {"clear_if!(in x out less?(x 5) [7 4 6 1 9 3 2])", "[7 6 9]"},
    });
    test.evaluate_all("clear_if table", {
        {"clear_if!(less <0:0>)", "<0:0>"},
        {"clear_if!(less <0:1>)", "<>"},
        {"clear_if!(less <0:0 1:2 2:3 5:4>)", "<0:0 5:4>"},
    });
    test.evaluate_all("clear_if string", {
        {R"(clear_if!(in x out 0 ""))", R"("")"},
        {R"(clear_if!(in x out 0 "a"))", R"("a")"},
        {R"(clear_if!(in x out 1 ""))", R"("")"},
        {R"(clear_if!(in x out 1 "a"))", R"("")"},
        {R"(clear_if!(in x out equal?(x 'a') "a"))", R"("")"},
        {R"(clear_if!(in x out equal?(x 'a') "ab"))", R"("b")"},
        {R"(clear_if!(in x out equal?(x 'a') "ba"))", R"("b")"},
        {R"(clear_if!(in x out equal?(x 'a') "bab"))", R"("bb")"},
    });
    test.evaluate_all("clear_item stack", {
        {"clear_item!(1 [])", "[]"},
        {"clear_item!(1 [0])", "[0]"},
        {"clear_item!(1 [1])", "[]"},
        {"clear_item!(1 [1 7 1 2 7 1 1])", "[7 2 7]"},
    });
    test.evaluate_all("replace stack", {
        {"replace!(1 [])", "[]"},
        {"replace!(1 [1])", "[1]"},
        {"replace!(1 [2])", "[1]"},
        {"replace!(1 [0 1 0 1 1])", "[1 1 1 1 1]"},
    });
    test.evaluate_all("replace string", {
        {R"(replace!('a' ""))", R"("")"},
        {R"(replace!('a' "a"))", R"("a")"},
        {R"(replace!('a' "c"))", R"("a")"},
        {R"(replace!('a' "ab_ba"))", R"("aaaaa")"},
    });
    test.evaluate_all("replace_item stack", {
        {"replace_item!(1 2 [])", "[]"},
        {"replace_item!(1 2 [1])", "[2]"},
        {"replace_item!(1 2 [2])", "[2]"},
        {"replace_item!(1 2 [0 1 0 1 1])", "[0 2 0 2 2]"},
    });
    test.evaluate_all("replace_item string", {
        {R"(replace_item!('a' 'b' ""))", R"("")"},
        {R"(replace_item!('a' 'b' "a"))", R"("b")"},
        {R"(replace_item!('a' 'b' "c"))", R"("c")"},
        {R"(replace_item!('a' 'b' "ab_ba"))", R"("bb_bb")"},
    });
    test.evaluate_all("replace_if stack", {
        {"replace_if!(in x out equal?(x 1) 2 [])", "[]"},
        {"replace_if!(in x out equal?(x 1) 2 [1])", "[2]"},
        {"replace_if!(in x out equal?(x 1) 2 [2])", "[2]"},
        {"replace_if!(in x out equal?(x 1) 2 [0 1 0 1 1])", "[0 2 0 2 2]"},
    });
    test.evaluate_all("replace_if string", {
        {R"(replace_if!(in x out equal?(x 'a') 'b' ""))", R"("")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "a"))", R"("b")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "c"))", R"("c")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "ab_ba"))", R"("bb_bb")"},
    });
    test.evaluate_types("enumerate stack", {
        {"enumerate![]", "[(NUMBER ANY)]"},
        {"enumerate![4]", "[(NUMBER NUMBER)]"},
        {"enumerate![4 3]", "[(NUMBER NUMBER)]"},
    });
    test.evaluate_all("enumerate stack", {
        {"enumerate![]", "[]"},
        {"enumerate![4]", "[(0 4)]"},
        {"enumerate![4 3]", "[(0 4) (1 3)]"},
    });
    test.evaluate_all("enumerate string", {
        {R"(enumerate!"")", "[]"},
        {R"(enumerate!"a")", "[(0 'a')]"},
        {R"(enumerate!"ab")", "[(0 'a') (1 'b')]"},
    });
    test.evaluate_all("split stack", {
        {"split!(0 [])", "[[]]"},
        {"split!(0 [1])", "[[1]]"},
        {"split!(0 [1 2])", "[[1 2]]"},
        {"split!(0 [0])", "[[] []]"},
        {"split!(0 [0 0])", "[[] [] []]"},
        {"split!(0 [0 1 2])", "[[] [1 2]]"},
        {"split!(0 [1 2 0])", "[[1 2] []]"},
        {"split!(0 [1 0 2])", "[[1] [2]]"},
    });
    test.evaluate_types("split stack", {
        {"split!(0 [])", "[[ANY]]"},
        {"split!(0 [0])", "[[ANY]]"},
        {"split!(0 [0 0])", "[[ANY]]"},
        {"split!(0 [0 0 0])", "[[NUMBER]]"},
        {"split!(0 [0 0 0 0])", "[[NUMBER]]"},
    });
    test.evaluate_all("split string", {
        {R"(split!(',' ""))", R"([""])"},
        {R"(split!(',' "a"))", R"(["a"])"},
        {R"(split!(',' ",a"))", R"(["" "a"])"},
        {R"(split!(',' "a,"))", R"(["a" ""])"},
        {R"(split!(',' "a,b,cd"))", R"(["a" "b" "cd"])"},
    });
    test.evaluate_all("cartesian_product2 stack", {
        {"cartesian_product2!([1 2] [3 4])", "[(2 4) (1 4) (2 3) (1 3)]"},
    });
    test.evaluate_all("put_column stack", {
        {"put_column!([1 2] [[3] [4]])", "[[1 3] [2 4]]"},
    });
    test.evaluate_all("transpose stack", {
        {"transpose![[]]", "[]"},
        {"transpose![[1]]", "[[1]]"},
        {"transpose![[1] [2]]", "[[1 2]]"},
        {"transpose![[1 2] [3 4]]", "[[1 3] [2 4]]"},
    });
    test.evaluate_all("sum stack", {
        {"sum![]", "0"},
        {"sum![1]", "1"},
        {"sum![1 2]", "3"},
        {"sum![1 2 3]", "6"},
    });
    test.evaluate_types("sum stack", {
        {"sum![]", "NUMBER"},
        {"sum![1]", "NUMBER"},
        {"sum![1 2]", "NUMBER"},
        {"sum![1 2 3]", "NUMBER"},
    });
    test.evaluate_all("product", {
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
    test.evaluate_all("put_each stack", {
        {"put_each!([] [])", "[]"},
        {"put_each!([] [2])", "[2]"},
        {"put_each!([1] [])", "[1]"},
        {"put_each!([1] [2])", "[1 2]"},
        {"put_each!([1 2] [3 4])", "[2 1 3 4]"},
        {"put_each!([1 2 3] [4 5 6])", "[3 2 1 4 5 6]"},
    });
    test.evaluate_all("put_each table", {
        {"put_each!(<> <>)", "<>"},
        {"put_each!(<> <1:2>)", "<1:2>"},
        {"put_each!(<1:2> <>)", "<1:2>"},
        {"put_each!(<1:2> <1:1>)", "<1:2>"},
        {"put_each!(<1:11> <2:22>)", "<1:11 2:22>"},
        {"put_each!(<1:11 3:33> <2:22 4:44>)", "<1:11 2:22 3:33 4:44>"},
    });
    test.evaluate_types("put_each", {
        {"put_each!([] [])", "[ANY]"},
        {"put_each!([] [2])", "[ANY]"}, // TODO
        {"put_each!([1] [])", "[NUMBER]"},
        {"put_each!([1] [2])", "[NUMBER]"},
        {"put_each!([1 2] [3 4])", "[NUMBER]"},
        {"put_each!([1 2 3] [4 5 6])", "[NUMBER]"},
    });
    test.evaluate_all("put_each string", {
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
    test.evaluate_all("merge_stack stack", {
        {"merge_stack![[] []]", "[]"},
        {"merge_stack![[] [2]]", "[2]"},
        {"merge_stack![[1] []]", "[1]"},
        {"merge_stack![[1] [2]]", "[1 2]"},
        {"merge_stack![[1] [2 3] [4 5 6]]", "[1 2 3 4 5 6]"},
    });
    test.evaluate_all("merge_stack table", {
        {"merge_stack![[(1 11)] <2:22>]", "[(1 11) (2 22)]"},
    });
    test.evaluate_all("merge_stack string", {
        {R"(merge_stack![['a'] "b"])", "['a' 'b']"},
    });
    test.evaluate_types("merge_stack", {
        {"merge_stack![[] []]", "[ANY]"},
        {"merge_stack![[] [2]]", "[ANY]"}, // TODO
        {"merge_stack![[1] []]", "[NUMBER]"},
        {"merge_stack![[1] [2]]", "[NUMBER]"},
        {"merge_stack![[1] [2 3] [4 5 6]]", "[NUMBER]"},
    });
    test.evaluate_all("merge_table", {
        {"merge_table![<> <>]", "<>"},
        {"merge_table![<> <2:22>]", "<2:22>"},
        {"merge_table![<1:11> <>]", "<1:11>"},
        {"merge_table![<1:11> <1:11>]", "<1:11>"},
        {"merge_table![<1:11> <2:22>]", "<1:11 2:22>"},
        {"merge_table![<1:11> <5:55 3:33> <4:44 5:0 6:66 2:22>]", "<1:11 2:22 3:33 4:44 5:55 6:66>"},
        {"merge_table![[(1 11)] <2:22>]", "<1:11 2:22>"},
    });
    test.evaluate_all("merge_string", {
        {R"(merge_string!["" ""])", R"("")"},
        {R"(merge_string!["" "b"])", R"("b")"},
        {R"(merge_string!["a" ""])", R"("a")"},
        {R"(merge_string!["a" "b"])", R"("ab")"},
        {R"(merge_string!["a" "bc" "def"])", R"("abcdef")"},
        {R"(merge_string![['a'] "b"])", R"("ab")"},
    });
    test.evaluate_types("merge_string", {
        {R"(merge_string!["" ""])", "STRING"},
        {R"(merge_string!["" "b"])", "STRING"},
        {R"(merge_string!["a" ""])", "STRING"},
        {R"(merge_string!["a" "b"])", "STRING"},
        {R"(merge_string!["a" "bc" "def"])", "STRING"},
    });
    test.evaluate_all("drop_many stack", {
        {"drop_many!(0 [3 7 6])", "[3 7 6]"},
        {"drop_many!(1 [3 7 6])", "[7 6]"},
        {"drop_many!(2 [3 7 6])", "[6]"},
        {"drop_many!(3 [3 7 6])", "[]"},
    });
    test.evaluate_all("drop_many string", {
        {R"(drop_many!(0 "abc"))", R"("abc")"},
        {R"(drop_many!(1 "abc"))", R"("bc")"},
        {R"(drop_many!(2 "abc"))", R"("c")"},
        {R"(drop_many!(3 "abc"))", R"("")"},
    });
    test.evaluate_all("drop_while string", {
        {R"(drop_while!(is_upper ""))", R"("")"},
        {R"(drop_while!(is_upper "a"))", R"("a")"},
        {R"(drop_while!(is_upper "A"))", R"("")"},
        {R"(drop_while!(is_upper "ABcd"))", R"("cd")"},
    });
    test.evaluate_all("drop_until_item stack", {
        {"drop_until_item!(1 [])", "[]"},
        {"drop_until_item!(2 [1])", "[]"},
        {"drop_until_item!(1 [1])", "[1]"},
        {"drop_until_item!(3 [1 2 3 4])", "[3 4]"},
    });
    test.evaluate_all("drop_until_item string", {
        {R"(drop_until_item!('a' ""))", R"("")"},
        {R"(drop_until_item!('a' "a"))", R"("a")"},
        {R"(drop_until_item!('a' "b"))", R"("")"},
        {R"(drop_until_item!('a' "ab"))", R"("ab")"},
        {R"(drop_until_item!('a' "ba"))", R"("a")"},
    });
    test.evaluate_all("take_many stack", {
        {"take_many!(0 [3 7 6])", "[]"},
        {"take_many!(1 [3 7 6])", "[3]"},
        {"take_many!(2 [3 7 6])", "[3 7]"},
        {"take_many!(3 [3 7 6])", "[3 7 6]"},
    });
    test.evaluate_all("take_many string", {
        {R"(take_many!(0 "abc"))", R"("")"},
        {R"(take_many!(1 "abc"))", R"("a")"},
        {R"(take_many!(2 "abc"))", R"("ab")"},
        {R"(take_many!(3 "abc"))", R"("abc")"},
    });
    test.evaluate_all("take_while string", {
        {R"(take_while!(is_upper ""))", R"("")"},
        {R"(take_while!(is_upper "a"))", R"("")"},
        {R"(take_while!(is_upper "A"))", R"("A")"},
        {R"(take_while!(is_upper "ABcd"))", R"("AB")"},
    });
    test.evaluate_all("take_until_item stack", {
        {"take_until_item!(1 [])", "[]"},
        {"take_until_item!(2 [1])", "[1]"},
        {"take_until_item!(1 [1])", "[]"},
        {"take_until_item!(3 [1 2 3 4])", "[1 2]"},
    });
    test.evaluate_all("take_until_item string", {
        {R"(take_until_item!('a' ""))", R"("")"},
        {R"(take_until_item!('b' "a"))", R"("a")"},
        {R"(take_until_item!('a' "a"))", R"("")"},
        {R"(take_until_item!('c' "ABcd"))", R"("AB")"},
    });
    test.evaluate_all("merge_sorted", {
        {"merge_sorted!([] [])", "[]"},
        {"merge_sorted!([0] [])", "[0]"},
        {"merge_sorted!([] [0])", "[0]"},
        {"merge_sorted!([0] [0])", "[0 0]"},
        {"merge_sorted!([0 2] [1 3])", "[0 1 2 3]"},
        {"merge_sorted!([0 1 2 5 8 9] [0 2 4 6 7])", "[0 0 1 2 2 4 5 6 7 8 9]"},
    });
    test.evaluate_all("indexing tuple", {
        {"get0!(11 12 13 14 15 16 17 18 19 20)", "11"},
        {"get1!(11 12 13 14 15 16 17 18 19 20)", "12"},
        {"get2!(11 12 13 14 15 16 17 18 19 20)", "13"},
        {"get3!(11 12 13 14 15 16 17 18 19 20)", "14"},
        {"get4!(11 12 13 14 15 16 17 18 19 20)", "15"},
        {"get5!(11 12 13 14 15 16 17 18 19 20)", "16"},
        {"get6!(11 12 13 14 15 16 17 18 19 20)", "17"},
        {"get7!(11 12 13 14 15 16 17 18 19 20)", "18"},
        {"get8!(11 12 13 14 15 16 17 18 19 20)", "19"},
        {"get9!(11 12 13 14 15 16 17 18 19 20)", "20"},
    });
    test.evaluate_all("indexing stack", {
        {"get0![11 12 13 14 15 16 17 18 19]", "11"},
        {"get1![11 12 13 14 15 16 17 18 19]", "12"},
        {"get2![11 12 13 14 15 16 17 18 19]", "13"},
        {"get3![11 12 13 14 15 16 17 18 19]", "14"},
        {"get4![11 12 13 14 15 16 17 18 19]", "15"},
        {"get5![11 12 13 14 15 16 17 18 19]", "16"},
        {"get6![11 12 13 14 15 16 17 18 19]", "17"},
        {"get7![11 12 13 14 15 16 17 18 19]", "18"},
        {"get8![11 12 13 14 15 16 17 18 19]", "19"},
        {"get9![11 12 13 14 15 16 17 18 19 20]", "20"},
    });
    test.evaluate_all("indexing string", {
        {R"(get0!"abcdefghijklmnopqrstuvwxyz")", "'a'"},
        {R"(get1!"abcdefghijklmnopqrstuvwxyz")", "'b'"},
        {R"(get2!"abcdefghijklmnopqrstuvwxyz")", "'c'"},
        {R"(get3!"abcdefghijklmnopqrstuvwxyz")", "'d'"},
        {R"(get4!"abcdefghijklmnopqrstuvwxyz")", "'e'"},
        {R"(get5!"abcdefghijklmnopqrstuvwxyz")", "'f'"},
        {R"(get6!"abcdefghijklmnopqrstuvwxyz")", "'g'"},
        {R"(get7!"abcdefghijklmnopqrstuvwxyz")", "'h'"},
        {R"(get8!"abcdefghijklmnopqrstuvwxyz")", "'i'"},
        {R"(get9!"abcdefghijklmnopqrstuvwxyz")", "'j'"},
    });
    test.evaluate_all("zip2", {
        {"zip2!([] [])", "[]"},
        {"zip2!([0] [1])", "[(0 1)]"},
        {"zip2!([0 1] [2 3])", "[(0 2) (1 3)]"},
        {"zip2!([0 1 2] [3 4 5])", "[(0 3) (1 4) (2 5)]"},
    });
    test.evaluate_types("zip2", {
        {"zip2!([] [])", "[(ANY ANY)]"},
        {"zip2!([0] [1])", "[(NUMBER NUMBER)]"},
        {"zip2!([0 1] [2 3])", "[(NUMBER NUMBER)]"},
        {"zip2!([0 1 2] [3 4 5])", "[(NUMBER NUMBER)]"},
    });
    test.evaluate_all("zip3", {
        {"zip3!([0 1 2] [3 4 5] [6 7 8])", "[(0 3 6) (1 4 7) (2 5 8)]"},
    });
    test.evaluate_all("zip4", {
        {"zip4!([0 1] [2 3] [4 5] [6 7])", "[(0 2 4 6) (1 3 5 7)]"},
    });
    test.evaluate_all("consecutive_pairs", {
        {"consecutive_pairs![6 4 8]", "[(6 4) (4 8)]"},
    });
    test.evaluate_all("unique", {
        {"unique![]", "[]"},
        {"unique![1]", "[1]"},
        {"unique![1 2]", "[1 2]"},
        {"unique![1 1]", "[1]"},
        {"unique![1 1 2 3 1 4 2 4 0]", "[0 1 2 3 4]"},
    });
    test.evaluate_all("count_elements", {
        {"count_elements![]", "<>"},
        {"count_elements![1]", "<1:1>"},
        {"count_elements![1 2]", "<1:1 2:1>"},
        {"count_elements![1 1]", "<1:2>"},
        {"count_elements![1 1 2 3 1 4 2 4 0]", "<0:1 1:3 2:2 3:1 4:2>"},
    });
    return test.exitCode();
}
