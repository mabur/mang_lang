#include "mang_lang.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

typedef struct TestCase {
    const char* input;
    const char* expected;
} TestCase;

typedef struct TestCases {
    TestCase* data;
    size_t count;
} TestCases;

#define TEST_CASES(...) MAKE_RANGE(TestCases, __VA_ARGS__)

int num_good_total = 0;
int num_bad_total = 0;
clock_t duration_total = 0;

typedef StringBuilder (*TestFunction)(const char*);

void parameterizedTest(
    TestFunction function,
    const char* function_name,
    const char* case_name,
    TestCases test_cases
) {
    auto num_good = 0;
    auto num_bad = 0;
    auto start = clock();
    FOR_EACH(test_case, test_cases) {
        auto actual = function(test_case->input);
        if (strcmp(test_case->expected, actual.data) == 0) {
            ++num_good;
        } else {
            ++num_bad;
            printf(
                "\n%s(%s) expected %s got %s\n",
                function_name, test_case->input, test_case->expected, actual.data
            );
        }
    }
    num_good_total += num_good;
    num_bad_total += num_bad;
    duration_total += clock() - start;
    printf(
        "%d/%d tests successful for case %s:%s\n",
        num_good, num_good + num_bad, function_name, case_name
    );
}

int summarizeTests() {
    printf("\n%d/%d tests successful in total. ", num_good_total, num_good_total + num_bad_total);
    if (num_bad_total != 0) {
        printf("%d TESTS FAILING! ", num_bad_total);
    }
    printf("Duration %.1f seconds.", (double)duration_total / CLOCKS_PER_SEC);
    return num_bad_total;
}

void testReformat(const char* case_name, TestCases test_cases) {
    parameterizedTest(reformat, "reformat", case_name, test_cases);
}

void testEvaluateTypes(const char* case_name, TestCases test_cases) {
    parameterizedTest(evaluate_types, "evaluate_types", case_name, test_cases);
}

void testEvaluateAll(const char* case_name, TestCases test_cases) {
    parameterizedTest(evaluate_all, "evaluate_all", case_name, test_cases);
}

