cmake_minimum_required(VERSION 2.8.2)
project(mang_lang_cpp)

set(CMAKE_CXX_STANDARD 14)

add_library(shared
        built_in_functions/arithmetic.cpp
        built_in_functions/binary_tuple.cpp
        built_in_functions/built_in_functions.cpp
        built_in_functions/container.cpp
        passes/bind.cpp
        passes/evaluate.cpp
        passes/parse.cpp
        passes/serialize.cpp
        expression.cpp
        factory.cpp
        mang_lang.cpp
        parsing.cpp
        )

add_executable(manglang interpreter.cpp)
add_executable(tests tests.cpp)

target_link_libraries(manglang shared)
target_link_libraries(tests shared)

if (NOT MSVC)
        target_compile_options(shared PRIVATE -Wall -pedantic -Werror)
        target_compile_options(manglang PRIVATE -Wall -pedantic -Werror)
        target_compile_options(tests PRIVATE -Wall -pedantic -Werror)
endif()

#target_link_options(manglang PRIVATE -static)
#target_link_options(tests PRIVATE -static)
