#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "mang_lang.h"
#include <string>

struct Data {
    std::string in;
    std::string out;
};

TEST_CASE("Number") {
    auto data = GENERATE(values<Data>({
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
    }));
    CHECK(reformat(data.in) == data.out);
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("String") {
    auto data = GENERATE(values<std::string>({
        R"("")",
        R"("a")",
        R"("ab")",
        R"("abc")",
        R"("1")",
        R"("-1")",
        R"("+1")",
        R"("[]")",
        R"("{}")",
    }));
    CHECK(reformat(data) == data);
    CHECK(evaluate(data) == data);
}

TEST_CASE("List") {
    auto data = GENERATE(values<Data>({
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
    }));
    CHECK(reformat(data.in) == data.out);
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("Dictionary") {
    auto data = GENERATE(values<Data>({
        {"{}", "{}"},
        {"{ }", "{}"},
        {"{a=1}", "{a=1}"},
        {"{ a = 1 }", "{a=1}"},
        {"{a=1,b=2}", "{a=1,b=2}"},
        {"{ a = 1 , b = 2 }", "{a=1,b=2}"},
    }));
    CHECK(reformat(data.in) == data.out);
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("Conditional") {
    auto data = GENERATE(values<Data>({
        {"if 1 then 2 else 3", "if 1 then 2 else 3"},
        {"if  1  then  2  else  3", "if 1 then 2 else 3"},
    }));
    CHECK(reformat(data.in) == data.out);
}

TEST_CASE("conditional_evaluation") {
    auto data = GENERATE(values<Data>({
        {"if 1 then 2 else 3", "2"},
        {"if 0 then 2 else 3", "3"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("symbol") {
    auto data = GENERATE(values<Data>({
        {"{a=1, b=a}", "{a=1,b=a}"},
    }));
    CHECK(reformat(data.in) == data.out);
}

TEST_CASE("symbol_evaluation") {
    auto data = GENERATE(values<Data>({
        {"{a=1, b=a}", "{a=1,b=1}"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("child_symbol") {
    auto data = GENERATE(values<Data>({
        {"a<{a=1}", "a<{a=1}"},
    }));
    CHECK(reformat(data.in) == data.out);
}

TEST_CASE("child_symbol_evaluation") {
    auto data = GENERATE(values<Data>({
        {"a<{a=1}", "1"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("lookup_function") {
    auto data = GENERATE(values<Data>({
        {"add []", "add []"},
    }));
    CHECK(reformat(data.in) == data.out);
}

TEST_CASE("min_evaluation") {
    auto data = GENERATE(values<Data>({
        {"min[0]", "0"},
        {"min[0,1]", "0"},
        {"min[1,0]", "0"},
        {"min[3,6,1]", "1"},
        {"min[7,-3,8,-9]", "-9"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("add_evaluation") {
    auto data = GENERATE(values<Data>({
        {"add[]", "0"},
        {"add[0]", "0"},
        {"add[1]", "1"},
        {"add[0,1]", "1"},
        {"add[0,1,2]", "3"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("mul_evaluation") {
    auto data = GENERATE(values<Data>({
        {"mul[]", "1"},
        {"mul[0]", "0"},
        {"mul[1]", "1"},
        {"mul[1,2]", "2"},
        {"mul[1,2,3]", "6"},
        {"mul[1,2,3,4]", "24"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("sub_evaluation") {
    auto data = GENERATE(values<Data>({
        {"sub[0,0]", "0"},
        {"sub[6,3]", "3"},
        {"sub[4,8]", "-4"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("div_evaluation") {
    auto data = GENERATE(values<Data>({
        {"div[0,1]", "0"},
        {"div[2,1]", "2"},
        {"div[9,3]", "3"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("abs_evaluation") {
    auto data = GENERATE(values<Data>({
        {"abs -1", "1"},
        {"abs 0", "0"},
        {"abs 1", "1"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("sqrt_evaluation") {
    auto data = GENERATE(values<Data>({
        {"sqrt 0", "0"},
        {"sqrt 1", "1"},
        {"sqrt 4", "2"},
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("boolean_evaluation") {
    auto data = GENERATE(values<Data>({
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
    }));
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("not_evaluation") {
    auto data = GENERATE(values<Data>({
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
    }));
    CHECK(evaluate(data.in) == data.out);
}