int main() {
    testReformat("expression", TEST_CASES(
        {"", "I did not find any expression to parse."},
        {"out", "Parse error. 'out' is a reserved keyword."},
        {"then", "Parse error. 'then' is a reserved keyword."},
        {"else", "Parse error. 'else' is a reserved keyword."},
        {"while", "Parse error. 'while' is a reserved keyword."},
        {"end", "Parse error. 'end' is a reserved keyword."},
    ));
    testEvaluateAll("number", TEST_CASES(
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
        {"+", "Reached end of file when parsing number"},
        {"-", "Reached end of file when parsing number"},
        {"1.", "Reached end of file when parsing number"},
    ));
    testEvaluateTypes("number", TEST_CASES(
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
    ));
    testReformat("character", TEST_CASES(
        {"'a'", "'a'"},
        {"'\n'", "'\n'"},
        {"'", "I found an error while parsing a character.\nIt ends too early."},
        {"'a", "I found an error while parsing a character.\nIt ends too early."},
        {"'\n", "I found an error while parsing a character.\nIt ends too early."}
    ));
    testEvaluateAll("character", TEST_CASES(
        {"'a'", "'a'"},
        {"'1'", "'1'"},
        {"'+'", "'+'"},
        {"'-'", "'-'"},
        {R"('"')", R"('"')"},
        {"')'", "')'"},
        {"')'", "')'"},
        {"'{'", "'{'"},
        {"'}'", "'}'"},
    ));
    testEvaluateTypes("character", TEST_CASES(
        {"'a'", "CHARACTER"},
        {"'1'", "CHARACTER"},
        {"'+'", "CHARACTER"},
        {"'-'", "CHARACTER"},
        {R"('"')", "CHARACTER"},
        {"')'", "CHARACTER"},
        {"')'", "CHARACTER"},
        {"'{'", "CHARACTER"},
        {"'}'", "CHARACTER"},
    ));
    testEvaluateAll("boolean", TEST_CASES(
        {"yes", "yes"},
        {"no", "no"},
    ));
    testEvaluateTypes("boolean", TEST_CASES(
        {"yes", "YES"},
        {"no", "NO"},
    ));
    testEvaluateAll("string", TEST_CASES(
        {R"("")", R"("")"},
        {R"("a")", R"("a")"},
        {R"("ab")", R"("ab")"},
        {R"("abc")", R"("abc")"},
        {R"("1")", R"("1")"},
        {R"("-1")", R"("-1")"},
        {R"("+1")", R"("+1")"},
        {R"#("()")#", R"#("()")#"},
        {R"("{}")", R"("{}")"},
    ));
    testEvaluateTypes("string", TEST_CASES(
        {R"("")", "EMPTY_STRING"},
        {R"("a")", "STRING"},
        {R"("ab")", "STRING"},
        {R"("abc")", "STRING"},
        {R"("1")", "STRING"},
        {R"("-1")", "STRING"},
        {R"("+1")", "STRING"},
        {R"#("()")#", "STRING"},
        {R"("{}")", "STRING"},
    ));
    testReformat("stack", TEST_CASES(
        {"[", "I found an error while parsing a stack.\nIt is missing a closing ']'."},
    ));
    testEvaluateAll("stack", TEST_CASES(
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
    ));
    testEvaluateTypes("stack", TEST_CASES(
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
    ));
    testReformat("table", TEST_CASES(
        {"<>", "<>"},
        {"<(1 2)>", "<(1 2)>"},
        {"< (1  2 ) >", "<(1 2)>"},
        {"<(<> <>)>", "<(<> <>)>"},
        {"<((0 0) (1 1))>", "<((0 0) (1 1))>"},
        {"<(inc!0 inc!1)>", "<(inc!0 inc!1)>"},
        {"<","I found an error while parsing a table.\nIt is missing a closing '>'."},
    ));
    testEvaluateTypes("table", TEST_CASES(
        {"<>", "<>"},
        {"<(1 2)>", "<(NUMBER NUMBER)>"},
        {"< (1  2 ) >", "<(NUMBER NUMBER)>"},
        {"<(<> <>)>", "<(<> <>)>"},
        {"<((0 0) (1 1))>", "<((NUMBER NUMBER) (NUMBER NUMBER))>"},
        {"<(inc!0 inc!1)>", "<(NUMBER NUMBER)>"},
        {"<(3 6) (4 8) (1 2) (2 4)>","<(NUMBER NUMBER)>"}
    ));
    testEvaluateAll("table", TEST_CASES(
        {"<>", "<>"},
        {"<(1 2)>", "<(1 2)>"},
        {"< (1  2 ) >", "<(1 2)>"},
        {"<(<> <>)>", "<(<> <>)>"},
        {"<((0 0) (1 1))>", "<((0 0) (1 1))>"},
        {"<(inc!0 inc!1)>", "<(1 2)>"},
        {"<(3 6) (4 8) (1 2) (2 4)>","<(1 2) (2 4) (3 6) (4 8)>"}
    ));
    testReformat("tuple", TEST_CASES(
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
        {"(", "I found an error while parsing a tuple.\nIt is missing a closing ')'."},
    ));
    testEvaluateTypes("tuple", TEST_CASES(
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
    ));
    testEvaluateAll("tuple", TEST_CASES(
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
    ));
    testEvaluateAll("tuple type checking", TEST_CASES(
        {"b@{a=() b=a:()}", "()"},
        {"b@{a=(1) b=a:(1)}", "(1)"},
        {"b@{a=(1 'a') b=a:(1 'a')}", "(1 'a')"},
    ));
    testEvaluateAll("types", TEST_CASES(
        {"Any:1", "1"},
        {"Number:1", "1"},
        {"Boolean:yes", "yes"},
        {"Character:'Q'", "'Q'"},
        {"Stack:[1]", "[1]"},
        {"Table:<(1 2)>", "<(1 2)>"},
        {"Numbers:[1 2]", "[1 2]"},
        {"Function:in x out x", "in x out x"},
        {"{a=Any b=a:1}", "{a=0 b=1}"},
    ));
    testReformat("dictionary iteration", TEST_CASES(
        {"{while 1 end}", "{while 1 end}"},
        {"{i=2 while i i=dec!i end}", "{i=2 while i i=dec!i end}"},
        {"{i=10 while i i=dec!i end j=1}", "{i=10 while i i=dec!i end j=1}"},
        {"{i=[] i++=[1]}", "{i=[] i++=[1]}"},
        {"{end}", "I find a parsing error.\nend is not matching a while or for"},
        {"{", "I found an error while parsing a dictionary.\nIt ended too early."},
    ));
    testReformat("dictionary for", TEST_CASES(
        {"{for i in c end}", "{for i in c end}"},
        {"{for c end}", "{for c end}"},
    ));
    testEvaluateTypes("dictionary iteration", TEST_CASES(
        {"{while 1 end}", "{}"},
        {"{i=2 while i i=dec!i end}", "{i=NUMBER}"},
        {"{i=10 while i i=dec!i end j=1}", "{i=NUMBER j=NUMBER}"},
        {"{c=yes for c return end s=0}", "{c=YES s=NUMBER}"},
        {"{c=no for c return end s=0}", "{c=NO s=NUMBER}"},
        {"{i=[] i++=[1]}", "{i=[NUMBER]}"},
    ));
    testEvaluateAll("dictionary iterations", TEST_CASES(
        {"{i=2 while i i=dec!i end}", "{i=0}"},
        {"{i=2 while i i=dec!i end j=1}", "{i=0 j=1}"},
        {"{i=2 tot=0 while i tot=add!(tot i) i=dec!i end}", "{i=0 tot=3}"},
        {"{i=1000 tot=0 while i tot=add!(tot i) i=dec!i end}", "{i=0 tot=500500}"},
        {"{i=2 c=[] while i c+=i i=dec!i end}", "{i=0 c=[1 2]}"},
        {"{i=[] i++=[]}", "{i=[]}"},
        {"{i=[] i++=[1]}", "{i=[1]}"},
        {"{i=[] i++=[1 2]}", "{i=[2 1]}"},
    ));
    testEvaluateAll("dictionary for", TEST_CASES(
        {"{c=3 s=0 for c s+=c end}", "{c=0 s=6}"},
        {"{c=3 s=0 for i in c s+=c end}", "{c=0 s=6 i=1}"},
        {"{c=yes for c return end s=0}", "{c=yes s=ANY}"}, // TODO: think about
        {"{c=no for c return end s=0}", "{c=no s=0}"},
        {"{c=yes s=0 for c s+=1 end}", "{c=no s=1}"},
        {"{c=yes s=0 for i in c s+=1 end}", "{c=no s=1 i=yes}"},
        {"{c=no s=0 for i in c s+=1 end}", "{c=no s=0 i=ANY}"}, // TODO: think about
        {"{c=[] for i in c end}", "{c=[] i=ANY}"}, // TODO: think about
        {"{c=[1] for i in c end}", "{c=[] i=1}"},
        {"{c=[1 2] for i in c end}", "{c=[] i=2}"},
        {"{a=[] c=[1 2 3] for i in c a+=i end}", "{a=[3 2 1] c=[] i=3}"},
        {"{c=[1 2] for i in c d=[2 3] for j in d end end}", "{c=[] i=2 d=[] j=3}"},
        {"r@{f=in c out {d=c for i in d end} r=f![]}", "{d=[] i=ANY}"},
        {"r@{f=in c out {d=c for i in d end} r=f![1]}", "{d=[] i=1}"},
    ));
    testEvaluateTypes("dictionary for", TEST_CASES(
        {"{c=[] for c end}", "{c=EMPTY_STACK}"},
        {"{c=[] for i in c end}", "{c=EMPTY_STACK i=ANY}"},
        {"{c=<> for i in c end}", "{c=<> i=(ANY ANY)}"},
        {"{c=<> d=<> for i in c d+=i end}", "{c=<> d=<(ANY ANY)> i=(ANY ANY)}"}, // TODO 
    ));
    testEvaluateTypes("ANY in dictionary", TEST_CASES(
        {"y@{y=take![]}", "ANY"},
        {"y@{y=1 y=take![]}", "NUMBER"},
    ));
    testEvaluateTypes("dictionary", TEST_CASES(
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
        {"{a=<> a+=(1 2)}", "{a=<(NUMBER NUMBER)>}"},
        {"{a=0 a+=1}", "{a=NUMBER}"},
    ));    
    testEvaluateTypes("dictionary type checking", TEST_CASES(
        {"{a={} b=a:{}}", "{a={} b={}}"},
        {"{a={} b=a:{x=1}}", "{a={} b={x=NUMBER}}"},
        {"{a={x=1} b=a:{x=1}}", "{a={x=NUMBER} b={x=NUMBER}}"},
        {"{a={} b=a:{x=1 y=1}}", "{a={} b={x=NUMBER y=NUMBER}}"},
        {"{a={x=1} b=a:{x=1 y=1}}", "{a={x=NUMBER} b={x=NUMBER y=NUMBER}}"},
        {"{a={x=1 y=1} b=a:{x=1 y=1}}", "{a={x=NUMBER y=NUMBER} b={x=NUMBER y=NUMBER}}"},
        {"{a={} b=a:{y=1 x=1}}", "{a={} b={y=NUMBER x=NUMBER}}"},
        {"{a={x=1} b=a:{y=1 x=1}}", "{a={x=NUMBER} b={y=NUMBER x=NUMBER}}"},
        {"{a={x=1 y=1} b=a:{y=1 x=1}}", "{a={x=NUMBER y=NUMBER} b={y=NUMBER x=NUMBER}}"},
    ));
    testEvaluateAll("dictionary", TEST_CASES(
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
        {"{a=<> a+=(1 2)}", "{a=<(1 2)>}"},
        {"{a=3 a+=4}", "{a=7}"},
    ));
    testReformat("drop assignment", TEST_CASES(
        {"{c=1 c--}", "{c=1 c--}"},
        {"{a=[1] while a a-- end}", "{a=[1] while a a-- end}"},
    ));
    testEvaluateTypes("drop assignment", TEST_CASES(
        {"{c=1 c--}", "{c=NUMBER}"},
        {"{c=[1] c--}", "{c=[NUMBER]}"},
        {"{c=<(1 2)> c--}", "{c=<(NUMBER NUMBER)>}"},
        {"{a=[1] while a a-- end}", "{a=[NUMBER]}"},
    ));
    testEvaluateAll("drop assignment", TEST_CASES(
        {"{c=1 c--}", "{c=0}"},
        {"{c=[1] c--}", "{c=[]}"},
        {"{c=[1 2] c--}", "{c=[2]}"},
        {"{c=<(1 2)> c--}", "{c=<>}"},
        {"{c=<(1 2) (3 4)> c--}", "{c=<(3 4)>}"},
        {"{a=[1] while a a-- end}", "{a=[]}"},
        {"{c=yes c--}", "{c=no}"},
    ));
    testEvaluateAll("put assignment", TEST_CASES(
        {"{c=1 c+=2}", "{c=3}"},
        {"{c=yes c+=yes}", "{c=yes}"},
        {"{c=yes c+=no}", "{c=no}"},
        {"{c=no c+=yes}", "{c=yes}"},
        {"{c=no c+=no}", "{c=no}"},
        {"{c=[1] c+=2}", "{c=[2 1]}"},
        {"{c=<(1 2)> c+=(0 3)}", "{c=<(0 3) (1 2)>}"},
    ));
    testReformat("conditional", TEST_CASES(
        {"if 1 then 2 else 3", "if 1 then 2 else 3"},
        {"if  1  then  2  else  3", "if 1 then 2 else 3"},
        {"if a then b else c", "if a then b else c"},
        {"if  a  then  b  c  then  e  else  f", "if a then b c then e else f"},
    ));
    testEvaluateTypes("conditional", TEST_CASES(
        {"if 1 then 2 else 3", "NUMBER"},
        {"if 0 then 2 else 3", "NUMBER"},
        {"if [0] then 2 else 3", "NUMBER"},
        {"if [] then 2 else 3", "NUMBER"},
        {"if 1 then 2 3 then 4 else 5", "NUMBER"},
    ));
    testEvaluateAll("conditional", TEST_CASES(
        {"if 1 then 2 else 3", "2"},
        {"if 0 then 2 else 3", "3"},
        {"if [0] then 2 else 3", "2"},
        {"if [] then 2 else 3", "3"},
        {"if 1 then 2 3 then 4 else 5", "2"},
        {"if 0 then 1 [] then 2 else 3", "3"},
        {"if yes then [] else [1]", "[]"},
        {"if yes then [1] else []", "[1]"},
        {R"(if yes then "" else "a")", R"("")"},
        {R"(if yes then "a" else "")", R"("a")"},
    ));
    testEvaluateAll("nested conditional", TEST_CASES(
        {"if 0 then if 1 then 2 else 3 4 then 5 else 6", "5"},
    ));
    testReformat("is", TEST_CASES(
        {"is 0 0 then 0 else 0", "is 0 0 then 0 else 0"},
        {"is 0 0 then 0 1 then 1 else 0", "is 0 0 then 0 1 then 1 else 0"},
    ));
    testEvaluateTypes("is", TEST_CASES(
        {"is 0 0 then 1 else 2", "NUMBER"},
        {"is 1 0 then 1 else 2", "NUMBER"},
        {"is 0 0 then 0 1 then 1 2 then 4 else 5", "NUMBER"},
        {"is 1 0 then 0 1 then 1 2 then 4 else 5", "NUMBER"},
        {"is 2 0 then 0 1 then 1 2 then 4 else 5", "NUMBER"},
        {"is 3 0 then 0 1 then 1 2 then 4 else 5", "NUMBER"},
        {"is 0 0 then [] else [1]", "[NUMBER]"},
        {"is 0 0 then [1] else []", "EMPTY_STACK"},
        {R"(is 0 0 then "" else "a")", "STRING"},
        {R"(is 0 0 then "a" else "")", "EMPTY_STRING"},
    ));
    testEvaluateAll("is", TEST_CASES(
        {"is 0 0 then 1 else 2", "1"},
        {"is 1 0 then 1 else 2", "2"},
        {"is 0 0 then 0 1 then 1 2 then 4 else 5", "0"},
        {"is 1 0 then 0 1 then 1 2 then 4 else 5", "1"},
        {"is 2 0 then 0 1 then 1 2 then 4 else 5", "4"},
        {"is 3 0 then 0 1 then 1 2 then 4 else 5", "5"},
        {"is (1 2) (1 2) then 1 else 2", "1"},
        {"is (1 2) (1 3) then 1 else 2", "2"},
        {"is (1 (2)) (1 (2)) then 1 else 2", "1"},
        {"is (1 (2)) (1 (3)) then 1 else 2", "2"},
    ));
    testReformat("symbol", TEST_CASES(
        {"a", "a"},
        {"{a=1 b=a}", "{a=1 b=a}"},
    ));
    testEvaluateTypes("symbol", TEST_CASES(
        {"{a=1 b=a}", "{a=NUMBER b=NUMBER}"},
        {"{a=1 b_0=a}", "{a=NUMBER b_0=NUMBER}"},
        {"{a=1 b={c=a}}", "{a=NUMBER b={c=NUMBER}}"},
        {"{a=1 b={c={d=a}}}", "{a=NUMBER b={c={d=NUMBER}}}"},
        {"{a=1 b=[a]}", "{a=NUMBER b=[NUMBER]}"},
        {"{a=1 b=[[a]]}", "{a=NUMBER b=[[NUMBER]]}"},
        {"{a=1 b=c@{c=a}}", "{a=NUMBER b=NUMBER}"},
        {"{a=1 b=add!(a a)}", "{a=NUMBER b=NUMBER}"},
        {"{a=1 b=if a then a else 2}", "{a=NUMBER b=NUMBER}"},
    ));
    testEvaluateAll("symbol", TEST_CASES(
        {"{a=1 b=a}", "{a=1 b=1}"},
        {"{a=1 b_0=a}", "{a=1 b_0=1}"},
        {"{a=1 b={c=a}}", "{a=1 b={c=1}}"},
        {"{a=1 b={c={d=a}}}", "{a=1 b={c={d=1}}}"},
        {"{a=1 b=[a]}", "{a=1 b=[1]}"},
        {"{a=1 b=[[a]]}", "{a=1 b=[[1]]}"},
        {"{a=1 b=c@{c=a}}", "{a=1 b=1}"},
        {"{a=1 b=add!(a a)}", "{a=1 b=2}"},
        {"{a=1 b=if a then a else 2}", "{a=1 b=1}"},
    ));
    testReformat("child_symbol", TEST_CASES(
        {"a@{a=1}", "a@{a=1}"},
        {"A_0@{A_0=1}", "A_0@{A_0=1}"},
    ));
    testEvaluateTypes("child_symbol", TEST_CASES(
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
    ));
    testEvaluateAll("child_symbol", TEST_CASES(
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
    ));
    testEvaluateAll("child_symbol_keyword_confusion", TEST_CASES(
        {"input@{input=5}", "5"},
        {"output@{output=5}", "5"},
        {"iffy@{iffy=5}", "5"},
        {"thenner@{thenner=5}", "5"},
        {"elsewhere@{elsewhere=5}", "5"},
        {"whiler@{whiler=5}", "5"},
        {"endar@{endar=5}", "5"},
    ));
    testReformat("lookup_function", TEST_CASES(
        {"add!(1 2)", "add!(1 2)"},
    ));
    testReformat("function", TEST_CASES(
        {"in T:x out x", "in T:x out x"}, // TODO
        {"in {x out x", "I found an error while parsing a function.\nThe input had a starting '{' but no ending '}'."},
        {"in (", "I found an error while parsing a function.\nThe function definition ended too early."},
    ));
    testEvaluateTypes("function", TEST_CASES(
        {"in x out x", "FUNCTION"},
        {"in T:x out x", "FUNCTION"}, // TODO
    ));
    testEvaluateAll("function", TEST_CASES(
        {"in x out x", "in x out x"},
        {"f@{f=in x out x}", "in x out x"},
        {"f@{T=1 f=in T:x out x}", "in T:x out x"}, //TODO:
    ));
    testEvaluateTypes("function dictionary", TEST_CASES(
        {"in {x} out x", "FUNCTION_DICTIONARY"},
        {"in {x y} out x", "FUNCTION_DICTIONARY"},
        {"in  {  x    y  }  out  x", "FUNCTION_DICTIONARY"},
    ));
    testEvaluateAll("function dictionary", TEST_CASES(
        {"in {x} out x", "in {x} out x"},
        {"in {x y} out x", "in {x y} out x"},
        {"in  {  x    y  }  out  x", "in {x y} out x"},
    ));
    testEvaluateTypes("function tuple", TEST_CASES(
        {"in (x) out x", "FUNCTION_TUPLE"},
        {"in (x y) out x", "FUNCTION_TUPLE"},
        {"in  (  x    y  )  out  x", "FUNCTION_TUPLE"},
    ));
    testEvaluateAll("function tuple", TEST_CASES(
        {"in (x) out x", "in (x) out x"},
        {"in (x y) out x", "in (x y) out x"},
        {"in  (  x    y  )  out  x", "in (x y) out x"},
    ));
    testEvaluateTypes("lookup function", TEST_CASES(
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
    ));
    testEvaluateAll("lookup function", TEST_CASES(
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
        {"a@{T=1 f=in T:x out x a=f!0}", "0"},
        {"a@{T=1 f=in (T:x T:y) out x a=f!(0 0)}", "0"},
        {"a@{T=1 f=in {T:x T:y} out x a=f!{x=0 y=0}}", "0"},
    ));
    testEvaluateTypes("recursive function", TEST_CASES(
        {"y@{f=in x out dynamic if x then add!(x f!dec!x) else 0 y=f!3}", "ANY"},
    ));
    testEvaluateAll("recursive function", TEST_CASES(
        {"y@{f=in x out dynamic if x then add!(x f!dec!x) else 0 y=f!3}", "6"},
    ));
    testReformat("dynamic", TEST_CASES(
        {"dynamic 1", "dynamic 1"},
    ));
    testEvaluateTypes("dynamic", TEST_CASES(
        {"dynamic 1", "ANY"},
    ));
    testEvaluateTypes("dynamic", TEST_CASES(
        {"dynamic if x then add!(x f!dec!x) else 0", "ANY"},
    ));
    testEvaluateAll("dynamic", TEST_CASES(
        {"dynamic 1", "1"},
    ));
    testReformat("typed expression", TEST_CASES(
        {"a:b", "a:b"},
        {" a : b ", "a:b"},
    ));
    testEvaluateTypes("typed expression", TEST_CASES(
        {"{a=1 b=a:1}", "{a=NUMBER b=NUMBER}"},
        {"{a=[] b=a:[]}", "{a=EMPTY_STACK b=EMPTY_STACK}"},
        {"{a=[] b=a:[1]}", "{a=EMPTY_STACK b=[NUMBER]}"},
        {"{a=[1] b=a:[]}", "{a=[NUMBER] b=EMPTY_STACK}"}, // TODO
    ));
    testEvaluateTypes("lookup function dictionary", TEST_CASES(
        {"a@{f=in {x} out x a=f!{x=0}}", "NUMBER"},
        {"a@{f=in {x y} out add!(x y) a=f!{x=2 y=3}}", "NUMBER"},
        {"a@{b=2 f=in {x} out add!(b x) a=f!{x=0}}", "NUMBER"},
    ));
    testEvaluateAll("lookup function dictionary", TEST_CASES(
        {"a@{f=in {x} out x a=f!{x=0}}", "0"},
        {"a@{f=in {x y} out add!(x y) a=f!{x=2 y=3}}", "5"},
        {"a@{b=2 f=in {x} out add!(b x) a=f!{x=0}}", "2"},
    ));
    testEvaluateTypes("lookup function tuple", TEST_CASES(
        {"a@{f=in (x) out x a=f!(0)}", "NUMBER"},
        {"a@{f=in (x y) out add!(x y) a=f!(2 3)}", "NUMBER"},
    ));
    testEvaluateAll("lookup function tuple", TEST_CASES(
        {"a@{f=in (x) out x a=f!(0)}", "0"},
        {"a@{f=in (x y) out add!(x y) a=f!(2 3)}", "5"},
    ));
    testEvaluateAll("lookup tuple indexing", TEST_CASES(
        {"a@{c=(3 2 1) a=c!0}", "3"},
        {"a@{c=(3 2 1) a=c!1}", "2"},
        {"a@{c=(3 2 1) a=c!2}", "1"},
        {"a@{i=1 c=(3 2 1) a=c!i}", "2"},
    ));
    testEvaluateTypes("lookup stack indexing", TEST_CASES(
        {"a@{a=1 c=[] a=c!0}", "NUMBER"},
    ));
    testEvaluateAll("lookup stack indexing", TEST_CASES(
        {"a@{c=[3 2 1] a=c!0}", "3"},
        {"a@{c=[3 2 1] a=c!1}", "2"},
        {"a@{c=[3 2 1] a=c!2}", "1"},
        {"a@{i=1 c=[3 2 1] a=c!i}", "2"},
        {"a@{c=range!100 a=c!99}", "99"},
    ));
    testEvaluateTypes("lookup string indexing", TEST_CASES(
        {R"(a@{c="" a=c!0})", "CHARACTER"},
    ));
    testEvaluateAll("lookup string indexing", TEST_CASES(
        {R"(a@{c="abc" a=c!0})", "'a'"},
        {R"(a@{c="abc" a=c!1})", "'b'"},
        {R"(a@{c="abc" a=c!2})", "'c'"},
        {R"(a@{i=1 c="abc" a=c!i})", "'b'"},
    ));
    testEvaluateAll("lookup table indexing", TEST_CASES(
        {"a@{c=<(2 3) (4 5)> a=c!2}", "3"},
        {"a@{c=<(2 3) (4 5)> a=c!4}", "5"},
    ));
    testEvaluateAll("add", TEST_CASES(
        {"add!(1 0)", "1"},
        {"add!(0 1)", "1"},
        {"add!(-1 +1)", "0"},
    ));
    testEvaluateAll("mul", TEST_CASES(
        {"mul!(0 1)", "0"},
        {"mul!(1 2)", "2"},
        {"mul!(2 3)", "6"},
        {"mul!(-1 +1)", "-1"},
    ));
    testEvaluateAll("sub", TEST_CASES(
        {"sub!(0 0)", "0"},
        {"sub!(6 3)", "3"},
        {"sub!(4 8)", "-4"},
    ));
    testEvaluateAll("div", TEST_CASES(
        {"div!(0 1)", "0"},
        {"div!(2 1)", "2"},
        {"div!(9 3)", "3"},
    ));
    testEvaluateAll("mod", TEST_CASES(
        {"mod!(0 2)", "0"},
        {"mod!(1 2)", "1"},
        {"mod!(2 2)", "0"},
        {"mod!(3 2)", "1"},
        {"mod!(4 2)", "0"},
        {"mod!(4.5 2)", "0.5"},
        {"mod!(1.2 0.5)", "0.2"},
    ));
    testEvaluateAll("less", TEST_CASES(
        {"less?(0 0)", "no"},
        {"less?(0 1)", "yes"},
        {"less?(-1 0)", "yes"},
        {"less?(1 0)", "no"},
        {"less?(0 -1)", "no"},
        {"less?(1 1)", "no"},
        {"less?(-1 -1)", "no"},
    ));
    testEvaluateAll("is_increasing", TEST_CASES(
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
    ));
    testEvaluateAll("round", TEST_CASES(
        {"round!-0.9", "-1"},
        {"round!-0.6", "-1"},
        {"round!-0.4", "-0"},
        {"round!-0.1", "-0"},
        {"round!0", "0"},
        {"round!0.1", "0"},
        {"round!0.4", "0"},
        {"round!0.6", "1"},
        {"round!0.9", "1"},
    ));
    testEvaluateAll("round_up", TEST_CASES(
        {"round_up!-0.9", "-0"},
        {"round_up!-0.6", "-0"},
        {"round_up!-0.4", "-0"},
        {"round_up!-0.1", "-0"},
        {"round_up!0", "0"},
        {"round_up!0.1", "1"},
        {"round_up!0.4", "1"},
        {"round_up!0.6", "1"},
        {"round_up!0.9", "1"},
    ));
    testEvaluateAll("round_down", TEST_CASES(
        {"round_down!-0.9", "-1"},
        {"round_down!-0.6", "-1"},
        {"round_down!-0.4", "-1"},
        {"round_down!-0.1", "-1"},
        {"round_down!0", "0"},
        {"round_down!0.1", "0"},
        {"round_down!0.4", "0"},
        {"round_down!0.6", "0"},
        {"round_down!0.9", "0"},
    ));
    testEvaluateAll("neg", TEST_CASES(
        {"neg!1", "-1"},
        {"neg!0", "0"},
        {"neg!-0", "0"},
        {"neg!-1", "1"},
    ));
    testEvaluateAll("abs", TEST_CASES(
        {"abs!-1", "1"},
        {"abs!0", "0"},
        {"abs!1", "1"},
    ));
    testEvaluateAll("sqrt", TEST_CASES(
        {"sqrt!0", "0"},
        {"sqrt!1", "1"},
        {"sqrt!4", "2"},
    ));
    testEvaluateAll("number", TEST_CASES(
        {"number!'0'", "48"},
        {"number!'9'", "57"},
        {"number!'A'", "65"},
        {"number!'Z'", "90"},
        {"number!'a'", "97"},
        {"number!'z'", "122"},
    ));
    testEvaluateAll("character", TEST_CASES(
        {"character!48", "'0'"},
        {"character!57", "'9'"},
        {"character!65", "'A'"},
        {"character!90", "'Z'"},
        {"character!97", "'a'"},
        {"character!122", "'z'"},
    ));
    testEvaluateAll("character constants", TEST_CASES(
        {"newline", "'\n'"},
    ));
    testEvaluateTypes("number constants", TEST_CASES(
        {"inf", "NUMBER"},
        {"-inf", "NUMBER"},
        {"nan", "NUMBER"},
        {"pi", "NUMBER"},
        {"tau", "NUMBER"},
    ));
    testEvaluateAll("number constants", TEST_CASES(
        {"inf", "inf"},
        {"-inf", "-inf"},
        {"nan", "nan"},
        {"pi", "3.14159265359"},
        {"tau", "6.28318530718"},
    ));
    testEvaluateAll("boolean", TEST_CASES(
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
    ));
    testEvaluateAll("not", TEST_CASES(
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
    ));
    testEvaluateAll("and", TEST_CASES(
        {"and?[no no]", "no"},
        {"and?[no yes]", "no"},
        {"and?[yes no]", "no"},
        {"and?[yes yes]", "yes"},
    ));
    testEvaluateAll("or", TEST_CASES(
        {"or?[no no]", "no"},
        {"or?[no yes]", "yes"},
        {"or?[yes no]", "yes"},
        {"or?[yes yes]", "yes"},
    ));
    testEvaluateAll("all", TEST_CASES(
        {"all?[]", "yes"},
        {"all?[0]", "no"},
        {"all?[1]", "yes"},
        {"all?[0 0]", "no"},
        {"all?[0 1]", "no"},
        {"all?[1 1]", "yes"},
    ));
    testEvaluateTypes("all", TEST_CASES(
        {"all?[]", "YES"},
        {"all?[0]", "YES"},
        {"all?[1]", "YES"},
        {"all?[0 0]", "YES"},
        {"all?[0 1]", "YES"},
        {"all?[1 1]", "YES"},
    ));
    testEvaluateAll("any", TEST_CASES(
        {"any?[]", "no"},
        {"any?[0]", "no"},
        {"any?[1]", "yes"},
        {"any?[0 0]", "no"},
        {"any?[0 1]", "yes"},
        {"any?[1 1]", "yes"},
    ));
    testEvaluateTypes("any", TEST_CASES(
        {"any?[]", "NO"},
        {"any?[0]", "NO"},
        {"any?[1]", "NO"},
        {"any?[0 0]", "NO"},
        {"any?[0 1]", "NO"},
        {"any?[1 1]", "NO"},
    ));
    testEvaluateAll("none", TEST_CASES(
        {"none?[]", "yes"},
        {"none?[0]", "yes"},
        {"none?[1]", "no"},
        {"none?[0 0]", "yes"},
        {"none?[0 1]", "no"},
        {"none?[1 1]", "no"},
    ));
    testEvaluateTypes("none", TEST_CASES(
        {"none?[]", "YES"},
        {"none?[0]", "YES"},
        {"none?[1]", "YES"},
        {"none?[0 0]", "YES"},
        {"none?[0 1]", "YES"},
        {"none?[1 1]", "YES"},
    ));
    testEvaluateAll("equal number", TEST_CASES(
        {"equal?(0 0)", "yes"},
        {"equal?(0 1)", "no"},
        {"equal?(1 0)", "no"},
        {"equal?(1 1)", "yes"},
    ));
    testEvaluateTypes("equal number", TEST_CASES(
        {"equal?(0 0)", "NO"},
        {"equal?(0 1)", "NO"},
        {"equal?(1 0)", "NO"},
        {"equal?(1 1)", "NO"},
    ));
    testEvaluateAll("unequal number", TEST_CASES(
        {"unequal?(0 0)", "no"},
        {"unequal?(0 1)", "yes"},
        {"unequal?(1 0)", "yes"},
        {"unequal?(1 1)", "no"},
    ));
    testEvaluateAll("equal character", TEST_CASES(
        {"equal?('a' 'a')", "yes"},
        {"equal?('a' 'b')", "no"},
        {"equal?('b' 'a')", "no"},
        {"equal?('b' 'b')", "yes"},
    ));
    testEvaluateAll("unequal character", TEST_CASES(
        {"unequal?('a' 'a')", "no"},
        {"unequal?('a' 'b')", "yes"},
        {"unequal?('b' 'a')", "yes"},
        {"unequal?('b' 'b')", "no"},
    ));
    testEvaluateAll("equal boolean", TEST_CASES(
        {"equal?(yes yes)", "yes"},
        {"equal?(yes no)", "no"},
        {"equal?(no yes)", "no"},
        {"equal?(no no)", "yes"},
    ));
    testEvaluateAll("unequal boolean", TEST_CASES(
        {"unequal?(yes yes)", "no"},
        {"unequal?(yes no)", "yes"},
        {"unequal?(no yes)", "yes"},
        {"unequal?(no no)", "no"},
    ));
    testEvaluateAll("equal stack", TEST_CASES(
        {"equal?([] [])", "yes"},
        {"equal?([1] [1])", "yes"},
        {"equal?([0] [1])", "no"},
        {"equal?([0 1] [0 1])", "yes"},
        {"equal?([0 1] [1 1])", "no"},
        {"equal?([0 1] [0])", "no"},
    ));
    testEvaluateTypes("equal stack", TEST_CASES(
        {"equal?([] [])", "NO"},
        {"equal?([1] [1])", "NO"},
        {"equal?([0] [1])", "NO"},
        {"equal?([0 1] [0 1])", "NO"},
        {"equal?([0 1] [1 1])", "NO"},
        {"equal?([0 1] [0])", "NO"},
    ));
    testEvaluateAll("unequal stack", TEST_CASES(
        {"unequal?([] [])", "no"},
        {"unequal?([1] [1])", "no"},
        {"unequal?([0] [1])", "yes"},
        {"unequal?([0 1] [0 1])", "no"},
        {"unequal?([0 1] [1 1])", "yes"},
        {"unequal?([0 1] [0])", "yes"},
    ));
    testEvaluateAll("equal string", TEST_CASES(
        {R"(equal?("" ""))", "yes"},
        {R"(equal?("a" ""))", "no"},
        {R"(equal?("" "a"))", "no"},
        {R"(equal?("a" "a"))", "yes"},
        {R"(equal?("a" "b"))", "no"},
        {R"(equal?("ab" "ab"))", "yes"},
        {R"(equal?("abc" "ab"))", "no"},
        {R"(equal?("ab" "abc"))", "no"},
    ));
    testEvaluateAll("unequal string", TEST_CASES(
        {R"(unequal?("" ""))", "no"},
        {R"(unequal?("a" ""))", "yes"},
        {R"(unequal?("" "a"))", "yes"},
        {R"(unequal?("a" "a"))", "no"},
        {R"(unequal?("a" "b"))", "yes"},
        {R"(unequal?("ab" "ab"))", "no"},
        {R"(unequal?("abc" "ab"))", "yes"},
        {R"(unequal?("ab" "abc"))", "yes"},
    ));
    testEvaluateTypes("clear stack", TEST_CASES(
        {"clear![]", "EMPTY_STACK"},
        {"clear![1]", "[NUMBER]"},
        {"clear![1 2]", "[NUMBER]"},
    ));
    testEvaluateAll("clear stack", TEST_CASES(
        {"clear![]", "[]"},
        {"clear![1]", "[]"},
        {"clear![1 2]", "[]"},
    ));
    testEvaluateAll("clear table", TEST_CASES(
        {"clear!<>", "<>"},
        {"clear!<(1 11)>", "<>"},
        {"clear!<(1 11) (2 22)>", "<>"},
    ));
    testEvaluateAll("clear number", TEST_CASES(
        {"clear!0", "0"},
        {"clear!1", "0"},
        {"clear!2", "0"},
    ));
    testEvaluateAll("clear boolean", TEST_CASES(
        {"clear!no", "no"},
        {"clear!yes", "no"},
    ));
    testEvaluateTypes("clear string", TEST_CASES(
        {R"(clear!"")", "EMPTY_STRING"},
        {R"(clear!"a")", "STRING"},
        {R"(clear!"ab")", "STRING"},
    ));
    testEvaluateAll("clear string", TEST_CASES(
        {R"(clear!"")", R"("")"},
        {R"(clear!"a")", R"("")"},
        {R"(clear!"ab")", R"("")"},
    ));
    testEvaluateTypes("take stack", TEST_CASES(
        {"take![4]", "NUMBER"},
        {"take![3 4]", "NUMBER"},
    ));
    testEvaluateAll("take stack", TEST_CASES(
        {"take![4]", "4"},
        {"take![3 4]", "3"},
    ));
    testEvaluateTypes("take number", TEST_CASES(
        {"take!1", "NUMBER"},
        {"take!2", "NUMBER"},
    ));
    testEvaluateAll("take number", TEST_CASES(
        {"take!1", "1"},
        {"take!2", "1"},
    ));
    testEvaluateAll("take boolean", TEST_CASES(
        {"take!no", "no"},
        {"take!yes", "yes"},
    ));
    testEvaluateTypes("take string", TEST_CASES(
        {R"(take!"b")", "CHARACTER"},
        {R"(take!"ab")", "CHARACTER"},
    ));
    testEvaluateAll("take string", TEST_CASES(
        {R"(take!"b")", "'b'"},
        {R"(take!"ab")", "'a'"},
    ));
    testEvaluateTypes("drop stack", TEST_CASES(
        {"drop![4]", "[NUMBER]"},
        {"drop![4 3]", "[NUMBER]"},
        {"drop![4 3 7]", "[NUMBER]"},
        {"drop![[]]", "[EMPTY_STACK]"},
        {"drop![]", "EMPTY_STACK"},
    ));
    testEvaluateAll("drop stack", TEST_CASES(
        {"drop![4]", "[]"},
        {"drop![4 3]", "[3]"},
        {"drop![4 3 7]", "[3 7]"},
        {"drop![[]]", "[]"},
    ));
    testEvaluateAll("drop number", TEST_CASES(
        {"drop!1", "0"},
        {"drop!2", "1"},
    ));
    testEvaluateAll("drop boolean", TEST_CASES(
        {"drop!no", "no"},
        {"drop!yes", "no"},
    ));
    testEvaluateTypes("drop string", TEST_CASES(
        {R"(drop!"a")", "STRING"},
        {R"(drop!"ab")", "STRING"},
        {R"(drop!"abc")", "STRING"},
    ));
    testEvaluateAll("drop string", TEST_CASES(
        {R"(drop!"a")", R"("")"},
        {R"(drop!"ab")", R"("b")"},
        {R"(drop!"abc")", R"("bc")"},
    ));
    testEvaluateTypes("put stack", TEST_CASES(
        {"put!(3 [])", "[NUMBER]"},
        {"put!(4 [5])", "[NUMBER]"},
        {"put!(4 [6 8])", "[NUMBER]"},
    ));
    testEvaluateAll("put stack", TEST_CASES(
        {"put!(3 [])", "[3]"},
        {"put!(4 [5])", "[4 5]"},
        {"put!(4 [6 8])", "[4 6 8]"},
    ));
    testEvaluateAll("put number", TEST_CASES(
        {"put!(1 0)", "1"},
        {"put!(2 0)", "2"},
        {"put!(1 1)", "2"},
        {"put!(2 1)", "3"},
    ));
    testEvaluateAll("put boolean", TEST_CASES(
        {"put!(no no)", "no"},
        {"put!(no yes)", "no"},
        {"put!(yes no)", "yes"},
        {"put!(yes yes)", "yes"},
    ));
    testEvaluateTypes("put string", TEST_CASES(
        {R"(put!('a' ""))", "STRING"},
        {R"(put!('a' "b"))", "STRING"},
        {R"(put!('a' "bc"))", "STRING"},
    ));
    testEvaluateAll("put string", TEST_CASES(
        {R"(put!('a' ""))", R"("a")"},
        {R"(put!('a' "b"))", R"("ab")"},
        {R"(put!('a' "bc"))", R"("abc")"},
    ));
    testEvaluateAll("put table", TEST_CASES(
        {"put!((1 11) <>)", "<(1 11)>"},
        {"put!((4 44) <(5 55)>)", "<(4 44) (5 55)>"},
        {"put!((4 44) <(6 66) (8 88)>)", "<(4 44) (6 66) (8 88)>"},
        {"put!((0 1) <>)", "<(0 1)>"},
        {"put!((0 1) <(0 2)>)", "<(0 1)>"},
        {"put!((0 5) <(2 3) (1 2) (0 1)>)", "<(0 5) (1 2) (2 3)>"},
    ));
    testEvaluateAll("get table", TEST_CASES(
        {"get!(0 <> 2)", "2"},
        {"get!(0 <(0 1)> 2)", "1"},
        {"get!(1 <(0 1)> 2)", "2"},
        {"get!(0 <(0 1) (1 2) (3 3)> 2)", "1"},
        {"get!((3) <((1) [1]) ((2) [2]) ((3) [3])> [])", "[3]"},
    ));
    testEvaluateAll("get_keys", TEST_CASES(
        {"get_keys!<>", "[]"},
        {"get_keys!<(0 1)>", "[0]"},
        {"get_keys!<(0 1) (2 3)>", "[0 2]"},
        {"get_keys!<(2 3) (0 1)>", "[0 2]"},
    ));
    testEvaluateAll("get_values", TEST_CASES(
        {"get_values!<(1 0)>", "[0]"},
        {"get_values!<(10 0) (11 1) (12 2)>", "[0 1 2]"},
        {"get_values!<(3 0) (2 1) (1 2)>", "[2 1 0]"},
    ));
    testEvaluateAll("get_items", TEST_CASES(
        {"get_items!<(0 1)>", "[(0 1)]"},
        {"get_items!<(0 1) (2 3) (4 5)>", "[(0 1) (2 3) (4 5)]"},
        {"get_items!<(4 1) (2 3) (0 5)>", "[(0 5) (2 3) (4 1)]"},
    ));
    testEvaluateAll("inc", TEST_CASES(
        {"inc!0", "1"},
    ));
    testEvaluateAll("dec", TEST_CASES(
        {"dec!0", "-1"},
    ));
    testEvaluateTypes("is_digit", TEST_CASES(
        {"is_digit?'A'", "YES"},
    ));
    testEvaluateAll("is_digit", TEST_CASES(
        {"is_digit?'A'", "no"},
        {"is_digit?'Z'", "no"},
        {"is_digit?'a'", "no"},
        {"is_digit?'z'", "no"},
        {"is_digit?'0'", "yes"},
        {"is_digit?'9'", "yes"},
    ));
    testEvaluateTypes("is_upper", TEST_CASES(
        {"is_upper?'A')", "YES"},
    ));
    testEvaluateAll("is_upper", TEST_CASES(
        {"is_upper?'A'", "yes"},
        {"is_upper?'Z'", "yes"},
        {"is_upper?'a'", "no"},
        {"is_upper?'z'", "no"},
        {"is_upper?'0'", "no"},
        {"is_upper?'9'", "no"},
    ));
    testEvaluateTypes("is_lower", TEST_CASES(
        {"is_lower?'A'", "YES"},
    ));
    testEvaluateAll("is_lower", TEST_CASES(
        {"is_lower?'A')", "no"},
        {"is_lower?'Z')", "no"},
        {"is_lower?'a')", "yes"},
        {"is_lower?'z')", "yes"},
        {"is_lower?'0')", "no"},
        {"is_lower?'9')", "no"},
    ));
    testEvaluateTypes("is_letter", TEST_CASES(
        {"is_letter?'A'", "NO"},
    ));
    testEvaluateAll("is_letter", TEST_CASES(
        {"is_letter?'A'", "yes"},
        {"is_letter?'Z'", "yes"},
        {"is_letter?'a'", "yes"},
        {"is_letter?'z'", "yes"},
        {"is_letter?'0'", "no"},
        {"is_letter?'9'", "no"},
    ));
    testEvaluateAll("to_upper", TEST_CASES(
        {"to_upper!'A'", "'A'"},
        {"to_upper!'Z'", "'Z'"},
        {"to_upper!'a'", "'A'"},
        {"to_upper!'z'", "'Z'"},
        {"to_upper!'0'", "'0'"},
        {"to_upper!'9'", "'9'"},
    ));
    testEvaluateAll("to_lower", TEST_CASES(
        {"to_lower!'A'", "'a'"},
        {"to_lower!'Z'", "'z'"},
        {"to_lower!'a'", "'a'"},
        {"to_lower!'z'", "'z'"},
        {"to_lower!'0'", "'0'"},
        {"to_lower!'9'", "'9'"},
    ));
    testEvaluateAll("parse_digit", TEST_CASES(
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
    ));
    testEvaluateAll("serialize_digit", TEST_CASES(
        {"serialize_digit!0", "'0'"},
        {"serialize_digit!1", "'1'"},
        {"serialize_digit!2", "'2'"},
        {"serialize_digit!3", "'3'"},
        {"serialize_digit!4", "'4'"},
        {"serialize_digit!5", "'5'"},
        {"serialize_digit!6", "'6'"},
        {"serialize_digit!7", "'7'"},
        {"serialize_digit!8", "'8'"},
        {"serialize_digit!9", "'9'"},
    ));
    testEvaluateAll("parse_natural_number", TEST_CASES(
        {R"(parse_natural_number!"0")", "0"},
        {R"(parse_natural_number!"1")", "1"},
        {R"(parse_natural_number!"2")", "2"},
        {R"(parse_natural_number!"10")", "10"},
        {R"(parse_natural_number!"11")", "11"},
        {R"(parse_natural_number!"12")", "12"},
        {R"(parse_natural_number!"20")", "20"},
        {R"(parse_natural_number!"123")", "123"},
    ));
    testEvaluateAll("serialize_natural_number", TEST_CASES(
        {"serialize_natural_number!0", R"("0")"},
        {"serialize_natural_number!1", R"("1")"},
        {"serialize_natural_number!2", R"("2")"},
        {"serialize_natural_number!10", R"("10")"},
        {"serialize_natural_number!11", R"("11")"},
        {"serialize_natural_number!12", R"("12")"},
        {"serialize_natural_number!20", R"("20")"},
        {"serialize_natural_number!123", R"("123")"},
    ));
    testEvaluateTypes("min_item stack", TEST_CASES(
        {"min_item![]", "NUMBER"},
        {"min_item![0]", "NUMBER"},
        {"min_item![0 1]", "NUMBER"},
        {"min_item![1 0]", "NUMBER"},
        {"min_item![3 6 1]", "NUMBER"},
        {"min_item![7 -3 8 -9]", "NUMBER"},
    ));
    testEvaluateAll("min_item stack", TEST_CASES(
        {"min_item![]", "inf"},
        {"min_item![0]", "0"},
        {"min_item![0 1]", "0"},
        {"min_item![1 0]", "0"},
        {"min_item![3 6 1]", "1"},
        {"min_item![7 -3 8 -9]", "-9"},
    ));
    testEvaluateTypes("max_item stack", TEST_CASES(
        {"max_item![]", "NUMBER"},
        {"max_item![0]", "NUMBER"},
        {"max_item![0 1]", "NUMBER"},
        {"max_item![1 0]", "NUMBER"},
        {"max_item![3 6 1]", "NUMBER"},
        {"max_item![7 -3 8 -9]", "NUMBER"},
    ));
    testEvaluateAll("max_item stack", TEST_CASES(
        {"max_item![]", "-inf"},
        {"max_item![0]", "0"},
        {"max_item![0 1]", "1"},
        {"max_item![1 0]", "1"},
        {"max_item![3 6 1]", "6"},
        {"max_item![7 -3 8 -9]", "8"},
    ));
    testEvaluateAll("min_key stack", TEST_CASES(
        {"min_key!(in (w h) out mul!(w h) [(1 2) (2 2) (3 1)])", "(1 2)"},
    ));
    testEvaluateAll("max_key stack", TEST_CASES(
        {"max_key!(in (w h) out mul!(w h) [(1 2) (2 2) (3 1)])", "(2 2)"},
    ));
    testEvaluateAll("min_predicate stack", TEST_CASES(
        {"min_predicate!(in (a b) out less?(mul!a mul!b) [(1 2) (2 2) (3 1)])", "(1 2)"},
    ));
    testEvaluateAll("max_predicate stack", TEST_CASES(
        {"max_predicate!(in (a b) out less?(mul!a mul!b) [(1 2) (2 2) (3 1)])", "(2 2)"},
    ));
    testEvaluateTypes("range", TEST_CASES(
        {"range!0", "[NUMBER]"},
        {"range!1", "[NUMBER]"},
        {"range!2", "[NUMBER]"},
        {"range!3", "[NUMBER]"},
    ));
    testEvaluateAll("range", TEST_CASES(
        {"range!0", "[]"},
        {"range!1", "[0]"},
        {"range!2", "[0 1]"},
        {"range!3", "[0 1 2]"},
    ));
    testEvaluateAll("iteration", TEST_CASES(
        {"count!range!100", "100"},
        {"sum!range!100", "4950"},
    ));
    testEvaluateTypes("count stack", TEST_CASES(
        {"count![]", "NUMBER"},
        {"count![[]]", "NUMBER"},
        {"count![[] []]", "NUMBER"},
    ));
    testEvaluateAll("count stack", TEST_CASES(
        {"count![]", "0"},
        {"count![[]]", "1"},
        {"count![[] []]", "2"},
    ));
    testEvaluateAll("count table", TEST_CASES(
        {"count!<>", "0"},
        {"count!<(1 1)>", "1"},
        {"count!<(1 1) (1 1)>", "1"},
        {"count!<(1 1) (2 2)>", "2"},
    ));
    testEvaluateTypes("count string", TEST_CASES(
        {R"(count!"")", "NUMBER"},
        {R"(count!"a")", "NUMBER"},
        {R"(count!"ab")", "NUMBER"},
    ));
    testEvaluateAll("count string", TEST_CASES(
        {R"(count!"")", "0"},
        {R"(count!"a")", "1"},
        {R"(count!"ab")", "2"},
    ));
    testEvaluateAll("count number", TEST_CASES(
        {"count!3", "3"},
    ));
    testEvaluateAll("count boolean", TEST_CASES(
        {"count!no", "0"},
        {"count!yes", "1"},
    ));
    testEvaluateTypes("count_item stack", TEST_CASES(
        {"count_item!(1 [])", "NUMBER"},
        {"count_item!(1 [1])", "NUMBER"},
        {"count_item!(1 [1 1])", "NUMBER"},
        {"count_item!(1 [1 0 1])", "NUMBER"},
    ));
    testEvaluateAll("count_item stack", TEST_CASES(
        {"count_item!(1 [])", "0"},
        {"count_item!(1 [1])", "1"},
        {"count_item!(1 [1 1])", "2"},
        {"count_item!(1 [1 0 1])", "2"},
    ));
    testEvaluateAll("count_item string", TEST_CASES(
        {R"(count_item!('a' ""))", "0"},
        {R"(count_item!('a' "a"))", "1"},
        {R"(count_item!('a' "aa"))", "2"},
        {R"(count_item!('a' "aba"))", "2"},
    ));
    testEvaluateAll("count_if stack", TEST_CASES(
        {"count_if!(in x out 0 [])", "0"},
        {"count_if!(in x out 0 [1])", "0"},
        {"count_if!(in x out 1 [])", "0"},
        {"count_if!(in x out 1 [1])", "1"},
        {"count_if!(in x out equal?(x 3) [1 2 3])", "1"},
        {"count_if!(in x out equal?(x 3) [3 2 3])", "2"},
    ));
    testEvaluateAll("count_if table", TEST_CASES(
        {"count_if!(less <(0 0)>)", "0"},
        {"count_if!(less <(0 1)>)", "1"},
        {"count_if!(less <(0 0) (1 2) (2 3) (4 5) (7 6)>)", "3"},
    ));
    testEvaluateAll("count_if string", TEST_CASES(
        {R"(count_if!(in x out 0 ""))", "0"},
        {R"(count_if!(in x out 0 "a"))", "0"},
        {R"(count_if!(in x out 1 ""))", "0"},
        {R"(count_if!(in x out 1 "a"))", "1"},
        {R"(count_if!(in x out equal?(x 'c') "abc"))", "1"},
        {R"(count_if!(in x out equal?(x 'c') "cbc"))", "2"},
    ));
    testEvaluateTypes("reverse stack", TEST_CASES(
        {"reverse![]", "EMPTY_STACK"},
        {"reverse![0]", "[NUMBER]"},
        {"reverse![0 1]", "[NUMBER]"},
        {"reverse![0 1 2]", "[NUMBER]"},
    ));
    testEvaluateAll("reverse stack", TEST_CASES(
        {"reverse![]", "[]"},
        {"reverse![0]", "[0]"},
        {"reverse![0 1]", "[1 0]"},
        {"reverse![0 1 2]", "[2 1 0]"},
    ));
    testEvaluateTypes("reverse table", TEST_CASES(
        {"reverse!<>", "<(ANY ANY)>"}, // TODO
        {"reverse!<(0 0)>", "<(NUMBER NUMBER)>"},
        {"reverse!<(0 0) (1 1)>", "<(NUMBER NUMBER)>"},
        {"reverse!<(0 0) (1 1) (2 2)>", "<(NUMBER NUMBER)>"},
    ));
    testEvaluateAll("reverse table", TEST_CASES(
        {"reverse!<>", "<>"},
        {"reverse!<(0 0)>", "<(0 0)>"},
        {"reverse!<(0 0) (1 1)>", "<(0 0) (1 1)>"},
        {"reverse!<(0 0) (1 1) (2 2)>", "<(0 0) (1 1) (2 2)>"},
    ));
    testEvaluateTypes("reverse string", TEST_CASES(
        {R"(reverse!"")", "STRING"},
        {R"(reverse!"a")", "STRING"},
        {R"(reverse!"ab")", "STRING"},
        {R"(reverse!"abc")", "STRING"},
    ));
    testEvaluateAll("reverse string", TEST_CASES(
        {R"(reverse!"")", R"("")"},
        {R"(reverse!"a")", R"("a")"},
        {R"(reverse!"ab")", R"("ba")"},
        {R"(reverse!"abc")", R"("cba")"},
    ));
    testEvaluateAll("make_stack stack", TEST_CASES(
        {"make_stack![]", "[]"},
        {"make_stack![3 1 2]", "[3 1 2]"},
    ));
    testEvaluateAll("make_stack string", TEST_CASES(
        {R"(make_stack!"")", "[]"},
        {R"(make_stack!"cab")", "['c' 'a' 'b']"},
    ));
    testEvaluateAll("make_stack table", TEST_CASES(
        {"make_stack!<>", "[]"},
        {"make_stack!<(3 33) (1 11) (2 22)>", "[(1 11) (2 22) (3 33)]"},
    ));
    testEvaluateAll("make_string", TEST_CASES(
        {R"(make_string![])", R"("")"},
        {R"(make_string!['c' 'a' 'b'])", R"("cab")"},
        {R"(make_string!"")", R"("")"},
        {R"(make_string!"cab")", R"("cab")"},
    ));
    testEvaluateTypes("make_table", TEST_CASES(
        {"make_table![]", "<>"},
        {"make_table![(3 33) (1 11) (2 22)]", "<(NUMBER NUMBER)>"},
        {"make_table!<>", "<(ANY ANY)>"}, // TODO
        {"make_table!<(3 33) (1 11) (2 22)>", "<(NUMBER NUMBER)>"},
    ));
    testEvaluateAll("make_table", TEST_CASES(
        {"make_table![]", "<>"},
        {"make_table![(3 33) (1 11) (2 22)]", "<(1 11) (2 22) (3 33)>"},
        {"make_table!<>", "<>"},
        {"make_table!<(3 33) (1 11) (2 22)>", "<(1 11) (2 22) (3 33)>"},
    ));
    testEvaluateAll("map_stack", TEST_CASES(
        {"map_stack!(inc [])", "[]"},
        {"map_stack!(inc [0])", "[1]"},
        {"map_stack!(inc [0 1])", "[1 2]"},
        {"map_stack!(in x out 2 [0 0])", "[2 2]"},
        {"a@{b=2 f=in x out b a=map_stack!(f [0 0])}", "[2 2]"},
    ));
    testEvaluateTypes("map_stack", TEST_CASES(
        {"map_stack!(inc [])", "[NUMBER]"},
        {"map_stack!(inc [0])", "[NUMBER]"},
        {"map_stack!(inc [0 1])", "[NUMBER]"},
        {"map_stack!(in x out 2 [0 0])", "[NUMBER]"},
        {"a@{b=2 f=in x out b a=map_stack!(f [0 0])}", "[NUMBER]"},
    ));
    testEvaluateAll("map_string", TEST_CASES(
        {R"(map_string!(to_upper ""))", R"("")"},
        {R"(map_string!(to_upper "abc"))", R"("ABC")"},
    ));
    testEvaluateTypes("map_string", TEST_CASES(
        {R"(map_string!(to_upper ""))", "STRING"},
        {R"(map_string!(to_upper "abc"))", "STRING"},
    ));
    testEvaluateAll("map table", TEST_CASES(
        {"map_table!(in x out (x x) [1 2])", "<(1 1) (2 2)>"},
        {"map_table!(in (x y) out (x inc!y) <(1 11) (2 22)>)", "<(1 12) (2 23)>"},
    ));
    testEvaluateAll("clear_if stack", TEST_CASES(
        {"clear_if!(in x out 1 [])", "[]"},
        {"clear_if!(in x out 1 [[]])", "[]"},
        {"clear_if!(in x out 0 [[]])", "[[]]"},
        {"clear_if!(in x out less?(x 5) [7 4 6 1 9 3 2])", "[7 6 9]"},
    ));
    testEvaluateAll("clear_if table", TEST_CASES(
        {"clear_if!(less <(0 0)>)", "<(0 0)>"},
        {"clear_if!(less <(0 1)>)", "<>"},
        {"clear_if!(less <(0 0) (1 2) (2 3) (5 4)>)", "<(0 0) (5 4)>"},
    ));
    testEvaluateAll("clear_if string", TEST_CASES(
        {R"(clear_if!(in x out 0 ""))", R"("")"},
        {R"(clear_if!(in x out 0 "a"))", R"("a")"},
        {R"(clear_if!(in x out 1 ""))", R"("")"},
        {R"(clear_if!(in x out 1 "a"))", R"("")"},
        {R"(clear_if!(in x out equal?(x 'a') "a"))", R"("")"},
        {R"(clear_if!(in x out equal?(x 'a') "ab"))", R"("b")"},
        {R"(clear_if!(in x out equal?(x 'a') "ba"))", R"("b")"},
        {R"(clear_if!(in x out equal?(x 'a') "bab"))", R"("bb")"},
    ));
    testEvaluateAll("clear_item stack", TEST_CASES(
        {"clear_item!(1 [])", "[]"},
        {"clear_item!(1 [0])", "[0]"},
        {"clear_item!(1 [1])", "[]"},
        {"clear_item!(1 [1 7 1 2 7 1 1])", "[7 2 7]"},
    ));
    testEvaluateAll("replace stack", TEST_CASES(
        {"replace!(1 [])", "[]"},
        {"replace!(1 [1])", "[1]"},
        {"replace!(1 [2])", "[1]"},
        {"replace!(1 [0 1 0 1 1])", "[1 1 1 1 1]"},
    ));
    testEvaluateAll("replace string", TEST_CASES(
        {R"(replace!('a' ""))", R"("")"},
        {R"(replace!('a' "a"))", R"("a")"},
        {R"(replace!('a' "c"))", R"("a")"},
        {R"(replace!('a' "ab_ba"))", R"("aaaaa")"},
    ));
    testEvaluateAll("replace_item stack", TEST_CASES(
        {"replace_item!(1 2 [])", "[]"},
        {"replace_item!(1 2 [1])", "[2]"},
        {"replace_item!(1 2 [2])", "[2]"},
        {"replace_item!(1 2 [0 1 0 1 1])", "[0 2 0 2 2]"},
    ));
    testEvaluateAll("replace_item string", TEST_CASES(
        {R"(replace_item!('a' 'b' ""))", R"("")"},
        {R"(replace_item!('a' 'b' "a"))", R"("b")"},
        {R"(replace_item!('a' 'b' "c"))", R"("c")"},
        {R"(replace_item!('a' 'b' "ab_ba"))", R"("bb_bb")"},
    ));
    testEvaluateAll("replace_if stack", TEST_CASES(
        {"replace_if!(in x out equal?(x 1) 2 [])", "[]"},
        {"replace_if!(in x out equal?(x 1) 2 [1])", "[2]"},
        {"replace_if!(in x out equal?(x 1) 2 [2])", "[2]"},
        {"replace_if!(in x out equal?(x 1) 2 [0 1 0 1 1])", "[0 2 0 2 2]"},
    ));
    testEvaluateAll("replace_if string", TEST_CASES(
        {R"(replace_if!(in x out equal?(x 'a') 'b' ""))", R"("")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "a"))", R"("b")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "c"))", R"("c")"},
        {R"(replace_if!(in x out equal?(x 'a') 'b' "ab_ba"))", R"("bb_bb")"},
    ));
    testEvaluateTypes("enumerate stack", TEST_CASES(
        {"enumerate![]", "[(NUMBER ANY)]"}, // TODO
        {"enumerate![4]", "[(NUMBER NUMBER)]"},
        {"enumerate![4 3]", "[(NUMBER NUMBER)]"},
    ));
    testEvaluateAll("enumerate stack", TEST_CASES(
        {"enumerate![]", "[]"},
        {"enumerate![4]", "[(0 4)]"},
        {"enumerate![4 3]", "[(0 4) (1 3)]"},
    ));
    testEvaluateAll("enumerate string", TEST_CASES(
        {R"(enumerate!"")", "[]"},
        {R"(enumerate!"a")", "[(0 'a')]"},
        {R"(enumerate!"ab")", "[(0 'a') (1 'b')]"},
    ));
    testEvaluateAll("split stack", TEST_CASES(
        {"split!(0 [])", "[[]]"},
        {"split!(0 [1])", "[[1]]"},
        {"split!(0 [1 2])", "[[1 2]]"},
        {"split!(0 [0])", "[[] []]"},
        {"split!(0 [0 0])", "[[] [] []]"},
        {"split!(0 [0 1 2])", "[[] [1 2]]"},
        {"split!(0 [1 2 0])", "[[1 2] []]"},
        {"split!(0 [1 0 2])", "[[1] [2]]"},
    ));
    testEvaluateTypes("split stack", TEST_CASES(
        {"split!(0 [])", "[EMPTY_STACK]"},
        {"split!(0 [0])", "[[NUMBER]]"},
        {"split!(0 [0 0])", "[[NUMBER]]"},
        {"split!(0 [0 0 0])", "[[NUMBER]]"},
        {"split!(0 [0 0 0 0])", "[[NUMBER]]"},
    ));
    testEvaluateAll("split string", TEST_CASES(
        {R"(split!(',' ""))", R"([""])"},
        {R"(split!(',' "a"))", R"(["a"])"},
        {R"(split!(',' ",a"))", R"(["" "a"])"},
        {R"(split!(',' "a,"))", R"(["a" ""])"},
        {R"(split!(',' "a,b,cd"))", R"(["a" "b" "cd"])"},
    ));
    testEvaluateAll("cartesian_product2 stack", TEST_CASES(
        {"cartesian_product2!([1 2] [3 4])", "[(2 4) (1 4) (2 3) (1 3)]"},
    ));
    testEvaluateAll("put_column stack", TEST_CASES(
        {"put_column!([1 2] [[3] [4]])", "[[1 3] [2 4]]"},
    ));
    testEvaluateAll("transpose stack", TEST_CASES(
        {"transpose![[]]", "[]"},
        {"transpose![[1]]", "[[1]]"},
        {"transpose![[1] [2]]", "[[1 2]]"},
        {"transpose![[1 2] [3 4]]", "[[1 3] [2 4]]"},
    ));
    testEvaluateAll("sum stack", TEST_CASES(
        {"sum![]", "0"},
        {"sum![1]", "1"},
        {"sum![1 2]", "3"},
        {"sum![1 2 3]", "6"},
    ));
    testEvaluateTypes("sum stack", TEST_CASES(
        {"sum![]", "NUMBER"},
        {"sum![1]", "NUMBER"},
        {"sum![1 2]", "NUMBER"},
        {"sum![1 2 3]", "NUMBER"},
    ));
    testEvaluateAll("product", TEST_CASES(
        {"product![]", "1"},
        {"product![1]", "1"},
        {"product![1 2]", "2"},
        {"product![1 2 3]", "6"},
    ));
    testEvaluateTypes("product", TEST_CASES(
        {"product![]", "NUMBER"},
        {"product![1]", "NUMBER"},
        {"product![1 2]", "NUMBER"},
        {"product![1 2 3]", "NUMBER"},
    ));
    testEvaluateAll("put_each stack", TEST_CASES(
        {"put_each!([] [])", "[]"},
        {"put_each!([] [2])", "[2]"},
        {"put_each!([1] [])", "[1]"},
        {"put_each!([1] [2])", "[1 2]"},
        {"put_each!([1 2] [3 4])", "[2 1 3 4]"},
        {"put_each!([1 2 3] [4 5 6])", "[3 2 1 4 5 6]"},
    ));
    testEvaluateAll("put_each table", TEST_CASES(
        {"put_each!(<> <>)", "<>"},
        {"put_each!(<> <(1 2)>)", "<(1 2)>"},
        {"put_each!(<(1 2)> <>)", "<(1 2)>"},
        {"put_each!(<(1 2)> <(1 1)>)", "<(1 2)>"},
        {"put_each!(<(1 11)> <(2 22)>)", "<(1 11) (2 22)>"},
        {"put_each!(<(1 11) (3 33)> <(2 22) (4 44)>)", "<(1 11) (2 22) (3 33) (4 44)>"},
    ));
    testEvaluateTypes("put_each", TEST_CASES(
        {"put_each!([] [])", "EMPTY_STACK"},
        {"put_each!([] [2])", "[NUMBER]"},
        {"put_each!([1] [])", "[NUMBER]"},
        {"put_each!([1] [2])", "[NUMBER]"},
        {"put_each!([1 2] [3 4])", "[NUMBER]"},
        {"put_each!([1 2 3] [4 5 6])", "[NUMBER]"},
    ));
    testEvaluateAll("put_each string", TEST_CASES(
        {R"(put_each!("" ""))", R"("")"},
        {R"(put_each!("" "a"))", R"("a")"},
        {R"(put_each!("a" ""))", R"("a")"},
        {R"(put_each!("a" "b"))", R"("ab")"},
        {R"(put_each!("ab" "cd"))", R"("bacd")"},
        {R"(put_each!("abc" "def"))", R"("cbadef")"},
    ));
    testEvaluateTypes("put_each string", TEST_CASES(
        {R"(put_each!("" ""))", "STRING"},
        {R"(put_each!("" "a"))", "STRING"},
        {R"(put_each!("a" ""))", "STRING"},
        {R"(put_each!("a" "b"))", "STRING"},
        {R"(put_each!("ab" "cd"))", "STRING"},
        {R"(put_each!("abc" "def"))", "STRING"},
    ));
    testEvaluateAll("merge_stack stack", TEST_CASES(
        {"merge_stack![[] []]", "[]"},
        {"merge_stack![[] [2]]", "[2]"},
        {"merge_stack![[1] []]", "[1]"},
        {"merge_stack![[1] [2]]", "[1 2]"},
        {"merge_stack![[1] [2 3] [4 5 6]]", "[1 2 3 4 5 6]"},
    ));
    testEvaluateAll("merge_stack table", TEST_CASES(
        {"merge_stack![[(1 11)] <(2 22)>]", "[(1 11) (2 22)]"},
    ));
    testEvaluateAll("merge_stack string", TEST_CASES(
        {R"(merge_stack![['a'] "b"])", "['a' 'b']"},
    ));
    testEvaluateTypes("merge_stack", TEST_CASES(
        {"merge_stack![[] []]", "EMPTY_STACK"},
        {"merge_stack![[] [2]]", "EMPTY_STACK"}, // TODO
        {"merge_stack![[1] []]", "[NUMBER]"},
        {"merge_stack![[1] [2]]", "[NUMBER]"},
        {"merge_stack![[1] [2 3] [4 5 6]]", "[NUMBER]"},
    ));
    testEvaluateAll("merge_table", TEST_CASES(
        {"merge_table![<> <>]", "<>"},
        {"merge_table![<> <(2 22)>]", "<(2 22)>"},
        {"merge_table![<(1 11)> <>]", "<(1 11)>"},
        {"merge_table![<(1 11)> <(1 11)>]", "<(1 11)>"},
        {"merge_table![<(1 11)> <(2 22)>]", "<(1 11) (2 22)>"},
        {"merge_table![<(1 11)> <(5 55) (3 33)> <(4 44) (5 0) (6 66) (2 22)>]", "<(1 11) (2 22) (3 33) (4 44) (5 55) (6 66)>"},
    ));
    testEvaluateAll("merge_string", TEST_CASES(
        {R"(merge_string!["" ""])", R"("")"},
        {R"(merge_string!["" "b"])", R"("b")"},
        {R"(merge_string!["a" ""])", R"("a")"},
        {R"(merge_string!["a" "b"])", R"("ab")"},
        {R"(merge_string!["a" "bc" "def"])", R"("abcdef")"},
        {R"(merge_string![['a'] "b"])", R"("ab")"},
    ));
    testEvaluateTypes("merge_string", TEST_CASES(
        {R"(merge_string!["" ""])", "STRING"},
        {R"(merge_string!["" "b"])", "STRING"},
        {R"(merge_string!["a" ""])", "STRING"},
        {R"(merge_string!["a" "b"])", "STRING"},
        {R"(merge_string!["a" "bc" "def"])", "STRING"},
    ));
    testEvaluateAll("drop_many stack", TEST_CASES(
        {"drop_many!(0 [3 7 6])", "[3 7 6]"},
        {"drop_many!(1 [3 7 6])", "[7 6]"},
        {"drop_many!(2 [3 7 6])", "[6]"},
        {"drop_many!(3 [3 7 6])", "[]"},
    ));
    testEvaluateAll("drop_many string", TEST_CASES(
        {R"(drop_many!(0 "abc"))", R"("abc")"},
        {R"(drop_many!(1 "abc"))", R"("bc")"},
        {R"(drop_many!(2 "abc"))", R"("c")"},
        {R"(drop_many!(3 "abc"))", R"("")"},
    ));
    testEvaluateAll("drop_while string", TEST_CASES(
        {R"(drop_while!(is_upper ""))", R"("")"},
        {R"(drop_while!(is_upper "a"))", R"("a")"},
        {R"(drop_while!(is_upper "A"))", R"("")"},
        {R"(drop_while!(is_upper "ABcd"))", R"("cd")"},
    ));
    testEvaluateAll("drop_until_item stack", TEST_CASES(
        {"drop_until_item!(1 [])", "[]"},
        {"drop_until_item!(2 [1])", "[]"},
        {"drop_until_item!(1 [1])", "[1]"},
        {"drop_until_item!(3 [1 2 3 4])", "[3 4]"},
    ));
    testEvaluateAll("drop_until_item string", TEST_CASES(
        {R"(drop_until_item!('a' ""))", R"("")"},
        {R"(drop_until_item!('a' "a"))", R"("a")"},
        {R"(drop_until_item!('a' "b"))", R"("")"},
        {R"(drop_until_item!('a' "ab"))", R"("ab")"},
        {R"(drop_until_item!('a' "ba"))", R"("a")"},
    ));
    testEvaluateAll("take_many stack", TEST_CASES(
        {"take_many!(0 [3 7 6])", "[]"},
        {"take_many!(1 [3 7 6])", "[3]"},
        {"take_many!(2 [3 7 6])", "[3 7]"},
        {"take_many!(3 [3 7 6])", "[3 7 6]"},
    ));
    testEvaluateAll("take_many string", TEST_CASES(
        {R"(take_many!(0 "abc"))", R"("")"},
        {R"(take_many!(1 "abc"))", R"("a")"},
        {R"(take_many!(2 "abc"))", R"("ab")"},
        {R"(take_many!(3 "abc"))", R"("abc")"},
    ));
    testEvaluateAll("take_while string", TEST_CASES(
        {R"(take_while!(is_upper ""))", R"("")"},
        {R"(take_while!(is_upper "a"))", R"("")"},
        {R"(take_while!(is_upper "A"))", R"("A")"},
        {R"(take_while!(is_upper "ABcd"))", R"("AB")"},
    ));
    testEvaluateAll("take_until_item stack", TEST_CASES(
        {"take_until_item!(1 [])", "[]"},
        {"take_until_item!(2 [1])", "[1]"},
        {"take_until_item!(1 [1])", "[]"},
        {"take_until_item!(3 [1 2 3 4])", "[1 2]"},
    ));
    testEvaluateAll("take_until_item string", TEST_CASES(
        {R"(take_until_item!('a' ""))", R"("")"},
        {R"(take_until_item!('b' "a"))", R"("a")"},
        {R"(take_until_item!('a' "a"))", R"("")"},
        {R"(take_until_item!('c' "ABcd"))", R"("AB")"},
    ));
    testEvaluateAll("merge_sorted", TEST_CASES(
        {"merge_sorted!([] [])", "[]"},
        {"merge_sorted!([0] [])", "[0]"},
        {"merge_sorted!([] [0])", "[0]"},
        {"merge_sorted!([0] [0])", "[0 0]"},
        {"merge_sorted!([0 2] [1 3])", "[0 1 2 3]"},
        {"merge_sorted!([0 1 2 5 8 9] [0 2 4 6 7])", "[0 0 1 2 2 4 5 6 7 8 9]"},
    ));
    testEvaluateAll("indexing tuple", TEST_CASES(
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
    ));
    testEvaluateAll("indexing stack", TEST_CASES(
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
    ));
    testEvaluateAll("indexing string", TEST_CASES(
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
    ));
    testEvaluateAll("zip2", TEST_CASES(
        {"zip2!([] [])", "[]"},
        {"zip2!([0] [1])", "[(0 1)]"},
        {"zip2!([0 1] [2 3])", "[(0 2) (1 3)]"},
        {"zip2!([0 1 2] [3 4 5])", "[(0 3) (1 4) (2 5)]"},
    ));
    testEvaluateTypes("zip2", TEST_CASES(
        {"zip2!([] [])", "[(ANY ANY)]"}, // TODO
        {"zip2!([0] [1])", "[(NUMBER NUMBER)]"},
        {"zip2!([0 1] [2 3])", "[(NUMBER NUMBER)]"},
        {"zip2!([0 1 2] [3 4 5])", "[(NUMBER NUMBER)]"},
    ));
    testEvaluateAll("zip3", TEST_CASES(
        {"zip3!([0 1 2] [3 4 5] [6 7 8])", "[(0 3 6) (1 4 7) (2 5 8)]"},
    ));
    testEvaluateAll("zip4", TEST_CASES(
        {"zip4!([0 1] [2 3] [4 5] [6 7])", "[(0 2 4 6) (1 3 5 7)]"},
    ));
    testEvaluateAll("consecutive_pairs", TEST_CASES(
        {"consecutive_pairs![6 4 8]", "[(6 4) (4 8)]"},
    ));
    testEvaluateAll("unique", TEST_CASES(
        {"unique![]", "[]"},
        {"unique![1]", "[1]"},
        {"unique![1 2]", "[1 2]"},
        {"unique![1 1]", "[1]"},
        {"unique![1 1 2 3 1 4 2 4 0]", "[0 1 2 3 4]"},
    ));
    testEvaluateAll("count_elements", TEST_CASES(
        {"count_elements![]", "<>"},
        {"count_elements![1]", "<(1 1)>"},
        {"count_elements![1 2]", "<(1 1) (2 1)>"},
        {"count_elements![1 1]", "<(1 2)>"},
        {"count_elements![1 1 2 3 1 4 2 4 0]", "<(0 1) (1 3) (2 2) (3 1) (4 2)>"},
    ));
    testEvaluateAll("vector math", TEST_CASES(
        {"addv!([1 2] [3 4])", "[4 6]"},
        {"subv!([1 2] [3 4])", "[-2 -2]"},
        {"mulv!([1 2] [3 4])", "[3 8]"},
        {"divv!([6 8] [3 2])", "[2 4]"},
        {"dot!([1 2] [3 4])", "11"},
        {"squared_norm![3 4]", "25"},
        {"norm![3 4]", "5"},
    ));
    return summarizeTests();
}
