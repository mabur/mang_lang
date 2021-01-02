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
