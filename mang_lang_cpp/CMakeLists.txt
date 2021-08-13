cmake_minimum_required(VERSION 3.17)
project(mang_lang_cpp)

set(CMAKE_CXX_STANDARD 20)

add_library(shared
        operations/apply.cpp
        operations/begin.cpp
        operations/boolean.cpp
        operations/end.cpp
        operations/evaluate.cpp
        operations/lookup.cpp
        operations/parse.cpp
        operations/serialize.cpp
        operations/starts_with.cpp
        built_in_functions/arithmetic.cpp
        built_in_functions/built_in_functions.cpp
        built_in_functions/list.cpp
        built_in_functions/logic.cpp
        Expression.cpp
        factory.cpp
        mang_lang.cpp
        parsing.cpp
        )

add_executable(tests tests.cpp)
target_link_libraries(tests shared)
target_compile_options(shared PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)
