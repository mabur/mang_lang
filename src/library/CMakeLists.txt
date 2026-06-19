cmake_minimum_required(VERSION 3.5)
project(mang_lang_lib)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        carma
        GIT_REPOSITORY https://github.com/mabur/carma.git
        GIT_TAG aa0095e610be4be05eab1d5f0ca38149d49a79ae
)
FetchContent_MakeAvailable(carma)

add_library(mang_lang
        built_in_functions/arithmetic.cpp
        built_in_functions/binary_tuple.cpp
        built_in_functions/built_in_functions.cpp
        built_in_functions/container.cpp
        passes/evaluate.cpp
        passes/parse.cpp
        passes/serialize.cpp
        exceptions.cpp
        expression.cpp
        expression_type.cpp
        factory.cpp
        mang_lang.cpp
        parsing.cpp
        mang_lang_string.cpp
        )

target_include_directories(mang_lang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${carma_SOURCE_DIR})
target_link_libraries(mang_lang carma)

if (NOT MSVC)
        target_compile_options(mang_lang PRIVATE -Wall -pedantic -Werror)
endif()
