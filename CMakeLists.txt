cmake_minimum_required(VERSION 3.5)
project(manglang_project)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        carma
        GIT_REPOSITORY https://github.com/mabur/carma.git
        GIT_TAG aa0095e610be4be05eab1d5f0ca38149d49a79ae
)
FetchContent_MakeAvailable(carma)

add_library(manglang_lib
        lib/built_in_functions/arithmetic.cpp
        lib/built_in_functions/binary_tuple.cpp
        lib/built_in_functions/built_in_functions.cpp
        lib/built_in_functions/container.cpp
        lib/passes/evaluate.cpp
        lib/passes/parse.cpp
        lib/passes/serialize.cpp
        lib/exceptions.cpp
        lib/expression.cpp
        lib/expression_type.cpp
        lib/factory.cpp
        lib/mang_lang.cpp
        lib/parsing.cpp
        lib/mang_lang_string.cpp
        )

target_include_directories(manglang_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib ${carma_SOURCE_DIR})
target_link_libraries(manglang_lib carma)

if (NOT MSVC)
        target_compile_options(manglang_lib PRIVATE -Wall -pedantic -Werror)
endif()

if(CMAKE_PROJECT_NAME STREQUAL manglang_project)
        add_subdirectory(app)
endif()
