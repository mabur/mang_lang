#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "parser.h"
#include <string>

std::string roundtrip(std::string code) {
    return parse(code)->serialize();
}

std::string evaluate(std::string code) {
    return parse(code)->evaluate()->serialize();
}

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
    CHECK(roundtrip(data.in) == data.out);
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
    CHECK(roundtrip(data) == data);
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
    CHECK(roundtrip(data.in) == data.out);
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
    CHECK(roundtrip(data.in) == data.out);
    CHECK(evaluate(data.in) == data.out);
}

TEST_CASE("Conditional") {
    auto data = GENERATE(values<Data>({
        {"if 1 then 2 else 3", "if 1 then 2 else 3"},
        {"if  1  then  2  else  3", "if 1 then 2 else 3"},
    }));
    CHECK(roundtrip(data.in) == data.out);
}

TEST_CASE("conditional_evaluation") {
    auto data = GENERATE(values<Data>({
        {"if 1 then 2 else 3", "2"},
        {"if 0 then 2 else 3", "3"},
    }));
    CHECK(evaluate(data.in) == data.out);
}
