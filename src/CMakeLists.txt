cmake_minimum_required(VERSION 3.5)
project(mang_lang_cpp)

set(CMAKE_CXX_STANDARD 20)

add_library(shared
        library/built_in_functions/arithmetic.cpp
        library/built_in_functions/binary_tuple.cpp
        library/built_in_functions/built_in_functions.cpp
        library/built_in_functions/container.cpp
        library/passes/evaluate.cpp
        library/passes/parse.cpp
        library/passes/serialize.cpp
        library/exceptions.cpp
        library/expression.cpp
        library/expression_type.cpp
        library/factory.cpp
        library/mang_lang.cpp
        library/parsing.cpp
        library/mang_lang_string.cpp
        )

add_executable(manglang interpreter.cpp)
add_executable(tests tests.cpp)

include(FetchContent)
FetchContent_Declare(
        carma
        GIT_REPOSITORY https://github.com/mabur/carma.git
        GIT_TAG aa0095e610be4be05eab1d5f0ca38149d49a79ae
)
FetchContent_MakeAvailable(carma)

target_include_directories(manglang PRIVATE ${carma_SOURCE_DIR} library)
target_include_directories(shared PRIVATE ${carma_SOURCE_DIR} library)
target_include_directories(tests PRIVATE ${carma_SOURCE_DIR} library)

target_link_libraries(manglang shared carma)
target_link_libraries(tests shared carma)

if (NOT MSVC)
        target_compile_options(shared PRIVATE -Wall -pedantic -Werror)
        target_compile_options(manglang PRIVATE -Wall -pedantic -Werror)
        target_compile_options(tests PRIVATE -Wall -pedantic -Werror)
endif()

target_link_options(manglang PRIVATE -static)
target_link_options(tests PRIVATE -static)
