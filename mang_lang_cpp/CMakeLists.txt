cmake_minimum_required(VERSION 3.17)
project(mang_lang_cpp)

set(CMAKE_CXX_STANDARD 20)

add_library(shared
        built_in_functions/built_in_functions.cpp
        expressions/Conditional.cpp
        expressions/Dictionary.cpp
        expressions/Expression.cpp
        expressions/Function.cpp
        expressions/FunctionBuiltIn.cpp
        expressions/List.cpp
        expressions/LookupChild.cpp
        expressions/LookupFunction.cpp
        expressions/LookupSymbol.cpp
        expressions/Name.cpp
        expressions/Number.cpp
        expressions/String.cpp
        mang_lang.cpp
        parse_utils.cpp
        )

add_executable(tests tests.cpp)
target_link_libraries(tests shared)
